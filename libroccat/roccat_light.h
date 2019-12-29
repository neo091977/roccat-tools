#ifndef __ROCCAT_LIGHT_H__
#define __ROCCAT_LIGHT_H__

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

#include <glib.h>

G_BEGIN_DECLS

typedef struct _RoccatLight RoccatLight;

struct _RoccatLight {
	guint8 index;
	guint8 red;
	guint8 green;
	guint8 blue;
} __attribute__ ((packed));

gboolean roccat_light_equal(RoccatLight const *left, RoccatLight const *right);
void roccat_light_copy(RoccatLight *destination, RoccatLight const *source);

G_END_DECLS

#endif
