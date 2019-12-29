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

#include "arvo_rkp.h"
#include "roccat_helper.h"
#include "roccat.h"
#include "i18n-lib.h"
#include <string.h>

guint8 arvo_rkp_button_info_calc_checksum(ArvoRkpButtonInfo const *rkp_button_info) {
	return ROCCAT_BYTESUM_PARTIALLY(rkp_button_info, ArvoRkpButtonInfo, number, checksum);
}

void arvo_rkp_button_info_set_checksum(ArvoRkpButtonInfo *rkp_button_info) {
	rkp_button_info->checksum = arvo_rkp_button_info_calc_checksum(rkp_button_info);
}

static void arvo_rkp_button_info_set_macroset_name(ArvoRkpButtonInfo *button_info, gchar const *new_name) {
	g_strlcpy((gchar *)(button_info->macroset_name), new_name, ARVO_RKP_BUTTON_INFO_MACROSET_NAME_LENGTH);
}

static void arvo_rkp_button_info_set_macro_name(ArvoRkpButtonInfo *button_info, gchar const *new_name) {
	g_strlcpy((gchar *)(button_info->macro_name), new_name, ARVO_RKP_BUTTON_INFO_MACRO_NAME_LENGTH);
}

static void arvo_rkp_button_info_set_timer_name(ArvoRkpButtonInfo *button_info, gchar const *new_name) {
	g_strlcpy((gchar *)(button_info->timer_name), new_name, ARVO_RKP_BUTTON_INFO_TIMER_NAME_LENGTH);
}

static void arvo_rkp_button_info_set_filename_ns(ArvoRkpButtonInfo *button_info, gchar const *new_name) {
	g_strlcpy((gchar *)(button_info->macroset_name), new_name, ARVO_RKP_BUTTON_INFO_FILENAME_LENGTH_NS);
}

static void gaminggear_macro_keystroke_to_arvo_rkp_button_info_keystroke(GaminggearMacroKeystroke const *from, RoccatKeystroke *to) {
	to->key = from->key;
	roccat_keystroke_set_period(to, gaminggear_macro_keystroke_get_period(from));

	if (from->action == GAMINGGEAR_MACRO_KEYSTROKE_ACTION_PRESS)
		to->action = ARVO_RKP_BUTTON_INFO_KEYSTROKE_ACTION_PRESS;
	else
		to->action = ARVO_RKP_BUTTON_INFO_KEYSTROKE_ACTION_RELEASE;
}

ArvoRkpButtonInfo *arvo_rkp_button_info_new(void) {
	return (ArvoRkpButtonInfo *)g_malloc0(sizeof(ArvoRkpButtonInfo));
}

void arvo_rkp_button_info_free(ArvoRkpButtonInfo *button_info) {
	g_free(button_info);
}

ArvoRkpButtonInfo *gaminggear_macro_to_arvo_rkp_button_info(GaminggearMacro const *gaminggear_macro, GError **error) {
	ArvoRkpButtonInfo *result;
	guint i, count;

	g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

	count = gaminggear_macro_keystrokes_get_count(&gaminggear_macro->keystrokes);
	if (count > ARVO_RKP_BUTTON_INFO_KEYSTROKES_NUM) {
		g_set_error(error, ROCCAT_ERROR, ROCCAT_ERROR_MACRO_TOO_LONG,
				_("Macro contains %u actions while device only supports %u actions"), count, ARVO_RKP_BUTTON_INFO_KEYSTROKES_NUM);
		return NULL;
	}

	result = arvo_rkp_button_info_new();
	result->type = ARVO_RKP_BUTTON_INFO_TYPE_MACRO;
	arvo_rkp_button_info_set_count(result, count);
	arvo_rkp_button_info_set_macroset_name(result, gaminggear_macro->macroset);
	arvo_rkp_button_info_set_macro_name(result, gaminggear_macro->macro);
	result->ns_loop = gaminggear_macro->keystrokes.loop;

	for (i = 0; i < count; ++i) {
		gaminggear_macro_keystroke_to_arvo_rkp_button_info_keystroke(&gaminggear_macro->keystrokes.keystrokes[i], &result->keystrokes[i]);
	}

	return result;
}

