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

#include "arvo_eventhandler_channel.h"
#include "g_cclosure_roccat_marshaller.h"
#include "roccat_device_hidraw.h"
#include "roccat.h"
#include "arvo.h"
#include "roccat_helper.h"
#include "i18n-lib.h"

#define ARVO_EVENTHANDLER_CHANNEL_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ARVO_EVENTHANDLER_CHANNEL_TYPE, ArvoEventhandlerChannelClass))
#define IS_ARVO_EVENTHANDLER_CHANNEL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ARVO_EVENTHANDLER_CHANNEL_TYPE))
#define ARVO_EVENTHANDLER_CHANNEL_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ARVO_EVENTHANDLER_CHANNEL_TYPE, ArvoEventhandlerChannelPrivate))

typedef struct _ArvoEventhandlerChannelClass ArvoEventhandlerChannelClass;
typedef struct _ArvoEventhandlerChannelPrivate ArvoEventhandlerChannelPrivate;

struct _ArvoEventhandlerChannel {
	GObject parent;
	ArvoEventhandlerChannelPrivate *priv;
};

struct _ArvoEventhandlerChannelClass {
	GObjectClass parent_class;
};

struct _ArvoEventhandlerChannelPrivate {
	guint event_source_id;
	gboolean running;
};

enum {
	BUTTON,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

G_DEFINE_TYPE(ArvoEventhandlerChannel, arvo_eventhandler_channel, G_TYPE_OBJECT);

static void process_chrdev_event(ArvoEventhandlerChannel *eventhandler_channel, ArvoSpecial const *event) {
	gchar *string;
	guint button;
	guint action;

	button = event->event & ARVO_SPECIAL_EVENT_MASK_BUTTON;
	action = event->event & ARVO_SPECIAL_EVENT_MASK_ACTION;

	switch (button) {
	case ARVO_SPECIAL_BUTTON_MODE_ON:
		break;
	case ARVO_SPECIAL_BUTTON_MODE_OFF:
		break;
	case ARVO_SPECIAL_BUTTON_1:
	case ARVO_SPECIAL_BUTTON_2:
	case ARVO_SPECIAL_BUTTON_3:
	case ARVO_SPECIAL_BUTTON_4:
	case ARVO_SPECIAL_BUTTON_5:
		g_signal_emit((gpointer)eventhandler_channel, signals[BUTTON], 0, action, button);
		break;
	default:
		string = roccat_data8_to_string((guchar const *)event, sizeof(ArvoSpecial));
		g_debug(_("Got unknown event: %s"), string);
		g_free(string);
	}
}

static gboolean io_cb(GIOChannel *channel, GIOCondition condition, gpointer user_data) {
	ArvoEventhandlerChannel *eventhandler_channel = ARVO_EVENTHANDLER_CHANNEL(user_data);
	ArvoSpecial event;
	GIOStatus status;
	GError *error = NULL;
	gsize length;

	if (condition & G_IO_HUP)
		return FALSE;

	status = g_io_channel_read_chars(channel, (gchar *)&event, sizeof(ArvoSpecial), &length, &error);

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

	if (length < sizeof(ArvoSpecial))
		return TRUE;

	process_chrdev_event(eventhandler_channel, &event);

	return TRUE; // keep on going
}

ArvoEventhandlerChannel *arvo_eventhandler_channel_new(void) {
	return ARVO_EVENTHANDLER_CHANNEL(g_object_new(ARVO_EVENTHANDLER_CHANNEL_TYPE, NULL));
}

static void arvo_eventhandler_channel_init(ArvoEventhandlerChannel *channel) {
	ArvoEventhandlerChannelPrivate *priv = ARVO_EVENTHANDLER_CHANNEL_GET_PRIVATE(channel);
	channel->priv = priv;
	priv->running = FALSE;
}

gboolean arvo_eventhandler_channel_start(ArvoEventhandlerChannel *eventhandler_channel, RoccatDevice *device, GError **error) {
	GError *local_error = NULL;
	ArvoEventhandlerChannelPrivate *priv = eventhandler_channel->priv;
	priv->event_source_id = roccat_device_hidraw_add_event_watch(device, ARVO_INTERFACE_MOUSE, io_cb, eventhandler_channel, &local_error);
	if (local_error) {
		g_propagate_error(error, local_error);
		priv->running = FALSE;
		return FALSE;
	} else {
		priv->running = TRUE;
		return TRUE;
	}
}

void arvo_eventhandler_channel_stop(ArvoEventhandlerChannel *eventhandler_channel) {
	ArvoEventhandlerChannelPrivate *priv = eventhandler_channel->priv;
	if (priv->running) {
		g_source_remove(priv->event_source_id);
		priv->running = FALSE;
	}
}

static void finalize(GObject *object) {
	ArvoEventhandlerChannel *eventhandler_channel = ARVO_EVENTHANDLER_CHANNEL(object);
	arvo_eventhandler_channel_stop(eventhandler_channel);
	G_OBJECT_CLASS(arvo_eventhandler_channel_parent_class)->finalize(object);
}

static void arvo_eventhandler_channel_class_init(ArvoEventhandlerChannelClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = (GObjectClass *)klass;

	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(ArvoEventhandlerChannelPrivate));
	
	signals[BUTTON] = g_signal_new("button", ARVO_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_VOID__UCHAR_UCHAR, G_TYPE_NONE,
			2, G_TYPE_UCHAR, G_TYPE_UCHAR);
}
