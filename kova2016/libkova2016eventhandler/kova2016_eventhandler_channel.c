/*
 * This file is part of roccat-tools.
 *
 * roccat-tools is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * roccat-tools is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with roccat-tools. If not, see <http://www.gnu.org/licenses/>.
 */

#include "kova2016_eventhandler_channel.h"
#include "g_cclosure_roccat_marshaller.h"
#include "kova2016_device.h"
#include "roccat_device_hidraw.h"
#include "roccat.h"
#include "roccat_helper.h"
#include "i18n-lib.h"
#include "../libroccatkova2016/kova2016_special.h"

#define KOVA2016_EVENTHANDLER_CHANNEL_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KOVA2016_EVENTHANDLER_CHANNEL_TYPE, Kova2016EventhandlerChannelClass))
#define IS_KOVA2016_EVENTHANDLER_CHANNEL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KOVA2016_EVENTHANDLER_CHANNEL_TYPE))
#define KOVA2016_EVENTHANDLER_CHANNEL_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KOVA2016_EVENTHANDLER_CHANNEL_TYPE, Kova2016EventhandlerChannelPrivate))

typedef struct _Kova2016EventhandlerChannelClass Kova2016EventhandlerChannelClass;
typedef struct _Kova2016EventhandlerChannelPrivate Kova2016EventhandlerChannelPrivate;

struct _Kova2016EventhandlerChannel {
	GObject parent;
	Kova2016EventhandlerChannelPrivate *priv;
};

struct _Kova2016EventhandlerChannelClass {
	GObjectClass parent_class;
};

struct _Kova2016EventhandlerChannelPrivate {
	guint event_source_id;
	gboolean running;
};

