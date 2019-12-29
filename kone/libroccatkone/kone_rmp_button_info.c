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

#include "kone_internal.h"
#include "roccat.h"
#include "roccat_helper.h"
#include "i18n-lib.h"
#include <string.h>

guint8 kone_rmp_button_info_calc_checksum(KoneRMPButtonInfo const *rmp_button_info) {
	return ROCCAT_BYTESUM_PARTIALLY(rmp_button_info, KoneRMPButtonInfo, number, checksum);
}

void kone_rmp_button_info_set_checksum(KoneRMPButtonInfo *rmp_button_info) {
	rmp_button_info->checksum = kone_rmp_button_info_calc_checksum(rmp_button_info);
}

static void kone_rmp_button_info_set_macro_set_name(KoneRMPButtonInfo *button_info, gchar const *new_name) {
	g_strlcpy((gchar *)(button_info->macro_set_name), new_name, KONE_RMP_BUTTON_INFO_MACRO_SET_NAME_LENGTH);
}

static void kone_rmp_button_info_set_macro_name(KoneRMPButtonInfo *button_info, gchar const *new_name) {
	g_strlcpy((gchar *)(button_info->macro_name), new_name, KONE_RMP_BUTTON_INFO_MACRO_NAME_LENGTH);
}

static void kone_button_info_set_macro_set_name(KoneButtonInfo *button_info, gchar const *new_name) {
	g_strlcpy((gchar *)(button_info->macro_set_name), new_name, KONE_BUTTON_INFO_MACRO_SET_NAME_LENGTH);
}

static void kone_button_info_set_macro_name(KoneButtonInfo *button_info, gchar const *new_name) {
	g_strlcpy((gchar *)(button_info->macro_name), new_name, KONE_BUTTON_INFO_MACRO_NAME_LENGTH);
}

static void gaminggear_macro_keystroke_to_kone_keystroke(GaminggearMacroKeystroke const *from, RoccatKeystroke *to) {
	to->key = from->key;
	roccat_keystroke_set_period(to, gaminggear_macro_keystroke_get_period(from));

	if (from->action == GAMINGGEAR_MACRO_KEYSTROKE_ACTION_PRESS)
		to->action = KONE_KEYSTROKE_ACTION_PRESS;
	else
		to->action = KONE_KEYSTROKE_ACTION_RELEASE;
}

KoneRMPButtonInfo *gaminggear_macro_to_kone_rmp_button_info(GaminggearMacro const *gaminggear_macro, GError **error) {
	KoneRMPButtonInfo *result;
	guint i, count;

	g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

	count = gaminggear_macro_keystrokes_get_count(&gaminggear_macro->keystrokes);
	if (count > KONE_RMP_BUTTON_INFO_KEYSTROKES_NUM) {
		g_set_error(error, ROCCAT_ERROR, ROCCAT_ERROR_MACRO_TOO_LONG,
				_("Macro contains %u actions while device only supports %u actions"), count, KONE_RMP_BUTTON_INFO_KEYSTROKES_NUM);
		return NULL;
	}

	result = g_malloc0(sizeof(KoneRMPButtonInfo));

	kone_rmp_button_info_set_macro_set_name(result, gaminggear_macro->macroset);
	kone_rmp_button_info_set_macro_name(result, gaminggear_macro->macro);
	kone_rmp_button_info_set_count(result, count);

	result->type = KONE_BUTTON_INFO_TYPE_MACRO;
	result->unknown2 = 0xcc;
	result->ns_loop = gaminggear_macro->keystrokes.loop;

	for (i = 0; i < count; ++i) {
		gaminggear_macro_keystroke_to_kone_keystroke(&gaminggear_macro->keystrokes.keystrokes[i], &result->keystrokes[i]);
	}

	return result;
}

static void kone_keystroke_to_gaminggear_macro_keystroke(RoccatKeystroke const *from, GaminggearMacroKeystroke *to) {
	to->key = from->key;
	gaminggear_macro_keystroke_set_period(to, roccat_keystroke_get_period(from));
	
	if (from->action == KONE_KEYSTROKE_ACTION_PRESS)
		to->action = GAMINGGEAR_MACRO_KEYSTROKE_ACTION_PRESS;
	else
		to->action = GAMINGGEAR_MACRO_KEYSTROKE_ACTION_RELEASE;
}

