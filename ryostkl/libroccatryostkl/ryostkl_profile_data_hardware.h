#ifndef __ROCCAT_RYOSTKL_PROFILE_DATA_HARDWARE_H__
#define __ROCCAT_RYOSTKL_PROFILE_DATA_HARDWARE_H__

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
#include "ryos_keys_function.h"
#include "ryos_keys_thumbster.h"
#include "ryos_keys_extra.h"
#include "ryos_keys_easyzone.h"
#include "ryos_key_mask.h"
#include "ryos_light.h"
#include "ryos_macro.h"
#include "ryostkl_light_layer.h"

G_BEGIN_DECLS

typedef struct _RyostklProfileDataHardware RyostklProfileDataHardware;

/* Data stored in device. Modification stored per individual request.
 * Modified data has to be written into device.
 */
struct _RyostklProfileDataHardware {
	guint8 modified_keys_primary;
	guint8 modified_keys_function;
	guint8 modified_keys_thumbster;
	guint8 modified_keys_extra;
	guint8 modified_keys_easyzone;
	guint8 modified_key_mask;
	guint8 modified_light;
	guint8 modified_macros[RYOS_RKP_KEYS_NUM];
	guint8 modified_light_layer_automatic[RYOS_STORED_LIGHTS_LAYER_NUM];
	guint8 modified_light_layer_manual[RYOS_STORED_LIGHTS_LAYER_NUM];
	guint8 modified_light_macro;

	RyosKeysPrimary keys_primary;
	RyosKeysFunction keys_function;
	RyosKeysThumbster keys_thumbster;
	RyosKeysExtra keys_extra;
	RyosKeysEasyzone keys_easyzone;
	RyosKeyMask key_mask;
	RyosLight light;
	RyosMacro macros[RYOS_RKP_KEYS_NUM];
	RyostklLightLayer light_layer_automatic[RYOS_STORED_LIGHTS_LAYER_NUM];
	RyostklLightLayer light_layer_manual[RYOS_STORED_LIGHTS_LAYER_NUM];
	RyosMacro light_macro; /* screensaver */
} __attribute__ ((packed));

gboolean ryostkl_profile_data_hardware_get_modified(RyostklProfileDataHardware const *profile_data);
void ryostkl_profile_data_hardware_set_modified(RyostklProfileDataHardware *profile_data);
void ryostkl_profile_data_hardware_set_unmodified(RyostklProfileDataHardware *profile_data);

void ryostkl_profile_data_hardware_set_keys_primary(RyostklProfileDataHardware *profile_data, RyosKeysPrimary const *keys_primary);
void ryostkl_profile_data_hardware_set_keys_function(RyostklProfileDataHardware *profile_data, RyosKeysFunction const *keys_function);
void ryostkl_profile_data_hardware_set_keys_extra(RyostklProfileDataHardware *profile_data, RyosKeysExtra const *keys_extra);
void ryostkl_profile_data_hardware_set_keys_thumbster(RyostklProfileDataHardware *profile_data, RyosKeysThumbster const *keys_thumbster);
void ryostkl_profile_data_hardware_set_keys_easyzone(RyostklProfileDataHardware *profile_data, RyosKeysEasyzone const *keys_easyzone);
void ryostkl_profile_data_hardware_set_macro(RyostklProfileDataHardware *profile_data, guint index, RyosMacro const *macro);
void ryostkl_profile_data_hardware_set_key_mask(RyostklProfileDataHardware *profile_data, RyosKeyMask const *key_mask);
void ryostkl_profile_data_hardware_set_light(RyostklProfileDataHardware *profile_data, RyosLight const *light);
void ryostkl_profile_data_hardware_set_light_layer_automatic(RyostklProfileDataHardware *profile_data, guint index, RyostklLightLayer const *light_layer);
void ryostkl_profile_data_hardware_set_light_layer_manual(RyostklProfileDataHardware *profile_data, guint index, RyostklLightLayer const *light_layer);
void ryostkl_profile_data_hardware_set_light_macro(RyostklProfileDataHardware *profile_data, RyosMacro const *light_macro);

void ryostkl_profile_data_hardware_update_with_default(RyostklProfileDataHardware *profile_data);
gboolean ryostkl_profile_data_hardware_update(RyostklProfileDataHardware *profile_data, RoccatDevice *device, guint profile_index, GError **error);

gboolean ryostkl_profile_data_hardware_save(RoccatDevice *device, RyostklProfileDataHardware *profile_data, guint profile_index, GError **error);

void ryostkl_profile_data_hardware_set_key_to_macro(RyostklProfileDataHardware *profile_data, guint macro_index);
void ryostkl_profile_data_hardware_set_key_to_macro_without_modified(RyostklProfileDataHardware *profile_data, guint macro_index);

G_END_DECLS

#endif
