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

#include "isku_rkp.h"
#include "roccat_helper.h"
#include "roccat.h"
#include "i18n-lib.h"
#include <string.h>

static gchar *isku_profile_dir(void) {
	gchar *base = roccat_profile_dir();
	gchar *dir = g_build_path("/", base, "isku", NULL);
	g_free(base);
	return dir;
}

static gboolean isku_profile_dir_create_if_needed(GError **error) {
	gchar *dir;
	gboolean retval;

	dir = isku_profile_dir();
	retval = roccat_profile_dir_create_if_needed(dir, error);
	g_free(dir);
	return retval;
}

static gchar *isku_build_config_rkp_path(guint profile_index) {
	gchar *dir = isku_profile_dir();
	gchar *filename = g_strdup_printf("actual%i." ROCCAT_KEYBOARD_PROFILE_EXTENSION, profile_index);
	gchar *path = g_build_path("/", dir, filename, NULL);
	g_free(filename);
	g_free(dir);
	return path;
}

static gboolean isku_rkp_keys_macro_modified(IskuRkp const *rkp) {
	guint i;
	for (i = ISKU_KEY_INDEX_M1; i <= ISKU_KEY_INDEX_SHIFT_M5; ++i)
		if (rkp->modified_keys[i])
			return TRUE;
	return FALSE;
}

static gboolean isku_rkp_keys_easyzone_modified(IskuRkp const *rkp) {
	guint i;
	for (i = ISKU_KEY_INDEX_1; i <= ISKU_KEY_INDEX_B; ++i)
		if (rkp->modified_keys[i])
			return TRUE;
	return FALSE;
}

static gboolean isku_rkp_keys_thumbster_modified(IskuRkp const *rkp) {
	guint i;
	for (i = ISKU_KEY_INDEX_T1; i <= ISKU_KEY_INDEX_SHIFT_T3; ++i)
		if (rkp->modified_keys[i])
			return TRUE;
	return FALSE;
}

static gboolean isku_rkp_keys_function_modified(IskuRkp const *rkp) {
	guint i;
	for (i = ISKU_KEY_INDEX_F1; i <= ISKU_KEY_INDEX_F12; ++i)
		if (rkp->modified_keys[i])
			return TRUE;
	return FALSE;
}

static gboolean isku_rkp_keys_media_modified(IskuRkp const *rkp) {
	guint i;
	for (i = ISKU_KEY_INDEX_MEDIA_MUTE; i <= ISKU_KEY_INDEX_MEDIA_COMPUTER; ++i)
		if (rkp->modified_keys[i])
			return TRUE;
	return FALSE;
}

gboolean isku_rkp_get_modified(IskuRkp const *rkp) {
	guint i;

	if (rkp->modified_light ||
			rkp->modified_key_mask ||
			rkp->modified_rkp)
		return TRUE;

	for (i = 0; i < ISKU_KEY_INDEX_NUM; ++i)
		if (rkp->modified_keys[i])
			return TRUE;

	return FALSE;
}

static void isku_rkp_set_modified_state(IskuRkp *rkp, gboolean state) {
	guint i;

	rkp->modified_rkp = state;
	rkp->modified_light = state;
	rkp->modified_key_mask = state;
	for (i = 0; i < ISKU_KEY_INDEX_NUM; ++i)
		rkp->modified_keys[i] = state;
}

void isku_rkp_set_modified(IskuRkp *rkp) {
	isku_rkp_set_modified_state(rkp, TRUE);
}

void isku_rkp_set_unmodified(IskuRkp *rkp) {
	isku_rkp_set_modified_state(rkp, FALSE);
}

static void isku_rkp_update_with_hardware_key_mask(RoccatDevice *isku, IskuRkp *rkp, guint profile_index) {
	GError *error = NULL;
	IskuKeyMask *hardware_key_mask;
	hardware_key_mask = isku_key_mask_read(isku, profile_index, &error);
	if (hardware_key_mask) {
		isku_rkp_update_with_key_mask(rkp, hardware_key_mask);
		g_free(hardware_key_mask);
	} else
		g_clear_error(&error);
}

static void isku_rkp_update_with_hardware_light(RoccatDevice *isku, IskuRkp *rkp, guint profile_index) {
	GError *error = NULL;
	IskuLight *hardware_light;
	hardware_light = isku_light_read(isku, profile_index, &error);
	if (hardware_light) {
		isku_rkp_update_with_light(rkp, hardware_light);
		g_free(hardware_light);
	} else
		g_clear_error(&error);
}

