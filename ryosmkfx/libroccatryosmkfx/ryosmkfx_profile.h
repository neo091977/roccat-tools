#ifndef __ROCCAT_RYOSMKFX_PROFILE_H__
#define __ROCCAT_RYOSMKFX_PROFILE_H__

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

#include "ryos_profile.h"
#include "roccat_helper.h"

G_BEGIN_DECLS

static inline guint ryosmkfx_profile_get_count(guint value) {
	return roccat_get_nibble8(value, ROCCAT_NIBBLE_HIGH);
}

static inline guint ryosmkfx_profile_get_index(guint value) {
	return roccat_get_nibble8(value, ROCCAT_NIBBLE_LOW);
}

static inline guint8 ryosmkfx_profile_create_value(guint index, guint count) {
	guint8 result;
	roccat_set_nibble8(&result, ROCCAT_NIBBLE_HIGH, count);
	roccat_set_nibble8(&result, ROCCAT_NIBBLE_LOW, index);
	return result;
}

G_END_DECLS

#endif
