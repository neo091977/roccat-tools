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

#include <string.h>
#include "ryostkl_profile_data_hardware.h"

gboolean ryostkl_profile_data_hardware_get_modified(RyostklProfileDataHardware const *profile_data) {
	guint i;

	if (profile_data->modified_keys_primary ||
			profile_data->modified_keys_function ||
			profile_data->modified_keys_thumbster ||
			profile_data->modified_keys_extra ||
			profile_data->modified_keys_easyzone ||
			profile_data->modified_key_mask ||
			profile_data->modified_light ||
			profile_data->modified_light_macro)
		return TRUE;

	for (i = 0; i < RYOS_RKP_KEYS_NUM; ++i)
		if (profile_data->modified_macros[i])
			return TRUE;

	for (i = 0; i < RYOS_STORED_LIGHTS_LAYER_NUM; ++i)
		if (profile_data->modified_light_layer_automatic[i] ||
				profile_data->modified_light_layer_manual[i])
			return TRUE;

	return FALSE;
}

static void ryostkl_profile_data_hardware_set_modified_state(RyostklProfileDataHardware *profile_data, gboolean state) {
	guint i;

	profile_data->modified_keys_primary = state;
	profile_data->modified_keys_function = state;
	profile_data->modified_keys_thumbster = state;
	profile_data->modified_keys_extra = state;
	profile_data->modified_keys_easyzone = state;
	profile_data->modified_key_mask = state;
	profile_data->modified_light = state;
	profile_data->modified_light_macro = state;

	for (i = 0; i < RYOS_RKP_KEYS_NUM; ++i)
		profile_data->modified_macros[i] = state;

	for (i = 0; i < RYOS_STORED_LIGHTS_LAYER_NUM; ++i) {
		profile_data->modified_light_layer_automatic[i] = state;
		profile_data->modified_light_layer_manual[i] = state;
	}
}

void ryostkl_profile_data_hardware_set_modified(RyostklProfileDataHardware *profile_data) {
	ryostkl_profile_data_hardware_set_modified_state(profile_data, TRUE);
}

void ryostkl_profile_data_hardware_set_unmodified(RyostklProfileDataHardware *profile_data) {
	ryostkl_profile_data_hardware_set_modified_state(profile_data, FALSE);
}

void ryostkl_profile_data_hardware_set_keys_primary(RyostklProfileDataHardware *profile_data, RyosKeysPrimary const *keys_primary) {
	if (!ryos_keys_primary_equal(&profile_data->keys_primary, keys_primary)) {
		ryos_keys_primary_copy(&profile_data->keys_primary, keys_primary);
		profile_data->modified_keys_primary = TRUE;
	}
}

void ryostkl_profile_data_hardware_set_keys_function(RyostklProfileDataHardware *profile_data, RyosKeysFunction const *keys_function) {
	if (!ryos_keys_function_equal(&profile_data->keys_function, keys_function)) {
		ryos_keys_function_copy(&profile_data->keys_function, keys_function);
		profile_data->modified_keys_function = TRUE;
	}
}

void ryostkl_profile_data_hardware_set_keys_extra(RyostklProfileDataHardware *profile_data, RyosKeysExtra const *keys_extra) {
	if (!ryos_keys_extra_equal(&profile_data->keys_extra, keys_extra)) {
		ryos_keys_extra_copy(&profile_data->keys_extra, keys_extra);
		profile_data->modified_keys_extra = TRUE;
	}
}

void ryostkl_profile_data_hardware_set_keys_thumbster(RyostklProfileDataHardware *profile_data, RyosKeysThumbster const *keys_thumbster) {
	if (!ryos_keys_thumbster_equal(&profile_data->keys_thumbster, keys_thumbster)) {
		ryos_keys_thumbster_copy(&profile_data->keys_thumbster, keys_thumbster);
		profile_data->modified_keys_thumbster = TRUE;
	}
}

void ryostkl_profile_data_hardware_set_keys_easyzone(RyostklProfileDataHardware *profile_data, RyosKeysEasyzone const *keys_easyzone) {
	if (!ryos_keys_easyzone_equal(&profile_data->keys_easyzone, keys_easyzone)) {
		ryos_keys_easyzone_copy(&profile_data->keys_easyzone, keys_easyzone);
		profile_data->modified_keys_easyzone = TRUE;
	}
}

