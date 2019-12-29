#ifndef __ROCCAT_KONEXTDOPTICAL_H__
#define __ROCCAT_KONEXTDOPTICAL_H__

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

#include "roccat_light.h"
#include "konepuremilitary.h"

G_BEGIN_DECLS

#define USB_DEVICE_ID_ROCCAT_KONEXTDOPTICAL 0x2e23

typedef struct _KonextdopticalProfileSettings KonextdopticalProfileSettings;

enum {
	KONEXTDOPTICAL_CPI_MIN = 100,
	KONEXTDOPTICAL_CPI_MAX = 6400,
	KONEXTDOPTICAL_CPI_STEP = 100,
};

struct _KonextdopticalProfileSettings {
	guint8 report_id; /* KONEPLUS_REPORT_ID_PROFILE_SETTINGS */
	guint8 size; /* always 46 */
	guint8 profile_index; /* range 0-4 */
	guint8 xy_sync;
	guint8 sensitivity_x; /* range 0x1-0xb */
	guint8 sensitivity_y; /* range 0x1-0xb */
	guint8 cpi_levels_enabled; /* bits 1-5 */
	guint8 cpi_levels_y[KONEPLUS_PROFILE_SETTING_CPI_LEVELS_NUM]; /* range 1-128 means 100-6400 cpi */
	guint8 cpi_startup_level; /* range 0-4 */
	guint8 cpi_levels_x[KONEPLUS_PROFILE_SETTING_CPI_LEVELS_NUM];
	guint8 advanced1;
	guint8 polling_rate;
	guint8 lights_enabled; /* bits 1-4 */
	guint8 light_effect_mode; /* unused */
	guint8 color_flow;
	guint8 light_effect;
	guint8 effect_speed; /* range 1-3 */
	RoccatLight lights[KONEPLUS_PROFILE_SETTING_LIGHTS_NUM];
	guint8 advanced2;
	guint8 angle_snapping; // 1-6
	gint8 sensor_alignment; // +-30
	guint16 checksum;
} __attribute__ ((packed));

void konextdoptical_profile_settings_finalize(KonextdopticalProfileSettings *profile_settings, guint profile_index);
gboolean konextdoptical_profile_settings_write(RoccatDevice *konextdoptical, guint profile_index, KonextdopticalProfileSettings *profile_settings, GError **error);
KonextdopticalProfileSettings *konextdoptical_profile_settings_read(RoccatDevice *konextdoptical, guint profile_index, GError **error);

RoccatDevice *konextdoptical_device_first(void);
RoccatDeviceScanner *konextdoptical_device_scanner_new(void);

RoccatKeyFile *konextdoptical_configuration_load(void);
gboolean konextdoptical_configuration_save(RoccatKeyFile *config, GError **error);

KoneplusRmp *konextdoptical_default_rmp(void);
GKeyFile *konextdoptical_rmp_defaults(void);

DBusGProxy *konextdoptical_dbus_proxy_new(void);
gboolean konextdoptical_dbus_emit_profile_data_changed_outside(DBusGProxy *proxy, guint profile_number);
gboolean konextdoptical_dbus_emit_profile_data_changed_outside_instant(guint profile_number);
gboolean konextdoptical_dbus_emit_profile_changed_outside(DBusGProxy *proxy, guint profile_number);
gboolean konextdoptical_dbus_emit_profile_changed_outside_instant(guint profile_number);
gboolean konextdoptical_dbus_emit_configuration_changed_outside(DBusGProxy *proxy);

KoneplusRmp *konextdoptical_rmp_load(RoccatDevice *konextdoptical, guint profile_number, GError **error);
gboolean konextdoptical_rmp_save(RoccatDevice *konextdoptical, KoneplusRmp *rmp, guint profile_index, GError **error);
KoneplusRmp *konextdoptical_rmp_load_save_after_reset(RoccatDevice *konextdoptical, guint profile_index, GError **error);

KoneplusRmp *konextdoptical_rmp_load_actual(guint profile_index);
gboolean konextdoptical_rmp_save_actual(KoneplusRmp *rmp, guint profile_index, GError **error);
void konextdoptical_rmp_update_from_device(KoneplusRmp *rmp, RoccatDevice *konextdoptical, guint profile_index);

KonextdopticalProfileSettings *konextdoptical_rmp_to_profile_settings(KoneplusRmp *rmp);
void konextdoptical_rmp_update_with_profile_settings(KoneplusRmp *rmp, KonextdopticalProfileSettings const *profile_settings);

guint konextdoptical_rmp_cpi_to_bin(guint rmp_value);
guint konextdoptical_bin_cpi_to_rmp(guint bin_value);

#define KONEXTDOPTICAL_DBUS_SERVER_PATH "/org/roccat/Konextdoptical"
#define KONEXTDOPTICAL_DBUS_SERVER_INTERFACE "org.roccat.Konextdoptical"

#define KONEXTDOPTICAL_DEVICE_NAME "Kone XTD Optical"

G_END_DECLS

#endif
