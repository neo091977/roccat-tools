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

#include "nyth_eventhandler_channel.h"
#include "g_cclosure_roccat_marshaller.h"
#include "nyth_device.h"
#include "roccat_device_hidraw.h"
#include "roccat.h"
#include "roccat_helper.h"
#include "i18n-lib.h"
#include "../libroccatnyth/nyth_special.h"

#define NYTH_EVENTHANDLER_CHANNEL_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), NYTH_EVENTHANDLER_CHANNEL_TYPE, NythEventhandlerChannelClass))
#define IS_NYTH_EVENTHANDLER_CHANNEL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), NYTH_EVENTHANDLER_CHANNEL_TYPE))
#define NYTH_EVENTHANDLER_CHANNEL_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), NYTH_EVENTHANDLER_CHANNEL_TYPE, NythEventhandlerChannelPrivate))

typedef struct _NythEventhandlerChannelClass NythEventhandlerChannelClass;
typedef struct _NythEventhandlerChannelPrivate NythEventhandlerChannelPrivate;

struct _NythEventhandlerChannel {
	GObject parent;
	NythEventhandlerChannelPrivate *priv;
};

struct _NythEventhandlerChannelClass {
	GObjectClass parent_class;
};

struct _NythEventhandlerChannelPrivate {
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
	SENSITIVITY_CHANGED, /* x, y */
	WINDOWS, /* subtype */
	OPEN, /* subtype, key_index */
	EASYSHIFT, /* key_index, action */
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

G_DEFINE_TYPE(NythEventhandlerChannel, nyth_eventhandler_channel, G_TYPE_OBJECT);

static void process_chrdev_event(NythEventhandlerChannel *eventhandler_channel, NythSpecial const *event) {
	gchar *string;

	switch (event->type) {
	case NYTH_SPECIAL_TYPE_RAD_LEFT:
	case NYTH_SPECIAL_TYPE_RAD_RIGHT:
	case NYTH_SPECIAL_TYPE_RAD_MIDDLE:
	case NYTH_SPECIAL_TYPE_RAD_FIN_LEFT:
	case NYTH_SPECIAL_TYPE_RAD_FIN_RIGHT:
	case NYTH_SPECIAL_TYPE_RAD_SCROLL_UP:
	case NYTH_SPECIAL_TYPE_RAD_SCROLL_DOWN:
	case NYTH_SPECIAL_TYPE_RAD_EASYAIM:
	case NYTH_SPECIAL_TYPE_RAD_DISTANCE:
		/* Ignore RAD events */
		break;
	case NYTH_SPECIAL_TYPE_PROFILE:
		g_signal_emit((gpointer)eventhandler_channel, signals[PROFILE_CHANGED], 0, event->data1 - 1);
		break;
	case NYTH_SPECIAL_TYPE_TCU:
		/* not interested here */
		break;
	case NYTH_SPECIAL_TYPE_OPEN_APPLICATION:
		if (event->data2 == NYTH_SPECIAL_ACTION_PRESS)
			g_signal_emit((gpointer)eventhandler_channel, signals[OPEN_APPLICATION], 0, event->data1 - 1);
		break;
	case NYTH_SPECIAL_TYPE_TIMER_START:
		if (event->data2 == NYTH_SPECIAL_ACTION_PRESS)
			g_signal_emit((gpointer)eventhandler_channel, signals[TIMER_START], 0, event->data1 - 1);
		break;
	case NYTH_SPECIAL_TYPE_TIMER_STOP:
		if (event->data2 == NYTH_SPECIAL_ACTION_PRESS)
			g_signal_emit((gpointer)eventhandler_channel, signals[TIMER_STOP], 0);
		break;
	case NYTH_SPECIAL_TYPE_OPEN_DRIVER:
		if (event->data1 == NYTH_SPECIAL_ACTION_PRESS)
			g_signal_emit((gpointer)eventhandler_channel, signals[OPEN_DRIVER], 0);
		break;
	case NYTH_SPECIAL_TYPE_CPI:
		g_signal_emit((gpointer)eventhandler_channel, signals[CPI_CHANGED], 0, event->data1 - 1);
		break;
	case NYTH_SPECIAL_TYPE_SENSITIVITY:
		g_signal_emit((gpointer)eventhandler_channel, signals[SENSITIVITY_CHANGED], 0, event->data1, event->data2);
		break;
	case NYTH_SPECIAL_TYPE_MULTIMEDIA:
		break;
	case NYTH_SPECIAL_TYPE_WINDOWS:
		if (event->data2 == NYTH_SPECIAL_ACTION_PRESS)
			g_signal_emit((gpointer)eventhandler_channel, signals[WINDOWS], 0, event->data1);
		break;
	case NYTH_SPECIAL_TYPE_OPEN:
		g_signal_emit((gpointer)eventhandler_channel, signals[OPEN], 0, event->data1, event->data2 - 1);
		break;
	case NYTH_SPECIAL_TYPE_EASYSHIFT:
		g_signal_emit((gpointer)eventhandler_channel, signals[EASYSHIFT], 0, event->data1 - 1, event->data2);
		break;
	default:
		string = roccat_data8_to_string((guchar const *)event, sizeof(NythSpecial));
		g_debug(_("Got unknown event: %s"), string);
		g_free(string);
		break;
	}
}

static gboolean io_cb(GIOChannel *channel, GIOCondition condition, gpointer user_data) {
	NythEventhandlerChannel *eventhandler_channel = NYTH_EVENTHANDLER_CHANNEL(user_data);
	NythSpecial event;
	GIOStatus status;
	GError *error = NULL;
	gsize length;

	if (condition & G_IO_HUP)
		return FALSE;

	status = g_io_channel_read_chars(channel, (gchar *)&event, sizeof(NythSpecial), &length, &error);

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

	if (length < sizeof(NythSpecial))
		return TRUE;

	if (event.report_id != NYTH_REPORT_ID_SPECIAL)
		return TRUE;

	process_chrdev_event(eventhandler_channel, &event);

	return TRUE; // keep on going
}

NythEventhandlerChannel *nyth_eventhandler_channel_new(void) {
	return NYTH_EVENTHANDLER_CHANNEL(g_object_new(NYTH_EVENTHANDLER_CHANNEL_TYPE, NULL));
}

static void nyth_eventhandler_channel_init(NythEventhandlerChannel *channel) {
	NythEventhandlerChannelPrivate *priv = NYTH_EVENTHANDLER_CHANNEL_GET_PRIVATE(channel);
	channel->priv = priv;
	priv->running = FALSE;
}

gboolean nyth_eventhandler_channel_start(NythEventhandlerChannel *eventhandler_channel, RoccatDevice *device, GError **error) {
	GError *local_error = NULL;
	NythEventhandlerChannelPrivate *priv = eventhandler_channel->priv;
	priv->event_source_id = roccat_device_hidraw_add_event_watch(device, NYTH_INTERFACE_MOUSE, io_cb, eventhandler_channel, &local_error);
	if (local_error) {
		g_propagate_error(error, local_error);
		priv->running = FALSE;
		return FALSE;
	} else {
		priv->running = TRUE;
		return TRUE;
	}
}

void nyth_eventhandler_channel_stop(NythEventhandlerChannel *eventhandler_channel) {
	NythEventhandlerChannelPrivate *priv = eventhandler_channel->priv;
	if (priv->running) {
		g_source_remove(priv->event_source_id);
		priv->running = FALSE;
	}
}

static void finalize(GObject *object) {
	NythEventhandlerChannel *eventhandler_channel = NYTH_EVENTHANDLER_CHANNEL(object);
	nyth_eventhandler_channel_stop(eventhandler_channel);
	G_OBJECT_CLASS(nyth_eventhandler_channel_parent_class)->finalize(object);
}

static void nyth_eventhandler_channel_class_init(NythEventhandlerChannelClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = (GObjectClass *)klass;

	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(NythEventhandlerChannelPrivate));

	signals[PROFILE_CHANGED] = g_signal_new("profile-changed", NYTH_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[OPEN_APPLICATION] = g_signal_new("open-application", NYTH_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[TIMER_START] = g_signal_new("timer-start", NYTH_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[TIMER_STOP] = g_signal_new("timer-stop", NYTH_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE,
			0);

	signals[OPEN_DRIVER] = g_signal_new("open-driver", NYTH_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE,
			0);

	signals[CPI_CHANGED] = g_signal_new("cpi-changed", NYTH_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[SENSITIVITY_CHANGED] = g_signal_new("sensitivity-changed", NYTH_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_VOID__UCHAR_UCHAR, G_TYPE_NONE,
			2, G_TYPE_UCHAR, G_TYPE_UCHAR);

	signals[WINDOWS] = g_signal_new("windows", NYTH_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[OPEN] = g_signal_new("open", NYTH_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_VOID__UCHAR_UCHAR, G_TYPE_NONE,
			2, G_TYPE_UCHAR, G_TYPE_UCHAR);

	signals[EASYSHIFT] = g_signal_new("easyshift", NYTH_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_VOID__UCHAR_UCHAR, G_TYPE_NONE,
			2, G_TYPE_UCHAR, G_TYPE_UCHAR);
}
