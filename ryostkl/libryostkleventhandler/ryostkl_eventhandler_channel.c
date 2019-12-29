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

#include "ryostkl_eventhandler_channel.h"
#include "ryostkl_device.h"
#include "ryostkl.h"
#include "roccat_device_hidraw.h"
#include "roccat.h"
#include "roccat_helper.h"
#include "g_cclosure_roccat_marshaller.h"
#include "i18n-lib.h"

#define RYOSTKL_EVENTHANDLER_CHANNEL_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), RYOSTKL_EVENTHANDLER_CHANNEL_TYPE, RyostklEventhandlerChannelClass))
#define IS_RYOSTKL_EVENTHANDLER_CHANNEL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), RYOSTKL_EVENTHANDLER_CHANNEL_TYPE))
#define RYOSTKL_EVENTHANDLER_CHANNEL_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), RYOSTKL_EVENTHANDLER_CHANNEL_TYPE, RyostklEventhandlerChannelPrivate))

typedef struct _RyostklEventhandlerChannelClass RyostklEventhandlerChannelClass;
typedef struct _RyostklEventhandlerChannelPrivate RyostklEventhandlerChannelPrivate;

struct _RyostklEventhandlerChannel {
	GObject parent;
	RyostklEventhandlerChannelPrivate *priv;
};

struct _RyostklEventhandlerChannelClass {
	GObjectClass parent_class;
};

struct _RyostklEventhandlerChannelPrivate {
	guint event_source_id;
	gboolean running;
};

