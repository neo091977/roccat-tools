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

#include "ryos_macro.h"
#include "ryos_device.h"
#include "roccat_helper.h"
#include "roccat.h"
#include "i18n-lib.h"
#include <string.h>

static void ryos_macro_set_clear_macroset_name(RyosMacro *macro) {
	memset(&macro->macroset_name, 0, RYOS_MACRO_MACROSET_NAME_LENGTH);
}

static void ryos_macro_set_macroset_name(RyosMacro *macro, gchar const *new_name) {
	g_strlcpy((gchar *)(macro->macroset_name), new_name, RYOS_MACRO_MACROSET_NAME_LENGTH);
}

static void ryos_macro_set_macro_name(RyosMacro *macro, gchar const *new_name) {
	g_strlcpy((gchar *)(macro->macro_name), new_name, RYOS_MACRO_MACRO_NAME_LENGTH);
}

static guint16 ryos_macro_calc_checksum(RyosMacro const *macro) {
	gsize calc_size = G_STRUCT_OFFSET(RyosMacro, keystrokes[0]) + ryos_macro_get_count(macro) * sizeof(RoccatKeystroke);
	return roccat_calc_bytesum(macro, calc_size);
}

static void ryos_macro_set_checksum(RyosMacro *macro, guint16 new_value) {
	gsize offset = G_STRUCT_OFFSET(RyosMacro, keystrokes[0]) + ryos_macro_get_count(macro) * sizeof(RoccatKeystroke);
	*(guint16 *)G_STRUCT_MEMBER_P(macro, offset) = GUINT16_TO_LE(new_value);
}

static guint16 ryos_macro_calc_size(RyosMacro const *macro) {
	return G_STRUCT_OFFSET(RyosMacro, keystrokes[0]) + ryos_macro_get_count(macro) * sizeof(RoccatKeystroke) + sizeof(guint16);
}

static void ryos_macro_set_size(RyosMacro *macro, guint16 new_value) {
	macro->size = GUINT16_TO_LE(new_value);
}

static void ryos_macro_finalize(RyosMacro *macro, guint report_id, guint profile_index, guint button_index) {
	macro->report_id = report_id;
	macro->profile_index = profile_index;
	macro->button_index = button_index;
	ryos_macro_set_size(macro, ryos_macro_calc_size(macro));
	ryos_macro_set_checksum(macro, ryos_macro_calc_checksum(macro));
}

gboolean ryos_macro_write(RoccatDevice *ryos, guint profile_index, guint button_index, RyosMacro *macro, GError **error) {
	g_assert(profile_index < RYOS_PROFILE_NUM);
	ryos_macro_finalize(macro, RYOS_REPORT_ID_MACRO, profile_index, button_index);
	return ryos_device_write(ryos, (gchar const *)macro, sizeof(RyosMacro), error);
}

RyosMacro *ryos_macro_read(RoccatDevice *ryos, guint profile_index, guint button_index, GError **error) {
	RyosMacro *macro;

	g_assert(profile_index < RYOS_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(ryos));

	if (!ryos_select(ryos, profile_index, button_index, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(ryos));
		return NULL;
	}

	macro = (RyosMacro *)ryos_device_read(ryos, RYOS_REPORT_ID_MACRO, sizeof(RyosMacro), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(ryos));

	return macro;
}

gboolean ryos_light_macro_write(RoccatDevice *ryos, guint profile_index, RyosMacro *light_macro, GError **error) {
	g_assert(profile_index < RYOS_PROFILE_NUM);

	/* light macro contains less info when readout. The lost data is set
	 * to 0 here, so modified info is not affected on next config startup.
	 */
	ryos_macro_set_clear_macroset_name(light_macro);
	light_macro->loop = 0;

	ryos_macro_finalize(light_macro, RYOS_REPORT_ID_LIGHT_MACRO, profile_index, 0);
	return ryos_device_write(ryos, (gchar const *)light_macro, sizeof(RyosMacro), error);
}

RyosMacro *ryos_light_macro_read(RoccatDevice *ryos, guint profile_index, GError **error) {
	RyosMacro *light_macro;

	g_assert(profile_index < RYOS_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(ryos));

	if (!ryos_select(ryos, profile_index, RYOS_CONTROL_REQUEST_LIGHT_MACRO, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(ryos));
		return NULL;
	}

	light_macro = (RyosMacro *)ryos_device_read(ryos, RYOS_REPORT_ID_LIGHT_MACRO, sizeof(RyosMacro), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(ryos));

	return light_macro;
}

gboolean gaminggear_macro_to_ryos_macro(GaminggearMacro const *gaminggear_macro, RyosMacro *ryos_macro, GError **error) {
	guint i, count;

	g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

	count = gaminggear_macro_keystrokes_get_count(&gaminggear_macro->keystrokes);
	if (count > RYOS_MACRO_KEYSTROKES_NUM) {
		g_set_error(error, ROCCAT_ERROR, ROCCAT_ERROR_MACRO_TOO_LONG,
				_("Macro contains %u actions while device only supports %u actions"), count, RYOS_MACRO_KEYSTROKES_NUM);
		return FALSE;
	}

	memset(ryos_macro, 0, sizeof(RyosMacro));
	ryos_macro_set_count(ryos_macro, count);
	ryos_macro->loop = gaminggear_macro->keystrokes.loop;
	ryos_macro_set_macroset_name(ryos_macro, gaminggear_macro->macroset);
	ryos_macro_set_macro_name(ryos_macro, gaminggear_macro->macro);

	for (i = 0; i < count; ++i)
		gaminggear_macro_keystroke_to_roccat_keystroke(&gaminggear_macro->keystrokes.keystrokes[i], &ryos_macro->keystrokes[i]);

	return TRUE;
}

GaminggearMacro *ryos_macro_to_gaminggear_macro(RyosMacro const *ryos_macro) {
	GaminggearMacro *gaminggear_macro;
	GaminggearMacroKeystroke keystroke;
	guint i, count;

	gaminggear_macro = gaminggear_macro_new((gchar const *)ryos_macro->macroset_name, (gchar const *)ryos_macro->macro_name, NULL);

	gaminggear_macro->keystrokes.loop = ryos_macro->loop;

	count = ryos_macro_get_count(ryos_macro);
	count = MIN(count, GAMINGGEAR_MACRO_KEYSTROKES_NUM);
	count = MIN(count, RYOS_MACRO_KEYSTROKES_NUM);

	for (i = 0; i < count; ++i) {
		roccat_keystroke_to_gaminggear_macro_keystroke(&ryos_macro->keystrokes[i], &keystroke);
		gaminggear_macro_keystrokes_add(&gaminggear_macro->keystrokes, &keystroke);
	}

	return gaminggear_macro;
}

gboolean ryos_macro_equal(RyosMacro const *left, RyosMacro const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, RyosMacro, loop, checksum);
	return equal ? FALSE : TRUE;
}

void ryos_macro_copy(RyosMacro *destination, RyosMacro const *source) {
	memcpy(destination, source, sizeof(RyosMacro));
}

RyosMacro *ryos_macro_dup(RyosMacro const *macro) {
	return (RyosMacro *)g_memdup(macro, sizeof(RyosMacro));
}
