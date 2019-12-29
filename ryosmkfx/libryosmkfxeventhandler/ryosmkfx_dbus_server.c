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
#include "ryosmkfx_dbus_server.h"
#include "g_cclosure_roccat_marshaller.h"
#include "ryosmkfx_dbus_services.h"
#include "i18n-lib.h"

static gboolean ryosmkfx_dbus_server_cb_talk_easyshift(RyosmkfxDBusServer *object, guchar state, GError **error);
static gboolean ryosmkfx_dbus_server_cb_talk_easyshift_lock(RyosmkfxDBusServer *object, guchar state, GError **error);
static gboolean ryosmkfx_dbus_server_cb_open_gui(RyosmkfxDBusServer *object, GError **error);
static gboolean ryosmkfx_dbus_server_cb_profile_changed_outside(RyosmkfxDBusServer *object, guchar profile_index, GError **error);
static gboolean ryosmkfx_dbus_server_cb_profile_data_changed_outside(RyosmkfxDBusServer *object, guchar profile_index, GError **error);
static gboolean ryosmkfx_dbus_server_cb_configuration_changed_outside(RyosmkfxDBusServer *object, GError **error);
static gboolean ryosmkfx_dbus_server_cb_talkfx_ryos_set_sdk_mode(RyosmkfxDBusServer *object, gboolean state, GError **error);
static gboolean ryosmkfx_dbus_server_cb_talkfx_ryos_set_all_leds(RyosmkfxDBusServer *object, GArray const *data, GError **error);
static gboolean ryosmkfx_dbus_server_cb_talkfx_ryos_turn_on_all_leds(RyosmkfxDBusServer *object, GError **error);
static gboolean ryosmkfx_dbus_server_cb_talkfx_ryos_turn_off_all_leds(RyosmkfxDBusServer *object, GError **error);
static gboolean ryosmkfx_dbus_server_cb_talkfx_ryos_set_led_on(RyosmkfxDBusServer *object, guint8 index, GError **error);
static gboolean ryosmkfx_dbus_server_cb_talkfx_ryos_set_led_off(RyosmkfxDBusServer *object, guint8 index, GError **error);
static gboolean ryosmkfx_dbus_server_cb_talkfx_ryos_all_key_blinking(RyosmkfxDBusServer *object, guint interval, guint count, GError **error);
static gboolean ryosmkfx_dbus_server_cb_gfx_set_led_rgb(RyosmkfxDBusServer *object, guchar index, guint32 color, GError **error);
static gboolean ryosmkfx_dbus_server_cb_gfx_get_led_rgb(RyosmkfxDBusServer *object, guchar index, guint32 *color, GError **error);
static gboolean ryosmkfx_dbus_server_cb_gfx_update(RyosmkfxDBusServer *object, GError **error);

#include "ryosmkfx_dbus_server_glue.h"

#define RYOSMKFX_DBUS_SERVER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), TYPE_RYOSMKFX_DBUS_SERVER, RyosmkfxDBusServerClass))
#define IS_RYOSMKFX_DBUS_SERVER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), TYPE_RYOSMKFX_DBUS_SERVER))
#define RYOSMKFX_DBUS_SERVER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), TYPE_RYOSMKFX_DBUS_SERVER, RyosmkfxDBusServerClass))

typedef struct _RyosmkfxDBusServerClass RyosmkfxDBusServerClass;

struct _RyosmkfxDBusServerClass {
	GObjectClass parent;
};

G_DEFINE_TYPE(RyosmkfxDBusServer, ryosmkfx_dbus_server, G_TYPE_OBJECT);

