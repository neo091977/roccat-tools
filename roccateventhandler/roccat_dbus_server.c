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
#include "roccat_dbus_server.h"
#include "g_cclosure_roccat_marshaller.h"
#include "roccat.h"
#include "roccat_talk.h"
#include "i18n.h"

static gboolean roccat_dbus_server_cb_talkfx_restore_led_rgb(RoccatDBusServer *object, GError **error);
static gboolean roccat_dbus_server_cb_talkfx_set_led_rgb(RoccatDBusServer *object, guint32 effect, guint32 ambient_color, guint32 event_color, GError **error);
static gboolean roccat_dbus_server_cb_talkfx_ryos_set_sdk_mode(RoccatDBusServer *object, gboolean state, GError **error);
static gboolean roccat_dbus_server_cb_talkfx_ryos_set_all_leds(RoccatDBusServer *object, GArray const *data, GError **error);
static gboolean roccat_dbus_server_cb_talkfx_ryos_turn_on_all_leds(RoccatDBusServer *object, GError **error);
static gboolean roccat_dbus_server_cb_talkfx_ryos_turn_off_all_leds(RoccatDBusServer *object, GError **error);
static gboolean roccat_dbus_server_cb_talkfx_ryos_set_led_on(RoccatDBusServer *object, guint8 index, GError **error);
static gboolean roccat_dbus_server_cb_talkfx_ryos_set_led_off(RoccatDBusServer *object, guint8 index, GError **error);
static gboolean roccat_dbus_server_cb_talkfx_ryos_all_key_blinking(RoccatDBusServer *object, guint interval, guint count, GError **error);

#include "roccat_dbus_server_glue.h"

#define ROCCAT_DBUS_SERVER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), TYPE_ROCCAT_DBUS_SERVER, RoccatDBusServerClass))
#define IS_ROCCAT_DBUS_SERVER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), TYPE_ROCCAT_DBUS_SERVER))
#define ROCCAT_DBUS_SERVER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), TYPE_ROCCAT_DBUS_SERVER, RoccatDBusServerClass))

typedef struct {
	GObjectClass parent;
} RoccatDBusServerClass;

G_DEFINE_TYPE(RoccatDBusServer, roccat_dbus_server, G_TYPE_OBJECT);

enum {
	TALKFX_SET_LED_RGB,
	TALKFX_RESTORE_LED_RGB,
	TALKFX_RYOS_SET_SDK_MODE,
	TALKFX_RYOS_SET_ALL_LEDS,
	TALKFX_RYOS_TURN_ON_ALL_LEDS,
	TALKFX_RYOS_TURN_OFF_ALL_LEDS,
	TALKFX_RYOS_SET_LED_ON,
	TALKFX_RYOS_SET_LED_OFF,
	TALKFX_RYOS_ALL_KEY_BLINKING,
	LAST_SIGNAL,
};

static guint signals[LAST_SIGNAL] = { 0 };

static void roccat_dbus_server_finalize(GObject *object) {
	G_OBJECT_CLASS(roccat_dbus_server_parent_class)->finalize(object);
}

static void roccat_dbus_server_class_init(RoccatDBusServerClass *klass) {
	GObjectClass *object_class;
	object_class = G_OBJECT_CLASS(klass);
	object_class->finalize = roccat_dbus_server_finalize;

	dbus_g_object_type_install_info(TYPE_ROCCAT_DBUS_SERVER, &dbus_glib_server_object_info);

	signals[TALKFX_SET_LED_RGB] = g_signal_new("talkfx-set-led-rgb", TYPE_ROCCAT_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_VOID__UINT_UINT_UINT_UINT, G_TYPE_NONE,
			4, G_TYPE_UINT, G_TYPE_UINT, G_TYPE_UINT, G_TYPE_UINT);

	signals[TALKFX_RESTORE_LED_RGB] = g_signal_new("talkfx-restore-led-rgb", TYPE_ROCCAT_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UINT, G_TYPE_NONE,
			1, G_TYPE_UINT);

	signals[TALKFX_RYOS_SET_SDK_MODE] = g_signal_new("talkfx-ryos-set-sdk-mode", TYPE_ROCCAT_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_VOID__UINT_BOOLEAN, G_TYPE_NONE,
			2, G_TYPE_UINT, G_TYPE_BOOLEAN);

	signals[TALKFX_RYOS_SET_ALL_LEDS] = g_signal_new("talkfx-ryos-set-all-leds", TYPE_ROCCAT_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_VOID__UINT_BOXED, G_TYPE_NONE,
			2, G_TYPE_UINT, dbus_g_type_get_collection("GArray", G_TYPE_UCHAR));

	signals[TALKFX_RYOS_TURN_ON_ALL_LEDS] = g_signal_new("talkfx-ryos-turn-on-all-leds", TYPE_ROCCAT_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UINT, G_TYPE_NONE,
			1, G_TYPE_UINT);

	signals[TALKFX_RYOS_TURN_OFF_ALL_LEDS] = g_signal_new("talkfx-ryos-turn-off-all-leds", TYPE_ROCCAT_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UINT, G_TYPE_NONE,
			1, G_TYPE_UINT);

	signals[TALKFX_RYOS_SET_LED_ON] = g_signal_new("talkfx-ryos-set-led-on", TYPE_ROCCAT_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_VOID__UINT_UCHAR, G_TYPE_NONE,
			2, G_TYPE_UINT, G_TYPE_UCHAR);

	signals[TALKFX_RYOS_SET_LED_OFF] = g_signal_new("talkfx-ryos-set-led-off", TYPE_ROCCAT_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_VOID__UINT_UCHAR, G_TYPE_NONE,
			2, G_TYPE_UINT, G_TYPE_UCHAR);

	signals[TALKFX_RYOS_ALL_KEY_BLINKING] = g_signal_new("talkfx-ryos-all-key-blinking", TYPE_ROCCAT_DBUS_SERVER,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_VOID__UINT_UINT_UINT, G_TYPE_NONE,
			3, G_TYPE_UINT, G_TYPE_UINT, G_TYPE_UINT);
}

