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

#include "roccat_light.h"
#include <string.h>

gboolean roccat_light_equal(RoccatLight const *left, RoccatLight const *right) {
	return memcmp(left, right, sizeof(RoccatLight)) ? FALSE : TRUE;
}

void roccat_light_copy(RoccatLight *destination, RoccatLight const *source) {
	memcpy (destination, source, sizeof(RoccatLight));
}
