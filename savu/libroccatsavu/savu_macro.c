/*
 * This file is part of roccat-tools.
 *
 * roccat-tools is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Macro Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * roccat-tools is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Macro Public License for more details.
 *
 * You should have received a copy of the GNU Macro Public License
 * along with roccat-tools. If not, see <http://www.gnu.org/licenses/>.
 */

#include "savu_rmp.h"
#include "roccat.h"
#include "roccat_helper.h"
#include "i18n-lib.h"

static void savu_macro_set_macroset_name(SavuMacro *macro, gchar const *new_name) {
	g_strlcpy((gchar *)(macro->macroset_name), new_name, SAVU_MACRO_MACROSET_NAME_LENGTH);
}

static void savu_macro_set_macro_name(SavuMacro *macro, gchar const *new_name) {
	g_strlcpy((gchar *)(macro->macro_name), new_name, SAVU_MACRO_MACRO_NAME_LENGTH);
}

static void savu_rmp_macro_key_info_set_name(SavuRmpMacroKeyInfo *macro_key_info, gchar const *new_name) {
	g_strlcpy((gchar *)(macro_key_info->name), new_name, SAVU_RMP_MACRO_KEY_INFO_NAME_LENGTH);
}

guint16 savu_macro_calc_checksum(SavuMacro const *macro) {
	return ROCCAT_BYTESUM_PARTIALLY(macro, SavuMacro, report_id, checksum);
}

static void savu_macro_finalize(SavuMacro *macro, guint profile_index, guint button_index) {
	macro->report_id = SAVU_REPORT_ID_MACRO;
	savu_macro_set_size(macro, sizeof(SavuMacro));
	macro->profile_index = profile_index;
	macro->button_index = button_index;
	savu_macro_set_checksum(macro, savu_macro_calc_checksum(macro));
}

SavuMacro *savu_macro_read(RoccatDevice *savu, guint profile_index, guint button_index, GError **error) {
	SavuMacro *macro;

	g_assert(profile_index < SAVU_PROFILE_NUM);
	g_assert(button_index >= 2); /* first 2 buttons are fixed */
	g_assert(button_index < SAVU_BUTTON_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(savu));

	if (!savu_select(savu, profile_index, button_index, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(savu));
		return NULL;
	}

	macro = (SavuMacro *)savu_device_read(savu, SAVU_REPORT_ID_MACRO, sizeof(SavuMacro), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(savu));

	return macro;
}

gboolean savu_macro_write(RoccatDevice *savu, guint profile_index, guint button_index, SavuMacro *macro, GError **error) {
	g_assert(profile_index < SAVU_PROFILE_NUM);
	g_assert(button_index < SAVU_BUTTON_NUM);
	savu_macro_finalize(macro, profile_index, button_index);
	return savu_device_write(savu, (gchar const *)macro, sizeof(SavuMacro), error);
}

static gchar *join_macro_name(gchar const *macroset_name, gchar const *macro_name) {
	return g_strjoin("/", macroset_name, macro_name, NULL);
}

static gchar **split_macro_name(gchar const *string) {
	return g_strsplit(string, "/", 2);
}

SavuMacro *savu_rmp_macro_key_info_to_macro(SavuRmpMacroKeyInfo const *macro_key_info) {
	SavuMacro *macro;
	gchar **strings;
	guint i, count;

	macro = g_malloc0(sizeof(SavuMacro));

	/* Trying to split joined names */
	strings = split_macro_name((gchar const *)macro_key_info->name);
	switch (g_strv_length(strings)) {
	case 1:
		savu_macro_set_macro_name(macro, strings[0]);
		break;
	case 2:
		savu_macro_set_macroset_name(macro, strings[0]);
		savu_macro_set_macro_name(macro, strings[1]);
		break;
	default:
		/* do nothing */
		break;
	}
	g_strfreev(strings);

	macro->loop = savu_rmp_macro_key_info_get_loop(macro_key_info);

	count = savu_rmp_macro_key_info_get_count(macro_key_info);
	savu_macro_set_count(macro, count);

	for (i = 0; i < count; ++i)
		macro->keystrokes[i] = macro_key_info->keystrokes[i];

	return macro;
}

SavuRmpMacroKeyInfo *savu_rmp_macro_key_info_new(void) {
	return (SavuRmpMacroKeyInfo *)g_malloc0(sizeof(SavuRmpMacroKeyInfo));
}

void savu_rmp_macro_key_info_free(SavuRmpMacroKeyInfo *macro_key_info) {
	g_free(macro_key_info);
}

SavuRmpMacroKeyInfo *savu_rmp_macro_key_info_dup(SavuRmpMacroKeyInfo const *source) {
	return g_memdup((gconstpointer)source, sizeof(SavuRmpMacroKeyInfo));
}

static gchar *savu_macro_get_name_joined(SavuMacro const *macro) {
	return join_macro_name((gchar const *)macro->macroset_name, (gchar const *)macro->macro_name);
}

