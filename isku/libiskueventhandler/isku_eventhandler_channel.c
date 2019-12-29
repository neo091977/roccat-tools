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

#include "isku_eventhandler_channel.h"
#include "g_cclosure_roccat_marshaller.h"
#include "roccat_device_hidraw.h"
#include "roccat.h"
#include "isku.h"
#include "roccat_helper.h"
#include "i18n-lib.h"
#include <string.h>

#define ISKU_EVENTHANDLER_CHANNEL_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ISKU_EVENTHANDLER_CHANNEL_TYPE, IskuEventhandlerChannelClass))
#define IS_ISKU_EVENTHANDLER_CHANNEL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ISKU_EVENTHANDLER_CHANNEL_TYPE))
#define ISKU_EVENTHANDLER_CHANNEL_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ISKU_EVENTHANDLER_CHANNEL_TYPE, IskuEventhandlerChannelPrivate))

typedef struct _IskuEventhandlerChannelClass IskuEventhandlerChannelClass;
typedef struct _IskuEventhandlerChannelPrivate IskuEventhandlerChannelPrivate;

struct _IskuEventhandlerChannel {
	GObject parent;
	IskuEventhandlerChannelPrivate *priv;
};

struct _IskuEventhandlerChannelClass {
	GObjectClass parent_class;
};

struct _IskuEventhandlerChannelPrivate {
	guint event_source_id;
	gboolean running;
};

