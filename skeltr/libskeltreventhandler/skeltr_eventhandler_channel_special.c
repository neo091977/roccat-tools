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

#include "skeltr_eventhandler_channel_special.h"
#include "g_cclosure_roccat_marshaller.h"
#include "skeltr_special.h"
#include "skeltr_device.h"
#include "roccat_device_hidraw.h"
#include "roccat_helper.h"
#include "i18n-lib.h"

#define SKELTR_EVENTHANDLER_CHANNEL_SPECIAL_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), SKELTR_EVENTHANDLER_CHANNEL_SPECIAL_TYPE, SkeltrEventhandlerChannelSpecialClass))
#define IS_SKELTR_EVENTHANDLER_CHANNEL_SPECIAL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), SKELTR_EVENTHANDLER_CHANNEL_SPECIAL_TYPE))
#define SKELTR_EVENTHANDLER_CHANNEL_SPECIAL_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), SKELTR_EVENTHANDLER_CHANNEL_SPECIAL_TYPE, SkeltrEventhandlerChannelSpecialPrivate))

typedef struct _SkeltrEventhandlerChannelSpecialClass SkeltrEventhandlerChannelSpecialClass;
typedef struct _SkeltrEventhandlerChannelSpecialPrivate SkeltrEventhandlerChannelSpecialPrivate;

struct _SkeltrEventhandlerChannelSpecial {
	GObject parent;
	SkeltrEventhandlerChannelSpecialPrivate *priv;
};

struct _SkeltrEventhandlerChannelSpecialClass {
	GObjectClass parent_class;
};

struct _SkeltrEventhandlerChannelSpecialPrivate {
	guint event_source_id;
	gboolean running;
};

enum {
	BRIGHTNESS_CHANGED, /* brightness_index */
	LIVE_RECORDING, /* key_index, event */
	OPEN_DRIVER,
	PROFILE_CHANGED, /* profile_index */
	OPEN_APP, /* key_index */
	TALK, /* key_type, event */
	TIMER_START, /* key_index */
	TIMER_STOP,
	OPEN, /* subtype, key_index */
	POWERSAVE, /* subtype */
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

G_DEFINE_TYPE(SkeltrEventhandlerChannelSpecial, skeltr_eventhandler_channel_special, G_TYPE_OBJECT);

static void debug_unknown_event(guchar const *event, gsize size) {
	gchar *string = roccat_data8_to_string(event, size);
	g_debug(_("Got unknown event: %s"), string);
	g_free(string);
}

static void process_chrdev_event(SkeltrEventhandlerChannelSpecial *channel, SkeltrSpecial const *event) {
	switch (event->type) {
	case SKELTR_SPECIAL_TYPE_PROFILE_START:
	case SKELTR_SPECIAL_TYPE_MACRO:
	case SKELTR_SPECIAL_TYPE_EASYSHIFT:
	case SKELTR_SPECIAL_TYPE_RAD:
	case SKELTR_SPECIAL_TYPE_EASYSHIFT_SELF:
	case SKELTR_SPECIAL_TYPE_KEY:// FIXME maybe key can be used to remove special typing channel
		/* do nothing */
		break;
	case SKELTR_SPECIAL_TYPE_PROFILE:
		g_signal_emit((gpointer)channel, signals[PROFILE_CHANGED], 0, event->data + 1);
		break;
	case SKELTR_SPECIAL_TYPE_LIVE_RECORDING:
		g_signal_emit((gpointer)channel, signals[LIVE_RECORDING], 0, event->data, event->action);
		break;
	case SKELTR_SPECIAL_TYPE_OPEN_APP:
		if (event->action == SKELTR_SPECIAL_ACTION_PRESS)
			g_signal_emit((gpointer)channel, signals[OPEN_APP], 0, event->data);
		break;
	case SKELTR_SPECIAL_TYPE_BACKLIGHT:
		g_signal_emit((gpointer)channel, signals[BRIGHTNESS_CHANGED], 0, event->data);
		break;
	case SKELTR_SPECIAL_TYPE_TIMER_START:
		if (event->action == SKELTR_SPECIAL_ACTION_PRESS)
			g_signal_emit((gpointer)channel, signals[TIMER_START], 0, event->data);
		break;
	case SKELTR_SPECIAL_TYPE_TIMER_STOP:
		if (event->action == SKELTR_SPECIAL_ACTION_PRESS)
			g_signal_emit((gpointer)channel, signals[TIMER_STOP], 0);
		break;
	case SKELTR_SPECIAL_TYPE_OPEN_DRIVER:
		if (event->data == SKELTR_SPECIAL_FUNCTION_ACTIVATE)
			g_signal_emit((gpointer)channel, signals[OPEN_DRIVER], 0);
		break;
	case SKELTR_SPECIAL_TYPE_OPEN:
		if (event->action == SKELTR_SPECIAL_ACTION_PRESS)
			g_signal_emit((gpointer)channel, signals[OPEN], 0, event->subtype, event->data);
		break;
	case SKELTR_SPECIAL_TYPE_POWERSAVE:
 		g_signal_emit((gpointer)channel, signals[POWERSAVE], 0, event->subtype);
		break;
	case SKELTR_SPECIAL_TYPE_TALK:
		g_signal_emit((gpointer)channel, signals[TALK], 0, event->data, event->action);
		break;
	default:
		debug_unknown_event((guchar const *)event, sizeof(SkeltrSpecial));
		break;
	}
}

static gboolean io_cb(GIOChannel *channel, GIOCondition condition, gpointer user_data) {
	SkeltrEventhandlerChannelSpecial *eventhandler_channel_special = SKELTR_EVENTHANDLER_CHANNEL_SPECIAL(user_data);
	GIOStatus status;
	GError *error = NULL;
	gsize length;
	SkeltrSpecial event;

	if (condition & G_IO_HUP)
		return FALSE;

	status = g_io_channel_read_chars(channel, (gchar *)&event, sizeof(SkeltrSpecial), &length, &error);

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

	if (length == sizeof(SkeltrSpecial) && event.report_id == SKELTR_REPORT_ID_SPECIAL)
		process_chrdev_event(eventhandler_channel_special, &event);
	else
		debug_unknown_event((guchar const *)&event, length);

	return TRUE; // keep on going
}

SkeltrEventhandlerChannelSpecial *skeltr_eventhandler_channel_special_new(void) {
	return SKELTR_EVENTHANDLER_CHANNEL_SPECIAL(g_object_new(SKELTR_EVENTHANDLER_CHANNEL_SPECIAL_TYPE, NULL));
}

static void skeltr_eventhandler_channel_special_init(SkeltrEventhandlerChannelSpecial *channel) {
	SkeltrEventhandlerChannelSpecialPrivate *priv = SKELTR_EVENTHANDLER_CHANNEL_SPECIAL_GET_PRIVATE(channel);
	channel->priv = priv;
	priv->running = FALSE;
}

gboolean skeltr_eventhandler_channel_special_start(SkeltrEventhandlerChannelSpecial *channel, RoccatDevice *device, GError **error) {
	GError *local_error = NULL;
	SkeltrEventhandlerChannelSpecialPrivate *priv = channel->priv;
	priv->event_source_id = roccat_device_hidraw_add_event_watch(device, SKELTR_INTERFACE_MOUSE, io_cb, channel, &local_error);
	if (local_error) {
		g_propagate_error(error, local_error);
		priv->running = FALSE;
		return FALSE;
	} else {
		priv->running = TRUE;
		return TRUE;
	}
}

void skeltr_eventhandler_channel_special_stop(SkeltrEventhandlerChannelSpecial *channel) {
	SkeltrEventhandlerChannelSpecialPrivate *priv = channel->priv;
	if (priv->running) {
		g_source_remove(priv->event_source_id);
		priv->running = FALSE;
	}
}

static void finalize(GObject *object) {
	SkeltrEventhandlerChannelSpecial *eventhandler_channel_special = SKELTR_EVENTHANDLER_CHANNEL_SPECIAL(object);
	skeltr_eventhandler_channel_special_stop(eventhandler_channel_special);
	G_OBJECT_CLASS(skeltr_eventhandler_channel_special_parent_class)->finalize(object);
}

static void skeltr_eventhandler_channel_special_class_init(SkeltrEventhandlerChannelSpecialClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = (GObjectClass *)klass;

	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(SkeltrEventhandlerChannelSpecialPrivate));

