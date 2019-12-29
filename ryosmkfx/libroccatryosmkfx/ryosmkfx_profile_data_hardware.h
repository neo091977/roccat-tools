#ifndef __ROCCAT_RYOSMKFX_PROFILE_DATA_HARDWARE_H__
#define __ROCCAT_RYOSMKFX_PROFILE_DATA_HARDWARE_H__

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
#include "ryos_keys_macro.h"
#include "ryos_keys_thumbster.h"
#include "ryos_keys_extra.h"
#include "ryos_keys_easyzone.h"
#include "ryos_key_mask.h"
#include "ryosmkfx_light.h"
#include "ryosmkfx_macro.h"
#include "ryosmkfx_stored_lights.h"

G_BEGIN_DECLS

typedef struct _RyosmkfxProfileDataHardware RyosmkfxProfileDataHardware;

struct _RyosmkfxProfileDataHardware {
	gboolean modified_keys_primary;
	gboolean modified_keys_function;
	gboolean modified_keys_macro;
	gboolean modified_keys_thumbster;
	gboolean modified_keys_extra;
	gboolean modified_keys_easyzone;
	gboolean modified_key_mask;
	gboolean modified_light;
	gboolean modified_stored_lights;
	gboolean modified_macro[RYOS_RKP_KEYS_NUM];

	RyosKeysPrimary keys_primary;
	RyosKeysFunction keys_function;
	RyosKeysMacro keys_macro;
	RyosKeysThumbster keys_thumbster;
	RyosKeysExtra keys_extra;
	RyosKeysEasyzone keys_easyzone;
	RyosKeyMask key_mask;
	RyosmkfxLight light;
	RyosmkfxStoredLights stored_lights;
	RyosmkfxMacro macros[RYOS_RKP_KEYS_NUM];
	// Somethings[RYOS_KEYS_PRIMARY_NUM]?
};

gboolean ryosmkfx_profile_data_hardware_get_modified(RyosmkfxProfileDataHardware const *profile_data);
void ryosmkfx_profile_data_hardware_set_modified(RyosmkfxProfileDataHardware *profile_data);
void ryosmkfx_profile_data_hardware_set_unmodified(RyosmkfxProfileDataHardware *profile_data);

void ryosmkfx_profile_data_hardware_set_keys_primary(RyosmkfxProfileDataHardware *profile_data, RyosKeysPrimary const *keys_primary);
void ryosmkfx_profile_data_hardware_set_keys_function(RyosmkfxProfileDataHardware *profile_data, RyosKeysFunction const *keys_function);
void ryosmkfx_profile_data_hardware_set_keys_macro(RyosmkfxProfileDataHardware *profile_data, RyosKeysMacro const *keys_macro);
void ryosmkfx_profile_data_hardware_set_keys_thumbster(RyosmkfxProfileDataHardware *profile_data, RyosKeysThumbster const *keys_thumbster);
void ryosmkfx_profile_data_hardware_set_keys_extra(RyosmkfxProfileDataHardware *profile_data, RyosKeysExtra const *keys_extra);
void ryosmkfx_profile_data_hardware_set_keys_easyzone(RyosmkfxProfileDataHardware *profile_data, RyosKeysEasyzone const *keys_easyzone);
void ryosmkfx_profile_data_hardware_set_key_mask(RyosmkfxProfileDataHardware *profile_data, RyosKeyMask const *key_mask);
void ryosmkfx_profile_data_hardware_set_light(RyosmkfxProfileDataHardware *profile_data, RyosmkfxLight const *light);
void ryosmkfx_profile_data_hardware_set_stored_lights(RyosmkfxProfileDataHardware *profile_data, RyosmkfxStoredLights const *stored_lights);
void ryosmkfx_profile_data_hardware_set_macro(RyosmkfxProfileDataHardware *profile_data, guint index, RyosmkfxMacro const *macro);

void ryosmkfx_profile_data_hardware_set_key_to_macro_without_modified(RyosmkfxProfileDataHardware *profile_data, guint macro_index);

void ryosmkfx_profile_data_hardware_update_with_default(RyosmkfxProfileDataHardware *profile_data);
gboolean ryosmkfx_profile_data_hardware_update(RyosmkfxProfileDataHardware *profile_data, RoccatDevice *device, guint profile_index, GError **error);

gboolean ryosmkfx_profile_data_hardware_save(RoccatDevice *device, RyosmkfxProfileDataHardware *profile_data, guint profile_index, GError **error);

G_END_DECLS

#endif