enum {
	LIGHT_CHANGED, /* brightness_number */
	LIVE_RECORDING, /* key_index, event */
	OPEN_DRIVER,
	PROFILE_CHANGED, /* profile_number */
	QUICKLAUNCH, /* key_index */
	TALK, /* key_type, event */
	TIMER_START, /* key_index */
	TIMER_STOP,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

G_DEFINE_TYPE(IskuEventhandlerChannel, isku_eventhandler_channel, G_TYPE_OBJECT);

static void process_chrdev_event(IskuEventhandlerChannel *eventhandler_channel, IskuSpecial const *event) {
	gchar *string;

	switch (event->event) {
	case ISKU_SPECIAL_EVENT_PROFILE_START:
	case ISKU_SPECIAL_EVENT_MACRO:
	case ISKU_SPECIAL_EVENT_RAD:
	case ISKU_SPECIAL_EVENT_MULTIMEDIA:
		/* do nothing */
		break;
	case ISKU_SPECIAL_EVENT_LIGHT:
		g_signal_emit((gpointer)eventhandler_channel, signals[LIGHT_CHANGED], 0, event->data1);
		break;
	case ISKU_SPECIAL_EVENT_PROFILE:
		g_signal_emit((gpointer)eventhandler_channel, signals[PROFILE_CHANGED], 0, event->data1);
		break;
	case ISKU_SPECIAL_EVENT_LIVE_RECORDING:
		g_signal_emit((gpointer)eventhandler_channel, signals[LIVE_RECORDING], 0, event->data1 - 1, event->data2);
		break;
	case ISKU_SPECIAL_EVENT_QUICKLAUNCH:
		if (event->data2 == ISKU_SPECIAL_EVENT_ACTION_PRESS)
			g_signal_emit((gpointer)eventhandler_channel, signals[QUICKLAUNCH], 0, event->data1 - 1);
		break;
	case ISKU_SPECIAL_EVENT_OPEN_DRIVER:
		if (event->data1 == ISKU_SPECIAL_OPEN_DRIVER_ACTION_PRESS)
			g_signal_emit((gpointer)eventhandler_channel, signals[OPEN_DRIVER], 0);
		break;
	case ISKU_SPECIAL_EVENT_TIMER_START:
		if (event->data2 == ISKU_SPECIAL_EVENT_ACTION_PRESS)
			g_signal_emit((gpointer)eventhandler_channel, signals[TIMER_START], 0, event->data1 - 1);
		break;
	case ISKU_SPECIAL_EVENT_TIMER_STOP:
		if (event->data2 == ISKU_SPECIAL_EVENT_ACTION_PRESS)
			g_signal_emit((gpointer)eventhandler_channel, signals[TIMER_STOP], 0);
		break;
	case ISKU_SPECIAL_EVENT_TALK:
		g_signal_emit((gpointer)eventhandler_channel, signals[TALK], 0, event->data1, event->data2);
		break;
	default:
		string = roccat_data8_to_string((guchar const *)event, sizeof(IskuSpecial));
		g_debug(_("Got unknown event: %s"), string);
		g_free(string);
	}
}

static gboolean io_cb(GIOChannel *channel, GIOCondition condition, gpointer user_data) {
	IskuEventhandlerChannel *eventhandler_channel = ISKU_EVENTHANDLER_CHANNEL(user_data);
	IskuSpecial event;
	GIOStatus status;
	GError *error = NULL;
	gsize length;

	if (condition & G_IO_HUP)
		return FALSE;

	status = g_io_channel_read_chars(channel, (gchar *)&event, sizeof(IskuSpecial), &length, &error);

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

	if (length < sizeof(IskuSpecial))
		return TRUE;

	switch (event.report_id) {
	case ISKU_REPORT_ID_SPECIAL:
		process_chrdev_event(eventhandler_channel, &event);
		break;
	default:
		break;
	}

	return TRUE;
}

IskuEventhandlerChannel *isku_eventhandler_channel_new(void) {
	return ISKU_EVENTHANDLER_CHANNEL(g_object_new(ISKU_EVENTHANDLER_CHANNEL_TYPE, NULL));
}

static void isku_eventhandler_channel_init(IskuEventhandlerChannel *channel) {
	IskuEventhandlerChannelPrivate *priv = ISKU_EVENTHANDLER_CHANNEL_GET_PRIVATE(channel);
	channel->priv = priv;
	priv->running = FALSE;
}

gboolean isku_eventhandler_channel_start(IskuEventhandlerChannel *eventhandler_channel, RoccatDevice *device, GError **error) {
	GError *local_error = NULL;
	IskuEventhandlerChannelPrivate *priv = eventhandler_channel->priv;
	priv->event_source_id = roccat_device_hidraw_add_event_watch(device, ISKU_INTERFACE_MOUSE, io_cb, eventhandler_channel, &local_error);
	if (local_error) {
		g_propagate_error(error, local_error);
		priv->running = FALSE;
		return FALSE;
	} else {
		priv->running = TRUE;
		return TRUE;
	}
}

void isku_eventhandler_channel_stop(IskuEventhandlerChannel *eventhandler_channel) {
	IskuEventhandlerChannelPrivate *priv = eventhandler_channel->priv;
	if (priv->running) {
		g_source_remove(priv->event_source_id);
		priv->running = FALSE;
	}
}

static void finalize(GObject *object) {
	IskuEventhandlerChannel *eventhandler_channel = ISKU_EVENTHANDLER_CHANNEL(object);
	isku_eventhandler_channel_stop(eventhandler_channel);
	G_OBJECT_CLASS(isku_eventhandler_channel_parent_class)->finalize(object);
}

static void isku_eventhandler_channel_class_init(IskuEventhandlerChannelClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = (GObjectClass *)klass;

	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(IskuEventhandlerChannelPrivate));
	
	signals[LIGHT_CHANGED] = g_signal_new("light-changed", ISKU_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[LIVE_RECORDING] = g_signal_new("live-recording", ISKU_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_VOID__UCHAR_UCHAR, G_TYPE_NONE,
			2, G_TYPE_UCHAR, G_TYPE_UCHAR);

	signals[OPEN_DRIVER] = g_signal_new("open-driver", ISKU_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE,
			0);

	signals[PROFILE_CHANGED] = g_signal_new("profile-changed", ISKU_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[QUICKLAUNCH] = g_signal_new("quicklaunch", ISKU_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[TALK] = g_signal_new("talk", ISKU_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_VOID__UCHAR_UCHAR, G_TYPE_NONE,
			2, G_TYPE_UCHAR, G_TYPE_UCHAR);

	signals[TIMER_START] = g_signal_new("timer-start", ISKU_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[TIMER_STOP] = g_signal_new("timer-stop", ISKU_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE,
			0);
}
