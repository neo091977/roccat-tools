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
#include "skeltr_dbus_server.h"
#include "g_cclosure_roccat_marshaller.h"
#include "skeltr_dbus_services.h"
#include "i18n-lib.h"

static gboolean skeltr_dbus_server_cb_talk_easyshift(SkeltrDBusServer *object, guchar state, GError **error);
static gboolean skeltr_dbus_server_cb_talk_easyshift_lock(SkeltrDBusServer *object, guchar state, GError **error);
static gboolean skeltr_dbus_server_cb_open_gui(SkeltrDBusServer *object, GError **error);
static gboolean skeltr_dbus_server_cb_profile_changed_outside(SkeltrDBusServer *object, guchar profile_index, GError **error);
static gboolean skeltr_dbus_server_cb_profile_data_changed_outside(SkeltrDBusServer *object, guchar profile_index, GError **error);
static gboolean skeltr_dbus_server_cb_configuration_changed_outside(SkeltrDBusServer *object, GError **error);
static gboolean skeltr_dbus_server_cb_gfx_set_led_rgb(SkeltrDBusServer *object, guchar index, guint32 color, GError **error);
static gboolean skeltr_dbus_server_cb_gfx_get_led_rgb(SkeltrDBusServer *object, guchar index, guint32 *color, GError **error);
static gboolean skeltr_dbus_server_cb_gfx_update(SkeltrDBusServer *object, GError **error);

#include "skeltr_dbus_server_glue.h"

#define SKELTR_DBUS_SERVER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), TYPE_SKELTR_DBUS_SERVER, SkeltrDBusServerClass))
#define IS_SKELTR_DBUS_SERVER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), TYPE_SKELTR_DBUS_SERVER))
#define SKELTR_DBUS_SERVER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), TYPE_SKELTR_DBUS_SERVER, SkeltrDBusServerClass))

typedef struct _SkeltrDBusServerClass SkeltrDBusServerClass;

struct _SkeltrDBusServerClass {
	GObjectClass parent;
};

G_DEFINE_TYPE(SkeltrDBusServer, skeltr_dbus_server, G_TYPE_OBJECT);

