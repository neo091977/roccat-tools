#ifndef __ROCCAT_RYOS_PROFILE_DATA_HARDWARE_H__
#define __ROCCAT_RYOS_PROFILE_DATA_HARDWARE_H__

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

#include "ryos_keys_primary.h"
#include "ryos_keys_macro.h"
#include "ryos_keys_function.h"
#include "ryos_keys_thumbster.h"
#include "ryos_keys_extra.h"
#include "ryos_keys_easyzone.h"
#include "ryos_key_mask.h"
#include "ryos_light.h"
#include "ryos_macro.h"
#include "ryos_stored_lights.h"

G_BEGIN_DECLS

typedef struct _RyosProfileDataHardware RyosProfileDataHardware;

/* Data stored in device. Modification stored per individual request.
 * Modified data has to be written into device.
 */
struct _RyosProfileDataHardware {
	guint8 modified_keys_primary;
	guint8 modified_keys_macro;
	guint8 modified_keys_function;
	guint8 modified_keys_thumbster;
	guint8 modified_keys_extra;
	guint8 modified_keys_easyzone;
	guint8 modified_key_mask;
	guint8 modified_light;
	guint8 modified_macros[RYOS_RKP_KEYS_NUM];
	guint8 modified_stored_lights_manual;
	guint8 modified_stored_lights_automatic;
	guint8 modified_light_macro;

	RyosKeysPrimary keys_primary;
	RyosKeysMacro keys_macro;
	RyosKeysFunction keys_function;
	RyosKeysThumbster keys_thumbster;
	RyosKeysExtra keys_extra;
	RyosKeysEasyzone keys_easyzone;
	RyosKeyMask key_mask;
	RyosLight light;
	RyosMacro macros[RYOS_RKP_KEYS_NUM];
	RyosStoredLights stored_lights_manual;
	RyosStoredLights stored_lights_automatic;
	RyosMacro light_macro; /* screensaver */
} __attribute__ ((packed));

gboolean ryos_profile_data_hardware_get_modified(RyosProfileDataHardware const *profile_data);
void ryos_profile_data_hardware_set_modified(RyosProfileDataHardware *profile_data);
void ryos_profile_data_hardware_set_unmodified(RyosProfileDataHardware *profile_data);

void ryos_profile_data_hardware_set_keys_primary(RyosProfileDataHardware *profile_data, RyosKeysPrimary const *keys_primary);
void ryos_profile_data_hardware_set_keys_macro(RyosProfileDataHardware *profile_data, RyosKeysMacro const *keys_macro);
void ryos_profile_data_hardware_set_keys_function(RyosProfileDataHardware *profile_data, RyosKeysFunction const *keys_function);
void ryos_profile_data_hardware_set_keys_extra(RyosProfileDataHardware *profile_data, RyosKeysExtra const *keys_extra);
void ryos_profile_data_hardware_set_keys_thumbster(RyosProfileDataHardware *profile_data, RyosKeysThumbster const *keys_thumbster);
void ryos_profile_data_hardware_set_keys_easyzone(RyosProfileDataHardware *profile_data, RyosKeysEasyzone const *keys_easyzone);
void ryos_profile_data_hardware_set_macro(RyosProfileDataHardware *profile_data, guint index, RyosMacro const *macro);
void ryos_profile_data_hardware_set_key_mask(RyosProfileDataHardware *profile_data, RyosKeyMask const *key_mask);
void ryos_profile_data_hardware_set_light(RyosProfileDataHardware *profile_data, RyosLight const *light);
void ryos_profile_data_hardware_set_stored_lights_manual(RyosProfileDataHardware *profile_data, RyosStoredLights const *stored_lights);
void ryos_profile_data_hardware_set_stored_lights_automatic(RyosProfileDataHardware *profile_data, RyosStoredLights const *stored_lights);
void ryos_profile_data_hardware_set_light_macro(RyosProfileDataHardware *profile_data, RyosMacro const *light_macro);

void ryos_profile_data_hardware_update_with_default(RyosProfileDataHardware *profile_data);
gboolean ryos_profile_data_hardware_update(RyosProfileDataHardware *profile_data, RoccatDevice *device, guint profile_index, GError **error);

gboolean ryos_profile_data_hardware_save(RoccatDevice *device, RyosProfileDataHardware *profile_data, guint profile_index, GError **error);

void ryos_profile_data_hardware_set_key_to_macro(RyosProfileDataHardware *profile_data, guint macro_index);
void ryos_profile_data_hardware_set_key_to_macro_without_modified(RyosProfileDataHardware *profile_data, guint macro_index);

G_END_DECLS

#endif