void ryostkl_profile_data_hardware_set_macro(RyostklProfileDataHardware *profile_data, guint index, RyosMacro const *macro) {
	if (!ryos_macro_equal(&profile_data->macros[index], macro)) {
		ryos_macro_copy(&profile_data->macros[index], macro);
		profile_data->modified_macros[index] = TRUE;
	}
}

void ryostkl_profile_data_hardware_set_key_mask(RyostklProfileDataHardware *profile_data, RyosKeyMask const *key_mask) {
	if (!ryos_key_mask_equal(&profile_data->key_mask, key_mask)) {
		ryos_key_mask_copy(&profile_data->key_mask, key_mask);
		profile_data->modified_key_mask = TRUE;
	}
}

void ryostkl_profile_data_hardware_set_light(RyostklProfileDataHardware *profile_data, RyosLight const *light) {
	if (!ryos_light_equal(&profile_data->light, light)) {
		ryos_light_copy(&profile_data->light, light);
		profile_data->modified_light = TRUE;
	}
}

void ryostkl_profile_data_hardware_set_light_macro(RyostklProfileDataHardware *profile_data, RyosMacro const *light_macro) {
	if (!ryos_macro_equal(&profile_data->light_macro, light_macro)) {
		ryos_macro_copy(&profile_data->light_macro, light_macro);
		profile_data->modified_light_macro = TRUE;
	}
}

void ryostkl_profile_data_hardware_set_light_layer_automatic(RyostklProfileDataHardware *profile_data, guint index, RyostklLightLayer const *light_layer) {
	if (!ryostkl_light_layer_equal(&profile_data->light_layer_automatic[index], light_layer)) {
		ryostkl_light_layer_copy(&profile_data->light_layer_automatic[index], light_layer);
		profile_data->modified_light_layer_automatic[index] = TRUE;
	}
}

void ryostkl_profile_data_hardware_set_light_layer_manual(RyostklProfileDataHardware *profile_data, guint index, RyostklLightLayer const *light_layer) {
	if (!ryostkl_light_layer_equal(&profile_data->light_layer_manual[index], light_layer)) {
		ryostkl_light_layer_copy(&profile_data->light_layer_manual[index], light_layer);
		profile_data->modified_light_layer_manual[index] = TRUE;
	}
}

void ryostkl_profile_data_hardware_update_with_default(RyostklProfileDataHardware *profile_data) {
	guint index;

	memset(profile_data, 0, sizeof(RyostklProfileDataHardware));

	ryostkl_profile_data_hardware_set_keys_primary(profile_data, ryos_keys_primary_default());
	ryostkl_profile_data_hardware_set_keys_function(profile_data, ryos_keys_function_default());
	ryostkl_profile_data_hardware_set_keys_extra(profile_data, ryos_keys_extra_default());
	ryostkl_profile_data_hardware_set_key_mask(profile_data, ryos_key_mask_default());

	ryostkl_profile_data_hardware_set_light(profile_data, ryos_light_default());
	/* defaults don't contain light_macro */

	ryostkl_profile_data_hardware_set_keys_thumbster(profile_data, ryos_keys_thumbster_default());
	ryostkl_profile_data_hardware_set_keys_easyzone(profile_data, ryos_keys_easyzone_default());
	/* defaults don't contain macros */

	ryostkl_profile_data_hardware_set_light_layer_automatic(profile_data, RYOS_STORED_LIGHTS_LAYER_NORMAL, ryostkl_light_layer_normal_default());
	ryostkl_profile_data_hardware_set_light_layer_automatic(profile_data, RYOS_STORED_LIGHTS_LAYER_EASY_SHIFT, ryostkl_light_layer_easy_shift_default());
	ryostkl_profile_data_hardware_set_light_layer_automatic(profile_data, RYOS_STORED_LIGHTS_LAYER_FN, ryostkl_light_layer_fn_default());

	for (index = RYOS_STORED_LIGHTS_LAYER_NORMAL; index <RYOS_STORED_LIGHTS_LAYER_EASY_SHIFT; ++index)
		ryostkl_profile_data_hardware_set_light_layer_manual(profile_data, index, ryostkl_light_layer_normal_default());
	ryostkl_profile_data_hardware_set_light_layer_manual(profile_data, RYOS_STORED_LIGHTS_LAYER_EASY_SHIFT, ryostkl_light_layer_easy_shift_default());
	ryostkl_profile_data_hardware_set_light_layer_manual(profile_data, RYOS_STORED_LIGHTS_LAYER_FN, ryostkl_light_layer_fn_default());

	ryostkl_profile_data_hardware_set_modified(profile_data);
}