enum {
	PROFILE_CHANGED, /* profile_index */
	OPEN_APPLICATION, /* key_index */
	TIMER_START, /* key_index */
	TIMER_STOP,
	OPEN_DRIVER,
	CPI_CHANGED, /* cpi_index */
	OPEN, /* subtype, key_index */
	EASYSHIFT, /* key_index, action */
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

G_DEFINE_TYPE(Kova2016EventhandlerChannel, kova2016_eventhandler_channel, G_TYPE_OBJECT);

static void process_chrdev_event(Kova2016EventhandlerChannel *eventhandler_channel, Kova2016Special const *event) {
	gchar *string;

	switch (event->type) {
	case KOVA2016_SPECIAL_TYPE_RAD_LEFT:
	case KOVA2016_SPECIAL_TYPE_RAD_RIGHT:
	case KOVA2016_SPECIAL_TYPE_RAD_MIDDLE:
	case KOVA2016_SPECIAL_TYPE_RAD_FORWARD:
	case KOVA2016_SPECIAL_TYPE_RAD_BACKWARD:
	case KOVA2016_SPECIAL_TYPE_RAD_SCROLL_UP:
	case KOVA2016_SPECIAL_TYPE_RAD_SCROLL_DOWN:
	case KOVA2016_SPECIAL_TYPE_RAD_EASYAIM:
		/* Ignore RAD events */
		break;
	case KOVA2016_SPECIAL_TYPE_PROFILE:
		g_signal_emit((gpointer)eventhandler_channel, signals[PROFILE_CHANGED], 0, event->data1 - 1);
		break;
	case KOVA2016_SPECIAL_TYPE_OPEN_APPLICATION:
		if (event->data2 == KOVA2016_SPECIAL_ACTION_PRESS)
			g_signal_emit((gpointer)eventhandler_channel, signals[OPEN_APPLICATION], 0, event->data1 - 1);
		break;
	case KOVA2016_SPECIAL_TYPE_TIMER_START:
		if (event->data2 == KOVA2016_SPECIAL_ACTION_PRESS)
			g_signal_emit((gpointer)eventhandler_channel, signals[TIMER_START], 0, event->data1 - 1);
		break;
	case KOVA2016_SPECIAL_TYPE_TIMER_STOP:
		if (event->data2 == KOVA2016_SPECIAL_ACTION_PRESS)
			g_signal_emit((gpointer)eventhandler_channel, signals[TIMER_STOP], 0);
		break;
	case KOVA2016_SPECIAL_TYPE_OPEN_DRIVER:
		if (event->data1 == KOVA2016_SPECIAL_ACTION_PRESS)
			g_signal_emit((gpointer)eventhandler_channel, signals[OPEN_DRIVER], 0);
		break;
	case KOVA2016_SPECIAL_TYPE_CPI:
		g_signal_emit((gpointer)eventhandler_channel, signals[CPI_CHANGED], 0, event->data1 - 1);
		break;
	case KOVA2016_SPECIAL_TYPE_MULTIMEDIA:
		break;
	case KOVA2016_SPECIAL_TYPE_OPEN:
		g_signal_emit((gpointer)eventhandler_channel, signals[OPEN], 0, event->data1, event->data2 - 1);
		break;
	case KOVA2016_SPECIAL_TYPE_EASYSHIFT:
		g_signal_emit((gpointer)eventhandler_channel, signals[EASYSHIFT], 0, event->data1 - 1, event->data2);
		break;
	default:
		string = roccat_data8_to_string((guchar const *)event, sizeof(Kova2016Special));
		g_debug(_("Got unknown event: %s"), string);
		g_free(string);
		break;
	}
}

static gboolean io_cb(GIOChannel *channel, GIOCondition condition, gpointer user_data) {
	Kova2016EventhandlerChannel *eventhandler_channel = KOVA2016_EVENTHANDLER_CHANNEL(user_data);
	Kova2016Special event;
	GIOStatus status;
	GError *error = NULL;
	gsize length;

	if (condition & G_IO_HUP)
		return FALSE;

	status = g_io_channel_read_chars(channel, (gchar *)&event, sizeof(Kova2016Special), &length, &error);

	if (status == G_IO_STATUS_AGAIN)
		return TRUE;

	if (status == G_IO_STATUS_ERROR) {
		g_critical(_("Could not read io_channel: %s"), error->message);
		return FALSE;
	}

	if (status == G_IO_STATUS_EOF) {
		g_warning(_("Could not read io_channel: %s"), _("End of file"));
		return FALSE;
	}

	if (length < sizeof(Kova2016Special))
		return TRUE;

	if (event.report_id != KOVA2016_REPORT_ID_SPECIAL)
		return TRUE;

	process_chrdev_event(eventhandler_channel, &event);

	return TRUE; // keep on going
}

Kova2016EventhandlerChannel *kova2016_eventhandler_channel_new(void) {
	return KOVA2016_EVENTHANDLER_CHANNEL(g_object_new(KOVA2016_EVENTHANDLER_CHANNEL_TYPE, NULL));
}

static void kova2016_eventhandler_channel_init(Kova2016EventhandlerChannel *channel) {
	Kova2016EventhandlerChannelPrivate *priv = KOVA2016_EVENTHANDLER_CHANNEL_GET_PRIVATE(channel);
	channel->priv = priv;
	priv->running = FALSE;
}

gboolean kova2016_eventhandler_channel_start(Kova2016EventhandlerChannel *eventhandler_channel, RoccatDevice *device, GError **error) {
	GError *local_error = NULL;
	Kova2016EventhandlerChannelPrivate *priv = eventhandler_channel->priv;
	priv->event_source_id = roccat_device_hidraw_add_event_watch(device, KOVA2016_INTERFACE_MOUSE, io_cb, eventhandler_channel, &local_error);
	if (local_error) {
		g_propagate_error(error, local_error);
		priv->running = FALSE;
		return FALSE;
	} else {
		priv->running = TRUE;
		return TRUE;
	}
}

void kova2016_eventhandler_channel_stop(Kova2016EventhandlerChannel *eventhandler_channel) {
	Kova2016EventhandlerChannelPrivate *priv = eventhandler_channel->priv;
	if (priv->running) {
		g_source_remove(priv->event_source_id);
		priv->running = FALSE;
	}
}

static void finalize(GObject *object) {
	Kova2016EventhandlerChannel *eventhandler_channel = KOVA2016_EVENTHANDLER_CHANNEL(object);
	kova2016_eventhandler_channel_stop(eventhandler_channel);
	G_OBJECT_CLASS(kova2016_eventhandler_channel_parent_class)->finalize(object);
}

static void kova2016_eventhandler_channel_class_init(Kova2016EventhandlerChannelClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = (GObjectClass *)klass;

	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(Kova2016EventhandlerChannelPrivate));

	signals[PROFILE_CHANGED] = g_signal_new("profile-changed", KOVA2016_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[OPEN_APPLICATION] = g_signal_new("open-application", KOVA2016_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[TIMER_START] = g_signal_new("timer-start", KOVA2016_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[TIMER_STOP] = g_signal_new("timer-stop", KOVA2016_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE,
			0);

	signals[OPEN_DRIVER] = g_signal_new("open-driver", KOVA2016_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE,
			0);

	signals[CPI_CHANGED] = g_signal_new("cpi-changed", KOVA2016_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[OPEN] = g_signal_new("open", KOVA2016_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_VOID__UCHAR_UCHAR, G_TYPE_NONE,
			2, G_TYPE_UCHAR, G_TYPE_UCHAR);

	signals[EASYSHIFT] = g_signal_new("easyshift", KOVA2016_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_VOID__UCHAR_UCHAR, G_TYPE_NONE,
			2, G_TYPE_UCHAR, G_TYPE_UCHAR);
}
