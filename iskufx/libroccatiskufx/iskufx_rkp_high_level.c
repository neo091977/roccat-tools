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

#include "roccat_helper.h"
#include "isku_rkp.h"
#include "iskufx.h"
#include "roccat.h"
#include "i18n-lib.h"
#include <string.h>

static gchar *iskufx_profile_dir(void) {
	gchar *base = roccat_profile_dir();
	gchar *dir = g_build_path("/", base, "iskufx", NULL);
	g_free(base);
	return dir;
}

static gboolean iskufx_profile_dir_create_if_needed(GError **error) {
	gchar *dir;
	gboolean retval;

	dir = iskufx_profile_dir();
	retval = roccat_profile_dir_create_if_needed(dir, error);
	g_free(dir);
	return retval;
}

static gchar *iskufx_build_config_rkp_path(guint profile_index) {
	gchar *dir = iskufx_profile_dir();
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

static void iskufx_rkp_update_with_hardware_light(RoccatDevice *iskufx, IskuRkp *rkp, guint profile_index) {
	GError *error = NULL;
	IskufxLight *hardware_light;
	hardware_light = iskufx_light_read(iskufx, profile_index, &error);
	if (hardware_light) {
		iskufx_rkp_update_with_light(rkp, hardware_light);
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

IskuRkp *iskufx_rkp_load_actual(guint profile_index) {
	IskuRkp *rkp;
	gchar *config_path;
	config_path = iskufx_build_config_rkp_path(profile_index);
	rkp = isku_rkp_read_with_path(config_path, iskufx_rkp_defaults(), NULL);
	g_free(config_path);
	if (!rkp)
		rkp = iskufx_default_rkp();
	return rkp;
}

void iskufx_rkp_update_from_device(RoccatDevice *iskufx, IskuRkp *rkp, guint profile_index) {
	gaminggear_device_lock(GAMINGGEAR_DEVICE(iskufx));

	isku_rkp_update_with_hardware_key_mask(iskufx, rkp, profile_index);
	iskufx_rkp_update_with_hardware_light(iskufx, rkp, profile_index);
	isku_rkp_update_with_hardware_keys_function(iskufx, rkp, profile_index);
	isku_rkp_update_with_hardware_keys_easyzone(iskufx, rkp, profile_index);
	isku_rkp_update_with_hardware_keys_media(iskufx, rkp, profile_index);
	isku_rkp_update_with_hardware_keys_thumbster(iskufx, rkp, profile_index);
	isku_rkp_update_with_hardware_keys_macro(iskufx, rkp, profile_index);
	isku_rkp_update_with_hardware_keys_capslock(iskufx, rkp, profile_index);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(iskufx));
}

IskuRkp *iskufx_rkp_load(RoccatDevice *iskufx, guint profile_index, GError **error) {
	IskuRkp *rkp;

	g_assert(profile_index < ISKU_PROFILE_NUM);

	rkp = iskufx_rkp_load_actual(profile_index);
	iskufx_rkp_update_from_device(iskufx, rkp, profile_index);
	isku_rkp_differ_warning(rkp, profile_index);

	return rkp;
}

gboolean iskufx_rkp_save_actual(IskuRkp *rkp, guint profile_index, GError **error) {
	gchar *config_path;
	gboolean retval;

	iskufx_profile_dir_create_if_needed(error);
	if (*error)
		return FALSE;

	config_path = iskufx_build_config_rkp_path(profile_index);
	retval = isku_rkp_write_with_path(config_path, rkp, error);
	g_free(config_path);
	return retval;
}

static gboolean iskufx_rkp_save_actual_when_needed(IskuRkp *rkp, guint profile_index, GError **error) {
	gboolean retval = TRUE;

	if (isku_rkp_get_modified(rkp)) {
		retval = iskufx_rkp_save_actual(rkp, profile_index, error);
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

static gboolean iskufx_rkp_save_light_when_needed(RoccatDevice *iskufx, IskuRkp *rkp, guint profile_index, GError **error) {
	IskufxLight *light;
	gboolean retval = TRUE;
	if (rkp->modified_light) {
		light = iskufx_rkp_to_light(rkp);
		retval = iskufx_light_write(iskufx, profile_index, light, error);
		g_free(light);
		if (retval)
			rkp->modified_light = FALSE;
	}
	return retval;
}

gboolean iskufx_rkp_save(RoccatDevice *iskufx, IskuRkp *rkp, guint profile_index, GError **error) {
	gboolean retval = FALSE;

	g_assert(profile_index < ISKU_PROFILE_NUM);

	if (!isku_rkp_get_modified(rkp)) return TRUE;

	if (!iskufx_rkp_save_actual_when_needed(rkp, profile_index, error)) return FALSE;

	gaminggear_device_lock(GAMINGGEAR_DEVICE(iskufx));

	if (!isku_rkp_save_key_mask_when_needed(iskufx, rkp, profile_index, error)) goto error;
	if (!isku_rkp_save_keys_function_full_when_needed(iskufx, rkp, profile_index, error)) goto error;
	if (!isku_rkp_save_keys_easyzone_full_when_needed(iskufx, rkp, profile_index, error)) goto error;
	if (!isku_rkp_save_keys_media_full_when_needed(iskufx, rkp, profile_index, error)) goto error;
	if (!isku_rkp_save_keys_thumbster_full_when_needed(iskufx, rkp, profile_index, error)) goto error;
	if (!isku_rkp_save_keys_macro_full_when_needed(iskufx, rkp, profile_index, error)) goto error;
	if (!isku_rkp_save_keys_capslock_when_needed(iskufx, rkp, profile_index, error)) goto error;
	if (!iskufx_rkp_save_light_when_needed(iskufx, rkp, profile_index, error)) goto error;

	retval = TRUE;
error:
	gaminggear_device_unlock(GAMINGGEAR_DEVICE(iskufx));
	return retval;
}

IskuRkp *iskufx_rkp_load_save_after_reset(RoccatDevice *isku, guint profile_index, GError **error) {
	IskuRkp *rkp;

	rkp = iskufx_default_rkp();

	/* Hardware profiles differ in light-color and -effects */
	iskufx_rkp_update_from_device(isku, rkp, profile_index);
	isku_rkp_set_unmodified(rkp);

	iskufx_rkp_save_actual(rkp, profile_index, error);
	return rkp;
}
