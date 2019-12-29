#ifndef __ROCCAT_KONEPUREMILITARY_H__
#define __ROCCAT_KONEPUREMILITARY_H__

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

#include "konepure.h"

G_BEGIN_DECLS

#define USB_DEVICE_ID_ROCCAT_KONEPURE_MILITARY 0x2dbf
#define USB_DEVICE_ID_ROCCAT_KONEPURE_OPTICAL_BLACK 0x2dc2

typedef struct _KonepuremilitaryLight KonepuremilitaryLight;
typedef struct _KonepuremilitaryProfileSettings KonepuremilitaryProfileSettings;

enum {
	KONEPUREMILITARY_CPI_LEVELS_NUM = 5,
	KONEPUREMILITARY_CPI_MIN = 100,
	KONEPUREMILITARY_CPI_MAX = 5000,
	KONEPUREMILITARY_CPI_STEP = 100,
	KONEPUREMILITARY_FIRMWARE_SIZE = 57336,
	KONEPUREMILITARY_ANGLESNAP_MIN = 1,
	KONEPUREMILITARY_ANGLESNAP_MAX = 6,
	KONEPUREMILITARY_SENSOR_ALIGNMENT_RANGE = 30,
};

struct _KonepuremilitaryLight {
	guint8 index;
	guint8 red;
	guint8 blue;
	guint8 green;
} __attribute__ ((packed));

struct _KonepuremilitaryProfileSettings {
	guint8 report_id; /* KONEPLUS_REPORT_ID_PROFILE_SETTINGS */
	guint8 size; /* 0x1f */
	guint8 profile_index; /* range 0-4 */
	guint8 xy_sync;
	guint8 sensitivity_x; /* range 0x1-0xb */
	guint8 sensitivity_y; /* range 0x1-0xb */
	guint8 cpi_levels_enabled; /* bits 1-5 */

	 /* range 1-50 means 100-5000 cpi */
	guint8 cpi_levels_x[KONEPLUS_PROFILE_SETTING_CPI_LEVELS_NUM];
	guint8 cpi_startup_level; /* range 0-4 */
	guint8 cpi_levels_y[KONEPLUS_PROFILE_SETTING_CPI_LEVELS_NUM];
	guint8 advanced;
	guint8 polling_rate;
	guint8 light_enabled; /* bit 0 */
	guint8 light_chose_type; /* KonextdRmpLightChoseType */
	guint8 color_flow_effect;
	guint8 light_effect_type;
	guint8 light_effect_speed; /* range 1-3 */
	KonepuremilitaryLight light;
	guint8 angle_snapping_value; /* 0x01 - 0x06 */
	guint8 sensor_alignment_value; /* 0x00 - 0x3c */
} __attribute__ ((packed));

static inline guint konepuremilitary_cpi_rpm_to_real(guint rpm_value) {
	return rpm_value * 100;
}

static inline guint konepuremilitary_cpi_real_to_rpm(guint real_value) {
	return real_value / 100;
}

static inline guint konepuremilitary_cpi_rpm_to_hardware(guint rpm_value) {
	return rpm_value << 1;
}

static inline guint konepuremilitary_cpi_hardware_to_rpm(guint hardware_value) {
	return hardware_value >> 1;
}

typedef enum {
	KONEPUREMILITARY_PROFILE_SETTINGS_ADVANCED_ANGLE_SNAP_BIT = 0,
	KONEPUREMILITARY_PROFILE_SETTINGS_ADVANCED_SENSOR_ALIGNMENT_BIT = 1,
} KonepuremilitaryProfileSettingsAdvanced;

typedef enum {
	KONEPUREMILITARY_RMP_ANGLESNAP_FLAG_OFF = 0,
	KONEPUREMILITARY_RMP_ANGLESNAP_FLAG_ON = 1,
} KonepuremilitaryRmpAnglesnapFlag;

typedef enum {
	KONEPUREMILITARY_RMP_SENSORALIGN_FLAG_OFF = 0,
	KONEPUREMILITARY_RMP_SENSORALIGN_FLAG_ON = 1,
} KonepuremilitaryRmpSensoralignFlag;

typedef enum {
	KONEPUREMILITARY_RMP_XY_CPI_FLAG_LOCKED = 1,
	KONEPUREMILITARY_RMP_XY_CPI_FLAG_UNLOCKED = 0,
} KonepuremilitaryRmpXyCpiFlag;

