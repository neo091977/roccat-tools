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

#include "kone_eventhandler_channel.h"
#include "g_cclosure_roccat_marshaller.h"
#include "kone.h"
#include "roccat.h"
#include "roccat_helper.h"
#include "i18n-lib.h"
#include <sys/ioctl.h>

#define KONE_EVENTHANDLER_CHANNEL_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KONE_EVENTHANDLER_CHANNEL_TYPE, KoneEventhandlerChannelClass))
#define IS_KONE_EVENTHANDLER_CHANNEL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KONE_EVENTHANDLER_CHANNEL_TYPE))
#define KONE_EVENTHANDLER_CHANNEL_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KONE_EVENTHANDLER_CHANNEL_TYPE, KoneEventhandlerChannelPrivate))

typedef struct _KoneEventhandlerChannelClass KoneEventhandlerChannelClass;
typedef struct _KoneEventhandlerChannelPrivate KoneEventhandlerChannelPrivate;

struct _KoneEventhandlerChannel {
	GObject parent;
	KoneEventhandlerChannelPrivate *priv;
};

struct _KoneEventhandlerChannelClass {
	GObjectClass parent_class;
};

struct _KoneEventhandlerChannelPrivate {
	guint event_source_id;
	guint report_size;
	gboolean running;
};

enum {
	CPI_CHANGED, /* raw cpi */
	CPI_OSD, /* raw cpi */
	PROFILE_CHANGED, /* profile_number */
	PROFILE_OSD, /* profile_number */
	MACRO, /* key_number */
	MULTIMEDIA, /* key_type */
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

#define ROCCATIOCGREPSIZE _IOR('H', 0xf1, int)

G_DEFINE_TYPE(KoneEventhandlerChannel, kone_eventhandler_channel, G_TYPE_OBJECT);

static void process_chrdev_event(KoneEventhandlerChannel *eventhandler_channel, KoneSpecial const *event) {
	gchar *string;

	switch (event->type) {
	case KONE_SPECIAL_TYPE_SWITCH_DPI:
		g_signal_emit((gpointer)eventhandler_channel, signals[CPI_CHANGED], 0, event->value);
		break;
	case KONE_SPECIAL_TYPE_OSD_DPI:
		g_signal_emit((gpointer)eventhandler_channel, signals[CPI_OSD], 0, event->value);
		break;
	case KONE_SPECIAL_TYPE_SWITCH_PROFILE:
		g_signal_emit((gpointer)eventhandler_channel, signals[PROFILE_CHANGED], 0, event->value);
		break;
	case KONE_SPECIAL_TYPE_OSD_PROFILE:
		g_signal_emit((gpointer)eventhandler_channel, signals[PROFILE_OSD], 0, event->value);
		break;
	case KONE_SPECIAL_TYPE_OVERLONG_MACRO:
		g_signal_emit((gpointer)eventhandler_channel, signals[MACRO], 0, event->key);
		break;
	case KONE_SPECIAL_TYPE_MULTIMEDIA:
		g_signal_emit((gpointer)eventhandler_channel, signals[MULTIMEDIA], 0, event->key);
		break;
	default:
		string = roccat_data8_to_string((guchar const *)event, sizeof(KoneSpecial));
		g_debug(_("Got unknown event: %s"), string);
		g_free(string);
	}
}

static gboolean io_cb(GIOChannel *channel, GIOCondition condition, gpointer user_data) {
	KoneEventhandlerChannel *eventhandler_channel = KONE_EVENTHANDLER_CHANNEL(user_data);
	KoneEventhandlerChannelPrivate *priv = eventhandler_channel->priv;
	gchar *event;
	GIOStatus status;
	GError *error = NULL;
	gsize length;

	if (condition & G_IO_HUP)
		return FALSE;

	event = g_malloc(priv->report_size);

	status = g_io_channel_read_chars(channel, event, priv->report_size, &length, &error);

	if (status == G_IO_STATUS_AGAIN)
		goto ok;

	if (status == G_IO_STATUS_ERROR) {
		g_critical(_("Could not read io_channel: %s"), error->message);
		return FALSE;
	}

	if (status == G_IO_STATUS_EOF) {
		g_warning(_("Could not read io_channel: %s"), _("End of file"));
		return FALSE;
	}

	if (length != priv->report_size) {
		g_critical(_("Report has wrong length, %zu instead of %zu"), length, (gsize)priv->report_size);
		goto error;
	}

	switch (priv->report_size) {
	case sizeof(KoneSpecial):
		process_chrdev_event(eventhandler_channel, (KoneSpecial const *)event);
		break;
	default:
		g_critical(_("Report has wrong length, %zu instead of %zu"), length, (gsize)priv->report_size);
		goto error;
	}

ok:
	g_free(event);
	return TRUE; // keep on going
error:
	g_free(event);
	return FALSE;
}

KoneEventhandlerChannel *kone_eventhandler_channel_new(void) {
	return KONE_EVENTHANDLER_CHANNEL(g_object_new(KONE_EVENTHANDLER_CHANNEL_TYPE, NULL));
}

static void kone_eventhandler_channel_init(KoneEventhandlerChannel *channel) {
	KoneEventhandlerChannelPrivate *priv = KONE_EVENTHANDLER_CHANNEL_GET_PRIVATE(channel);
	channel->priv = priv;
	priv->running = FALSE;
}

gboolean kone_eventhandler_channel_start(KoneEventhandlerChannel *eventhandler_channel, gchar const *path, GError **error) {
	KoneEventhandlerChannelPrivate *priv = eventhandler_channel->priv;
	GIOChannel *channel;
	GIOStatus status;
	int fd;
	
	channel = g_io_channel_new_file(path, "r", error);
	if (!channel)
		return FALSE;

	status = g_io_channel_set_encoding(channel, NULL, error);
	if (status != G_IO_STATUS_NORMAL) {
		g_io_channel_unref(channel);
		return FALSE;
	}

	g_io_channel_set_buffered(channel, FALSE);

	fd = g_io_channel_unix_get_fd(channel);
	if (ioctl(fd, ROCCATIOCGREPSIZE, &priv->report_size) == -1)
		priv->report_size = sizeof(KoneSpecial);

	if (priv->report_size != sizeof(KoneSpecial)) {
		g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_FAILED, _("Ioctl reported unsupported event size %i"), priv->report_size);
		g_io_channel_unref(channel);
		return FALSE;
	}

