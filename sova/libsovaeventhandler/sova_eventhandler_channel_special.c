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

#include "sova_eventhandler_channel_special.h"
#include "g_cclosure_roccat_marshaller.h"
#include "sova_event.h"
#include "sova_device.h"
#include "roccat_device_hidraw.h"
#include "roccat_helper.h"
#include "i18n-lib.h"

#define SOVA_EVENTHANDLER_CHANNEL_SPECIAL_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), SOVA_EVENTHANDLER_CHANNEL_SPECIAL_TYPE, SovaEventhandlerChannelSpecialClass))
#define IS_SOVA_EVENTHANDLER_CHANNEL_SPECIAL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), SOVA_EVENTHANDLER_CHANNEL_SPECIAL_TYPE))
#define SOVA_EVENTHANDLER_CHANNEL_SPECIAL_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), SOVA_EVENTHANDLER_CHANNEL_SPECIAL_TYPE, SovaEventhandlerChannelSpecialPrivate))

typedef struct _SovaEventhandlerChannelSpecialClass SovaEventhandlerChannelSpecialClass;
typedef struct _SovaEventhandlerChannelSpecialPrivate SovaEventhandlerChannelSpecialPrivate;

struct _SovaEventhandlerChannelSpecial {
	GObject parent;
	SovaEventhandlerChannelSpecialPrivate *priv;
};

struct _SovaEventhandlerChannelSpecialClass {
	GObjectClass parent_class;
};

struct _SovaEventhandlerChannelSpecialPrivate {
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

G_DEFINE_TYPE(SovaEventhandlerChannelSpecial, sova_eventhandler_channel_special, G_TYPE_OBJECT);

static void debug_unknown_event(guchar const *event, gsize size) {
	gchar *string = roccat_data8_to_string(event, size);
	g_debug(_("Got unknown event: %s"), string);
	g_free(string);
}

static void process_chrdev_event(SovaEventhandlerChannelSpecial *channel, SovaSpecial const *event) {
	switch (event->type) {
	case SOVA_SPECIAL_TYPE_PROFILE_START:
	case SOVA_SPECIAL_TYPE_MACRO:
	case SOVA_SPECIAL_TYPE_EASYSHIFT:
	case SOVA_SPECIAL_TYPE_RAD:
	case SOVA_SPECIAL_TYPE_EASYSHIFT_SELF:
		/* do nothing */
		break;
	case SOVA_SPECIAL_TYPE_PROFILE:
		g_signal_emit((gpointer)channel, signals[PROFILE_CHANGED], 0, event->data + 1);
		break;
	case SOVA_SPECIAL_TYPE_LIVE_RECORDING:
		g_signal_emit((gpointer)channel, signals[LIVE_RECORDING], 0, event->data, event->action);
		break;
	case SOVA_SPECIAL_TYPE_OPEN_APP:
		if (event->action == SOVA_SPECIAL_ACTION_PRESS)
			g_signal_emit((gpointer)channel, signals[OPEN_APP], 0, event->data);
		break;
	case SOVA_SPECIAL_TYPE_BACKLIGHT:
		g_signal_emit((gpointer)channel, signals[BRIGHTNESS_CHANGED], 0, event->data);
		break;
	case SOVA_SPECIAL_TYPE_TIMER_START:
		if (event->action == SOVA_SPECIAL_ACTION_PRESS)
			g_signal_emit((gpointer)channel, signals[TIMER_START], 0, event->data);
		break;
	case SOVA_SPECIAL_TYPE_TIMER_STOP:
		if (event->action == SOVA_SPECIAL_ACTION_PRESS)
			g_signal_emit((gpointer)channel, signals[TIMER_STOP], 0);
		break;
	case SOVA_SPECIAL_TYPE_OPEN_DRIVER:
		if (event->data == SOVA_SPECIAL_FUNCTION_ACTIVATE)
			g_signal_emit((gpointer)channel, signals[OPEN_DRIVER], 0);
		break;
	case SOVA_SPECIAL_TYPE_OPEN:
		if (event->action == SOVA_SPECIAL_ACTION_PRESS)
			g_signal_emit((gpointer)channel, signals[OPEN], 0, event->subtype, event->data);
		break;
	case SOVA_SPECIAL_TYPE_POWERSAVE:
		g_signal_emit((gpointer)channel, signals[POWERSAVE], 0, event->subtype);
		break;
	case SOVA_SPECIAL_TYPE_TALK:
		g_signal_emit((gpointer)channel, signals[TALK], 0, event->data, event->action);
		break;
	default:
		debug_unknown_event((guchar const *)event, sizeof(SovaSpecial));
		break;
	}
}

static gboolean io_cb(GIOChannel *channel, GIOCondition condition, gpointer user_data) {
	SovaEventhandlerChannelSpecial *eventhandler_channel_special = SOVA_EVENTHANDLER_CHANNEL_SPECIAL(user_data);
	GIOStatus status;
	GError *error = NULL;
	gsize length;
	SovaSpecial event;

	if (condition & G_IO_HUP)
		return FALSE;

	status = g_io_channel_read_chars(channel, (gchar *)&event, sizeof(SovaSpecial), &length, &error);

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

	if (length == sizeof(SovaSpecial) && event.report_id == SOVA_REPORT_ID_SPECIAL)
		process_chrdev_event(eventhandler_channel_special, &event);
	else
		debug_unknown_event((guchar const *)&event, length);

	return TRUE; // keep on going
}

SovaEventhandlerChannelSpecial *sova_eventhandler_channel_special_new(void) {
	return SOVA_EVENTHANDLER_CHANNEL_SPECIAL(g_object_new(SOVA_EVENTHANDLER_CHANNEL_SPECIAL_TYPE, NULL));
}

static void sova_eventhandler_channel_special_init(SovaEventhandlerChannelSpecial *channel) {
	SovaEventhandlerChannelSpecialPrivate *priv = SOVA_EVENTHANDLER_CHANNEL_SPECIAL_GET_PRIVATE(channel);
	channel->priv = priv;
	priv->running = FALSE;
}

gboolean sova_eventhandler_channel_special_start(SovaEventhandlerChannelSpecial *channel, RoccatDevice *device, GError **error) {
	GError *local_error = NULL;
	SovaEventhandlerChannelSpecialPrivate *priv = channel->priv;
	priv->event_source_id = roccat_device_hidraw_add_event_watch(device, SOVA_INTERFACE_MOUSE, io_cb, channel, &local_error);
	if (local_error) {
		g_propagate_error(error, local_error);
		priv->running = FALSE;
		return FALSE;
	} else {
		priv->running = TRUE;
		return TRUE;
	}
}

void sova_eventhandler_channel_special_stop(SovaEventhandlerChannelSpecial *channel) {
	SovaEventhandlerChannelSpecialPrivate *priv = channel->priv;
	if (priv->running) {
		g_source_remove(priv->event_source_id);
		priv->running = FALSE;
	}
}

static void finalize(GObject *object) {
	SovaEventhandlerChannelSpecial *eventhandler_channel_special = SOVA_EVENTHANDLER_CHANNEL_SPECIAL(object);
	sova_eventhandler_channel_special_stop(eventhandler_channel_special);
	G_OBJECT_CLASS(sova_eventhandler_channel_special_parent_class)->finalize(object);
}

static void sova_eventhandler_channel_special_class_init(SovaEventhandlerChannelSpecialClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = (GObjectClass *)klass;

	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(SovaEventhandlerChannelSpecialPrivate));

