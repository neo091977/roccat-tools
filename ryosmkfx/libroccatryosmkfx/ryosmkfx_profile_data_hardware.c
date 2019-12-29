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

#include "ryosmkfx_profile_data_hardware.h"
#include <string.h>

gboolean ryosmkfx_profile_data_hardware_get_modified(RyosmkfxProfileDataHardware const *profile_data) {
	guint i;

	if (profile_data->modified_keys_primary ||
			profile_data->modified_keys_function ||
			profile_data->modified_keys_macro ||
			profile_data->modified_keys_thumbster ||
			profile_data->modified_keys_extra ||
			profile_data->modified_keys_easyzone ||
			profile_data->modified_key_mask ||
			profile_data->modified_light ||
			profile_data->modified_stored_lights)
		return TRUE;

	for (i = 0; i < RYOS_RKP_KEYS_NUM; ++i)
		if (profile_data->modified_macro[i])
			return TRUE;

	return FALSE;
}

static void ryosmkfx_profile_data_hardware_set_modified_state(RyosmkfxProfileDataHardware *profile_data, gboolean state) {
	guint i;

	profile_data->modified_keys_primary = state;
	profile_data->modified_keys_function = state;
	profile_data->modified_keys_macro = state;
	profile_data->modified_keys_thumbster = state;
	profile_data->modified_keys_extra = state;
	profile_data->modified_keys_easyzone = state;
	profile_data->modified_key_mask = state;
	profile_data->modified_light = state;
	profile_data->modified_stored_lights = state;

	for (i = 0; i < RYOS_RKP_KEYS_NUM; ++i)
		profile_data->modified_macro[i] = state;
}

void ryosmkfx_profile_data_hardware_set_modified(RyosmkfxProfileDataHardware *profile_data) {
	ryosmkfx_profile_data_hardware_set_modified_state(profile_data, TRUE);
}

void ryosmkfx_profile_data_hardware_set_unmodified(RyosmkfxProfileDataHardware *profile_data) {
	ryosmkfx_profile_data_hardware_set_modified_state(profile_data, FALSE);
}

void ryosmkfx_profile_data_hardware_set_keys_primary(RyosmkfxProfileDataHardware *profile_data, RyosKeysPrimary const *keys_primary) {
	if (!ryos_keys_primary_equal(&profile_data->keys_primary, keys_primary)) {
		ryos_keys_primary_copy(&profile_data->keys_primary, keys_primary);
		profile_data->modified_keys_primary = TRUE;
	}
}

void ryosmkfx_profile_data_hardware_set_keys_function(RyosmkfxProfileDataHardware *profile_data, RyosKeysFunction const *keys_function) {
	if (!ryos_keys_function_equal(&profile_data->keys_function, keys_function)) {
		ryos_keys_function_copy(&profile_data->keys_function, keys_function);
		profile_data->modified_keys_function = TRUE;
	}
}

void ryosmkfx_profile_data_hardware_set_keys_macro(RyosmkfxProfileDataHardware *profile_data, RyosKeysMacro const *keys_macro) {
	if (!ryos_keys_macro_equal(&profile_data->keys_macro, keys_macro)) {
		ryos_keys_macro_copy(&profile_data->keys_macro, keys_macro);
		profile_data->modified_keys_macro = TRUE;
	}
}

void ryosmkfx_profile_data_hardware_set_keys_thumbster(RyosmkfxProfileDataHardware *profile_data, RyosKeysThumbster const *keys_thumbster) {
	if (!ryos_keys_thumbster_equal(&profile_data->keys_thumbster, keys_thumbster)) {
		ryos_keys_thumbster_copy(&profile_data->keys_thumbster, keys_thumbster);
		profile_data->modified_keys_thumbster = TRUE;
	}
}

void ryosmkfx_profile_data_hardware_set_keys_extra(RyosmkfxProfileDataHardware *profile_data, RyosKeysExtra const *keys_extra) {
	if (!ryos_keys_extra_equal(&profile_data->keys_extra, keys_extra)) {
		ryos_keys_extra_copy(&profile_data->keys_extra, keys_extra);
		profile_data->modified_keys_extra = TRUE;
	}
}

