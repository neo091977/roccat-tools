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

#include "roccat_keystroke.h"

void gaminggear_macro_keystroke_to_roccat_keystroke(GaminggearMacroKeystroke const *from, RoccatKeystroke *to) {
	to->key = from->key;
	roccat_keystroke_set_period(to, gaminggear_macro_keystroke_get_period(from));

	to->action = (from->action == GAMINGGEAR_MACRO_KEYSTROKE_ACTION_PRESS) ?
		ROCCAT_KEYSTROKE_ACTION_PRESS : ROCCAT_KEYSTROKE_ACTION_RELEASE;
}

void roccat_keystroke_to_gaminggear_macro_keystroke(RoccatKeystroke const *from, GaminggearMacroKeystroke *to) {
	to->key = from->key;
	gaminggear_macro_keystroke_set_period(to, roccat_keystroke_get_period(from));

	to->action = (from->action == ROCCAT_KEYSTROKE_ACTION_PRESS) ?
		GAMINGGEAR_MACRO_KEYSTROKE_ACTION_PRESS : GAMINGGEAR_MACRO_KEYSTROKE_ACTION_RELEASE;
}