	signals[POWERSAVE] = g_signal_new("powersave", SKELTR_EVENTHANDLER_CHANNEL_SPECIAL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[BRIGHTNESS_CHANGED] = g_signal_new("brightness-changed", SKELTR_EVENTHANDLER_CHANNEL_SPECIAL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[LIVE_RECORDING] = g_signal_new("live-recording", SKELTR_EVENTHANDLER_CHANNEL_SPECIAL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_VOID__UCHAR_UCHAR, G_TYPE_NONE,
			2, G_TYPE_UCHAR, G_TYPE_UCHAR);

	signals[OPEN_DRIVER] = g_signal_new("open-driver", SKELTR_EVENTHANDLER_CHANNEL_SPECIAL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE,
			0);

	signals[PROFILE_CHANGED] = g_signal_new("profile-changed", SKELTR_EVENTHANDLER_CHANNEL_SPECIAL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[OPEN_APP] = g_signal_new("open-app", SKELTR_EVENTHANDLER_CHANNEL_SPECIAL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[TALK] = g_signal_new("talk", SKELTR_EVENTHANDLER_CHANNEL_SPECIAL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_VOID__UCHAR_UCHAR, G_TYPE_NONE,
			2, G_TYPE_UCHAR, G_TYPE_UCHAR);

	signals[TIMER_START] = g_signal_new("timer-start", SKELTR_EVENTHANDLER_CHANNEL_SPECIAL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[TIMER_STOP] = g_signal_new("timer-stop", SKELTR_EVENTHANDLER_CHANNEL_SPECIAL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE,
			0);

	signals[OPEN] = g_signal_new("open", SKELTR_EVENTHANDLER_CHANNEL_SPECIAL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_VOID__UCHAR_UCHAR, G_TYPE_NONE,
			2, G_TYPE_UCHAR, G_TYPE_UCHAR);
}