static void isku_rkp_update_with_hardware_macro(RoccatDevice *isku, IskuRkp *rkp, guint profile_index, guint key_index) {
	GError *error = NULL;
	IskuMacro *hardware_macro;
	hardware_macro = isku_macro_read(isku, profile_index, key_index, &error);
	if (hardware_macro) {
		isku_rkp_update_with_macro(rkp, key_index, hardware_macro);
		g_free(hardware_macro);
	} else
		g_clear_error(&error);
}

static void isku_rkp_update_with_hardware_keys_function(RoccatDevice *isku, IskuRkp *rkp, guint profile_index) {
	GError *error = NULL;
	IskuKeysFunction *hardware_keys;
	hardware_keys = isku_keys_function_read(isku, profile_index, &error);
	if (hardware_keys) {
		isku_rkp_update_with_keys_function(rkp, hardware_keys);
		g_free(hardware_keys);
	} else
		g_clear_error(&error);
}

static void isku_rkp_update_with_hardware_keys_easyzone(RoccatDevice *isku, IskuRkp *rkp, guint profile_index) {
	GError *error = NULL;
	IskuKeysEasyzone *hardware_keys;
	guint i;
	hardware_keys = isku_keys_easyzone_read(isku, profile_index, &error);
	if (hardware_keys) {
		isku_rkp_update_with_keys_easyzone(rkp, hardware_keys);
		for (i = 0; i < ISKU_KEYS_EASYZONE_NUM; ++i) {
			if (hardware_keys && hardware_keys->types[i] == ISKU_KEY_TYPE_MACRO)
				isku_rkp_update_with_hardware_macro(isku, rkp, profile_index, i + ISKU_KEY_INDEX_1);
		}
		g_free(hardware_keys);
	} else
		g_clear_error(&error);
}

static void isku_rkp_update_with_hardware_keys_media(RoccatDevice *isku, IskuRkp *rkp, guint profile_index) {
	GError *error = NULL;
	IskuKeysMedia *hardware_keys;
	hardware_keys = isku_keys_media_read(isku, profile_index, &error);
	if (hardware_keys) {
		isku_rkp_update_with_keys_media(rkp, hardware_keys);
		g_free(hardware_keys);
	} else
		g_clear_error(&error);
}

static void isku_rkp_update_with_hardware_keys_thumbster(RoccatDevice *isku, IskuRkp *rkp, guint profile_index) {
	GError *error = NULL;
	IskuKeysThumbster *hardware_keys;
	guint i;
	hardware_keys = isku_keys_thumbster_read(isku, profile_index, &error);
	if (hardware_keys) {
		isku_rkp_update_with_keys_thumbster(rkp, hardware_keys);
		for (i = 0; i < ISKU_KEYS_THUMBSTER_NUM; ++i) {
			if (hardware_keys && hardware_keys->types[i] == ISKU_KEY_TYPE_MACRO)
				isku_rkp_update_with_hardware_macro(isku, rkp, profile_index, i + ISKU_KEY_INDEX_T1);
		}
		g_free(hardware_keys);
	} else
		g_clear_error(&error);
}

static void isku_rkp_update_with_hardware_keys_macro(RoccatDevice *isku, IskuRkp *rkp, guint profile_index) {
	GError *error = NULL;
	IskuKeysMacro *hardware_keys;
	guint i;
	hardware_keys = isku_keys_macro_read(isku, profile_index, &error);
	if (hardware_keys) {
		isku_rkp_update_with_keys_macro(rkp, hardware_keys);
		for (i = 0; i < ISKU_KEYS_MACRO_NUM; ++i) {
			if (hardware_keys && hardware_keys->types[i] == ISKU_KEY_TYPE_MACRO)
				isku_rkp_update_with_hardware_macro(isku, rkp, profile_index, i + ISKU_KEY_INDEX_M1);
		}
		g_free(hardware_keys);
	} else
		g_clear_error(&error);
}

