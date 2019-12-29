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

#include "sova_macro.h"
#include "sova.h"
#include "roccat_helper.h"
#include "roccat.h"
#include "i18n-lib.h"
#include <string.h>

/*
 * Macro resulting from live recording has it's size calculated depending on
 * action count with checksum at end of variable number of actions. Also the
 * name section is split in three.
 * Macro written with Swarm is const size = sizeof(SovaMacro), checksum always
 * after SOVA_MACRO_KEYSTROKES_NUM actions. Name section is split in two.
 */
static gboolean sova_macro_is_editor_namesection(SovaMacro const *macro) {
	if (sova_macro_get_size(macro) - (guint)SOVA_MACRO_CONST_SIZE == sova_macro_get_count(macro) * sizeof(RoccatKeystroke))
		return FALSE;

	if (strcmp((gchar const *)macro->record.macroset_name, "MLR") == 0)
		return FALSE;

	return TRUE;
}

static void sova_macro_set_editor_macroset_name(SovaMacro *macro, gchar const *new_name) {
	g_strlcpy((gchar *)(macro->editor.macroset_name), new_name, SOVA_MACRO_EDITOR_MACROSET_NAME_LENGTH);
}

static void sova_macro_set_editor_macro_name(SovaMacro *macro, gchar const *new_name) {
	g_strlcpy((gchar *)(macro->editor.macro_name), new_name, SOVA_MACRO_EDITOR_MACRO_NAME_LENGTH);
}

static gchar *sova_macro_get_unified_macroset_name(SovaMacro const *macro) {
	if (sova_macro_is_editor_namesection(macro))
		return g_strdup((gchar const *)macro->editor.macroset_name);
	else
		return g_strdup_printf("%s %s", (gchar const *)macro->record.device, (gchar const *)macro->record.macroset_name);
}

gchar const *sova_macro_get_macroset_name(SovaMacro const *macro) {
	if (sova_macro_is_editor_namesection(macro))
		return (gchar const *)macro->editor.macroset_name;
	else
		return (gchar const *)macro->record.macroset_name;
}

gchar const *sova_macro_get_macro_name(SovaMacro const *macro) {
	if (sova_macro_is_editor_namesection(macro))
		return (gchar const *)macro->editor.macro_name;
	else
		return (gchar const *)macro->record.macro_name;
}

static guint16 sova_macro_calc_checksum(SovaMacro const *macro) {
	return ROCCAT_BYTESUM_PARTIALLY(macro, SovaMacro, report_id, checksum);
}

static void sova_macro_set_checksum(SovaMacro *macro, guint16 new_value) {
	macro->checksum = GUINT16_TO_LE(new_value);
}

void sova_macro_finalize(SovaMacro *macro, guint profile_index, guint keys_index) {
	macro->report_id = SOVA_REPORT_ID_MACRO;
	macro->size = GUINT16_TO_LE(sizeof(SovaMacro));
	macro->profile_index = profile_index;
	macro->button_index = keys_index;
	sova_macro_set_checksum(macro, sova_macro_calc_checksum(macro));
}

gboolean gaminggear_macro_to_sova_macro(GaminggearMacro const *gaminggear_macro, SovaMacro *sova_macro, GError **error) {
	guint i, count;

	g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

	count = gaminggear_macro_keystrokes_get_count(&gaminggear_macro->keystrokes);
	if (count > SOVA_MACRO_KEYSTROKES_NUM) {
		g_set_error(error, ROCCAT_ERROR, ROCCAT_ERROR_MACRO_TOO_LONG,
				_("Macro contains %u actions while device only supports %u actions"), count, SOVA_MACRO_KEYSTROKES_NUM);
		return FALSE;
	}

	memset(sova_macro, 0, sizeof(SovaMacro));
	sova_macro_set_count(sova_macro, count);
	sova_macro->loop = gaminggear_macro->keystrokes.loop;
	sova_macro_set_editor_macroset_name(sova_macro, gaminggear_macro->macroset);
	sova_macro_set_editor_macro_name(sova_macro, gaminggear_macro->macro);

	for (i = 0; i < count; ++i)
		gaminggear_macro_keystroke_to_roccat_keystroke(&gaminggear_macro->keystrokes.keystrokes[i], &sova_macro->keystrokes[i]);

	return TRUE;
}

GaminggearMacro *sova_macro_to_gaminggear_macro(SovaMacro const *sova_macro) {
	GaminggearMacro *gaminggear_macro;
	GaminggearMacroKeystroke keystroke;
	guint i, count;
	gchar *macroset_name;

	macroset_name = sova_macro_get_unified_macroset_name(sova_macro);
	gaminggear_macro = gaminggear_macro_new(macroset_name, sova_macro_get_macro_name(sova_macro), NULL);
	g_free(macroset_name);

	gaminggear_macro->keystrokes.loop = sova_macro->loop;

	count = sova_macro_get_count(sova_macro);
	count = MIN(count, GAMINGGEAR_MACRO_KEYSTROKES_NUM);
	count = MIN(count, SOVA_MACRO_KEYSTROKES_NUM);

	for (i = 0; i < count; ++i) {
		roccat_keystroke_to_gaminggear_macro_keystroke(&sova_macro->keystrokes[i], &keystroke);
		gaminggear_macro_keystrokes_add(&gaminggear_macro->keystrokes, &keystroke);
	}

	return gaminggear_macro;
}

gboolean sova_macro_equal(SovaMacro const *left, SovaMacro const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, SovaMacro, loop, checksum);
	return equal ? FALSE : TRUE;
}

void sova_macro_copy(SovaMacro *destination, SovaMacro const *source) {
	memcpy(destination, source, sizeof(SovaMacro));
}

SovaMacro *sova_macro_dup(SovaMacro const *old) {
	return g_memdup(old, sizeof(SovaMacro));
}

gboolean sova_macro_write(RoccatDevice *device, guint profile_index, guint button_index, SovaMacro *macro, GError **error) {
	g_assert(profile_index < SOVA_PROFILE_NUM);
	sova_macro_finalize(macro, profile_index, button_index);
	return sova_device_write(device, (gchar const *)macro, sizeof(SovaMacro), error);
}

SovaMacro *sova_macro_read(RoccatDevice *device, guint profile_index, guint button_index, GError **error) {
	SovaMacro *macro;

	g_assert(profile_index < SOVA_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(device));

	if (!sova_select(device, profile_index, button_index, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));
		return NULL;
	}

	macro = (SovaMacro *)sova_device_read(device, SOVA_REPORT_ID_MACRO, sizeof(SovaMacro), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));

	return macro;
}