GaminggearMacro *kone_rmp_button_info_to_gaminggear_macro(KoneRMPButtonInfo const *button_info) {
	GaminggearMacro *gaminggear_macro;
	GaminggearMacroKeystroke keystroke;
	guint i;
	
	if (button_info->type != KONE_BUTTON_INFO_TYPE_MACRO)
		return NULL;
		
	gaminggear_macro = gaminggear_macro_new((gchar const *)button_info->macro_set_name, (gchar const *)button_info->macro_name, NULL);
	
	for (i = 0; i < kone_rmp_button_info_get_count(button_info); ++i) {
		kone_keystroke_to_gaminggear_macro_keystroke(&button_info->keystrokes[i], &keystroke);
		gaminggear_macro_add_keystroke(gaminggear_macro, &keystroke);
	}
	
	gaminggear_macro->keystrokes.loop = (button_info->ns_loop == 0) ? 1 : button_info->ns_loop;

	return gaminggear_macro;
}

void kone_rmp_button_info_to_button_info(KoneRMPButtonInfo const *from, KoneButtonInfo *to) {
	guint i;
	guint16 count;

	memset(to, 0, sizeof(KoneButtonInfo));

	to->number = from->number;
	to->type = from->type;

	kone_button_info_set_macro_set_name(to, (gchar *)(from->macro_set_name));
	kone_button_info_set_macro_name(to, (gchar *)(from->macro_name));

	count = kone_rmp_button_info_get_count(from);

	switch (to->type) {
	case KONE_BUTTON_INFO_TYPE_MACRO:
		if (count > KONE_BUTTON_INFO_KEYSTROKES_NUM || from->ns_loop > 1) {
			to->macro_type = KONE_BUTTON_INFO_MACRO_TYPE_LONG;
			to->count = 0;
		} else {
			to->macro_type = KONE_BUTTON_INFO_MACRO_TYPE_SHORT;
			to->count = count;
		}
		break;
	case KONE_BUTTON_INFO_TYPE_KEY:
		to->count = count;
		break;
	case KONE_BUTTON_INFO_TYPE_SHORTCUT:
		to->count = count;
		break;
	default:
		to->count = 0;
	}

	for (i = 0; i < to->count; ++i)
		to->keystrokes[i] = from->keystrokes[i];
}

/*
 * \note overlong macros cannot be restored
 */
void kone_button_info_to_rmp_button_info(KoneButtonInfo const *from, KoneRMPButtonInfo *to) {
	guint i;

	memset(to, 0, sizeof(KoneRMPButtonInfo));

	to->number = from->number;
	to->type = from->type;

	kone_rmp_button_info_set_macro_set_name(to, (gchar const *)(from->macro_set_name));
	kone_rmp_button_info_set_macro_name(to, (gchar const *)(from->macro_name));

	to->unknown2 = 0xcc;
	kone_rmp_button_info_set_count(to, from->count);

	for (i = 0; i < from->count; ++i)
		to->keystrokes[i] = from->keystrokes[i];

	to->ns_loop = 1;

	kone_rmp_button_info_set_checksum(to);
}

KoneRMPButtonInfo *kone_rmp_button_info_new_single_key(guint hid_usage_id) {
	KoneRMPButtonInfo *result;

	result = g_malloc0(sizeof(KoneRMPButtonInfo));

	result->type = KONE_BUTTON_INFO_TYPE_KEY;
	kone_rmp_button_info_set_count(result, 1);
	result->keystrokes[0].key = hid_usage_id;

	return result;
}

KoneRMPButtonInfo *kone_rmp_button_info_new_special(KoneButtonInfoType type) {
	KoneRMPButtonInfo *result;
	result = g_malloc0(sizeof(KoneRMPButtonInfo));
	result->type = type;
	return result;
}

KoneRMPButtonInfo *kone_rmp_button_info_new_shortcut(guint hid_usage_id, guint modifier) {
	KoneRMPButtonInfo *result = g_malloc0(sizeof(KoneRMPButtonInfo));

	result->type = KONE_BUTTON_INFO_TYPE_SHORTCUT;
	result->keystrokes[0].key = hid_usage_id;
	result->keystrokes[0].action = modifier;
	kone_rmp_button_info_set_count(result, 1);

	return result;
}

gboolean kone_rmp_button_info_equal(KoneRMPButtonInfo const *left, KoneRMPButtonInfo const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, KoneRMPButtonInfo, type, checksum);
	return equal ? FALSE : TRUE;
}

KoneRMPButtonInfo *kone_rmp_button_info_dup(KoneRMPButtonInfo const *source) {
	return g_memdup((gconstpointer)source, sizeof(KoneRMPButtonInfo));
}