	priv->event_source_id = g_io_add_watch(channel, G_IO_IN | G_IO_HUP | G_IO_PRI, io_cb, eventhandler_channel);
	/* g_io_add_watch increases reference count
	 * this passes ownership of the channel to the watch */
	g_io_channel_unref(channel);

	return TRUE;
}

void kone_eventhandler_channel_stop(KoneEventhandlerChannel *eventhandler_channel) {
	KoneEventhandlerChannelPrivate *priv = eventhandler_channel->priv;
	if (priv->running) {
		g_source_remove(priv->event_source_id);
		priv->running = FALSE;
	}
}

static void finalize(GObject *object) {
	KoneEventhandlerChannel *eventhandler_channel = KONE_EVENTHANDLER_CHANNEL(object);
	kone_eventhandler_channel_stop(eventhandler_channel);
	G_OBJECT_CLASS(kone_eventhandler_channel_parent_class)->finalize(object);
}

static void kone_eventhandler_channel_class_init(KoneEventhandlerChannelClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = (GObjectClass *)klass;

	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(KoneEventhandlerChannelPrivate));
	
	signals[CPI_CHANGED] = g_signal_new("cpi-changed", KONE_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[CPI_OSD] = g_signal_new("cpi-osd", KONE_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[PROFILE_CHANGED] = g_signal_new("profile-changed", KONE_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[PROFILE_OSD] = g_signal_new("profile-osd", KONE_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[MACRO] = g_signal_new("macro", KONE_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[MULTIMEDIA] = g_signal_new("multimedia", KONE_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);
}