void ryosmkfx_profile_data_hardware_set_keys_easyzone(RyosmkfxProfileDataHardware *profile_data, RyosKeysEasyzone const *keys_easyzone) {
	if (!ryos_keys_easyzone_equal(&profile_data->keys_easyzone, keys_easyzone)) {
		ryos_keys_easyzone_copy(&profile_data->keys_easyzone, keys_easyzone);
		profile_data->modified_keys_easyzone = TRUE;
	}
}

void ryosmkfx_profile_data_hardware_set_key_mask(RyosmkfxProfileDataHardware *profile_data, RyosKeyMask const *key_mask) {
	if (!ryos_key_mask_equal(&profile_data->key_mask, key_mask)) {
		ryos_key_mask_copy(&profile_data->key_mask, key_mask);
		profile_data->modified_key_mask = TRUE;
	}
}

void ryosmkfx_profile_data_hardware_set_light(RyosmkfxProfileDataHardware *profile_data, RyosmkfxLight const *light) {
	if (!ryosmkfx_light_equal(&profile_data->light, light)) {
		ryosmkfx_light_copy(&profile_data->light, light);
		profile_data->modified_light = TRUE;
	}
}

void ryosmkfx_profile_data_hardware_set_stored_lights(RyosmkfxProfileDataHardware *profile_data, RyosmkfxStoredLights const *stored_lights) {
	if (!ryosmkfx_stored_lights_equal(&profile_data->stored_lights, stored_lights)) {
		ryosmkfx_stored_lights_copy(&profile_data->stored_lights, stored_lights);
		profile_data->modified_stored_lights = TRUE;
	}
}

void ryosmkfx_profile_data_hardware_set_macro(RyosmkfxProfileDataHardware *profile_data, guint index, RyosmkfxMacro const *macro) {
	if (!ryosmkfx_macro_equal(&profile_data->macros[index], macro)) {
		ryosmkfx_macro_copy(&profile_data->macros[index], macro);
		profile_data->modified_macro[index] = TRUE;
	}
}

void ryosmkfx_profile_data_hardware_set_key_to_macro_without_modified(RyosmkfxProfileDataHardware *profile_data, guint macro_index) {
	if (ryos_macro_index_is_keys_easyzone(macro_index)) {
		roccat_button_set_to_normal(&profile_data->keys_easyzone.keys[ryos_macro_index_to_keys_easyzone_index(macro_index)], RYOS_KEY_TYPE_MACRO);
	} else if (ryos_macro_index_is_keys_macro(macro_index)) {
		roccat_button_set_to_normal(&profile_data->keys_macro.keys[ryos_macro_index_to_keys_macro_index(macro_index)], RYOS_KEY_TYPE_MACRO);
	} else if (ryos_macro_index_is_keys_thumbster(macro_index)) {
		roccat_button_set_to_normal(&profile_data->keys_thumbster.keys[ryos_macro_index_to_keys_thumbster_index(macro_index)], RYOS_KEY_TYPE_MACRO);
	} else {
		; // error
	}
}

void ryosmkfx_profile_data_hardware_update_with_default(RyosmkfxProfileDataHardware *profile_data) {
	memset(profile_data, 0, sizeof(RyosmkfxProfileDataHardware));
	ryosmkfx_profile_data_hardware_set_keys_primary(profile_data, ryos_keys_primary_default());
	ryosmkfx_profile_data_hardware_set_keys_function(profile_data, ryos_keys_function_default());
	ryosmkfx_profile_data_hardware_set_keys_macro(profile_data, ryos_keys_macro_default());
	ryosmkfx_profile_data_hardware_set_keys_thumbster(profile_data, ryos_keys_thumbster_default());
	ryosmkfx_profile_data_hardware_set_keys_extra(profile_data, ryos_keys_extra_default());
	ryosmkfx_profile_data_hardware_set_keys_easyzone(profile_data, ryos_keys_easyzone_default());
	ryosmkfx_profile_data_hardware_set_key_mask(profile_data, ryos_key_mask_default());
	ryosmkfx_profile_data_hardware_set_light(profile_data, ryosmkfx_light_default());
	ryosmkfx_profile_data_hardware_set_stored_lights(profile_data, ryosmkfx_stored_lights_default());
	ryosmkfx_profile_data_hardware_set_modified(profile_data);
}

