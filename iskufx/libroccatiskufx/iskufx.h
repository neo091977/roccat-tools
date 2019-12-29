#ifndef __ROCCAT_ISKUFX_H__
#define __ROCCAT_ISKUFX_H__

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

#include "isku.h"
#include "talkfx.h"
#include "roccat_light.h"

G_BEGIN_DECLS

#define USB_DEVICE_ID_ROCCAT_ISKUFX 0x3264

typedef struct _IskufxLight IskufxLight;
typedef struct _IskufxRkpLightInfo IskufxRkpLightInfo;
typedef struct _IskufxTalkfx IskufxTalkfx;

enum {
	ISKUFX_LIGHTS_NUM = 1,
	ISKUFX_RKP_LIGHT_INFO_COLORS_NUM = 16,
};

struct _IskufxLight {
	guint8 report_id; /* ISKUFX_REPORT_ID_LIGHT */
	guint8 size; /* always 0x10 */
	guint8 profile_index; /* 0-4 */
	guint8 state; /* 0 = off, 1 = on */
	guint8 breathing; /* 0 = off, 1 = on */
	guint8 color_type;
	RoccatLight light;
	guint8 brightness; /* 0-5 */
	guint8 dimness;
	guint8 timeout; /* minutes */
	guint8 talkfx;
	guint16 checksum;
} __attribute__ ((packed));

typedef enum {
	ISKUFX_LIGHT_TALKFX_OFF = 0,
	ISKUFX_LIGHT_TALKFX_ON = 1,
} IskufxLightTalkFx;

typedef enum {
	ISKUFX_LIGHT_STATE_OFF = 0,
	ISKUFX_LIGHT_STATE_ON = 1,
} IskufxLightState;

typedef enum {
	ISKUFX_LIGHT_BREATHING_OFF = 0,
	ISKUFX_LIGHT_BREATHING_ON = 1,
} IskufxLightBreathing;

typedef enum {
	ISKUFX_LIGHT_COLOR_TYPE_SINGLE = 0,
	ISKUFX_LIGHT_COLOR_TYPE_CUSTOM = 1,
	ISKUFX_LIGHT_COLOR_TYPE_FLOW = 3,
} IskufxLightColorType;

static inline guint16 iskufx_light_get_checksum(IskufxLight const *light) {
	return GUINT16_FROM_LE(light->checksum);
}

static inline void iskufx_light_set_checksum(IskufxLight *light, guint16 new_value) {
	light->checksum = GUINT16_TO_LE(new_value);
}

IskufxLight *iskufx_rkp_to_light(IskuRkp *rkp);
void iskufx_rkp_update_with_light(IskuRkp *rkp, IskufxLight const *light);
guint16 iskufx_light_calc_checksum(IskufxLight const *light);
gboolean iskufx_light_write(RoccatDevice *isku, guint profile_index, IskufxLight *light, GError **error);
IskufxLight *iskufx_light_read(RoccatDevice *isku, guint profile_index, GError **error);
gboolean iskufx_light_equal(IskufxLight const *left, IskufxLight const *right);

struct _IskufxRkpLightInfo {
	guint8 index; /* means a color index of 16 colors */
	guint8 state;
	guint8 red;
	guint8 green;
	guint8 blue;
	guint8 checksum;
} __attribute__ ((packed));

typedef enum {
	ISKUFX_RKP_LIGHT_INFO_STATE_ON = 1,
	ISKUFX_RKP_LIGHT_INFO_STATE_OFF = 2,
} IskufxRkpLightInfoState;

struct _IskufxTalkfx {
	guint8 report_id; /* ISKUFX_REPORT_ID_TALKFX */
	guint8 size; /* always 0x10 */
	guint8 event_red;
	guint8 event_green;
	guint8 event_blue;
	guint8 ambient_red;
	guint8 ambient_green;
	guint8 ambient_blue;
	guint8 state;
	guint8 zone;
	guint8 effect;
	guint8 speed;
	guint8 unused[4];
} __attribute__ ((packed));

typedef enum {
	ISKUFX_TALKFX_ZONE_EVENT = 1,
	ISKUFX_TALKFX_ZONE_AMBIENT = 2,
} IskufxTalkfxZone;

typedef enum {
	ISKUFX_REPORT_ID_TALKFX = 0x17,
} IskufxReportId;