enum {
	TALK_EASYSHIFT,
	TALK_EASYSHIFT_LOCK,
	OPEN_GUI,
	PROFILE_CHANGED_OUTSIDE,
	PROFILE_DATA_CHANGED_OUTSIDE,
	CONFIGURATION_CHANGED_OUTSIDE,
	TALKFX_RYOS_SET_SDK_MODE,
	TALKFX_RYOS_SET_ALL_LEDS,
	TALKFX_RYOS_TURN_ON_ALL_LEDS,
	TALKFX_RYOS_TURN_OFF_ALL_LEDS,
	TALKFX_RYOS_SET_LED_ON,
	TALKFX_RYOS_SET_LED_OFF,
	TALKFX_RYOS_ALL_KEY_BLINKING,
	GFX_SET_LED_RGB,
	GFX_GET_LED_RGB,
	GFX_UPDATE,
	PROFILE_CHANGED,
	BRIGHTNESS_CHANGED,
	MACRO_CHANGED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

static void ryosmkfx_dbus_server_finalize(GObject *object) {
	G_OBJECT_CLASS(ryosmkfx_dbus_server_parent_class)->finalize(object);
}

static void ryosmkfx_dbus_server_class_init(RyosmkfxDBusServerClass *klass) {
	GObjectClass *object_class;
	object_class = G_OBJECT_CLASS(klass);
	object_class->finalize = ryosmkfx_dbus_server_finalize;

	dbus_g_object_type_install_info(TYPE_RYOSMKFX_DBUS_SERVER, &dbus_glib_server_object_info);

	signals[TALK_EASYSHIFT] = g_signal_new("talk-easyshift", TYPE_RYOSMKFX_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[TALK_EASYSHIFT_LOCK] = g_signal_new("talk-easyshift-lock", TYPE_RYOSMKFX_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[OPEN_GUI] = g_signal_new("open-gui", TYPE_RYOSMKFX_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE,
			0);

	signals[PROFILE_CHANGED_OUTSIDE] = g_signal_new("profile-changed-outside", TYPE_RYOSMKFX_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[PROFILE_DATA_CHANGED_OUTSIDE] = g_signal_new("profile-data-changed-outside", TYPE_RYOSMKFX_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[CONFIGURATION_CHANGED_OUTSIDE] = g_signal_new("configuration-changed-outside", TYPE_RYOSMKFX_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE,
			0);

	signals[TALKFX_RYOS_SET_SDK_MODE] = g_signal_new("talkfx-ryos-set-sdk-mode", TYPE_RYOSMKFX_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__BOOLEAN, G_TYPE_NONE,
			1, G_TYPE_BOOLEAN);

	signals[TALKFX_RYOS_SET_ALL_LEDS] = g_signal_new("talkfx-ryos-set-all-leds", TYPE_RYOSMKFX_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__BOXED, G_TYPE_NONE,
			1, dbus_g_type_get_collection("GArray", G_TYPE_UCHAR));

	signals[TALKFX_RYOS_TURN_ON_ALL_LEDS] = g_signal_new("talkfx-ryos-turn-on-all-leds", TYPE_RYOSMKFX_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE,
			0);

	signals[TALKFX_RYOS_TURN_OFF_ALL_LEDS] = g_signal_new("talkfx-ryos-turn-off-all-leds", TYPE_RYOSMKFX_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE,
			0);

	signals[TALKFX_RYOS_SET_LED_ON] = g_signal_new("talkfx-ryos-set-led-on", TYPE_RYOSMKFX_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[TALKFX_RYOS_SET_LED_OFF] = g_signal_new("talkfx-ryos-set-led-off", TYPE_RYOSMKFX_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[TALKFX_RYOS_ALL_KEY_BLINKING] = g_signal_new("talkfx-ryos-all-key-blinking", TYPE_RYOSMKFX_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_VOID__UINT_UINT, G_TYPE_NONE,
			2, G_TYPE_UINT, G_TYPE_UINT);

	signals[GFX_SET_LED_RGB] = g_signal_new("gfx-set-led-rgb", TYPE_RYOSMKFX_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_VOID__UINT_UINT, G_TYPE_NONE,
			2, G_TYPE_UINT, G_TYPE_UINT);

	signals[GFX_GET_LED_RGB] = g_signal_new("gfx-get-led-rgb", TYPE_RYOSMKFX_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_VOID__UINT_POINTER, G_TYPE_NONE,
			2, G_TYPE_UINT, G_TYPE_POINTER);

	signals[GFX_UPDATE] = g_signal_new("gfx-update", TYPE_RYOSMKFX_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE,
			0);

	signals[PROFILE_CHANGED] = g_signal_new("profile-changed", TYPE_RYOSMKFX_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[BRIGHTNESS_CHANGED] = g_signal_new("brightness-changed", TYPE_RYOSMKFX_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_VOID__UCHAR_UCHAR, G_TYPE_NONE,
			2, G_TYPE_UCHAR, G_TYPE_UCHAR);

	signals[MACRO_CHANGED] = g_signal_new("macro-changed", TYPE_RYOSMKFX_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_VOID__UCHAR_UCHAR_STRING, G_TYPE_NONE,
			3, G_TYPE_UCHAR, G_TYPE_UCHAR, G_TYPE_STRING);
}

static void ryosmkfx_dbus_server_init(RyosmkfxDBusServer *object) {}

RyosmkfxDBusServer *ryosmkfx_dbus_server_new(void) {
	return g_object_new(TYPE_RYOSMKFX_DBUS_SERVER, NULL);
}

static gboolean ryosmkfx_dbus_server_cb_talk_easyshift(RyosmkfxDBusServer *object, guchar state, GError **error) {
	g_signal_emit((gpointer)object, signals[TALK_EASYSHIFT], 0, state);
	return TRUE;
}

static gboolean ryosmkfx_dbus_server_cb_talk_easyshift_lock(RyosmkfxDBusServer *object, guchar state, GError **error) {
	g_signal_emit((gpointer)object, signals[TALK_EASYSHIFT_LOCK], 0, state);
	return TRUE;
}

static gboolean ryosmkfx_dbus_server_cb_open_gui(RyosmkfxDBusServer *object, GError **error) {
	g_signal_emit((gpointer)object, signals[OPEN_GUI], 0);
	return TRUE;
}

static gboolean ryosmkfx_dbus_server_cb_profile_changed_outside(RyosmkfxDBusServer *object, guchar profile_index, GError **error) {
	g_signal_emit((gpointer)object, signals[PROFILE_CHANGED_OUTSIDE], 0, profile_index);
	return TRUE;
}

static gboolean ryosmkfx_dbus_server_cb_profile_data_changed_outside(RyosmkfxDBusServer *object, guchar profile_index, GError **error) {
	g_signal_emit((gpointer)object, signals[PROFILE_DATA_CHANGED_OUTSIDE], 0, profile_index);
	return TRUE;
}

static gboolean ryosmkfx_dbus_server_cb_configuration_changed_outside(RyosmkfxDBusServer *object, GError **error) {
	g_signal_emit((gpointer)object, signals[CONFIGURATION_CHANGED_OUTSIDE], 0);
	return TRUE;
}

static gboolean ryosmkfx_dbus_server_cb_talkfx_ryos_set_sdk_mode(RyosmkfxDBusServer *object, gboolean state, GError **error) {
	g_signal_emit((gpointer)object, signals[TALKFX_RYOS_SET_SDK_MODE], 0, state);
	return TRUE;
}

static gboolean ryosmkfx_dbus_server_cb_talkfx_ryos_set_all_leds(RyosmkfxDBusServer *object, GArray const *data, GError **error) {
	g_signal_emit((gpointer)object, signals[TALKFX_RYOS_SET_ALL_LEDS], 0, data);
	return TRUE;
}

static gboolean ryosmkfx_dbus_server_cb_talkfx_ryos_turn_on_all_leds(RyosmkfxDBusServer *object, GError **error) {
	g_signal_emit((gpointer)object, signals[TALKFX_RYOS_TURN_ON_ALL_LEDS], 0);
	return TRUE;
}

static gboolean ryosmkfx_dbus_server_cb_talkfx_ryos_turn_off_all_leds(RyosmkfxDBusServer *object, GError **error) {
	g_signal_emit((gpointer)object, signals[TALKFX_RYOS_TURN_OFF_ALL_LEDS], 0);
	return TRUE;
}

static gboolean ryosmkfx_dbus_server_cb_talkfx_ryos_set_led_on(RyosmkfxDBusServer *object, guint8 index, GError **error) {
	g_signal_emit((gpointer)object, signals[TALKFX_RYOS_SET_LED_ON], 0, index);
	return TRUE;
}

static gboolean ryosmkfx_dbus_server_cb_talkfx_ryos_set_led_off(RyosmkfxDBusServer *object, guint8 index, GError **error) {
	g_signal_emit((gpointer)object, signals[TALKFX_RYOS_SET_LED_OFF], 0, index);
	return TRUE;
}

static gboolean ryosmkfx_dbus_server_cb_talkfx_ryos_all_key_blinking(RyosmkfxDBusServer *object, guint interval, guint count, GError **error) {
	g_signal_emit((gpointer)object, signals[TALKFX_RYOS_ALL_KEY_BLINKING], 0, interval, count);
	return TRUE;
}

static gboolean ryosmkfx_dbus_server_cb_gfx_set_led_rgb(RyosmkfxDBusServer *object, guchar index, guint32 color, GError **error) {
	g_signal_emit((gpointer)object, signals[GFX_SET_LED_RGB], 0, index, color);
	return TRUE;
}

static gboolean ryosmkfx_dbus_server_cb_gfx_get_led_rgb(RyosmkfxDBusServer *object, guchar index, guint32 *color, GError **error) {
	g_signal_emit((gpointer)object, signals[GFX_GET_LED_RGB], 0, index, color);
	return TRUE;
}

static gboolean ryosmkfx_dbus_server_cb_gfx_update(RyosmkfxDBusServer *object, GError **error) {
	g_signal_emit((gpointer)object, signals[GFX_UPDATE], 0);
	return TRUE;
}

void ryosmkfx_dbus_server_emit_brightness_changed(RyosmkfxDBusServer *object, guchar profile_number, guchar brightness_number) {
	g_signal_emit((gpointer)object, signals[BRIGHTNESS_CHANGED], 0, profile_number, brightness_number);
}

void ryosmkfx_dbus_server_emit_macro_changed(RyosmkfxDBusServer *object, guchar profile_number, guchar button_number, RyosmkfxMacro const *macro) {
	gchar *macro_encoded;

	macro_encoded = g_base64_encode((guchar const *)macro, sizeof(RyosmkfxMacro));
	g_signal_emit((gpointer)object, signals[MACRO_CHANGED], 0, profile_number, button_number, macro_encoded);
	g_free(macro_encoded);
}

void ryosmkfx_dbus_server_emit_profile_changed(RyosmkfxDBusServer *object, guchar profile_number) {
	g_signal_emit((gpointer)object, signals[PROFILE_CHANGED], 0, profile_number);
}

gboolean ryosmkfx_dbus_server_connect(RyosmkfxDBusServer *dbus_server) {
	DBusGConnection *connection;
	GError *error = NULL;

	connection = dbus_g_bus_get(DBUS_BUS_SESSION, &error);
	if (!connection) {
		g_warning(_("Could not get dbus: %s"), error->message);
		return FALSE;
	}

	dbus_g_connection_register_g_object(connection, RYOSMKFX_DBUS_SERVER_PATH, (GObject *)dbus_server);
	dbus_g_connection_unref(connection);

	return TRUE;
}
