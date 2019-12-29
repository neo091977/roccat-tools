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

#include "pyra.h"
#include "roccat.h"
#include "roccat_button.h"
#include "roccat_helper.h"
#include "i18n-lib.h"
#include <string.h>

guint8 pyra_rmp_macro_key_info_calc_checksum(PyraRMPMacroKeyInfo const *rmp_macro_key_info) {
	return ROCCAT_BYTESUM_PARTIALLY(rmp_macro_key_info, PyraRMPMacroKeyInfo, number, checksum);
}

void pyra_rmp_macro_key_info_set_checksum(PyraRMPMacroKeyInfo *rmp_macro_key_info) {
	rmp_macro_key_info->checksum = pyra_rmp_macro_key_info_calc_checksum(rmp_macro_key_info);
}

static void pyra_rmp_macro_key_info_set_macro_set_name(PyraRMPMacroKeyInfo *macro_key_info, gchar const *new_name) {
	g_strlcpy((gchar *)(macro_key_info->macro_set_name), new_name, PYRA_RMP_MACRO_KEY_INFO_MACRO_SET_NAME_LENGTH);
}

static void pyra_rmp_macro_key_info_set_macro_name(PyraRMPMacroKeyInfo *macro_key_info, gchar const *new_name) {
	g_strlcpy((gchar *)(macro_key_info->macro_name), new_name, PYRA_RMP_MACRO_KEY_INFO_MACRO_NAME_LENGTH);
}

static void pyra_rmp_macro_key_info_set_filename(PyraRMPMacroKeyInfo *macro_key_info, gchar const *new_name) {
	g_strlcpy((gchar *)(macro_key_info->filename), new_name, PYRA_RMP_MACRO_KEY_INFO_FILENAME_LENGTH);
}

PyraRMPMacroKeyInfo *gaminggear_macro_to_pyra_rmp_macro_key_info(GaminggearMacro const *gaminggear_macro, GError **error) {
	PyraRMPMacroKeyInfo *result;
	guint i, count;

	count = gaminggear_macro_keystrokes_get_count(&gaminggear_macro->keystrokes);
	if (count > PYRA_RMP_MACRO_KEY_INFO_KEYSTROKES_NUM) {
		g_set_error(error, ROCCAT_ERROR, ROCCAT_ERROR_MACRO_TOO_LONG,
				_("Macro contains %u actions while device only supports %u actions"), count, PYRA_RMP_MACRO_KEY_INFO_KEYSTROKES_NUM);
		return NULL;
	}

	result = g_malloc0(sizeof(PyraRMPMacroKeyInfo));
	result->type = PYRA_RMP_MACRO_KEY_INFO_TYPE_MACRO;
	pyra_rmp_macro_key_info_set_macro_set_name(result, gaminggear_macro->macroset);
	pyra_rmp_macro_key_info_set_macro_name(result, gaminggear_macro->macro);
	pyra_rmp_macro_key_info_set_count(result, count);
	result->loop = gaminggear_macro->keystrokes.loop;

	for (i = 0; i < count; ++i) {
		gaminggear_macro_keystroke_to_roccat_keystroke(&gaminggear_macro->keystrokes.keystrokes[i], &result->keystrokes[i]);
	}

	return result;
}

GaminggearMacro *pyra_rmp_macro_key_info_to_gaminggear_macro(PyraRMPMacroKeyInfo const *button_info) {
	GaminggearMacro *gaminggear_macro;
	GaminggearMacroKeystroke keystroke;
	guint i;
	
	if (button_info->type != PYRA_RMP_MACRO_KEY_INFO_TYPE_MACRO)
		return NULL;
		
	gaminggear_macro = gaminggear_macro_new((gchar const *)button_info->macro_set_name, (gchar const *)button_info->macro_name, NULL);
	
	for (i = 0; i < pyra_rmp_macro_key_info_get_count(button_info); ++i) {
		roccat_keystroke_to_gaminggear_macro_keystroke(&button_info->keystrokes[i], &keystroke);
		gaminggear_macro_add_keystroke(gaminggear_macro, &keystroke);
	}
	
	return gaminggear_macro;
}

PyraRMPMacroKeyInfo *pyra_rmp_macro_key_info_new_quicklaunch(gchar const *filename) {
	PyraRMPMacroKeyInfo *result = g_malloc0(sizeof(PyraRMPMacroKeyInfo));
	result->type = PYRA_RMP_MACRO_KEY_INFO_TYPE_QUICKLAUNCH;
	pyra_rmp_macro_key_info_set_filename(result, filename);
	return result;
}

PyraRMPMacroKeyInfo *pyra_rmp_macro_key_info_new_special(PyraRMPMacroKeyInfoType type) {
	PyraRMPMacroKeyInfo *result = g_malloc0(sizeof(PyraRMPMacroKeyInfo));
	result->type = type;
	return result;
}

PyraRMPMacroKeyInfo *pyra_rmp_macro_key_info_new_shortcut(guint hid_usage_id,
		gboolean ctrl, gboolean shift, gboolean alt, gboolean win) {
	PyraRMPMacroKeyInfo *result = g_malloc0(sizeof(PyraRMPMacroKeyInfo));

	result->type = PYRA_RMP_MACRO_KEY_INFO_TYPE_SHORTCUT;
	result->keystrokes[0].key = hid_usage_id;
	pyra_rmp_macro_key_info_set_count(result, 1);

	roccat_set_bit8(&result->keystrokes[0].action, ROCCAT_BUTTON_MODIFIER_BIT_CTRL, ctrl);
	roccat_set_bit8(&result->keystrokes[0].action, ROCCAT_BUTTON_MODIFIER_BIT_SHIFT, shift);
	roccat_set_bit8(&result->keystrokes[0].action, ROCCAT_BUTTON_MODIFIER_BIT_ALT, alt);
	roccat_set_bit8(&result->keystrokes[0].action, ROCCAT_BUTTON_MODIFIER_BIT_WIN, win);

	return result;
}

gboolean pyra_rmp_macro_key_info_equal(PyraRMPMacroKeyInfo const *left, PyraRMPMacroKeyInfo const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, PyraRMPMacroKeyInfo, type, checksum);
	return equal ? FALSE : TRUE;
}

PyraRMPMacroKeyInfo *pyra_rmp_macro_key_info_dup(PyraRMPMacroKeyInfo const *source) {
	return g_memdup((gconstpointer)source, sizeof(PyraRMPMacroKeyInfo));
}