GaminggearMacro *savu_rmp_macro_key_info_to_gaminggear_macro(SavuRmpMacroKeyInfo const *macro) {
	GaminggearMacro *gaminggear_macro;
	gchar **strings;
	guint i, count;
	GaminggearMacroKeystroke keystroke;

	gaminggear_macro = gaminggear_macro_new(NULL, NULL, NULL);

	strings = split_macro_name((gchar const *)macro->name);
	switch (g_strv_length(strings)) {
	case 1:
		gaminggear_macro_set_macro_name(gaminggear_macro, strings[0]);
		break;
	case 2:
		gaminggear_macro_set_macroset_name(gaminggear_macro, strings[0]);
		gaminggear_macro_set_macro_name(gaminggear_macro, strings[1]);
		break;
	default:
		/* do nothing */
		break;
	}
	g_strfreev(strings);

	count = gaminggear_macro_keystrokes_get_count(&gaminggear_macro->keystrokes);
	for (i = 0; i < count; ++i) {
		roccat_keystroke_to_gaminggear_macro_keystroke(&macro->keystrokes[i], &keystroke);
		gaminggear_macro_add_keystroke(gaminggear_macro, &keystroke);
	}

	gaminggear_macro->keystrokes.loop = savu_rmp_macro_key_info_get_loop(macro);

	return gaminggear_macro;
}

SavuRmpMacroKeyInfo *savu_gaminggear_macro_to_rmp_macro_key_info(GaminggearMacro const *gaminggear_macro, GError **error) {
	SavuRmpMacroKeyInfo *macro_key_info;
	gchar *string;
	guint i, count;

	count = gaminggear_macro_keystrokes_get_count(&gaminggear_macro->keystrokes);
	if (count > SAVU_MACRO_KEYSTROKES_NUM) {
		g_set_error(error, ROCCAT_ERROR, ROCCAT_ERROR_MACRO_TOO_LONG,
				_("Macro contains %u actions while device only supports %u actions"), count, SAVU_MACRO_KEYSTROKES_NUM);
		return NULL;
	}

	macro_key_info = savu_rmp_macro_key_info_new();

	/*
	 * Windows driver stores only macro name, we join macroset and macro
	 * name for better clarity.
	 */
	string = gaminggear_macro_get_name_joined(gaminggear_macro);
	savu_rmp_macro_key_info_set_name(macro_key_info, string);
	g_free(string);

	savu_rmp_macro_key_info_set_loop(macro_key_info, gaminggear_macro->keystrokes.loop);
	savu_rmp_macro_key_info_set_count(macro_key_info, count);

	for (i = 0; i < count; ++i)
		gaminggear_macro_keystroke_to_roccat_keystroke(&gaminggear_macro->keystrokes.keystrokes[i],
				&macro_key_info->keystrokes[i]);

	return macro_key_info;
}

static SavuRmpMacroKeyInfo *savu_macro_to_rmp_macro_key_info(SavuMacro const *macro) {
	SavuRmpMacroKeyInfo *macro_key_info;
	gchar *string;
	guint i, count;

	macro_key_info = savu_rmp_macro_key_info_new();

	/*
	 * Windows driver stores only macro name, we join macroset and macro
	 * name for better clarity.
	 */
	string = savu_macro_get_name_joined(macro);
	savu_rmp_macro_key_info_set_name(macro_key_info, string);
	g_free(string);

	savu_rmp_macro_key_info_set_loop(macro_key_info, macro->loop);

	count = savu_macro_get_count(macro);
	savu_rmp_macro_key_info_set_count(macro_key_info, count);

	for (i = 0; i < count; ++i)
		macro_key_info->keystrokes[i] = macro->keystrokes[i];

	return macro_key_info;
}

SavuMacro *savu_rmp_to_macro(SavuRmp *rmp, guint index) {
	SavuRmpMacroKeyInfo *macro_key_info;
	RoccatButton *button;
	SavuMacro *macro;
	guint type;

	button = savu_rmp_get_button(rmp, index);
	type = button->type;
	g_free(button);
	if (type != SAVU_BUTTON_TYPE_MACRO)
		return NULL;

	macro_key_info = savu_rmp_get_macro_key_info(rmp, index);
	macro = savu_rmp_macro_key_info_to_macro(macro_key_info);
	g_free(macro_key_info);
	return macro;
}

static void savu_rmp_update_with_savu_rmp_macro_key_info(SavuRmp *rmp, guint index, SavuRmpMacroKeyInfo *macro_key_info) {
	RoccatButton button;

	if (!macro_key_info)
		return;

	button.type = SAVU_BUTTON_TYPE_MACRO;
	button.modifier = 0;
	button.key = 0;
	savu_rmp_set_button(rmp, index, &button);

	savu_rmp_set_macro_key_info(rmp, index, macro_key_info);
}

void savu_rmp_update_with_macro(SavuRmp *rmp, guint index, SavuMacro const *macro) {
	SavuRmpMacroKeyInfo *macro_key_info;

	if (!macro)
		return;

	macro_key_info = savu_macro_to_rmp_macro_key_info(macro);
	savu_rmp_update_with_savu_rmp_macro_key_info(rmp, index, macro_key_info);
	g_free(macro_key_info);
}
