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

#include "suora_macro.h"
#include "suora_device.h"
#include "roccat.h"
#include <string.h>

SuoraMacro *suora_macro_read(RoccatDevice *device, guint index, GError **error) {
	SuoraDataDeclaration declaration = {0};
	declaration.macro_index = index;
	return (SuoraMacro *)suora_read_interrupt(device, SUORA_FEATURE_REQUEST_MACRO, &declaration, sizeof(SuoraMacro), error);
}

gboolean suora_macro_write(RoccatDevice *device, guint index, SuoraMacro const *macro, GError **error) {
	SuoraDataDeclaration declaration = {0};
	declaration.macro_index = index;
	return suora_write_interrupt(device, SUORA_FEATURE_REQUEST_MACRO, &declaration, (gchar const *)macro, sizeof(SuoraMacro), error);
}