static void isku_rkp_update_with_hardware_keys_capslock(RoccatDevice *isku, IskuRkp *rkp, guint profile_index) {
	GError *error = NULL;
	IskuKeysCapslock *hardware_keys;
	hardware_keys = isku_keys_capslock_read(isku, profile_index, &error);
	if (hardware_keys) {
		isku_rkp_update_with_keys_capslock(rkp, hardware_keys);
		g_free(hardware_keys);
	} else
		g_clear_error(&error);
}

static void isku_rkp_differ_warning(IskuRkp const *rkp, guint profile_index) {
	guint i;

	if (isku_rkp_get_modified(rkp))
		g_warning(_("Hardware data and rkp for profile %u differ"), profile_index + 1);

	if (rkp->modified_light)
		g_warning(_("light"));
	if (rkp->modified_key_mask)
		g_warning(_("key_mask"));

	for (i = 0; i < ISKU_KEY_INDEX_NUM; ++i)
		if (rkp->modified_keys[i])
			g_warning(_("key 0x%02x"), i);
}

IskuRkp *isku_rkp_load_actual(guint profile_index) {
	IskuRkp *rkp;
	gchar *config_path;
	config_path = isku_build_config_rkp_path(profile_index);
	rkp = isku_rkp_read_with_path(config_path, isku_rkp_defaults(), NULL);
	g_free(config_path);
	if (!rkp)
		rkp = isku_default_rkp();
	return rkp;
}

void isku_rkp_update_from_device(RoccatDevice *isku, IskuRkp *rkp, guint profile_index) {
	gaminggear_device_lock(GAMINGGEAR_DEVICE(isku));

	isku_rkp_update_with_hardware_key_mask(isku, rkp, profile_index);
	isku_rkp_update_with_hardware_light(isku, rkp, profile_index);
	isku_rkp_update_with_hardware_keys_function(isku, rkp, profile_index);
	isku_rkp_update_with_hardware_keys_easyzone(isku, rkp, profile_index);
	isku_rkp_update_with_hardware_keys_media(isku, rkp, profile_index);
	isku_rkp_update_with_hardware_keys_thumbster(isku, rkp, profile_index);
	isku_rkp_update_with_hardware_keys_macro(isku, rkp, profile_index);
	isku_rkp_update_with_hardware_keys_capslock(isku, rkp, profile_index);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(isku));
}

IskuRkp *isku_rkp_load(RoccatDevice *isku, guint profile_index, GError **error) {
	IskuRkp *rkp;

	g_assert(profile_index < ISKU_PROFILE_NUM);

	rkp = isku_rkp_load_actual(profile_index);
	isku_rkp_update_from_device(isku, rkp, profile_index);
	isku_rkp_differ_warning(rkp, profile_index);

	return rkp;
}

gboolean isku_rkp_save_actual(IskuRkp *rkp, guint profile_index, GError **error) {
	gchar *config_path;
	gboolean retval;

	isku_profile_dir_create_if_needed(error);
	if (*error)
		return FALSE;

	config_path = isku_build_config_rkp_path(profile_index);
	retval = isku_rkp_write_with_path(config_path, rkp, error);
	g_free(config_path);
	return retval;
}

static gboolean isku_rkp_save_actual_when_needed(IskuRkp *rkp, guint profile_index, GError **error) {
	gboolean retval = TRUE;

	if (isku_rkp_get_modified(rkp)) {
		retval = isku_rkp_save_actual(rkp, profile_index, error);
		if (retval)
			rkp->modified_rkp = FALSE;
	}

	return retval;
}

static gboolean isku_rkp_save_macro_when_needed(RoccatDevice *isku, IskuRkp *rkp, guint profile_index, guint key_index, GError **error) {
	IskuMacro *macro;

	/* These keys can't hold macros */
	if (key_index >= ISKU_KEY_INDEX_F1)
		return TRUE;

	if (rkp->modified_keys[key_index]) {
		macro = isku_rkp_to_macro(rkp, key_index);
		if (macro) {
			isku_macro_write(isku, profile_index, key_index, macro, error);
			g_free(macro);
			if (*error)
				return FALSE;
		}
		rkp->modified_keys[key_index] = FALSE;
	}
	return TRUE;
}

static gboolean isku_rkp_save_macros_when_needed(RoccatDevice *isku, IskuRkp *rkp, guint profile_index, guint first, guint last, GError **error) {
	guint i;

	g_assert(first <= last);

	for (i = first; i <= last; ++i)
		if (!isku_rkp_save_macro_when_needed(isku, rkp, profile_index, i, error))
			return FALSE;
	return TRUE;
}

