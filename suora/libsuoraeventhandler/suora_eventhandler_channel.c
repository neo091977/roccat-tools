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

#include "suora_eventhandler_channel.h"
#include "g_cclosure_roccat_marshaller.h"
#include "suora.h"
#include "suora_device.h"
#include "roccat_device_hidraw.h"
#include "roccat.h"
#include "roccat_helper.h"
#include "i18n-lib.h"

#define SUORA_EVENTHANDLER_CHANNEL_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), SUORA_EVENTHANDLER_CHANNEL_TYPE, SuoraEventhandlerChannelClass))
#define IS_SUORA_EVENTHANDLER_CHANNEL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), SUORA_EVENTHANDLER_CHANNEL_TYPE))
#define SUORA_EVENTHANDLER_CHANNEL_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), SUORA_EVENTHANDLER_CHANNEL_TYPE, SuoraEventhandlerChannelPrivate))

typedef struct _SuoraEventhandlerChannelClass SuoraEventhandlerChannelClass;
typedef struct _SuoraEventhandlerChannelPrivate SuoraEventhandlerChannelPrivate;

struct _SuoraEventhandlerChannel {
	GObject parent;
	SuoraEventhandlerChannelPrivate *priv;
};

struct _SuoraEventhandlerChannelClass {
	GObjectClass parent_class;
};

struct _SuoraEventhandlerChannelPrivate {
	guint event_source_id;
	gboolean running;
	guint sum;
};

enum {
	KEY_PRESS,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

G_DEFINE_TYPE(SuoraEventhandlerChannel, suora_eventhandler_channel, G_TYPE_OBJECT);

static void debug_unknown_event(guchar const *event, gsize size) {
	gchar *string = roccat_data8_to_string(event, size);
	g_debug(_("Got unknown event: %s"), string);
	g_free(string);
}

static void process_keyboard_event(SuoraEventhandlerChannel *eventhandler_channel, gchar event[SUORA_KEYBOARD_EVENT_SIZE]) {
	SuoraEventhandlerChannelPrivate *priv = eventhandler_channel->priv;
	guint sum;

	sum = roccat_calc_bytesum(event, SUORA_KEYBOARD_EVENT_SIZE);
	if (sum > priv->sum)
		g_signal_emit((gpointer)eventhandler_channel, signals[KEY_PRESS], 0);
	priv->sum = sum;
}

static gboolean io_cb(GIOChannel *channel, GIOCondition condition, gpointer user_data) {
	SuoraEventhandlerChannel *eventhandler_channel = SUORA_EVENTHANDLER_CHANNEL(user_data);
	GIOStatus status;
	GError *error = NULL;
	gsize length;
	gchar event[SUORA_KEYBOARD_EVENT_SIZE];

	if (condition & G_IO_HUP)
		return FALSE;

	status = g_io_channel_read_chars(channel, (gchar *)&event, SUORA_KEYBOARD_EVENT_SIZE, &length, &error);

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

	if (length == SUORA_KEYBOARD_EVENT_SIZE)
		process_keyboard_event(eventhandler_channel, event);
	else
		debug_unknown_event((guchar const *)&event, length);

	return TRUE; // keep on going
}

SuoraEventhandlerChannel *suora_eventhandler_channel_new(void) {
	return SUORA_EVENTHANDLER_CHANNEL(g_object_new(SUORA_EVENTHANDLER_CHANNEL_TYPE, NULL));
}

static void suora_eventhandler_channel_init(SuoraEventhandlerChannel *channel) {
	SuoraEventhandlerChannelPrivate *priv = SUORA_EVENTHANDLER_CHANNEL_GET_PRIVATE(channel);
	channel->priv = priv;
	priv->running = FALSE;
}

gboolean suora_eventhandler_channel_start(SuoraEventhandlerChannel *eventhandler_channel, RoccatDevice *device, GError **error) {
	GError *local_error = NULL;
	SuoraEventhandlerChannelPrivate *priv = eventhandler_channel->priv;
	priv->sum = 0;
	priv->event_source_id = roccat_device_hidraw_add_event_watch(device, SUORA_INTERFACE_KEYBOARD, io_cb, eventhandler_channel, &local_error);
	if (local_error) {
		g_propagate_error(error, local_error);
		priv->running = FALSE;
		return FALSE;
	} else {
		priv->running = TRUE;
		return TRUE;
	}
}

void suora_eventhandler_channel_stop(SuoraEventhandlerChannel *eventhandler_channel) {
	SuoraEventhandlerChannelPrivate *priv = eventhandler_channel->priv;
	if (priv->running) {
		g_source_remove(priv->event_source_id);
		priv->running = FALSE;
	}
}

static void finalize(GObject *object) {
	SuoraEventhandlerChannel *eventhandler_channel = SUORA_EVENTHANDLER_CHANNEL(object);
	suora_eventhandler_channel_stop(eventhandler_channel);
	G_OBJECT_CLASS(suora_eventhandler_channel_parent_class)->finalize(object);
}

static void suora_eventhandler_channel_class_init(SuoraEventhandlerChannelClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = (GObjectClass *)klass;

	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(SuoraEventhandlerChannelPrivate));

	signals[KEY_PRESS] = g_signal_new("key-press", SUORA_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE,
			0);
}