static void roccat_dbus_server_init(RoccatDBusServer *object) {}

RoccatDBusServer *roccat_dbus_server_new(void) {
	return g_object_new(TYPE_ROCCAT_DBUS_SERVER, NULL);
}

static gboolean roccat_dbus_server_cb_talkfx_set_led_rgb(RoccatDBusServer *object,
		guint32 effect, guint32 ambient_color, guint32 event_color, GError **error) {
	g_signal_emit((gpointer)object, signals[TALKFX_SET_LED_RGB], 0, ROCCAT_TALK_DEVICE_ALL, effect, ambient_color, event_color);
	return TRUE;
}

static gboolean roccat_dbus_server_cb_talkfx_restore_led_rgb(RoccatDBusServer *object, GError **error) {
	g_signal_emit((gpointer)object, signals[TALKFX_RESTORE_LED_RGB], 0, ROCCAT_TALK_DEVICE_ALL);
	return TRUE;
}

static gboolean roccat_dbus_server_cb_talkfx_ryos_set_sdk_mode(RoccatDBusServer *object, gboolean state, GError **error) {
	g_signal_emit((gpointer)object, signals[TALKFX_RYOS_SET_SDK_MODE], 0, ROCCAT_TALK_DEVICE_ALL, state);
	return TRUE;
}

static gboolean roccat_dbus_server_cb_talkfx_ryos_set_all_leds(RoccatDBusServer *object, GArray const *data, GError **error) {
	g_signal_emit((gpointer)object, signals[TALKFX_RYOS_SET_ALL_LEDS], 0, ROCCAT_TALK_DEVICE_ALL, data);
	return TRUE;
}

static gboolean roccat_dbus_server_cb_talkfx_ryos_turn_on_all_leds(RoccatDBusServer *object, GError **error) {
	g_signal_emit((gpointer)object, signals[TALKFX_RYOS_TURN_ON_ALL_LEDS], 0, ROCCAT_TALK_DEVICE_ALL);
	return TRUE;
}

static gboolean roccat_dbus_server_cb_talkfx_ryos_turn_off_all_leds(RoccatDBusServer *object, GError **error) {
	g_signal_emit((gpointer)object, signals[TALKFX_RYOS_TURN_OFF_ALL_LEDS], 0, ROCCAT_TALK_DEVICE_ALL);
	return TRUE;
}

static gboolean roccat_dbus_server_cb_talkfx_ryos_set_led_on(RoccatDBusServer *object, guint8 index, GError **error) {
	g_signal_emit((gpointer)object, signals[TALKFX_RYOS_SET_LED_ON], 0, ROCCAT_TALK_DEVICE_ALL, index);
	return TRUE;
}

static gboolean roccat_dbus_server_cb_talkfx_ryos_set_led_off(RoccatDBusServer *object, guint8 index, GError **error) {
	g_signal_emit((gpointer)object, signals[TALKFX_RYOS_SET_LED_OFF], 0, ROCCAT_TALK_DEVICE_ALL, index);
	return TRUE;
}

static gboolean roccat_dbus_server_cb_talkfx_ryos_all_key_blinking(RoccatDBusServer *object, guint interval, guint count, GError **error) {
	g_signal_emit((gpointer)object, signals[TALKFX_RYOS_ALL_KEY_BLINKING], 0, ROCCAT_TALK_DEVICE_ALL, interval, count);
	return TRUE;
}

gboolean roccat_dbus_server_connect(RoccatDBusServer *dbus_server) {
	DBusGConnection *connection;
	GError *error = NULL;

	connection = dbus_g_bus_get(DBUS_BUS_SESSION, &error);
	if (!connection) {
		g_warning(_("Could not get dbus: %s"), error->message);
		return FALSE;
	}

	dbus_g_connection_register_g_object(connection, ROCCAT_DBUS_SERVER_PATH, (GObject *)dbus_server);
	dbus_g_connection_unref(connection);

	return TRUE;
}
