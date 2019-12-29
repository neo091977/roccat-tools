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

/* keep inclusion order of following two headers */
#include "kone_dbus_server.h"
#include "g_cclosure_roccat_marshaller.h"
#include "kone.h"
#include "i18n-lib.h"

static gboolean kone_dbus_server_cb_open_gui(KoneDBusServer *object, GError **error);
static gboolean kone_dbus_server_cb_profile_changed_outside(KoneDBusServer *object, guchar number, GError **error);
static gboolean kone_dbus_server_cb_profile_data_changed_outside(KoneDBusServer *object, guchar number, GError **error);
static gboolean kone_dbus_server_cb_configuration_changed_outside(KoneDBusServer *object, GError **error);

#include "kone_dbus_server_glue.h"

#define KONE_DBUS_SERVER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), TYPE_KONE_DBUS_SERVER, KoneDBusServerClass))
#define IS_KONE_DBUS_SERVER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), TYPE_KONE_DBUS_SERVER))
#define KONE_DBUS_SERVER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), TYPE_KONE_DBUS_SERVER, KoneDBusServerClass))

typedef struct _KoneDBusServerClass KoneDBusServerClass;

struct _KoneDBusServerClass {
	GObjectClass parent;
};

G_DEFINE_TYPE(KoneDBusServer, kone_dbus_server, G_TYPE_OBJECT);

enum {
	OPEN_GUI,
	PROFILE_CHANGED_OUTSIDE,
	PROFILE_DATA_CHANGED_OUTSIDE,
	CONFIGURATION_CHANGED_OUTSIDE,
	PROFILE_CHANGED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

static void kone_dbus_server_finalize(GObject *object) {
	G_OBJECT_CLASS(kone_dbus_server_parent_class)->finalize(object);
}

static void kone_dbus_server_class_init(KoneDBusServerClass *klass) {
	GObjectClass *object_class;
	object_class = G_OBJECT_CLASS(klass);
	object_class->finalize = kone_dbus_server_finalize;

	dbus_g_object_type_install_info(TYPE_KONE_DBUS_SERVER, &dbus_glib_server_object_info);

	signals[OPEN_GUI] = g_signal_new("open-gui", TYPE_KONE_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE,
			0);

	signals[PROFILE_CHANGED_OUTSIDE] = g_signal_new("profile-changed-outside", TYPE_KONE_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[PROFILE_DATA_CHANGED_OUTSIDE] = g_signal_new("profile-data-changed-outside", TYPE_KONE_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[CONFIGURATION_CHANGED_OUTSIDE] = g_signal_new("configuration-changed-outside", TYPE_KONE_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE,
			0);

	signals[PROFILE_CHANGED] = g_signal_new("profile-changed", TYPE_KONE_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);
}

static void kone_dbus_server_init(KoneDBusServer *object) {}

KoneDBusServer *kone_dbus_server_new(void) {
	return g_object_new(TYPE_KONE_DBUS_SERVER, NULL);
}

static gboolean kone_dbus_server_cb_open_gui(KoneDBusServer *object, GError **error) {
	g_signal_emit((gpointer)object, signals[OPEN_GUI], 0);
	return TRUE;
}

static gboolean kone_dbus_server_cb_profile_changed_outside(KoneDBusServer *object, guchar number, GError **error) {
	g_signal_emit((gpointer)object, signals[PROFILE_CHANGED_OUTSIDE], 0, number);
	return TRUE;
}

static gboolean kone_dbus_server_cb_profile_data_changed_outside(KoneDBusServer *object, guchar number, GError **error) {
	g_signal_emit((gpointer)object, signals[PROFILE_DATA_CHANGED_OUTSIDE], 0, number);
	return TRUE;
}

static gboolean kone_dbus_server_cb_configuration_changed_outside(KoneDBusServer *object, GError **error) {
	g_signal_emit((gpointer)object, signals[CONFIGURATION_CHANGED_OUTSIDE], 0);
	return TRUE;
}

void kone_dbus_server_emit_profile_changed(KoneDBusServer *object, guchar number) {
	g_signal_emit((gpointer)object, signals[PROFILE_CHANGED], 0, number);
}

gboolean kone_dbus_server_connect(KoneDBusServer *dbus_server) {
	DBusGConnection *connection;
	GError *error = NULL;

	connection = dbus_g_bus_get(DBUS_BUS_SESSION, &error);
	if (!connection) {
		g_warning(_("Could not get dbus: %s"), error->message);
		return FALSE;
	}

	dbus_g_connection_register_g_object(connection, KONE_DBUS_SERVER_PATH, (GObject *)dbus_server);
	dbus_g_connection_unref(connection);

	return TRUE;
}