gboolean ryosmkfx_profile_data_hardware_update(RyosmkfxProfileDataHardware *profile_data, RoccatDevice *device, guint profile_index, GError **error) {
	RyosKeysPrimary *keys_primary;
	RyosKeysFunction *keys_function;
	RyosKeysMacro *keys_macro;
	RyosKeysThumbster *keys_thumbster;
	RyosKeysExtra *keys_extra;
	RyosKeysEasyzone *keys_easyzone;
	RyosKeyMask *key_mask;
	RyosmkfxLight *light;
	RyosmkfxStoredLights *stored_lights;
	RyosmkfxMacro *macro;
	guint i;
	guint button_index;

	keys_primary = ryos_keys_primary_read(device, profile_index, error);
	if (!keys_primary)
		return FALSE;
	ryosmkfx_profile_data_hardware_set_keys_primary(profile_data, keys_primary);
	g_free(keys_primary);

	keys_function = ryos_keys_function_read(device, profile_index, error);
	if (!keys_function)
		return FALSE;
	ryosmkfx_profile_data_hardware_set_keys_function(profile_data, keys_function);
	g_free(keys_function);

	keys_macro = ryos_keys_macro_read(device, profile_index, error);
	if (!keys_macro)
		return FALSE;
	ryosmkfx_profile_data_hardware_set_keys_macro(profile_data, keys_macro);
	for (i = 0; i < RYOS_KEYS_MACRO_NUM * 2; ++i) {
		if (keys_macro->keys[i].type == RYOS_KEY_TYPE_MACRO) {
			button_index = ryos_keys_macro_index_to_macro_index(i);
			macro = ryosmkfx_macro_read(device, profile_index, button_index, error);
			if (!macro)
				return FALSE;
			ryosmkfx_profile_data_hardware_set_macro(profile_data, button_index, macro);
			g_free(macro);
		}
	}
	g_free(keys_macro);

	keys_thumbster = ryos_keys_thumbster_read(device, profile_index, error);
	if (!keys_thumbster)
		return FALSE;
	ryosmkfx_profile_data_hardware_set_keys_thumbster(profile_data, keys_thumbster);
	for (i = 0; i < RYOS_KEYS_THUMBSTER_NUM * 2; ++i) {
		if (keys_thumbster->keys[i].type == RYOS_KEY_TYPE_MACRO) {
			button_index = ryos_keys_thumbster_index_to_macro_index(i);
			macro = ryosmkfx_macro_read(device, profile_index, button_index, error);
			if (!macro)
				return FALSE;
			ryosmkfx_profile_data_hardware_set_macro(profile_data, button_index, macro);
			g_free(macro);
		}
	}
	g_free(keys_thumbster);

	keys_extra = ryos_keys_extra_read(device, profile_index, error);
	if (!keys_extra)
		return FALSE;
	ryosmkfx_profile_data_hardware_set_keys_extra(profile_data, keys_extra);
	g_free(keys_extra);

	keys_easyzone = ryos_keys_easyzone_read(device, profile_index, error);
	if (!keys_easyzone)
		return FALSE;
	ryosmkfx_profile_data_hardware_set_keys_easyzone(profile_data, keys_easyzone);
	for (i = 0; i < RYOS_KEYS_EASYZONE_NUM; ++i) {
		if (keys_easyzone->keys[i].type == RYOS_KEY_TYPE_MACRO) {
			button_index = ryos_keys_easyzone_index_to_macro_index(i);
			macro = ryosmkfx_macro_read(device, profile_index, button_index, error);
			if (!macro)
				return FALSE;
			ryosmkfx_profile_data_hardware_set_macro(profile_data, button_index, macro);
			g_free(macro);
		}
	}
	g_free(keys_easyzone);

	key_mask = ryos_key_mask_read(device, profile_index, error);
	if (!key_mask)
		return FALSE;
	ryosmkfx_profile_data_hardware_set_key_mask(profile_data, key_mask);
	g_free(key_mask);

	light = ryosmkfx_light_read(device, profile_index, error);
	if (!light)
		return FALSE;
	ryosmkfx_profile_data_hardware_set_light(profile_data, light);
	g_free(light);

	stored_lights = ryosmkfx_stored_lights_read(device, profile_index, error);
	if (!stored_lights)
		return FALSE;
	ryosmkfx_profile_data_hardware_set_stored_lights(profile_data, stored_lights);
	g_free(stored_lights);

	ryosmkfx_profile_data_hardware_set_unmodified(profile_data);

	return TRUE;
}

