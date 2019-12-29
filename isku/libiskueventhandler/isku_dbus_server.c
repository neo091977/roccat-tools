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
#include "isku_dbus_server.h"
#include "g_cclosure_roccat_marshaller.h"
#include "isku.h"
#include "i18n-lib.h"

static gboolean isku_dbus_server_cb_talk_easyshift(IskuDBusServer *object, guchar state, GError **error);
static gboolean isku_dbus_server_cb_talk_easyshift_lock(IskuDBusServer *object, guchar state, GError **error);
static gboolean isku_dbus_server_cb_open_gui(IskuDBusServer *object, GError **error);
static gboolean isku_dbus_server_cb_profile_changed_outside(IskuDBusServer *object, guchar number, GError **error);
static gboolean isku_dbus_server_cb_profile_data_changed_outside(IskuDBusServer *object, guchar number, GError **error);
static gboolean isku_dbus_server_cb_configuration_changed_outside(IskuDBusServer *object, GError **error);

#include "isku_dbus_server_glue.h"

#define ISKU_DBUS_SERVER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), TYPE_ISKU_DBUS_SERVER, IskuDBusServerClass))
#define IS_ISKU_DBUS_SERVER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), TYPE_ISKU_DBUS_SERVER))
#define ISKU_DBUS_SERVER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), TYPE_ISKU_DBUS_SERVER, IskuDBusServerClass))

typedef struct _IskuDBusServerClass IskuDBusServerClass;

struct _IskuDBusServerClass {
	GObjectClass parent;
};

G_DEFINE_TYPE(IskuDBusServer, isku_dbus_server, G_TYPE_OBJECT);

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

static void isku_dbus_server_finalize(GObject *object) {
	G_OBJECT_CLASS(isku_dbus_server_parent_class)->finalize(object);
}

static void isku_dbus_server_class_init(IskuDBusServerClass *klass) {
	GObjectClass *object_class;
	object_class = G_OBJECT_CLASS(klass);
	object_class->finalize = isku_dbus_server_finalize;

	dbus_g_object_type_install_info(TYPE_ISKU_DBUS_SERVER, &dbus_glib_server_object_info);

	signals[TALK_EASYSHIFT] = g_signal_new("talk-easyshift", TYPE_ISKU_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[TALK_EASYSHIFT_LOCK] = g_signal_new("talk-easyshift-lock", TYPE_ISKU_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[OPEN_GUI] = g_signal_new("open-gui", TYPE_ISKU_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE,
			0);

	signals[PROFILE_CHANGED_OUTSIDE] = g_signal_new("profile-changed-outside", TYPE_ISKU_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[PROFILE_DATA_CHANGED_OUTSIDE] = g_signal_new("profile-data-changed-outside", TYPE_ISKU_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[CONFIGURATION_CHANGED_OUTSIDE] = g_signal_new("configuration-changed-outside", TYPE_ISKU_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE,
			0);

	signals[PROFILE_CHANGED] = g_signal_new("profile-changed", TYPE_ISKU_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[BRIGHTNESS_CHANGED] = g_signal_new("brightness-changed", TYPE_ISKU_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_VOID__UCHAR_UCHAR, G_TYPE_NONE,
			2, G_TYPE_UCHAR, G_TYPE_UCHAR);

	signals[MACRO_CHANGED] = g_signal_new("macro-changed", TYPE_ISKU_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_VOID__UCHAR_UCHAR_STRING, G_TYPE_NONE,
			3, G_TYPE_UCHAR, G_TYPE_UCHAR, G_TYPE_STRING);
}

static void isku_dbus_server_init(IskuDBusServer *object) {}

IskuDBusServer *isku_dbus_server_new(void) {
	return g_object_new(TYPE_ISKU_DBUS_SERVER, NULL);
}

static gboolean isku_dbus_server_cb_talk_easyshift(IskuDBusServer *object, guchar state, GError **error) {
	g_signal_emit((gpointer)object, signals[TALK_EASYSHIFT], 0, state);
	return TRUE;
}

static gboolean isku_dbus_server_cb_talk_easyshift_lock(IskuDBusServer *object, guchar state, GError **error) {
	g_signal_emit((gpointer)object, signals[TALK_EASYSHIFT_LOCK], 0, state);
	return TRUE;
}

static gboolean isku_dbus_server_cb_open_gui(IskuDBusServer *object, GError **error) {
	g_signal_emit((gpointer)object, signals[OPEN_GUI], 0);
	return TRUE;
}

static gboolean isku_dbus_server_cb_profile_changed_outside(IskuDBusServer *object, guchar number, GError **error) {
	g_signal_emit((gpointer)object, signals[PROFILE_CHANGED_OUTSIDE], 0, number);
	return TRUE;
}

static gboolean isku_dbus_server_cb_profile_data_changed_outside(IskuDBusServer *object, guchar number, GError **error) {
	g_signal_emit((gpointer)object, signals[PROFILE_DATA_CHANGED_OUTSIDE], 0, number);
	return TRUE;
}

static gboolean isku_dbus_server_cb_configuration_changed_outside(IskuDBusServer *object, GError **error) {
	g_signal_emit((gpointer)object, signals[CONFIGURATION_CHANGED_OUTSIDE], 0);
	return TRUE;
}

void isku_dbus_server_emit_brightness_changed(IskuDBusServer *object, guchar profile_number, guchar brightness_number) {
	g_signal_emit((gpointer)object, signals[BRIGHTNESS_CHANGED], 0, profile_number, brightness_number);
}

void isku_dbus_server_emit_macro_changed(IskuDBusServer *object, guchar profile_number, guchar button_number, IskuRkpMacroKeyInfo const *key_info) {
	gchar *key_info_encoded;

	key_info_encoded = g_base64_encode((guchar const *)key_info, sizeof(IskuRkpMacroKeyInfo));
	g_signal_emit((gpointer)object, signals[MACRO_CHANGED], 0, profile_number, button_number, key_info_encoded);
	g_free(key_info_encoded);
}

void isku_dbus_server_emit_profile_changed(IskuDBusServer *object, guchar profile_number) {
	g_signal_emit((gpointer)object, signals[PROFILE_CHANGED], 0, profile_number);
}

gboolean isku_dbus_server_connect(IskuDBusServer *dbus_server) {
	DBusGConnection *connection;
	GError *error = NULL;

	connection = dbus_g_bus_get(DBUS_BUS_SESSION, &error);
	if (!connection) {
		g_warning(_("Could not get dbus: %s"), error->message);
		return FALSE;
	}

	dbus_g_connection_register_g_object(connection, ISKU_DBUS_SERVER_PATH, (GObject *)dbus_server);
	dbus_g_connection_unref(connection);

	return TRUE;
}
