#ifndef __ROCCAT_NYTH_BUTTON_SET_H__
#define __ROCCAT_NYTH_BUTTON_SET_H__

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

#include "roccat_device.h"

G_BEGIN_DECLS

typedef struct _NythButtonSet NythButtonSet;

enum {
	NYTH_BUTTON_SET_NAME_LENGTH = 24,
};

struct _NythButtonSet {
	guint8 report_id; /* NYTH_REPORT_ID_BUTTON_SET */
	guint8 size; /* always 0x1e */
	guint8 profile_index;
	guint8 name[NYTH_BUTTON_SET_NAME_LENGTH];
	guint16 singles;
	guint8 doubles;
} __attribute__ ((packed));

static inline void nyth_button_set_set_singles(NythButtonSet *button_set, guint16 new_value) {
	button_set->singles = GUINT16_TO_LE(new_value);
}

static inline guint16 nyth_button_set_get_singles(NythButtonSet const *button_set) {
	return GUINT16_FROM_LE(button_set->singles);
}

NythButtonSet const *nyth_button_set_default(void);

NythButtonSet *nyth_button_set_read(RoccatDevice *device, guint profile_index, GError **error);
gboolean nyth_button_set_write(RoccatDevice *device, guint profile_index, NythButtonSet *button_set, GError **error);

/* Needed because string in NythButtonSet does not need to be 0-terminated */
gchar *nyth_button_set_get_name(NythButtonSet const *button_set);
void nyth_button_set_set_name(NythButtonSet *button_set, gchar const *new_name);

gboolean nyth_button_set_equal(NythButtonSet const *left, NythButtonSet const *right);
void nyth_button_set_copy(NythButtonSet *destination, NythButtonSet const *source);

G_END_DECLS

#endif