enum {
	LIGHT_CHANGED, /* brightness_index */
	LIVE_RECORDING, /* key_index, event */
	OPEN_DRIVER,
	PROFILE_CHANGED, /* profile_index */
	QUICKLAUNCH, /* key_index */
	EFFECT, /* function, hid */
	TALK, /* key_type, event */
	TIMER_START, /* key_index */
	TIMER_STOP,
	LED_MACRO, /* key_index */
	LAYER, /* RyosStoredLightsLayers */
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

G_DEFINE_TYPE(RyostklEventhandlerChannel, ryostkl_eventhandler_channel, G_TYPE_OBJECT);

static void debug_unknown_event(guchar const *event, gsize size) {
	gchar *string = roccat_data8_to_string(event, size);
	g_debug(_("Got unknown event: %s"), string);
	g_free(string);
}

static void process_chrdev_event(RyostklEventhandlerChannel *eventhandler_channel, RyosSpecial const *event) {
	switch (event->type) {
	case RYOS_SPECIAL_TYPE_EFFECT:
		g_signal_emit((gpointer)eventhandler_channel, signals[EFFECT], 0, event->action, event->data);
		break;
	case RYOS_SPECIAL_TYPE_LAYER:
		g_signal_emit((gpointer)eventhandler_channel, signals[LAYER], 0, event->data);
		break;
	case RYOS_SPECIAL_TYPE_EASYSHIFT:
	case RYOS_SPECIAL_TYPE_EASYSHIFT_SELF:
	case RYOS_SPECIAL_TYPE_PROFILE_START:
	case RYOS_SPECIAL_TYPE_MULTIMEDIA:
	case RYOS_SPECIAL_TYPE_RAD:
		/* do nothing */
		break;
	case RYOS_SPECIAL_TYPE_LED_MACRO:
		if (event->action == RYOS_SPECIAL_ACTION_PRESS)
			g_signal_emit((gpointer)eventhandler_channel, signals[LED_MACRO], 0, event->data);
		break;
	case RYOS_SPECIAL_TYPE_BACKLIGHT:
		g_signal_emit((gpointer)eventhandler_channel, signals[LIGHT_CHANGED], 0, event->data);
		break;
	case RYOS_SPECIAL_TYPE_PROFILE:
		g_signal_emit((gpointer)eventhandler_channel, signals[PROFILE_CHANGED], 0, event->data + 1);
		break;
	case RYOS_SPECIAL_TYPE_LIVE_RECORDING:
		g_signal_emit((gpointer)eventhandler_channel, signals[LIVE_RECORDING], 0, event->data, event->action);
		break;
	case RYOS_SPECIAL_TYPE_QUICKLAUNCH:
		if (event->action == RYOS_SPECIAL_ACTION_PRESS)
			g_signal_emit((gpointer)eventhandler_channel, signals[QUICKLAUNCH], 0, event->data);
		break;
	case RYOS_SPECIAL_TYPE_OPEN_DRIVER:
		if (event->data == RYOS_SPECIAL_FUNCTION_ACTIVATE)
			g_signal_emit((gpointer)eventhandler_channel, signals[OPEN_DRIVER], 0);
		break;
	case RYOS_SPECIAL_TYPE_TIMER_START:
		if (event->action == RYOS_SPECIAL_ACTION_PRESS)
			g_signal_emit((gpointer)eventhandler_channel, signals[TIMER_START], 0, event->data);
		break;
	case RYOS_SPECIAL_TYPE_TIMER_STOP:
		if (event->action == RYOS_SPECIAL_ACTION_PRESS)
			g_signal_emit((gpointer)eventhandler_channel, signals[TIMER_STOP], 0);
		break;
	case RYOS_SPECIAL_TYPE_TALK:
		g_signal_emit((gpointer)eventhandler_channel, signals[TALK], 0, event->data, event->action);
		break;
	default:
		debug_unknown_event((guchar const *)event, sizeof(RyosSpecial));
		break;
	}
}

static gboolean io_cb(GIOChannel *channel, GIOCondition condition, gpointer user_data) {
	RyostklEventhandlerChannel *eventhandler_channel = RYOSTKL_EVENTHANDLER_CHANNEL(user_data);
	GIOStatus status;
	GError *error = NULL;
	gsize length;
	RyosSpecial event;

	if (condition & G_IO_HUP)
		return FALSE;

	status = g_io_channel_read_chars(channel, (gchar *)&event, sizeof(RyosSpecial), &length, &error);

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

	if (length == sizeof(RyosSpecial) && event.report_id == RYOS_REPORT_ID_SPECIAL)
		process_chrdev_event(eventhandler_channel, &event);
	else
		debug_unknown_event((guchar const *)&event, length);

	return TRUE; // keep on going
}

RyostklEventhandlerChannel *ryostkl_eventhandler_channel_new(void) {
	return RYOSTKL_EVENTHANDLER_CHANNEL(g_object_new(RYOSTKL_EVENTHANDLER_CHANNEL_TYPE, NULL));
}

static void ryostkl_eventhandler_channel_init(RyostklEventhandlerChannel *channel) {
	RyostklEventhandlerChannelPrivate *priv = RYOSTKL_EVENTHANDLER_CHANNEL_GET_PRIVATE(channel);
	channel->priv = priv;
	priv->running = FALSE;
}

gboolean ryostkl_eventhandler_channel_start(RyostklEventhandlerChannel *eventhandler_channel, RoccatDevice *device, GError **error) {
	GError *local_error = NULL;
	RyostklEventhandlerChannelPrivate *priv = eventhandler_channel->priv;
	priv->event_source_id = roccat_device_hidraw_add_event_watch(device, RYOSTKL_INTERFACE_MOUSE, io_cb, eventhandler_channel, &local_error);
	if (local_error) {
		g_propagate_error(error, local_error);
		priv->running = FALSE;
		return FALSE;
	} else {
		priv->running = TRUE;
		return TRUE;
	}
}

void ryostkl_eventhandler_channel_stop(RyostklEventhandlerChannel *eventhandler_channel) {
	RyostklEventhandlerChannelPrivate *priv = eventhandler_channel->priv;
	if (priv->running) {
		g_source_remove(priv->event_source_id);
		priv->running = FALSE;
	}
}

static void finalize(GObject *object) {
	RyostklEventhandlerChannel *eventhandler_channel = RYOSTKL_EVENTHANDLER_CHANNEL(object);
	ryostkl_eventhandler_channel_stop(eventhandler_channel);
	G_OBJECT_CLASS(ryostkl_eventhandler_channel_parent_class)->finalize(object);
}

static void ryostkl_eventhandler_channel_class_init(RyostklEventhandlerChannelClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = (GObjectClass *)klass;

	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(RyostklEventhandlerChannelPrivate));

	signals[LAYER] = g_signal_new("layer", RYOSTKL_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[LED_MACRO] = g_signal_new("led-macro", RYOSTKL_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[LIGHT_CHANGED] = g_signal_new("light-changed", RYOSTKL_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[LIVE_RECORDING] = g_signal_new("live-recording", RYOSTKL_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_VOID__UCHAR_UCHAR, G_TYPE_NONE,
			2, G_TYPE_UCHAR, G_TYPE_UCHAR);

	signals[OPEN_DRIVER] = g_signal_new("open-driver", RYOSTKL_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE,
			0);

	signals[PROFILE_CHANGED] = g_signal_new("profile-changed", RYOSTKL_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[QUICKLAUNCH] = g_signal_new("quicklaunch", RYOSTKL_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[EFFECT] = g_signal_new("effect", RYOSTKL_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_VOID__UCHAR_UCHAR, G_TYPE_NONE,
			2, G_TYPE_UCHAR, G_TYPE_UCHAR);

	signals[TALK] = g_signal_new("talk", RYOSTKL_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_VOID__UCHAR_UCHAR, G_TYPE_NONE,
			2, G_TYPE_UCHAR, G_TYPE_UCHAR);

	signals[TIMER_START] = g_signal_new("timer-start", RYOSTKL_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[TIMER_STOP] = g_signal_new("timer-stop", RYOSTKL_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE,
			0);
}
