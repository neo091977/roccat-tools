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
#include <string.h>

static void isku_macro_set_macroset_name(IskuMacro *macro, gchar const *new_name) {
	g_strlcpy((gchar *)(macro->macroset_name), new_name, ISKU_MACRO_MACROSET_NAME_LENGTH);
}

static void isku_macro_set_macro_name(IskuMacro *macro, gchar const *new_name) {
	g_strlcpy((gchar *)(macro->macro_name), new_name, ISKU_MACRO_MACRO_NAME_LENGTH);
}

static guint16 isku_macro_calc_checksum(IskuMacro const *macro) {
	return ROCCAT_BYTESUM_PARTIALLY(macro, IskuMacro, report_id, checksum);
}

static void isku_macro_set_checksum(IskuMacro *macro) {
	macro->checksum = GUINT16_TO_LE(isku_macro_calc_checksum(macro));
}

static void isku_macro_finalize(IskuMacro *macro, guint profile_number, guint key_index) {
	macro->report_id = ISKU_REPORT_ID_MACRO;
	isku_macro_set_size(macro, sizeof(IskuMacro));
	macro->profile_number = profile_number;
	macro->index = key_index;
	isku_macro_set_checksum(macro);
}

IskuMacro *isku_rkp_macro_key_info_to_macro(IskuRkpMacroKeyInfo const *macro_key_info) {
	IskuMacro *macro;
	guint i, count;

	if (macro_key_info->type != ISKU_KEY_TYPE_MACRO)
		return NULL;

	macro = g_malloc0(sizeof(IskuMacro));

	macro->index = macro_key_info->index;
	macro->loop = isku_rkp_macro_key_info_get_loop(macro_key_info);
	isku_macro_set_macroset_name(macro, (gchar const *)macro_key_info->macroset_name);
	isku_macro_set_macro_name(macro, (gchar const *)macro_key_info->macro_name);
	isku_macro_set_count(macro, isku_rkp_macro_key_info_get_count(macro_key_info));

	count = isku_macro_get_count(macro);
	count = MIN(count, ISKU_MACRO_KEYSTROKES_NUM);

	for (i = 0; i < count; ++i) {
		macro->keystrokes[i] = macro_key_info->keystrokes[i];
	}

	return macro;
}

IskuMacro *isku_rkp_to_macro(IskuRkp *rkp, guint index) {
	IskuRkpMacroKeyInfo *key_info;
	IskuMacro *macro;

	key_info = isku_rkp_get_macro_key_info(rkp, index);
	macro = isku_rkp_macro_key_info_to_macro(key_info);
	g_free(key_info);
	return macro;
}

void isku_rkp_update_with_macro(IskuRkp *rkp, guint index, IskuMacro const *macro) {
	IskuRkpMacroKeyInfo *key_info;
	if (!macro)
		return;
	key_info = isku_macro_to_rkp_macro_key_info(macro);
	isku_rkp_set_macro_key_info(rkp, index, key_info);
	g_free(key_info);
}

gboolean isku_macro_write(RoccatDevice *isku, guint profile_number, guint key_index, IskuMacro *macro, GError **error) {
	g_assert(profile_number < ISKU_PROFILE_NUM);
	g_assert(key_index < ISKU_KEY_INDEX_F1);

	isku_macro_finalize(macro, profile_number, key_index);
	return isku_device_write(isku, (gchar const *)macro, sizeof(IskuMacro), error);
}

IskuMacro *isku_macro_read(RoccatDevice *isku, guint profile_number, guint key_index, GError **error) {
	IskuMacro *macro;

	g_assert(profile_number < ISKU_PROFILE_NUM);
	g_assert(key_index < ISKU_KEY_INDEX_F1);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(isku));

	if (!isku_select(isku, profile_number, key_index, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(isku));
		return NULL;
	}

	macro = (IskuMacro *)isku_device_read(isku, ISKU_REPORT_ID_MACRO, sizeof(IskuMacro), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(isku));

	return macro;
}
