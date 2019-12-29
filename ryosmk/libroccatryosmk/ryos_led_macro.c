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

#include "ryos_led_macro.h"
#include "ryos_macro.h"
#include "roccat_helper.h"
#include "roccat.h"
#include "i18n-lib.h"

static void ryos_led_macro_set_macroset_name(RyosLedMacro *led_macro, gchar const *new_name) {
	g_strlcpy((gchar *)(led_macro->macroset_name), new_name, RYOS_MACRO_MACROSET_NAME_LENGTH);
}

static void ryos_led_macro_set_macro_name(RyosLedMacro *led_macro, gchar const *new_name) {
	g_strlcpy((gchar *)(led_macro->macro_name), new_name, RYOS_MACRO_MACRO_NAME_LENGTH);
}

gboolean gaminggear_macro_to_ryos_led_macro(GaminggearMacro const *gaminggear_macro, RyosLedMacro *ryos_led_macro, GError **error) {
	guint i, count;

	g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

	count = gaminggear_macro_keystrokes_get_count(&gaminggear_macro->keystrokes);
	if (count > RYOS_MACRO_KEYSTROKES_NUM) {
		g_set_error(error, ROCCAT_ERROR, ROCCAT_ERROR_MACRO_TOO_LONG,
				_("Macro contains %u actions while device only supports %u actions"), count, RYOS_MACRO_KEYSTROKES_NUM);
		return FALSE;
	}

	memset(ryos_led_macro, 0, sizeof(RyosLedMacro));
	ryos_led_macro_set_count(ryos_led_macro, count);
	ryos_led_macro->loop = gaminggear_macro->keystrokes.loop;
	ryos_led_macro_set_macroset_name(ryos_led_macro, gaminggear_macro->macroset);
	ryos_led_macro_set_macro_name(ryos_led_macro, gaminggear_macro->macro);

	for (i = 0; i < count; ++i)
		gaminggear_macro_keystroke_to_roccat_keystroke(&gaminggear_macro->keystrokes.keystrokes[i], &ryos_led_macro->keystrokes[i]);

	return TRUE;
}

gboolean ryos_led_macro_equal(RyosLedMacro const *left, RyosLedMacro const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, RyosLedMacro, loop, unused);
	return equal ? FALSE : TRUE;
}

void ryos_led_macro_copy(RyosLedMacro *destination, RyosLedMacro const *source) {
	memcpy(destination, source, sizeof(RyosLedMacro));
}

RyosLedMacro *ryos_led_macro_dup(RyosLedMacro const *led_macro) {
	return (RyosLedMacro *)g_memdup(led_macro, sizeof(RyosLedMacro));
}