static void arvo_rkp_button_info_keystroke_to_gaminggear_macro_keystroke(RoccatKeystroke const *from, GaminggearMacroKeystroke *to) {
	to->key = from->key;
	gaminggear_macro_keystroke_set_period(to, roccat_keystroke_get_period(from));
	
	if (from->action == ARVO_RKP_BUTTON_INFO_KEYSTROKE_ACTION_PRESS)
		to->action = GAMINGGEAR_MACRO_KEYSTROKE_ACTION_PRESS;
	else
		to->action = GAMINGGEAR_MACRO_KEYSTROKE_ACTION_RELEASE;
}

GaminggearMacro *arvo_rkp_button_info_to_gaminggear_macro(ArvoRkpButtonInfo const *button_info) {
	GaminggearMacro *gaminggear_macro;
	GaminggearMacroKeystroke keystroke;
	guint i;
	
	if (button_info->type != ARVO_RKP_BUTTON_INFO_TYPE_MACRO)
		return NULL;
		
	gaminggear_macro = gaminggear_macro_new((gchar const *)button_info->macroset_name, (gchar const *)button_info->macro_name, NULL);
	
	for (i = 0; i < arvo_rkp_button_info_get_count(button_info); ++i) {
		arvo_rkp_button_info_keystroke_to_gaminggear_macro_keystroke(&button_info->keystrokes[i], &keystroke);
		gaminggear_macro_add_keystroke(gaminggear_macro, &keystroke);
	}
	
	gaminggear_macro->keystrokes.loop = (button_info->ns_loop == 0) ? 1 : button_info->ns_loop;
	
	return gaminggear_macro;
}

ArvoRkpButtonInfo *arvo_rkp_button_info_new_quicklaunch_ns(gchar const *filename) {
	ArvoRkpButtonInfo *result = arvo_rkp_button_info_new();
	result->type = ARVO_RKP_BUTTON_INFO_TYPE_QUICKLAUNCH_NS;
	arvo_rkp_button_info_set_filename_ns(result, filename);
	return result;
}

ArvoRkpButtonInfo *arvo_rkp_button_info_new_special(ArvoRkpButtonInfoType type) {
	ArvoRkpButtonInfo *result = arvo_rkp_button_info_new();
	result->type = type;
	return result;
}

ArvoRkpButtonInfo *arvo_rkp_button_info_new_timer(gchar const *name, guint seconds) {
	ArvoRkpButtonInfo *result = arvo_rkp_button_info_new();
	result->type = ARVO_RKP_BUTTON_INFO_TYPE_TIMER_START;
	arvo_rkp_button_info_set_timer_name(result, name);
	arvo_rkp_button_info_set_timer_length(result, seconds);
	return result;
}

ArvoRkpButtonInfo *arvo_rkp_button_info_new_shortcut(guint hid_usage_id,
		guint shift, guint ctrl, guint win, guint alt) {
	ArvoRkpButtonInfo *result = arvo_rkp_button_info_new();

	result->type = ARVO_RKP_BUTTON_INFO_TYPE_SHORTCUT;
	result->shortcut_key = hid_usage_id;
	result->shortcut_shift = shift;
	result->shortcut_ctrl = ctrl;
	result->shortcut_win = win;
	result->shortcut_alt = alt;

	return result;
}

gboolean arvo_rkp_button_info_equal(ArvoRkpButtonInfo const *left, ArvoRkpButtonInfo const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, ArvoRkpButtonInfo, type, checksum);
	return equal ? FALSE : TRUE;
}

ArvoRkpButtonInfo *arvo_rkp_button_info_dup(ArvoRkpButtonInfo const *source) {
	return g_memdup((gconstpointer)source, sizeof(ArvoRkpButtonInfo));
}