static gboolean isku_rkp_save_key_mask_when_needed(RoccatDevice *isku, IskuRkp *rkp, guint profile_index, GError **error) {
	IskuKeyMask *key_mask;
	gboolean retval = TRUE;
	if (rkp->modified_key_mask) {
		key_mask = isku_rkp_to_key_mask(rkp);
		retval = isku_key_mask_write(isku, profile_index, key_mask, error);
		g_free(key_mask);
		if (retval)
			rkp->modified_key_mask = FALSE;
	}
	return retval;
}

static gboolean isku_rkp_save_keys_function(RoccatDevice *isku, IskuRkp *rkp, guint profile_index, GError **error) {
	IskuKeysFunction *keys_function;
	gboolean retval;

	keys_function = isku_rkp_to_keys_function(rkp);
	retval = isku_keys_function_write(isku, profile_index, keys_function, error);
	g_free(keys_function);

	return retval;
}

static gboolean isku_rkp_save_keys_function_full_when_needed(RoccatDevice *isku, IskuRkp *rkp, guint profile_index, GError **error) {
	if (isku_rkp_keys_function_modified(rkp)) {
		if (!isku_rkp_save_keys_function(isku, rkp, profile_index, error))
			return FALSE;
		if (!isku_rkp_save_macros_when_needed(isku, rkp, profile_index, ISKU_KEY_INDEX_F1, ISKU_KEY_INDEX_F12, error))
			return FALSE;
	}
	return TRUE;
}

static gboolean isku_rkp_save_keys_easyzone(RoccatDevice *isku, IskuRkp *rkp, guint profile_index, GError **error) {
	IskuKeysEasyzone *keys_easyzone;
	gboolean retval;

	keys_easyzone = isku_rkp_to_keys_easyzone(rkp);
	retval = isku_keys_easyzone_write(isku, profile_index, keys_easyzone, error);
	g_free(keys_easyzone);

	return retval;
}

static gboolean isku_rkp_save_keys_easyzone_full_when_needed(RoccatDevice *isku, IskuRkp *rkp, guint profile_index, GError **error) {
	if (isku_rkp_keys_easyzone_modified(rkp)) {
		if (!isku_rkp_save_keys_easyzone(isku, rkp, profile_index, error))
			return FALSE;
		if (!isku_rkp_save_macros_when_needed(isku, rkp, profile_index, ISKU_KEY_INDEX_1, ISKU_KEY_INDEX_B, error))
			return FALSE;
	}
	return TRUE;
}

static gboolean isku_rkp_save_keys_media(RoccatDevice *isku, IskuRkp *rkp, guint profile_index, GError **error) {
	IskuKeysMedia *keys_media;
	gboolean retval;

	keys_media = isku_rkp_to_keys_media(rkp);
	retval = isku_keys_media_write(isku, profile_index, keys_media, error);
	g_free(keys_media);

	return retval;
}

static gboolean isku_rkp_save_keys_media_full_when_needed(RoccatDevice *isku, IskuRkp *rkp, guint profile_index, GError **error) {
	if (isku_rkp_keys_media_modified(rkp)) {
		if (!isku_rkp_save_keys_media(isku, rkp, profile_index, error))
			return FALSE;
		if (!isku_rkp_save_macros_when_needed(isku, rkp, profile_index, ISKU_KEY_INDEX_MEDIA_MUTE, ISKU_KEY_INDEX_MEDIA_COMPUTER, error))
			return FALSE;
	}
	return TRUE;
}

static gboolean isku_rkp_save_keys_thumbster(RoccatDevice *isku, IskuRkp *rkp, guint profile_index, GError **error) {
	IskuKeysThumbster *keys_thumbster;
	gboolean retval;

	keys_thumbster = isku_rkp_to_keys_thumbster(rkp);
	retval = isku_keys_thumbster_write(isku, profile_index, keys_thumbster, error);
	g_free(keys_thumbster);

	return retval;
}

static gboolean isku_rkp_save_keys_thumbster_full_when_needed(RoccatDevice *isku, IskuRkp *rkp, guint profile_index, GError **error) {
	if (isku_rkp_keys_thumbster_modified(rkp)) {
		if (!isku_rkp_save_keys_thumbster(isku, rkp, profile_index, error))
			return FALSE;
		if (!isku_rkp_save_macros_when_needed(isku, rkp, profile_index, ISKU_KEY_INDEX_T1, ISKU_KEY_INDEX_SHIFT_T3, error))
			return FALSE;
	}
	return TRUE;
}

