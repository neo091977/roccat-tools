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
#include "sova_dbus_server.h"
#include "g_cclosure_roccat_marshaller.h"
#include "sova_dbus_services.h"
#include "i18n-lib.h"

static gboolean sova_dbus_server_cb_talk_easyshift(SovaDBusServer *object, guchar state, GError **error);
static gboolean sova_dbus_server_cb_talk_easyshift_lock(SovaDBusServer *object, guchar state, GError **error);
static gboolean sova_dbus_server_cb_open_gui(SovaDBusServer *object, GError **error);
static gboolean sova_dbus_server_cb_profile_changed_outside(SovaDBusServer *object, guchar profile_index, GError **error);
static gboolean sova_dbus_server_cb_profile_data_changed_outside(SovaDBusServer *object, guchar profile_index, GError **error);
static gboolean sova_dbus_server_cb_configuration_changed_outside(SovaDBusServer *object, GError **error);

#include "sova_dbus_server_glue.h"

#define SOVA_DBUS_SERVER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), TYPE_SOVA_DBUS_SERVER, SovaDBusServerClass))
#define IS_SOVA_DBUS_SERVER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), TYPE_SOVA_DBUS_SERVER))
#define SOVA_DBUS_SERVER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), TYPE_SOVA_DBUS_SERVER, SovaDBusServerClass))

typedef struct _SovaDBusServerClass SovaDBusServerClass;

struct _SovaDBusServerClass {
	GObjectClass parent;
};

G_DEFINE_TYPE(SovaDBusServer, sova_dbus_server, G_TYPE_OBJECT);

enum {
	TALK_EASYSHIFT,
	TALK_EASYSHIFT_LOCK,
	OPEN_GUI,
	PROFILE_CHANGED_OUTSIDE,
	PROFILE_DATA_CHANGED_OUTSIDE,
	CONFIGURATION_CHANGED_OUTSIDE,
	PROFILE_CHANGED,
	BRIGHTNESS_CHANGED,
	MACRO_CHANGED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

static void sova_dbus_server_finalize(GObject *object) {
	G_OBJECT_CLASS(sova_dbus_server_parent_class)->finalize(object);
}

static void sova_dbus_server_class_init(SovaDBusServerClass *klass) {
	GObjectClass *object_class;
	object_class = G_OBJECT_CLASS(klass);
	object_class->finalize = sova_dbus_server_finalize;

	dbus_g_object_type_install_info(TYPE_SOVA_DBUS_SERVER, &dbus_glib_server_object_info);

	signals[TALK_EASYSHIFT] = g_signal_new("talk-easyshift", TYPE_SOVA_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[TALK_EASYSHIFT_LOCK] = g_signal_new("talk-easyshift-lock", TYPE_SOVA_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[OPEN_GUI] = g_signal_new("open-gui", TYPE_SOVA_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE,
			0);

	signals[PROFILE_CHANGED_OUTSIDE] = g_signal_new("profile-changed-outside", TYPE_SOVA_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[PROFILE_DATA_CHANGED_OUTSIDE] = g_signal_new("profile-data-changed-outside", TYPE_SOVA_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[CONFIGURATION_CHANGED_OUTSIDE] = g_signal_new("configuration-changed-outside", TYPE_SOVA_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE,
			0);

	signals[PROFILE_CHANGED] = g_signal_new("profile-changed", TYPE_SOVA_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[BRIGHTNESS_CHANGED] = g_signal_new("brightness-changed", TYPE_SOVA_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_VOID__UCHAR_UCHAR, G_TYPE_NONE,
			2, G_TYPE_UCHAR, G_TYPE_UCHAR);

	signals[MACRO_CHANGED] = g_signal_new("macro-changed", TYPE_SOVA_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_VOID__UCHAR_UCHAR_STRING, G_TYPE_NONE,
			3, G_TYPE_UCHAR, G_TYPE_UCHAR, G_TYPE_STRING);
}

static void sova_dbus_server_init(SovaDBusServer *object) {}

SovaDBusServer *sova_dbus_server_new(void) {
	return g_object_new(TYPE_SOVA_DBUS_SERVER, NULL);
}

static gboolean sova_dbus_server_cb_talk_easyshift(SovaDBusServer *object, guchar state, GError **error) {
	g_signal_emit((gpointer)object, signals[TALK_EASYSHIFT], 0, state);
	return TRUE;
}

static gboolean sova_dbus_server_cb_talk_easyshift_lock(SovaDBusServer *object, guchar state, GError **error) {
	g_signal_emit((gpointer)object, signals[TALK_EASYSHIFT_LOCK], 0, state);
	return TRUE;
}

static gboolean sova_dbus_server_cb_open_gui(SovaDBusServer *object, GError **error) {
	g_signal_emit((gpointer)object, signals[OPEN_GUI], 0);
	return TRUE;
}

static gboolean sova_dbus_server_cb_profile_changed_outside(SovaDBusServer *object, guchar profile_index, GError **error) {
	g_signal_emit((gpointer)object, signals[PROFILE_CHANGED_OUTSIDE], 0, profile_index);
	return TRUE;
}

static gboolean sova_dbus_server_cb_profile_data_changed_outside(SovaDBusServer *object, guchar profile_index, GError **error) {
	g_signal_emit((gpointer)object, signals[PROFILE_DATA_CHANGED_OUTSIDE], 0, profile_index);
	return TRUE;
}

static gboolean sova_dbus_server_cb_configuration_changed_outside(SovaDBusServer *object, GError **error) {
	g_signal_emit((gpointer)object, signals[CONFIGURATION_CHANGED_OUTSIDE], 0);
	return TRUE;
}

void sova_dbus_server_emit_brightness_changed(SovaDBusServer *object, guchar profile_number, guchar brightness_number) {
	g_signal_emit((gpointer)object, signals[BRIGHTNESS_CHANGED], 0, profile_number, brightness_number);
}

void sova_dbus_server_emit_macro_changed(SovaDBusServer *object, guchar profile_number, guchar button_number, SovaMacro const *macro) {
	gchar *macro_encoded;

	macro_encoded = g_base64_encode((guchar const *)macro, sizeof(SovaMacro));
	g_signal_emit((gpointer)object, signals[MACRO_CHANGED], 0, profile_number, button_number, macro_encoded);
	g_free(macro_encoded);
}

void sova_dbus_server_emit_profile_changed(SovaDBusServer *object, guchar profile_number) {
	g_signal_emit((gpointer)object, signals[PROFILE_CHANGED], 0, profile_number);
}

gboolean sova_dbus_server_connect(SovaDBusServer *dbus_server) {
	DBusGConnection *connection;
	GError *error = NULL;

	connection = dbus_g_bus_get(DBUS_BUS_SESSION, &error);
	if (!connection) {
		g_warning(_("Could not get dbus: %s"), error->message);
		return FALSE;
	}

	dbus_g_connection_register_g_object(connection, SOVA_DBUS_SERVER_PATH, (GObject *)dbus_server);
	dbus_g_connection_unref(connection);

	return TRUE;
}
