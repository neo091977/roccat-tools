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

#include "suora_rkp_macro.h"
#include "roccat_helper.h"
#include "roccat.h"
#include "i18n-lib.h"
#include <string.h>

void suora_rkp_macro_set_macroset_name(SuoraRkpMacro *macro, gchar const *new_name) {
	g_strlcpy((gchar *)(macro->macroset_name), new_name, SUORA_RKP_MACRO_MACROSET_NAME_LENGTH);
}

void suora_rkp_macro_set_macro_name(SuoraRkpMacro *macro, gchar const *new_name) {
	g_strlcpy((gchar *)(macro->macro_name), new_name, SUORA_RKP_MACRO_MACRO_NAME_LENGTH);
}

void suora_rkp_macro_finalize(SuoraRkpMacro *macro, guint keys_index) {
	macro->report_id = 0x0e;
	macro->size = GUINT16_TO_LE(sizeof(SuoraRkpMacro));
	macro->profile_index = 0;
	macro->button_index = keys_index;
}

gboolean gaminggear_macro_to_suora_rkp_macro(GaminggearMacro const *gaminggear_macro, SuoraRkpMacro *suora_rkp_macro, GError **error) {
	guint i, count;

	g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

	count = gaminggear_macro_keystrokes_get_count(&gaminggear_macro->keystrokes);
	if (count > SUORA_MACRO_ACTION_NUM) {
		g_set_error(error, ROCCAT_ERROR, ROCCAT_ERROR_MACRO_TOO_LONG,
				_("Macro contains %u actions while device only supports %u actions"), count, SUORA_MACRO_ACTION_NUM);
		return FALSE;
	}

	memset(suora_rkp_macro, 0, sizeof(SuoraRkpMacro));
	suora_rkp_macro_set_count(suora_rkp_macro, count);
	suora_rkp_macro->loop = gaminggear_macro->keystrokes.loop;
	suora_rkp_macro_set_macroset_name(suora_rkp_macro, gaminggear_macro->macroset);
	suora_rkp_macro_set_macro_name(suora_rkp_macro, gaminggear_macro->macro);

	for (i = 0; i < count; ++i)
		gaminggear_macro_keystroke_to_roccat_keystroke(&gaminggear_macro->keystrokes.keystrokes[i], &suora_rkp_macro->keystrokes[i]);

	return TRUE;
}

gboolean suora_rkp_macro_equal(SuoraRkpMacro const *left, SuoraRkpMacro const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, SuoraRkpMacro, loop, unused);
	return equal ? FALSE : TRUE;
}

void suora_rkp_macro_copy(SuoraRkpMacro *destination, SuoraRkpMacro const *source) {
	memcpy(destination, source, sizeof(SuoraRkpMacro));
}

SuoraRkpMacro *suora_rkp_macro_dup(SuoraRkpMacro const *old) {
	return g_memdup(old, sizeof(SuoraRkpMacro));
}

static void roccat_keystroke_to_suora_macro_action(RoccatKeystroke const *from, SuoraMacroAction *to) {
	to->properties = (from->action == ROCCAT_KEYSTROKE_ACTION_PRESS) ?
			SUORA_MACRO_ACTION_PROPERTIES_ACTION_PRESS : SUORA_MACRO_ACTION_PROPERTIES_ACTION_RELEASE;
	to->properties |= MAX(1, roccat_keystroke_get_period(from) / SUORA_MACRO_ACTION_PERIOD_FACTOR);
	to->key = from->key;
}

gboolean suora_rkp_macro_to_macro(SuoraRkpMacro const *rkp_macro, SuoraMacro *suora_macro, GError **error) {
	guint i, count;
	
	count = suora_rkp_macro_get_count(rkp_macro);
	if (count > SUORA_MACRO_ACTION_NUM) {
		g_set_error(error, ROCCAT_ERROR, ROCCAT_ERROR_MACRO_TOO_LONG, _("Macro contains %u actions while device only supports %u actions"), count, SUORA_MACRO_ACTION_NUM);
		return FALSE;
	}

	memset(suora_macro, 0, sizeof(SuoraMacro));
	suora_macro_set_loop(suora_macro, rkp_macro->loop);

	for (i = 0; i < count; ++i)
		roccat_keystroke_to_suora_macro_action(&rkp_macro->keystrokes[i], &suora_macro->actions[i]);

	return TRUE;
}

static void suora_macro_action_to_roccat_keystroke(SuoraMacroAction const *from, RoccatKeystroke *to) {
	to->action = (from->properties & SUORA_MACRO_ACTION_PROPERTIES_ACTION_MASK) == SUORA_MACRO_ACTION_PROPERTIES_ACTION_PRESS ?
			GAMINGGEAR_MACRO_KEYSTROKE_ACTION_PRESS : GAMINGGEAR_MACRO_KEYSTROKE_ACTION_RELEASE;
	roccat_keystroke_set_period(to, (from->properties & SUORA_MACRO_ACTION_PROPERTIES_PERIOD_MASK) * SUORA_MACRO_ACTION_PERIOD_FACTOR);
	to->key = from->key;
}

void suora_macro_to_rkp_macro(SuoraMacro const *suora_macro, SuoraRkpMacro *rkp_macro) {
	guint i;
	guint count;
	
	g_assert((guint)SUORA_MACRO_ACTION_NUM < (guint)SUORA_RKP_MACRO_KEYSTROKES_NUM);

	count = 0;
	for (i = 0; i < SUORA_MACRO_ACTION_NUM; ++i) {
		if (suora_macro->actions[i].key == 0)
			break;
		suora_macro_action_to_roccat_keystroke(&suora_macro->actions[i], &rkp_macro->keystrokes[i]);
		++count;
	}
	
	rkp_macro->loop = suora_macro_get_loop(suora_macro);
	suora_rkp_macro_set_macroset_name(rkp_macro, "Suora");
	suora_rkp_macro_set_macro_name(rkp_macro, "Macro");
	suora_rkp_macro_set_count(rkp_macro, count);
}