enum {
	TALK_EASYSHIFT,
	TALK_EASYSHIFT_LOCK,
	OPEN_GUI,
	PROFILE_CHANGED_OUTSIDE,
	PROFILE_DATA_CHANGED_OUTSIDE,
	CONFIGURATION_CHANGED_OUTSIDE,
	GFX_SET_LED_RGB,
	GFX_GET_LED_RGB,
	GFX_UPDATE,
	PROFILE_CHANGED,
	BRIGHTNESS_CHANGED,
	MACRO_CHANGED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

static void skeltr_dbus_server_finalize(GObject *object) {
	G_OBJECT_CLASS(skeltr_dbus_server_parent_class)->finalize(object);
}

static void skeltr_dbus_server_class_init(SkeltrDBusServerClass *klass) {
	GObjectClass *object_class;
	object_class = G_OBJECT_CLASS(klass);
	object_class->finalize = skeltr_dbus_server_finalize;

	dbus_g_object_type_install_info(TYPE_SKELTR_DBUS_SERVER, &dbus_glib_server_object_info);

	signals[TALK_EASYSHIFT] = g_signal_new("talk-easyshift", TYPE_SKELTR_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[TALK_EASYSHIFT_LOCK] = g_signal_new("talk-easyshift-lock", TYPE_SKELTR_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[OPEN_GUI] = g_signal_new("open-gui", TYPE_SKELTR_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE,
			0);

	signals[PROFILE_CHANGED_OUTSIDE] = g_signal_new("profile-changed-outside", TYPE_SKELTR_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[PROFILE_DATA_CHANGED_OUTSIDE] = g_signal_new("profile-data-changed-outside", TYPE_SKELTR_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[CONFIGURATION_CHANGED_OUTSIDE] = g_signal_new("configuration-changed-outside", TYPE_SKELTR_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE,
			0);

	signals[GFX_SET_LED_RGB] = g_signal_new("gfx-set-led-rgb", TYPE_SKELTR_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_VOID__UINT_UINT, G_TYPE_NONE,
			2, G_TYPE_UINT, G_TYPE_UINT);

	signals[GFX_GET_LED_RGB] = g_signal_new("gfx-get-led-rgb", TYPE_SKELTR_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_VOID__UINT_POINTER, G_TYPE_NONE,
			2, G_TYPE_UINT, G_TYPE_POINTER);

	signals[GFX_UPDATE] = g_signal_new("gfx-update", TYPE_SKELTR_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE,
			0);

	signals[PROFILE_CHANGED] = g_signal_new("profile-changed", TYPE_SKELTR_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[BRIGHTNESS_CHANGED] = g_signal_new("brightness-changed", TYPE_SKELTR_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_VOID__UCHAR_UCHAR, G_TYPE_NONE,
			2, G_TYPE_UCHAR, G_TYPE_UCHAR);

	signals[MACRO_CHANGED] = g_signal_new("macro-changed", TYPE_SKELTR_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_VOID__UCHAR_UCHAR_STRING, G_TYPE_NONE,
			3, G_TYPE_UCHAR, G_TYPE_UCHAR, G_TYPE_STRING);
}

static void skeltr_dbus_server_init(SkeltrDBusServer *object) {}

SkeltrDBusServer *skeltr_dbus_server_new(void) {
	return g_object_new(TYPE_SKELTR_DBUS_SERVER, NULL);
}

static gboolean skeltr_dbus_server_cb_talk_easyshift(SkeltrDBusServer *object, guchar state, GError **error) {
	g_signal_emit((gpointer)object, signals[TALK_EASYSHIFT], 0, state);
	return TRUE;
}

static gboolean skeltr_dbus_server_cb_talk_easyshift_lock(SkeltrDBusServer *object, guchar state, GError **error) {
	g_signal_emit((gpointer)object, signals[TALK_EASYSHIFT_LOCK], 0, state);
	return TRUE;
}

static gboolean skeltr_dbus_server_cb_open_gui(SkeltrDBusServer *object, GError **error) {
	g_signal_emit((gpointer)object, signals[OPEN_GUI], 0);
	return TRUE;
}

static gboolean skeltr_dbus_server_cb_profile_changed_outside(SkeltrDBusServer *object, guchar profile_index, GError **error) {
	g_signal_emit((gpointer)object, signals[PROFILE_CHANGED_OUTSIDE], 0, profile_index);
	return TRUE;
}

static gboolean skeltr_dbus_server_cb_profile_data_changed_outside(SkeltrDBusServer *object, guchar profile_index, GError **error) {
	g_signal_emit((gpointer)object, signals[PROFILE_DATA_CHANGED_OUTSIDE], 0, profile_index);
	return TRUE;
}

static gboolean skeltr_dbus_server_cb_configuration_changed_outside(SkeltrDBusServer *object, GError **error) {
	g_signal_emit((gpointer)object, signals[CONFIGURATION_CHANGED_OUTSIDE], 0);
	return TRUE;
}

static gboolean skeltr_dbus_server_cb_gfx_set_led_rgb(SkeltrDBusServer *object, guchar index, guint32 color, GError **error) {
	g_signal_emit((gpointer)object, signals[GFX_SET_LED_RGB], 0, index, color);
	return TRUE;
}

static gboolean skeltr_dbus_server_cb_gfx_get_led_rgb(SkeltrDBusServer *object, guchar index, guint32 *color, GError **error) {
	g_signal_emit((gpointer)object, signals[GFX_GET_LED_RGB], 0, index, color);
	return TRUE;
}

static gboolean skeltr_dbus_server_cb_gfx_update(SkeltrDBusServer *object, GError **error) {
	g_signal_emit((gpointer)object, signals[GFX_UPDATE], 0);
	return TRUE;
}

void skeltr_dbus_server_emit_brightness_changed(SkeltrDBusServer *object, guchar profile_number, guchar brightness_number) {
	g_signal_emit((gpointer)object, signals[BRIGHTNESS_CHANGED], 0, profile_number, brightness_number);
}

void skeltr_dbus_server_emit_macro_changed(SkeltrDBusServer *object, guchar profile_number, guchar button_number, SkeltrMacro const *macro) {
	gchar *macro_encoded;

	macro_encoded = g_base64_encode((guchar const *)macro, sizeof(SkeltrMacro));
	g_signal_emit((gpointer)object, signals[MACRO_CHANGED], 0, profile_number, button_number, macro_encoded);
	g_free(macro_encoded);
}

void skeltr_dbus_server_emit_profile_changed(SkeltrDBusServer *object, guchar profile_number) {
	g_signal_emit((gpointer)object, signals[PROFILE_CHANGED], 0, profile_number);
}

gboolean skeltr_dbus_server_connect(SkeltrDBusServer *dbus_server) {
	DBusGConnection *connection;
	GError *error = NULL;

	connection = dbus_g_bus_get(DBUS_BUS_SESSION, &error);
	if (!connection) {
		g_warning(_("Could not get dbus: %s"), error->message);
		return FALSE;
	}

	dbus_g_connection_register_g_object(connection, SKELTR_DBUS_SERVER_PATH, (GObject *)dbus_server);
	dbus_g_connection_unref(connection);

	return TRUE;
}
