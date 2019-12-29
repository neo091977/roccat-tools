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

#include "pyra_eventhandler_channel.h"
#include "g_cclosure_roccat_marshaller.h"
#include "pyra.h"
#include "roccat_device_hidraw.h"
#include "roccat.h"
#include "roccat_helper.h"
#include "i18n-lib.h"

#define PYRA_EVENTHANDLER_CHANNEL_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), PYRA_EVENTHANDLER_CHANNEL_TYPE, PyraEventhandlerChannelClass))
#define IS_PYRA_EVENTHANDLER_CHANNEL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), PYRA_EVENTHANDLER_CHANNEL_TYPE))
#define PYRA_EVENTHANDLER_CHANNEL_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), PYRA_EVENTHANDLER_CHANNEL_TYPE, PyraEventhandlerChannelPrivate))

typedef struct _PyraEventhandlerChannelClass PyraEventhandlerChannelClass;
typedef struct _PyraEventhandlerChannelPrivate PyraEventhandlerChannelPrivate;

struct _PyraEventhandlerChannel {
	GObject parent;
	PyraEventhandlerChannelPrivate *priv;
};

struct _PyraEventhandlerChannelClass {
	GObjectClass parent_class;
};

struct _PyraEventhandlerChannelPrivate {
	guint event_source_id;
	gboolean running;
};

enum {
	CPI_CHANGED, /* cpi_index */
	PROFILE_CHANGED, /* profile_number */
	QUICKLAUNCH, /* key_index */
	MACRO,
	SHORTCUT_PRESS,
	SHORTCUT_RELEASE,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

G_DEFINE_TYPE(PyraEventhandlerChannel, pyra_eventhandler_channel, G_TYPE_OBJECT);

static void process_chrdev_event(PyraEventhandlerChannel *eventhandler_channel, PyraSpecial const *event) {
	gchar *string;

	switch (event->type) {
	case PYRA_SPECIAL_TYPE_PROFILE_2:
		g_signal_emit((gpointer)eventhandler_channel, signals[PROFILE_CHANGED], 0, event->data1);
		break;
	case PYRA_SPECIAL_TYPE_CPI:
		g_signal_emit((gpointer)eventhandler_channel, signals[CPI_CHANGED], 0, event->data1);
		break;
	case PYRA_SPECIAL_TYPE_MACRO:
		if (event->data2 == PYRA_SPECIAL_BUTTON_PRESS)
			g_signal_emit((gpointer)eventhandler_channel, signals[MACRO], 0, event->data1);
		break;
	case PYRA_SPECIAL_TYPE_SHORTCUT:
		if (event->data2 == PYRA_SPECIAL_BUTTON_PRESS)
			g_signal_emit((gpointer)eventhandler_channel, signals[SHORTCUT_PRESS], 0, event->data1);
		else
			g_signal_emit((gpointer)eventhandler_channel, signals[SHORTCUT_RELEASE], 0, event->data1);
		break;
	case PYRA_SPECIAL_TYPE_QUICKLAUNCH:
		if (event->data2 == PYRA_SPECIAL_BUTTON_PRESS)
			g_signal_emit((gpointer)eventhandler_channel, signals[QUICKLAUNCH], 0, event->data1);
		break;
	case PYRA_SPECIAL_TYPE_TILT:
	case PYRA_SPECIAL_TYPE_MULTIMEDIA:
	case PYRA_SPECIAL_TYPE_PROFILE_1:
		/* just ignore these events */
		break;
	default:
		string = roccat_data8_to_string((guchar const *)event, sizeof(PyraSpecial));
		g_debug(_("Got unknown event: %s"), string);
		g_free(string);
	}
}

static gboolean io_cb(GIOChannel *channel, GIOCondition condition, gpointer user_data) {
	PyraEventhandlerChannel *eventhandler_channel = PYRA_EVENTHANDLER_CHANNEL(user_data);
	PyraSpecial event;
	GIOStatus status;
	GError *error = NULL;
	gsize length;

	if (condition & G_IO_HUP)
		return FALSE;

	status = g_io_channel_read_chars(channel, (gchar *)&event, sizeof(PyraSpecial), &length, &error);

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

	if (length < sizeof(PyraSpecial))
		return TRUE;

	switch (event.report_id) {
	case PYRA_REPORT_ID_SPECIAL:
		process_chrdev_event(eventhandler_channel, &event);
		break;
	default:
		break;
	}

	return TRUE; // keep on going
}

PyraEventhandlerChannel *pyra_eventhandler_channel_new(void) {
	return PYRA_EVENTHANDLER_CHANNEL(g_object_new(PYRA_EVENTHANDLER_CHANNEL_TYPE, NULL));
}

static void pyra_eventhandler_channel_init(PyraEventhandlerChannel *channel) {
	PyraEventhandlerChannelPrivate *priv = PYRA_EVENTHANDLER_CHANNEL_GET_PRIVATE(channel);
	channel->priv = priv;
	priv->running = FALSE;
}

gboolean pyra_eventhandler_channel_start(PyraEventhandlerChannel *eventhandler_channel, RoccatDevice *device, GError **error) {
	GError *local_error = NULL;
	PyraEventhandlerChannelPrivate *priv = eventhandler_channel->priv;
	priv->event_source_id = roccat_device_hidraw_add_event_watch(device, PYRA_INTERFACE_MOUSE, io_cb, eventhandler_channel, &local_error);
	if (local_error) {
		g_propagate_error(error, local_error);
		priv->running = FALSE;
		return FALSE;
	} else {
		priv->running = TRUE;
		return TRUE;
	}
}

void pyra_eventhandler_channel_stop(PyraEventhandlerChannel *eventhandler_channel) {
	PyraEventhandlerChannelPrivate *priv = eventhandler_channel->priv;
	if (priv->running) {
		g_source_remove(priv->event_source_id);
		priv->running = FALSE;
	}
}

static void finalize(GObject *object) {
	PyraEventhandlerChannel *eventhandler_channel = PYRA_EVENTHANDLER_CHANNEL(object);
	pyra_eventhandler_channel_stop(eventhandler_channel);
	G_OBJECT_CLASS(pyra_eventhandler_channel_parent_class)->finalize(object);
}

static void pyra_eventhandler_channel_class_init(PyraEventhandlerChannelClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = (GObjectClass *)klass;

	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(PyraEventhandlerChannelPrivate));
	
	signals[CPI_CHANGED] = g_signal_new("cpi-changed", PYRA_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[PROFILE_CHANGED] = g_signal_new("profile-changed", PYRA_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[QUICKLAUNCH] = g_signal_new("quicklaunch", PYRA_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[MACRO] = g_signal_new("macro", PYRA_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[SHORTCUT_PRESS] = g_signal_new("shortcut-press", PYRA_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[SHORTCUT_RELEASE] = g_signal_new("shortcut-release", PYRA_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);
}
