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

#include "kiro_eventhandler_channel.h"
#include "g_cclosure_roccat_marshaller.h"
#include "kiro_device.h"
#include "roccat_device_hidraw.h"
#include "roccat.h"
#include "roccat_helper.h"
#include "i18n-lib.h"
#include "../libroccatkiro/kiro_special.h"

#define KIRO_EVENTHANDLER_CHANNEL_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KIRO_EVENTHANDLER_CHANNEL_TYPE, KiroEventhandlerChannelClass))
#define IS_KIRO_EVENTHANDLER_CHANNEL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KIRO_EVENTHANDLER_CHANNEL_TYPE))
#define KIRO_EVENTHANDLER_CHANNEL_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KIRO_EVENTHANDLER_CHANNEL_TYPE, KiroEventhandlerChannelPrivate))

typedef struct _KiroEventhandlerChannelClass KiroEventhandlerChannelClass;
typedef struct _KiroEventhandlerChannelPrivate KiroEventhandlerChannelPrivate;

struct _KiroEventhandlerChannel {
	GObject parent;
	KiroEventhandlerChannelPrivate *priv;
};

struct _KiroEventhandlerChannelClass {
	GObjectClass parent_class;
};

struct _KiroEventhandlerChannelPrivate {
	guint event_source_id;
	gboolean running;
};

enum {
	OPEN_APPLICATION, /* key_index */
	OPEN_DRIVER,
	CPI_CHANGED, /* cpi_index */
	SENSITIVITY_CHANGED, /* x, y */
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

G_DEFINE_TYPE(KiroEventhandlerChannel, kiro_eventhandler_channel, G_TYPE_OBJECT);

static void process_chrdev_event(KiroEventhandlerChannel *eventhandler_channel, KiroSpecial const *event) {
	gchar *string;

	switch (event->type) {
	case KIRO_SPECIAL_TYPE_TILT:
		break;
	case KIRO_SPECIAL_TYPE_OPEN_APPLICATION:
		if (event->data2 == KIRO_SPECIAL_ACTION_PRESS)
			g_signal_emit((gpointer)eventhandler_channel, signals[OPEN_APPLICATION], 0, event->data1 - 1);
		break;
	case KIRO_SPECIAL_TYPE_OPEN_DRIVER:
		if (event->data1 == KIRO_SPECIAL_ACTION_PRESS)
			g_signal_emit((gpointer)eventhandler_channel, signals[OPEN_DRIVER], 0);
		break;
	case KIRO_SPECIAL_TYPE_CPI:
		g_signal_emit((gpointer)eventhandler_channel, signals[CPI_CHANGED], 0, event->data1 - 1);
		break;
	case KIRO_SPECIAL_TYPE_SENSITIVITY:
		g_signal_emit((gpointer)eventhandler_channel, signals[SENSITIVITY_CHANGED], 0, event->data1);
		break;
	case KIRO_SPECIAL_TYPE_MULTIMEDIA:
		break;
	default:
		string = roccat_data8_to_string((guchar const *)event, sizeof(KiroSpecial));
		g_debug(_("Got unknown event: %s"), string);
		g_free(string);
		break;
	}
}

static gboolean io_cb(GIOChannel *channel, GIOCondition condition, gpointer user_data) {
	KiroEventhandlerChannel *eventhandler_channel = KIRO_EVENTHANDLER_CHANNEL(user_data);
	KiroSpecial event;
	GIOStatus status;
	GError *error = NULL;
	gsize length;

	if (condition & G_IO_HUP)
		return FALSE;

	status = g_io_channel_read_chars(channel, (gchar *)&event, sizeof(KiroSpecial), &length, &error);

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

	if (length < sizeof(KiroSpecial))
		return TRUE;

	if (event.report_id != KIRO_REPORT_ID_SPECIAL)
		return TRUE;

	process_chrdev_event(eventhandler_channel, &event);

	return TRUE; // keep on going
}

KiroEventhandlerChannel *kiro_eventhandler_channel_new(void) {
	return KIRO_EVENTHANDLER_CHANNEL(g_object_new(KIRO_EVENTHANDLER_CHANNEL_TYPE, NULL));
}

static void kiro_eventhandler_channel_init(KiroEventhandlerChannel *channel) {
	KiroEventhandlerChannelPrivate *priv = KIRO_EVENTHANDLER_CHANNEL_GET_PRIVATE(channel);
	channel->priv = priv;
	priv->running = FALSE;
}

gboolean kiro_eventhandler_channel_start(KiroEventhandlerChannel *eventhandler_channel, RoccatDevice *device, GError **error) {
	GError *local_error = NULL;
	KiroEventhandlerChannelPrivate *priv = eventhandler_channel->priv;
	priv->event_source_id = roccat_device_hidraw_add_event_watch(device, KIRO_INTERFACE_MOUSE, io_cb, eventhandler_channel, &local_error);
	if (local_error) {
		g_propagate_error(error, local_error);
		priv->running = FALSE;
		return FALSE;
	} else {
		priv->running = TRUE;
		return TRUE;
	}
}

void kiro_eventhandler_channel_stop(KiroEventhandlerChannel *eventhandler_channel) {
	KiroEventhandlerChannelPrivate *priv = eventhandler_channel->priv;
	if (priv->running) {
		g_source_remove(priv->event_source_id);
		priv->running = FALSE;
	}
}

static void finalize(GObject *object) {
	KiroEventhandlerChannel *eventhandler_channel = KIRO_EVENTHANDLER_CHANNEL(object);
	kiro_eventhandler_channel_stop(eventhandler_channel);
	G_OBJECT_CLASS(kiro_eventhandler_channel_parent_class)->finalize(object);
}

static void kiro_eventhandler_channel_class_init(KiroEventhandlerChannelClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = (GObjectClass *)klass;

	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(KiroEventhandlerChannelPrivate));

	signals[OPEN_APPLICATION] = g_signal_new("open-application", KIRO_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[OPEN_DRIVER] = g_signal_new("open-driver", KIRO_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE,
			0);

	signals[CPI_CHANGED] = g_signal_new("cpi-changed", KIRO_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[SENSITIVITY_CHANGED] = g_signal_new("sensitivity-changed", KIRO_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_VOID__UCHAR_UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);
}