gboolean ryostkl_profile_data_hardware_update(RyostklProfileDataHardware *profile_data, RoccatDevice *device, guint profile_index, GError **error) {
	guint index;
	guint macro_index;
	void *pointer;

	ryostkl_profile_data_hardware_set_modified(profile_data);

	pointer = ryos_keys_primary_read(device, profile_index, error);
	if (!pointer)
		return FALSE;
	ryostkl_profile_data_hardware_set_keys_primary(profile_data, (RyosKeysPrimary const *)pointer);
	g_free(pointer);

	pointer = ryos_keys_function_read(device, profile_index, error);
	if (!pointer)
		return FALSE;
	ryostkl_profile_data_hardware_set_keys_function(profile_data, (RyosKeysFunction const *)pointer);
	g_free(pointer);

	pointer = ryos_keys_extra_read(device, profile_index, error);
	if (!pointer)
		return FALSE;
	ryostkl_profile_data_hardware_set_keys_extra(profile_data, (RyosKeysExtra const *)pointer);
	g_free(pointer);

	pointer = ryos_keys_thumbster_read(device, profile_index, error);
	if (!pointer)
		return FALSE;
	ryostkl_profile_data_hardware_set_keys_thumbster(profile_data, (RyosKeysThumbster const *)pointer);
	g_free(pointer);

	for (index = 0; index < RYOS_KEYS_THUMBSTER_NUM * 2; ++index) {
		macro_index = ryos_keys_thumbster_index_to_macro_index(index);
		if (profile_data->keys_thumbster.keys[index].type == RYOS_KEY_TYPE_MACRO) {
			pointer = ryos_macro_read(device, profile_index, macro_index, error);
			if (!pointer)
				return FALSE;
			ryostkl_profile_data_hardware_set_macro(profile_data, macro_index, (RyosMacro const *)pointer);
			g_free(pointer);
		}
	}

	pointer = ryos_keys_easyzone_read(device, profile_index, error);
	if (!pointer)
		return FALSE;
	ryostkl_profile_data_hardware_set_keys_easyzone(profile_data, (RyosKeysEasyzone const *)pointer);
	g_free(pointer);

	for (index = 0; index < RYOS_KEYS_EASYZONE_NUM; ++index) {
		macro_index = ryos_keys_easyzone_index_to_macro_index(index);
		if (profile_data->keys_easyzone.keys[index].type == RYOS_KEY_TYPE_MACRO) {
			pointer = ryos_macro_read(device, profile_index, macro_index, error);
			if (!pointer)
				return FALSE;
			ryostkl_profile_data_hardware_set_macro(profile_data, macro_index, (RyosMacro const *)pointer);
			g_free(pointer);
		}
	}

	pointer = ryos_key_mask_read(device, profile_index, error);
	if (!pointer)
		return FALSE;
	ryostkl_profile_data_hardware_set_key_mask(profile_data, (RyosKeyMask const *)pointer);
	g_free(pointer);

	pointer = ryos_light_read(device, profile_index, error);
	if (!pointer)
		return FALSE;
	ryostkl_profile_data_hardware_set_light(profile_data, (RyosLight const *)pointer);
	g_free(pointer);

	if (profile_data->light.dimness_type == RYOS_LIGHT_DIMNESS_TYPE_MACRO) {
		pointer = ryos_light_macro_read(device, profile_index, error);
		if (!pointer)
			return FALSE;
		ryostkl_profile_data_hardware_set_light_macro(profile_data, (RyosMacro const *)pointer);
		g_free(pointer);
	}

	pointer = ryostkl_light_layer_read(device, profile_index, RYOS_ILLUMINATION_MODE_AUTOMATIC, RYOS_STORED_LIGHTS_LAYER_NORMAL, error);
	if (!pointer)
		return FALSE;
	ryostkl_profile_data_hardware_set_light_layer_automatic(profile_data, RYOS_STORED_LIGHTS_LAYER_NORMAL, (RyostklLightLayer const *)pointer);
	g_free(pointer);

	pointer = ryostkl_light_layer_read(device, profile_index, RYOS_ILLUMINATION_MODE_AUTOMATIC, RYOS_STORED_LIGHTS_LAYER_EASY_SHIFT, error);
	if (!pointer)
		return FALSE;
	ryostkl_profile_data_hardware_set_light_layer_automatic(profile_data, RYOS_STORED_LIGHTS_LAYER_EASY_SHIFT, (RyostklLightLayer const *)pointer);
	g_free(pointer);

	pointer = ryostkl_light_layer_read(device, profile_index, RYOS_ILLUMINATION_MODE_AUTOMATIC, RYOS_STORED_LIGHTS_LAYER_FN, error);
	if (!pointer)
		return FALSE;
	ryostkl_profile_data_hardware_set_light_layer_automatic(profile_data, RYOS_STORED_LIGHTS_LAYER_FN, (RyostklLightLayer const *)pointer);
	g_free(pointer);

	for (index = 0; index < RYOS_STORED_LIGHTS_LAYER_NUM; ++index) {
		pointer = ryostkl_light_layer_read(device, profile_index, RYOS_ILLUMINATION_MODE_MANUAL, index, error);
		if (!pointer)
			return FALSE;
		ryostkl_profile_data_hardware_set_light_layer_manual(profile_data, index, (RyostklLightLayer const *)pointer);
		g_free(pointer);
	}

	ryostkl_profile_data_hardware_set_unmodified(profile_data);
	return TRUE;
}

