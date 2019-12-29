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

#include "skeltr_macro.h"
#include "skeltr.h"
#include "roccat_helper.h"
#include "roccat.h"
#include "i18n-lib.h"
#include <string.h>

/*
 * Macro resulting from live recording has it's size calculated depending on
 * action count with checksum at end of variable number of actions. Also the
 * name section is split in three.
 * Macro written with Swarm is const size = sizeof(SkeltrMacro), checksum always
 * after SKELTR_MACRO_KEYSTROKES_NUM actions. Name section is split in two.
 */
static gboolean skeltr_macro_is_editor_namesection(SkeltrMacro const *macro) {
	if (skeltr_macro_get_size(macro) - (guint)SKELTR_MACRO_CONST_SIZE == skeltr_macro_get_count(macro) * sizeof(RoccatKeystroke))
		return FALSE;

	if (strcmp((gchar const *)macro->record.macroset_name, "MLR") == 0)
		return FALSE;

	return TRUE;
}

static void skeltr_macro_set_editor_macroset_name(SkeltrMacro *macro, gchar const *new_name) {
	g_strlcpy((gchar *)(macro->editor.macroset_name), new_name, SKELTR_MACRO_EDITOR_MACROSET_NAME_LENGTH);
}

static void skeltr_macro_set_editor_macro_name(SkeltrMacro *macro, gchar const *new_name) {
	g_strlcpy((gchar *)(macro->editor.macro_name), new_name, SKELTR_MACRO_EDITOR_MACRO_NAME_LENGTH);
}

static gchar *skeltr_macro_get_unified_macroset_name(SkeltrMacro const *macro) {
	if (skeltr_macro_is_editor_namesection(macro))
		return g_strdup((gchar const *)macro->editor.macroset_name);
	else
		return g_strdup_printf("%s %s", (gchar const *)macro->record.device, (gchar const *)macro->record.macroset_name);
}

gchar const *skeltr_macro_get_macroset_name(SkeltrMacro const *macro) {
	if (skeltr_macro_is_editor_namesection(macro))
		return (gchar const *)macro->editor.macroset_name;
	else
		return (gchar const *)macro->record.macroset_name;
}

gchar const *skeltr_macro_get_macro_name(SkeltrMacro const *macro) {
	if (skeltr_macro_is_editor_namesection(macro))
		return (gchar const *)macro->editor.macro_name;
	else
		return (gchar const *)macro->record.macro_name;
}

static guint16 skeltr_macro_calc_checksum(SkeltrMacro const *macro) {
	return ROCCAT_BYTESUM_PARTIALLY(macro, SkeltrMacro, report_id, checksum);
}

void skeltr_macro_finalize(SkeltrMacro *macro, guint profile_index, guint keys_index) {
	macro->report_id = SKELTR_REPORT_ID_MACRO;
	macro->size = GUINT16_TO_LE(sizeof(SkeltrMacro));
	macro->profile_index = profile_index;
	macro->button_index = keys_index;
	skeltr_macro_set_checksum(macro, skeltr_macro_calc_checksum(macro));
}

gboolean gaminggear_macro_to_skeltr_macro(GaminggearMacro const *gaminggear_macro, SkeltrMacro *skeltr_macro, GError **error) {
	guint i, count;

	g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

	count = gaminggear_macro_keystrokes_get_count(&gaminggear_macro->keystrokes);
	if (count > SKELTR_MACRO_KEYSTROKES_NUM) {
		g_set_error(error, ROCCAT_ERROR, ROCCAT_ERROR_MACRO_TOO_LONG,
				_("Macro contains %u actions while device only supports %u actions"), count, SKELTR_MACRO_KEYSTROKES_NUM);
		return FALSE;
	}

	memset(skeltr_macro, 0, sizeof(SkeltrMacro));
	skeltr_macro_set_count(skeltr_macro, count);
	skeltr_macro->loop = gaminggear_macro->keystrokes.loop;
	skeltr_macro_set_editor_macroset_name(skeltr_macro, gaminggear_macro->macroset);
	skeltr_macro_set_editor_macro_name(skeltr_macro, gaminggear_macro->macro);

	for (i = 0; i < count; ++i)
		gaminggear_macro_keystroke_to_roccat_keystroke(&gaminggear_macro->keystrokes.keystrokes[i], &skeltr_macro->keystrokes[i]);

	return TRUE;
}

GaminggearMacro *skeltr_macro_to_gaminggear_macro(SkeltrMacro const *skeltr_macro) {
	GaminggearMacro *gaminggear_macro;
	GaminggearMacroKeystroke keystroke;
	guint i, count;
	gchar *macroset_name;

	macroset_name = skeltr_macro_get_unified_macroset_name(skeltr_macro);
	gaminggear_macro = gaminggear_macro_new(macroset_name, skeltr_macro_get_macro_name(skeltr_macro), NULL);
	g_free(macroset_name);

	gaminggear_macro->keystrokes.loop = skeltr_macro->loop;

	count = skeltr_macro_get_count(skeltr_macro);
	count = MIN(count, GAMINGGEAR_MACRO_KEYSTROKES_NUM);
	count = MIN(count, SKELTR_MACRO_KEYSTROKES_NUM);

	for (i = 0; i < count; ++i) {
		roccat_keystroke_to_gaminggear_macro_keystroke(&skeltr_macro->keystrokes[i], &keystroke);
		gaminggear_macro_keystrokes_add(&gaminggear_macro->keystrokes, &keystroke);
	}

	return gaminggear_macro;
}

gboolean skeltr_macro_equal(SkeltrMacro const *left, SkeltrMacro const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, SkeltrMacro, loop, checksum);
	return equal ? FALSE : TRUE;
}

void skeltr_macro_copy(SkeltrMacro *destination, SkeltrMacro const *source) {
	memcpy(destination, source, sizeof(SkeltrMacro));
}

SkeltrMacro *skeltr_macro_dup(SkeltrMacro const *old) {
	return g_memdup(old, sizeof(SkeltrMacro));
}

gboolean skeltr_macro_write(RoccatDevice *device, guint profile_index, guint button_index, SkeltrMacro *macro, GError **error) {
	g_assert(profile_index < SKELTR_PROFILE_NUM);
	skeltr_macro_finalize(macro, profile_index, button_index);
	return skeltr_device_write(device, (gchar const *)macro, sizeof(SkeltrMacro), error);
}

SkeltrMacro *skeltr_macro_read(RoccatDevice *device, guint profile_index, guint button_index, GError **error) {
	SkeltrMacro *macro;

	g_assert(profile_index < SKELTR_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(device));

	if (!skeltr_select(device, profile_index, button_index, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));
		return NULL;
	}

	macro = (SkeltrMacro *)skeltr_device_read(device, SKELTR_REPORT_ID_MACRO, sizeof(SkeltrMacro), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));

	return macro;
}