	signals[POWERSAVE] = g_signal_new("powersave", SOVA_EVENTHANDLER_CHANNEL_SPECIAL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[BRIGHTNESS_CHANGED] = g_signal_new("brightness-changed", SOVA_EVENTHANDLER_CHANNEL_SPECIAL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[LIVE_RECORDING] = g_signal_new("live-recording", SOVA_EVENTHANDLER_CHANNEL_SPECIAL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_VOID__UCHAR_UCHAR, G_TYPE_NONE,
			2, G_TYPE_UCHAR, G_TYPE_UCHAR);

	signals[OPEN_DRIVER] = g_signal_new("open-driver", SOVA_EVENTHANDLER_CHANNEL_SPECIAL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE,
			0);

	signals[PROFILE_CHANGED] = g_signal_new("profile-changed", SOVA_EVENTHANDLER_CHANNEL_SPECIAL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[OPEN_APP] = g_signal_new("open-app", SOVA_EVENTHANDLER_CHANNEL_SPECIAL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[TALK] = g_signal_new("talk", SOVA_EVENTHANDLER_CHANNEL_SPECIAL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_VOID__UCHAR_UCHAR, G_TYPE_NONE,
			2, G_TYPE_UCHAR, G_TYPE_UCHAR);

	signals[TIMER_START] = g_signal_new("timer-start", SOVA_EVENTHANDLER_CHANNEL_SPECIAL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[TIMER_STOP] = g_signal_new("timer-stop", SOVA_EVENTHANDLER_CHANNEL_SPECIAL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE,
			0);

	signals[OPEN] = g_signal_new("open", SOVA_EVENTHANDLER_CHANNEL_SPECIAL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_VOID__UCHAR_UCHAR, G_TYPE_NONE,
			2, G_TYPE_UCHAR, G_TYPE_UCHAR);
}
