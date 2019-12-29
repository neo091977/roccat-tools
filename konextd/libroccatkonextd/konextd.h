#ifndef __ROCCAT_KONEXTD_H__
#define __ROCCAT_KONEXTD_H__

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

#include "koneplus.h"

G_BEGIN_DECLS

#define USB_DEVICE_ID_ROCCAT_KONEXTD 0x2e22

enum {
	KONEXTD_PROFILE_SETTING_CPI_MIN = 1,
	KONEXTD_PROFILE_SETTING_CPI_MAX = 41,
};

typedef enum {
	/* data1 = count, usually 0x0a */
	KONEXTD_CHRDEV_EVENT_TYPE_RAD_LEFT = 0xe1,
	KONEXTD_CHRDEV_EVENT_TYPE_RAD_RIGHT = 0xe2,
	KONEXTD_CHRDEV_EVENT_TYPE_RAD_MIDDLE = 0xe3,
	KONEXTD_CHRDEV_EVENT_TYPE_RAD_THUMB_1 = 0xe4,
	KONEXTD_CHRDEV_EVENT_TYPE_RAD_THUMB_2 = 0xe5,
	KONEXTD_CHRDEV_EVENT_TYPE_RAD_SCROLL_WHEEL_UP = 0xe6,
	KONEXTD_CHRDEV_EVENT_TYPE_RAD_SCROLL_WHEEL_DOWN = 0xe7,
	KONEXTD_CHRDEV_EVENT_TYPE_RAD_DISTANCE = 0xea, // TODO unit?
	KONEXTD_CHRDEV_EVENT_TYPE_RAD_EASYSHIFT = 0xed,
	KONEXTD_CHRDEV_EVENT_TYPE_RAD_EASYAIM = 0xee,
} KonextdChrdevEventType;

typedef enum {
	KONEXTD_REPORT_ID_SROM_WRITE = 0x0d,
} KonextdReportId;

typedef enum {
	KONEXTD_RMP_LIGHT_CHOSE_TYPE_TABLE = 0,
	KONEXTD_RMP_LIGHT_CHOSE_TYPE_CUSTOM = 1,
} KonextdRmpLightChoseType;


RoccatDevice *konextd_device_first(void);
RoccatDeviceScanner *konextd_device_scanner_new(void);

RoccatKeyFile *konextd_configuration_load(void);
gboolean konextd_configuration_save(RoccatKeyFile *config, GError **error);

KoneplusRmp *konextd_default_rmp(void);
GKeyFile *konextd_rmp_defaults(void);

DBusGProxy *konextd_dbus_proxy_new(void);
gboolean konextd_dbus_emit_profile_data_changed_outside(DBusGProxy *proxy, guint profile_number);
gboolean konextd_dbus_emit_profile_data_changed_outside_instant(guint profile_number);
gboolean konextd_dbus_emit_profile_changed_outside(DBusGProxy *proxy, guint profile_number);
gboolean konextd_dbus_emit_profile_changed_outside_instant(guint profile_number);
gboolean konextd_dbus_emit_configuration_changed_outside(DBusGProxy *proxy);

KoneplusRmp *konextd_rmp_load(RoccatDevice *konextd, guint profile_number, GError **error);
gboolean konextd_rmp_save(RoccatDevice *konextd, KoneplusRmp *rmp, guint profile_index, GError **error);
KoneplusRmp *konextd_rmp_load_save_after_reset(RoccatDevice *konextd, guint profile_index, GError **error);

KoneplusRmp *konextd_rmp_load_actual(guint profile_index);
gboolean konextd_rmp_save_actual(KoneplusRmp *rmp, guint profile_index, GError **error);
void konextd_rmp_update_from_device(KoneplusRmp *rmp, RoccatDevice *konextd, guint profile_index);

void konextd_rmp_set_light_chose_type(KoneplusRmp *rmp, guint value);
guint konextd_rmp_get_light_chose_type(KoneplusRmp *rmp);
void konextd_rmp_set_custom_light_info(KoneplusRmp *rmp, guint index, KoneplusRmpLightInfo *rmp_light_info);
KoneplusRmpLightInfo *konextd_rmp_get_custom_light_info(KoneplusRmp *rmp, guint index);

KoneplusProfileSettings *konextd_rmp_to_profile_settings(KoneplusRmp *rmp);
void konextd_rmp_update_with_profile_settings(KoneplusRmp *rmp, KoneplusProfileSettings const *profile_settings);

guint konextd_rmp_cpi_to_bin(guint rmp_value);
guint konextd_bin_cpi_to_rmp(guint bin_value);

#define KONEXTD_DBUS_SERVER_PATH "/org/roccat/Konextd"
#define KONEXTD_DBUS_SERVER_INTERFACE "org.roccat.Konextd"

#define KONEXTD_DEVICE_NAME "Kone XTD"

G_END_DECLS

#endif