static gboolean should_save_macro(RyosmkfxProfileDataHardware const *hardware, guint macro_index) {
	guint type = RYOS_KEY_TYPE_DISABLED;

	if (ryos_macro_index_is_keys_easyzone(macro_index))
		type = hardware->keys_easyzone.keys[ryos_macro_index_to_keys_easyzone_index(macro_index)].type;
	else if (ryos_macro_index_is_keys_macro(macro_index))
		type = hardware->keys_macro.keys[ryos_macro_index_to_keys_macro_index(macro_index)].type;
	else if (ryos_macro_index_is_keys_thumbster(macro_index))
		type = hardware->keys_thumbster.keys[ryos_macro_index_to_keys_thumbster_index(macro_index)].type;

	return type == RYOS_KEY_TYPE_MACRO;
}

gboolean ryosmkfx_profile_data_hardware_save(RoccatDevice *device, RyosmkfxProfileDataHardware *profile_data, guint profile_index, GError **error) {
	guint i;

	if (profile_data->modified_keys_primary)
		if (!ryos_keys_primary_write(device, profile_index, &profile_data->keys_primary, error))
			return FALSE;

	if (profile_data->modified_keys_function)
		if (!ryos_keys_function_write(device, profile_index, &profile_data->keys_function, error))
			return FALSE;

	if (profile_data->modified_keys_macro)
		if (!ryos_keys_macro_write(device, profile_index, &profile_data->keys_macro, error))
			return FALSE;

	if (profile_data->modified_keys_thumbster)
		if (!ryos_keys_thumbster_write(device, profile_index, &profile_data->keys_thumbster, error))
			return FALSE;

	if (profile_data->modified_keys_extra)
		if (!ryos_keys_extra_write(device, profile_index, &profile_data->keys_extra, error))
			return FALSE;

	if (profile_data->modified_keys_easyzone)
		if (!ryos_keys_easyzone_write(device, profile_index, &profile_data->keys_easyzone, error))
			return FALSE;

	if (profile_data->modified_key_mask)
		if (!ryos_key_mask_write(device, profile_index, &profile_data->key_mask, error))
			return FALSE;

	if (profile_data->modified_light)
		if (!ryosmkfx_light_write(device, profile_index, &profile_data->light, error))
			return FALSE;

	if (profile_data->modified_stored_lights)
		if (!ryosmkfx_stored_lights_write(device, profile_index, &profile_data->stored_lights, error))
			return FALSE;

	for (i = 0; i < RYOS_RKP_KEYS_NUM; ++i) {
		if (profile_data->modified_macro[i] && should_save_macro(profile_data, i)) {
			if (!ryosmkfx_macro_write(device, profile_index, i, &profile_data->macros[i], error))
				return FALSE;
		}

	}

	ryosmkfx_profile_data_hardware_set_unmodified(profile_data);

	return TRUE;
}
