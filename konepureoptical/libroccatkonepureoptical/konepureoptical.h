#ifndef __ROCCAT_KONEPUREOPTICAL_H__
#define __ROCCAT_KONEPUREOPTICAL_H__

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
#include "konepure.h"

G_BEGIN_DECLS

enum {
	KONEPUREOPTICAL_CPI_VALUES_NUM = 7,
};

#define USB_DEVICE_ID_ROCCAT_KONEPURE_OPTICAL 0x2db4

typedef struct _KonepureopticalProfileSettings KonepureopticalProfileSettings;

struct _KonepureopticalProfileSettings {
	guint8 report_id; /* KONEPLUS_REPORT_ID_PROFILE_SETTINGS */
	guint8 size; /* 0x1f */
	guint8 profile_number; /* range 0-4 */
	guint8 xy_sync;
	guint8 sensitivity_x; /* range 0x1-0xb */
	guint8 sensitivity_y; /* range 0x1-0xb */
	guint8 cpi_levels_enabled; /* bits 1-5 */

	 /* range 1-60 means 100-6000 cpi */
	guint8 cpi_levels[KONEPLUS_PROFILE_SETTING_CPI_LEVELS_NUM];
	guint8 unused[KONEPLUS_PROFILE_SETTING_CPI_LEVELS_NUM];
	guint8 cpi_startup_level; /* range 0-4 */
	guint8 unknown1;
	guint8 polling_rate;
	guint8 light_enabled; /* bit 0 */
	guint8 light_chose_type; /* KonextdRmpLightChoseType */
	guint8 color_flow_effect;
	guint8 light_effect_type;
	guint8 light_effect_speed; /* range 1-3 */
	RoccatLight light;
	guint16 checksum;
} __attribute__ ((packed));

#define KONEPUREOPTICAL_DBUS_SERVER_PATH "/org/roccat/Konepureoptical"
#define KONEPUREOPTICAL_DBUS_SERVER_INTERFACE "org.roccat.Konepureoptical"

#define KONEPUREOPTICAL_DEVICE_NAME "Kone Pure Optical"

RoccatDevice *konepureoptical_device_first(void);
RoccatDeviceScanner *konepureoptical_device_scanner_new(void);

KoneplusRmp *konepureoptical_default_rmp(void);
GKeyFile *konepureoptical_rmp_defaults(void);
KoneplusRmp *konepureoptical_rmp_load_actual(guint profile_index);
void konepureoptical_rmp_update_from_device(KoneplusRmp *rmp, RoccatDevice *device, guint profile_index);
KoneplusRmp *konepureoptical_rmp_load(RoccatDevice *device, guint profile_index, GError **error);
gboolean konepureoptical_rmp_save_actual(KoneplusRmp *rmp, guint profile_index, GError **error);
gboolean konepureoptical_rmp_save(RoccatDevice *device, KoneplusRmp *rmp, guint profile_index, GError **error);
KoneplusRmp *konepureoptical_rmp_load_save_after_reset(RoccatDevice *device, guint profile_index, GError **error);

void konepureoptical_profile_settings_finalize(KonepureopticalProfileSettings *profile_settings, guint index);
gboolean konepureoptical_profile_settings_write(RoccatDevice *konepureoptical, guint profile_number, KonepureopticalProfileSettings *profile_settings, GError **error);
KonepureopticalProfileSettings *konepureoptical_profile_settings_read(RoccatDevice *konepureoptical, guint profile_number, GError **error);
gboolean konepureoptical_profile_settings_equal(KonepureopticalProfileSettings const *left, KonepureopticalProfileSettings const *right);

KonepureopticalProfileSettings *konepureoptical_rmp_to_profile_settings(KoneplusRmp *rmp);
void konepureoptical_rmp_update_with_profile_settings(KoneplusRmp *rmp, KonepureopticalProfileSettings const *profile_settings);

RoccatKeyFile *konepureoptical_configuration_load(void);
gboolean konepureoptical_configuration_save(RoccatKeyFile *config, GError **error);

DBusGProxy *konepureoptical_dbus_proxy_new(void);
gboolean konepureoptical_dbus_emit_profile_data_changed_outside(DBusGProxy *proxy, guint profile_number);
gboolean konepureoptical_dbus_emit_profile_data_changed_outside_instant(guint profile_number);
gboolean konepureoptical_dbus_emit_profile_changed_outside(DBusGProxy *proxy, guint profile_number);
gboolean konepureoptical_dbus_emit_profile_changed_outside_instant(guint profile_number);
gboolean konepureoptical_dbus_emit_configuration_changed_outside(DBusGProxy *proxy);

G_END_DECLS

#endif