static gboolean isku_rkp_save_keys_macro(RoccatDevice *isku, IskuRkp *rkp, guint profile_index, GError **error) {
	IskuKeysMacro *keys_macro;
	gboolean retval;

	keys_macro = isku_rkp_to_keys_macro(rkp);
	retval = isku_keys_macro_write(isku, profile_index, keys_macro, error);
	g_free(keys_macro);

	return retval;
}

static gboolean isku_rkp_save_keys_macro_full_when_needed(RoccatDevice *isku, IskuRkp *rkp, guint profile_index, GError **error) {
	if (isku_rkp_keys_macro_modified(rkp)) {
		if (!isku_rkp_save_keys_macro(isku, rkp, profile_index, error))
			return FALSE;
		if (!isku_rkp_save_macros_when_needed(isku, rkp, profile_index, ISKU_KEY_INDEX_M1, ISKU_KEY_INDEX_SHIFT_M5, error))
			return FALSE;
	}
	return TRUE;
}

static gboolean isku_rkp_save_keys_capslock_when_needed(RoccatDevice *isku, IskuRkp *rkp, guint profile_index, GError **error) {
	IskuKeysCapslock *keys_capslock;
	gboolean retval = TRUE;
	if (rkp->modified_keys[ISKU_KEY_INDEX_CAPSLOCK]) {
		keys_capslock = isku_rkp_to_keys_capslock(rkp);
		retval = isku_keys_capslock_write(isku, profile_index, keys_capslock, error);
		g_free(keys_capslock);
		if (retval)
			rkp->modified_keys[ISKU_KEY_INDEX_CAPSLOCK] = FALSE;
	}
	return retval;
}

static gboolean isku_rkp_save_light_when_needed(RoccatDevice *isku, IskuRkp *rkp, guint profile_index, GError **error) {
	IskuLight *light;
	gboolean retval = TRUE;
	if (rkp->modified_light) {
		light = isku_rkp_to_light(rkp);
		retval = isku_light_write(isku, profile_index, light, error);
		g_free(light);
		if (retval)
			rkp->modified_light = FALSE;

	}
	return retval;
}

gboolean isku_rkp_save(RoccatDevice *isku, IskuRkp *rkp, guint profile_index, GError **error) {
	gboolean retval = FALSE;

	g_assert(profile_index < ISKU_PROFILE_NUM);

	if (!isku_rkp_get_modified(rkp)) return TRUE;

	if (!isku_rkp_save_actual_when_needed(rkp, profile_index, error)) return FALSE;

	gaminggear_device_lock(GAMINGGEAR_DEVICE(isku));

	if (!isku_rkp_save_key_mask_when_needed(isku, rkp, profile_index, error)) goto error;
	if (!isku_rkp_save_keys_function_full_when_needed(isku, rkp, profile_index, error)) goto error;
	if (!isku_rkp_save_keys_easyzone_full_when_needed(isku, rkp, profile_index, error)) goto error;
	if (!isku_rkp_save_keys_media_full_when_needed(isku, rkp, profile_index, error)) goto error;
	if (!isku_rkp_save_keys_thumbster_full_when_needed(isku, rkp, profile_index, error)) goto error;
	if (!isku_rkp_save_keys_macro_full_when_needed(isku, rkp, profile_index, error)) goto error;
	if (!isku_rkp_save_keys_capslock_when_needed(isku, rkp, profile_index, error)) goto error;
	if (!isku_rkp_save_light_when_needed(isku, rkp, profile_index, error)) goto error;

	retval = TRUE;
error:
	gaminggear_device_unlock(GAMINGGEAR_DEVICE(isku));
	return retval;
}

IskuRkp *isku_rkp_load_save_after_reset(RoccatDevice *isku, guint profile_index, GError **error) {
	IskuRkp *rkp;

	rkp = isku_default_rkp();

	/* Hardware profiles differ in light-color and -effects */
	isku_rkp_update_from_device(isku, rkp, profile_index);
	isku_rkp_set_unmodified(rkp);

	isku_rkp_save_actual(rkp, profile_index, error);
	return rkp;
}
