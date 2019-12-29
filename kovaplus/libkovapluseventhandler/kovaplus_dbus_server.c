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
#include "kovaplus_dbus_server.h"
#include "g_cclosure_roccat_marshaller.h"
#include "kovaplus.h"
#include "i18n-lib.h"

static gboolean kovaplus_dbus_server_cb_open_gui(KovaplusDBusServer *object, GError **error);
static gboolean kovaplus_dbus_server_cb_profile_changed_outside(KovaplusDBusServer *object, guchar number, GError **error);
static gboolean kovaplus_dbus_server_cb_profile_data_changed_outside(KovaplusDBusServer *object, guchar number, GError **error);
static gboolean kovaplus_dbus_server_cb_configuration_changed_outside(KovaplusDBusServer *object, GError **error);

#include "kovaplus_dbus_server_glue.h"

#define KOVAPLUS_DBUS_SERVER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), TYPE_KOVAPLUS_DBUS_SERVER, KovaplusDBusServerClass))
#define IS_KOVAPLUS_DBUS_SERVER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), TYPE_KOVAPLUS_DBUS_SERVER))
#define KOVAPLUS_DBUS_SERVER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), TYPE_KOVAPLUS_DBUS_SERVER, KovaplusDBusServerClass))

typedef struct _KovaplusDBusServerClass KovaplusDBusServerClass;

struct _KovaplusDBusServerClass {
	GObjectClass parent;
};

G_DEFINE_TYPE(KovaplusDBusServer, kovaplus_dbus_server, G_TYPE_OBJECT);

enum {
	OPEN_GUI,
	PROFILE_CHANGED_OUTSIDE,
	PROFILE_DATA_CHANGED_OUTSIDE,
	CONFIGURATION_CHANGED_OUTSIDE,
	PROFILE_CHANGED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

static void kovaplus_dbus_server_finalize(GObject *object) {
	G_OBJECT_CLASS(kovaplus_dbus_server_parent_class)->finalize(object);
}

static void kovaplus_dbus_server_class_init(KovaplusDBusServerClass *klass) {
	GObjectClass *object_class;
	object_class = G_OBJECT_CLASS(klass);
	object_class->finalize = kovaplus_dbus_server_finalize;

	dbus_g_object_type_install_info(TYPE_KOVAPLUS_DBUS_SERVER, &dbus_glib_server_object_info);

	signals[OPEN_GUI] = g_signal_new("open-gui", TYPE_KOVAPLUS_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE,
			0);

	signals[PROFILE_CHANGED_OUTSIDE] = g_signal_new("profile-changed-outside", TYPE_KOVAPLUS_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[PROFILE_DATA_CHANGED_OUTSIDE] = g_signal_new("profile-data-changed-outside", TYPE_KOVAPLUS_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[CONFIGURATION_CHANGED_OUTSIDE] = g_signal_new("configuration-changed-outside", TYPE_KOVAPLUS_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE,
			0);

	signals[PROFILE_CHANGED] = g_signal_new("profile-changed", TYPE_KOVAPLUS_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);
}

static void kovaplus_dbus_server_init(KovaplusDBusServer *object) {}

KovaplusDBusServer *kovaplus_dbus_server_new(void) {
	return g_object_new(TYPE_KOVAPLUS_DBUS_SERVER, NULL);
}

static gboolean kovaplus_dbus_server_cb_open_gui(KovaplusDBusServer *object, GError **error) {
	g_signal_emit((gpointer)object, signals[OPEN_GUI], 0);
	return TRUE;
}

static gboolean kovaplus_dbus_server_cb_profile_changed_outside(KovaplusDBusServer *object, guchar number, GError **error) {
	g_signal_emit((gpointer)object, signals[PROFILE_CHANGED_OUTSIDE], 0, number);
	return TRUE;
}

static gboolean kovaplus_dbus_server_cb_profile_data_changed_outside(KovaplusDBusServer *object, guchar number, GError **error) {
	g_signal_emit((gpointer)object, signals[PROFILE_DATA_CHANGED_OUTSIDE], 0, number);
	return TRUE;
}

static gboolean kovaplus_dbus_server_cb_configuration_changed_outside(KovaplusDBusServer *object, GError **error) {
	g_signal_emit((gpointer)object, signals[CONFIGURATION_CHANGED_OUTSIDE], 0);
	return TRUE;
}

void kovaplus_dbus_server_emit_profile_changed(KovaplusDBusServer *object, guchar number) {
	g_signal_emit((gpointer)object, signals[PROFILE_CHANGED], 0, number);
}

gboolean kovaplus_dbus_server_connect(KovaplusDBusServer *dbus_server) {
	DBusGConnection *connection;
	GError *error = NULL;

	connection = dbus_g_bus_get(DBUS_BUS_SESSION, &error);
	if (!connection) {
		g_warning(_("Could not get dbus: %s"), error->message);
		return FALSE;
	}

	dbus_g_connection_register_g_object(connection, KOVAPLUS_DBUS_SERVER_PATH, (GObject *)dbus_server);
	dbus_g_connection_unref(connection);

	return TRUE;
}