#define KONEPUREMILITARY_DBUS_SERVER_PATH "/org/roccat/Konepuremilitary"
#define KONEPUREMILITARY_DBUS_SERVER_INTERFACE "org.roccat.Konepuremilitary"

#define KONEPUREMILITARY_DEVICE_NAME "Kone Pure Military"
#define KONEPUREOPTICALBLACK_DEVICE_NAME "Kone Pure Optical Black"
#define KONEPUREMILITARY_DEVICE_NAME_COMBINED "Kone Pure Military/Optical Black"

RoccatDevice *konepuremilitary_device_first(void);
RoccatDeviceScanner *konepuremilitary_device_scanner_new(void);

KoneplusRmp *konepuremilitary_default_rmp(void);
GKeyFile *konepuremilitary_rmp_defaults(void);
KoneplusRmp *konepuremilitary_rmp_load_actual(guint profile_index);
void konepuremilitary_rmp_update_from_device(KoneplusRmp *rmp, RoccatDevice *device, guint profile_index);
KoneplusRmp *konepuremilitary_rmp_load(RoccatDevice *device, guint profile_index, GError **error);
gboolean konepuremilitary_rmp_save_actual(KoneplusRmp *rmp, guint profile_index, GError **error);
gboolean konepuremilitary_rmp_save(RoccatDevice *device, KoneplusRmp *rmp, guint profile_index, GError **error);
KoneplusRmp *konepuremilitary_rmp_load_save_after_reset(RoccatDevice *device, guint profile_index, GError **error);

KonepuremilitaryProfileSettings *konepuremilitary_rmp_to_profile_settings(KoneplusRmp *rmp);
void konepuremilitary_rmp_update_with_profile_settings(KoneplusRmp *rmp, KonepuremilitaryProfileSettings const *profile_settings);

RoccatKeyFile *konepuremilitary_configuration_load(void);
gboolean konepuremilitary_configuration_save(RoccatKeyFile *config, GError **error);

DBusGProxy *konepuremilitary_dbus_proxy_new(void);
gboolean konepuremilitary_dbus_emit_profile_data_changed_outside(DBusGProxy *proxy, guint profile_number);
gboolean konepuremilitary_dbus_emit_profile_data_changed_outside_instant(guint profile_number);
gboolean konepuremilitary_dbus_emit_profile_changed_outside(DBusGProxy *proxy, guint profile_number);
gboolean konepuremilitary_dbus_emit_profile_changed_outside_instant(guint profile_number);
gboolean konepuremilitary_dbus_emit_configuration_changed_outside(DBusGProxy *proxy);

gboolean konepuremilitary_profile_settings_write(RoccatDevice *konepuremilitary, guint profile_index, KonepuremilitaryProfileSettings *profile_settings, GError **error);
KonepuremilitaryProfileSettings *konepuremilitary_profile_settings_read(RoccatDevice *konepuremilitary, guint profile_index, GError **error);
gboolean konepuremilitary_profile_settings_equal(KonepuremilitaryProfileSettings const *left, KonepuremilitaryProfileSettings const *right);

void konepuremilitary_rmp_set_anglesnap_value(KoneplusRmp *rmp, guint value);
guint konepuremilitary_rmp_get_anglesnap_value(KoneplusRmp *rmp);
void konepuremilitary_rmp_set_anglesnap_flag(KoneplusRmp *rmp, guint value);
guint konepuremilitary_rmp_get_anglesnap_flag(KoneplusRmp *rmp);
void konepuremilitary_rmp_set_sensoralign_value(KoneplusRmp *rmp, guint value);
guint konepuremilitary_rmp_get_sensoralign_value(KoneplusRmp *rmp);
void konepuremilitary_rmp_set_sensoralign_flag(KoneplusRmp *rmp, guint value);
guint konepuremilitary_rmp_get_sensoralign_flag(KoneplusRmp *rmp);
void konepuremilitary_rmp_set_xy_cpi_flag(KoneplusRmp *rmp, guint index, gboolean value);
gboolean konepuremilitary_rmp_get_xy_cpi_flag(KoneplusRmp *rmp, guint index);

G_END_DECLS

#endif

