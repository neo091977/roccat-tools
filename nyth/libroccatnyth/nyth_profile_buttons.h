#ifndef __ROCCAT_NYTH_PROFILE_BUTTONS_H__
#define __ROCCAT_NYTH_PROFILE_BUTTONS_H__

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

#include "nyth.h"
#include "nyth_profile_button.h"
#include "roccat_device.h"

G_BEGIN_DECLS

typedef struct _NythProfileButtons NythProfileButtons;

struct _NythProfileButtons {
	guint8 report_id; /* NYTH_REPORT_ID_PROFILE_BUTTONS */
	guint8 size; /* always 0x7b */
	guint8 profile_index; /* range 0-4 */
	RoccatButton buttons[NYTH_PROFILE_BUTTON_NUM];
} __attribute__ ((packed));

typedef enum {
	NYTH_BUTTON_INDEX_LEFT = 0,
	NYTH_BUTTON_INDEX_RIGHT,
	NYTH_BUTTON_INDEX_MIDDLE,
	NYTH_BUTTON_INDEX_WHEEL_UP,
	NYTH_BUTTON_INDEX_WHEEL_DOWN,
	NYTH_BUTTON_INDEX_TOP,
	NYTH_BUTTON_INDEX_FIN_LEFT,
	NYTH_BUTTON_INDEX_FIN_RIGHT,
	NYTH_BUTTON_INDEX_1,
	NYTH_BUTTON_INDEX_2,
	NYTH_BUTTON_INDEX_3,
	NYTH_BUTTON_INDEX_4,
	NYTH_BUTTON_INDEX_5,
	NYTH_BUTTON_INDEX_6,
	NYTH_BUTTON_INDEX_7,
	NYTH_BUTTON_INDEX_8,
	NYTH_BUTTON_INDEX_9,
	NYTH_BUTTON_INDEX_10,
	NYTH_BUTTON_INDEX_11,
	NYTH_BUTTON_INDEX_12,
	NYTH_BUTTON_INDEX_SHIFT_LEFT,
	NYTH_BUTTON_INDEX_SHIFT_RIGHT,
	NYTH_BUTTON_INDEX_SHIFT_MIDDLE,
	NYTH_BUTTON_INDEX_SHIFT_WHEEL_UP,
	NYTH_BUTTON_INDEX_SHIFT_WHEEL_DOWN,
	NYTH_BUTTON_INDEX_SHIFT_TOP,
	NYTH_BUTTON_INDEX_SHIFT_FIN_LEFT,
	NYTH_BUTTON_INDEX_SHIFT_FIN_RIGHT,
	NYTH_BUTTON_INDEX_SHIFT_1,
	NYTH_BUTTON_INDEX_SHIFT_2,
	NYTH_BUTTON_INDEX_SHIFT_3,
	NYTH_BUTTON_INDEX_SHIFT_4,
	NYTH_BUTTON_INDEX_SHIFT_5,
	NYTH_BUTTON_INDEX_SHIFT_6,
	NYTH_BUTTON_INDEX_SHIFT_7,
	NYTH_BUTTON_INDEX_SHIFT_8,
	NYTH_BUTTON_INDEX_SHIFT_9,
	NYTH_BUTTON_INDEX_SHIFT_10,
	NYTH_BUTTON_INDEX_SHIFT_11,
	NYTH_BUTTON_INDEX_SHIFT_12,
} NythButtonIndex;

NythProfileButtons const *nyth_profile_buttons_default(void);

gboolean nyth_profile_buttons_write(RoccatDevice *device, guint profile_index, NythProfileButtons *profile_buttons, GError **error);
NythProfileButtons *nyth_profile_buttons_read(RoccatDevice *device, guint profile_index, GError **error);

gboolean nyth_profile_buttons_equal(NythProfileButtons const *left, NythProfileButtons const *right);
void nyth_profile_buttons_copy(NythProfileButtons *destination, NythProfileButtons const *source);

G_END_DECLS

#endif