gboolean ryostkl_profile_data_hardware_save(RoccatDevice *device, RyostklProfileDataHardware *profile_data, guint profile_index, GError **error) {
	guint index;
	guint macro_index;

	if (profile_data->modified_keys_primary)
		if (!ryos_keys_primary_write(device, profile_index, &profile_data->keys_primary, error))
			return FALSE;

	if (profile_data->modified_keys_function)
		if (!ryos_keys_function_write(device, profile_index, &profile_data->keys_function, error))
			return FALSE;

	if (profile_data->modified_keys_extra)
		if (!ryos_keys_extra_write(device, profile_index, &profile_data->keys_extra, error))
			return FALSE;

	if (profile_data->modified_keys_thumbster)
		if (!ryos_keys_thumbster_write(device, profile_index, &profile_data->keys_thumbster, error))
			return FALSE;

	for (index = 0; index < RYOS_KEYS_THUMBSTER_NUM * 2; ++index) {
		macro_index = ryos_keys_thumbster_index_to_macro_index(index);
		if (profile_data->keys_thumbster.keys[index].type == RYOS_KEY_TYPE_MACRO && profile_data->modified_macros[macro_index]) {
			if (!ryos_macro_write(device, profile_index, macro_index, &profile_data->macros[macro_index], error))
				return FALSE;
		}
	}

	if (profile_data->modified_keys_easyzone)
		if (!ryos_keys_easyzone_write(device, profile_index, &profile_data->keys_easyzone, error))
			return FALSE;

	for (index = 0; index < RYOS_KEYS_EASYZONE_NUM; ++index) {
		macro_index = ryos_keys_easyzone_index_to_macro_index(index);
		if (profile_data->keys_easyzone.keys[index].type == RYOS_KEY_TYPE_MACRO && profile_data->modified_macros[macro_index]) {
			if (!ryos_macro_write(device, profile_index, macro_index, &profile_data->macros[macro_index], error))
				return FALSE;
		}
	}

	if (profile_data->modified_key_mask)
		if (!ryos_key_mask_write(device, profile_index, &profile_data->key_mask, error))
			return FALSE;

	if (profile_data->modified_light)
		if (!ryos_light_write(device, profile_index, &profile_data->light, error))
			return FALSE;

	if (profile_data->light.dimness_type == RYOS_LIGHT_DIMNESS_TYPE_MACRO && profile_data->modified_light_macro) {
		if (!ryos_light_macro_write(device, profile_index, &profile_data->light_macro, error))
			return FALSE;
	}

	if (profile_data->modified_light_layer_automatic[RYOS_STORED_LIGHTS_LAYER_NORMAL])
		if (!ryostkl_light_layer_write(device, profile_index, RYOS_ILLUMINATION_MODE_AUTOMATIC, RYOS_STORED_LIGHTS_LAYER_NORMAL, &profile_data->light_layer_automatic[RYOS_STORED_LIGHTS_LAYER_NORMAL], error))
			return FALSE;

	if (profile_data->modified_light_layer_automatic[RYOS_STORED_LIGHTS_LAYER_EASY_SHIFT])
		if (!ryostkl_light_layer_write(device, profile_index, RYOS_ILLUMINATION_MODE_AUTOMATIC, RYOS_STORED_LIGHTS_LAYER_EASY_SHIFT, &profile_data->light_layer_automatic[RYOS_STORED_LIGHTS_LAYER_EASY_SHIFT], error))
			return FALSE;

	if (profile_data->modified_light_layer_automatic[RYOS_STORED_LIGHTS_LAYER_FN])
		if (!ryostkl_light_layer_write(device, profile_index, RYOS_ILLUMINATION_MODE_AUTOMATIC, RYOS_STORED_LIGHTS_LAYER_FN, &profile_data->light_layer_automatic[RYOS_STORED_LIGHTS_LAYER_FN], error))
			return FALSE;

	for (index = 0; index < RYOS_STORED_LIGHTS_LAYER_NUM; ++index) {
		if (profile_data->modified_light_layer_manual[index])
			if (!ryostkl_light_layer_write(device, profile_index, RYOS_ILLUMINATION_MODE_MANUAL, index, &profile_data->light_layer_manual[index], error))
				return FALSE;
	}

	ryostkl_profile_data_hardware_set_unmodified(profile_data);
	return TRUE;
}