typedef enum {
	/* event = ISKU_SPECIAL_EVENT_RAD
	 * data1 = FIXME 1 or 2
	 */
	ISKUFX_SPECIAL_RAD_W = 0x01,
	ISKUFX_SPECIAL_RAD_A = 0x02,
	ISKUFX_SPECIAL_RAD_S = 0x03,
	ISKUFX_SPECIAL_RAD_D = 0x04,
	ISKUFX_SPECIAL_RAD_THUMB_1 = 0x05,
	ISKUFX_SPECIAL_RAD_THUMB_2 = 0x06,
	ISKUFX_SPECIAL_RAD_THUMB_3 = 0x07,
	ISKUFX_SPECIAL_RAD_EASYSHIFT = 0x08,
	ISKUFX_SPECIAL_RAD_MULTIMEDIA = 0x09,
	ISKUFX_SPECIAL_RAD_M1 = 0x0a,
	ISKUFX_SPECIAL_RAD_M2 = 0x0b,
	ISKUFX_SPECIAL_RAD_M3 = 0x0c,
	ISKUFX_SPECIAL_RAD_M4 = 0x0d,
	ISKUFX_SPECIAL_RAD_M5 = 0x0e,
	ISKUFX_SPECIAL_RAD_MACRO_SHORTCUT = 0x0f,
	ISKUFX_SPECIAL_RAD_TALK = 0x10,
	ISKUFX_SPECIAL_RAD_LIVE_REC = 0x11,
	ISKUFX_SPECIAL_RAD_BACKLIGHT = 0x12,
	ISKUFX_SPECIAL_RAD_TOTAL = 0x13,
} IskufxSpecialRad;

RoccatDevice *iskufx_device_first(void);
RoccatDeviceScanner *iskufx_device_scanner_new(void);

RoccatKeyFile *iskufx_configuration_load(void);
gboolean iskufx_configuration_save(RoccatKeyFile *config, GError **error);

IskuRkp *iskufx_default_rkp(void);
GKeyFile *iskufx_rkp_defaults(void);
IskuRkp *iskufx_rkp_load_actual(guint profile_index);
void iskufx_rkp_update_from_device(RoccatDevice *iskufx, IskuRkp *rkp, guint profile_index);
IskuRkp *iskufx_rkp_load(RoccatDevice *iskufx, guint profile_index, GError **error);
gboolean iskufx_rkp_save_actual(IskuRkp *rkp, guint profile_index, GError **error);
gboolean iskufx_rkp_save(RoccatDevice *iskufx, IskuRkp *rkp, guint profile_index, GError **error);
IskuRkp *iskufx_rkp_load_save_after_reset(RoccatDevice *isku, guint profile_index, GError **error);

IskufxRkpLightInfo const *iskufx_rkp_light_info_get_standard(guint index);
guint8 iskufx_rkp_light_info_calc_checksum(IskufxRkpLightInfo const *rkp_light_info);
void iskufx_rkp_light_info_set_checksum(IskufxRkpLightInfo *rkp_light_info);
void iskufx_rkp_light_info_to_roccat_light(IskufxRkpLightInfo const *from, RoccatLight *to);
void roccat_light_to_rkp_light_info(RoccatLight const *light, IskufxRkpLightInfo *rkp_light_info);
gboolean iskufx_rkp_light_info_equal(IskufxRkpLightInfo const *left, IskufxRkpLightInfo const *right);

void iskufx_rkp_set_light_switch(IskuRkp *rkp, guint value);
guint iskufx_rkp_get_light_switch(IskuRkp *rkp);
void iskufx_rkp_set_light_breath_switch(IskuRkp *rkp, guint value);
guint iskufx_rkp_get_light_breath_switch(IskuRkp *rkp);
void iskufx_rkp_set_light_color_type(IskuRkp *rkp, guint value);
guint iskufx_rkp_get_light_color_type(IskuRkp *rkp);
void iskufx_rkp_set_rkp_light_info(IskuRkp *rkp, IskufxRkpLightInfo *rkp_light_info);
IskufxRkpLightInfo *iskufx_rkp_get_rkp_light_info(IskuRkp *rkp);
guint iskufx_rkp_get_talkfx(IskuRkp *rkp);
void iskufx_rkp_set_talkfx(IskuRkp *rkp, guint value);

IskufxTalkfx *iskufx_talkfx_read(RoccatDevice *iskufx, GError **error);
gboolean iskufx_talkfx_write(RoccatDevice *iskufx, IskufxTalkfx *talkfx, GError **error);
gboolean iskufx_talkfx(RoccatDevice *iskufx, guint32 effect, guint32 ambient_color, guint32 event_color, GError **error);
gboolean iskufx_talkfx_off(RoccatDevice *iskufx, GError **error);

#define ISKUFX_DBUS_SERVER_PATH "/org/roccat/Iskufx"
#define ISKUFX_DBUS_SERVER_INTERFACE "org.roccat.Iskufx"

DBusGProxy *iskufx_dbus_proxy_new(void);
gboolean iskufx_dbus_emit_profile_data_changed_outside(DBusGProxy *proxy, guint profile_number);
gboolean iskufx_dbus_emit_profile_data_changed_outside_instant(guint profile_number);
gboolean iskufx_dbus_emit_profile_changed_outside(DBusGProxy *proxy, guint profile_number);
gboolean iskufx_dbus_emit_profile_changed_outside_instant(guint profile_number);
gboolean iskufx_dbus_emit_configuration_changed_outside(DBusGProxy *proxy);

#define ISKUFX_DEVICE_NAME "IskuFX"

G_END_DECLS

#endif