void ryostkl_profile_data_hardware_set_key_to_macro(RyostklProfileDataHardware *profile_data, guint macro_index) {
	RoccatButton *key;

	if (ryos_macro_index_is_keys_easyzone(macro_index)) {
		key = &profile_data->keys_easyzone.keys[ryos_macro_index_to_keys_easyzone_index(macro_index)];
		if (key->type != RYOS_KEY_TYPE_MACRO) {
			roccat_button_set_to_normal(key, RYOS_KEY_TYPE_MACRO);
			profile_data->modified_keys_easyzone = TRUE;
		}
	} else if (ryos_macro_index_is_keys_thumbster(macro_index)) {
		key = &profile_data->keys_thumbster.keys[ryos_macro_index_to_keys_thumbster_index(macro_index)];
		if (key->type != RYOS_KEY_TYPE_MACRO) {
			roccat_button_set_to_normal(key, RYOS_KEY_TYPE_MACRO);
			profile_data->modified_keys_thumbster = TRUE;
		}
	} else {
		; // TODO error
	}
}

void ryostkl_profile_data_hardware_set_key_to_macro_without_modified(RyostklProfileDataHardware *profile_data, guint macro_index) {
	RoccatButton *key;

	if (ryos_macro_index_is_keys_easyzone(macro_index)) {
		key = &profile_data->keys_easyzone.keys[ryos_macro_index_to_keys_easyzone_index(macro_index)];
		roccat_button_set_to_normal(key, RYOS_KEY_TYPE_MACRO);
	} else if (ryos_macro_index_is_keys_thumbster(macro_index)) {
		key = &profile_data->keys_thumbster.keys[ryos_macro_index_to_keys_thumbster_index(macro_index)];
		roccat_button_set_to_normal(key, RYOS_KEY_TYPE_MACRO);
	} else {
		; // TODO error
	}
}
