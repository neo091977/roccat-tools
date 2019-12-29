#ifndef __ROCCAT_KOVA2016_PROFILE_BUTTONS_H__
#define __ROCCAT_KOVA2016_PROFILE_BUTTONS_H__

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

#include "kova2016.h"
#include "kova2016_profile_button.h"
#include "roccat_device.h"

G_BEGIN_DECLS

typedef struct _Kova2016ProfileButtons Kova2016ProfileButtons;

struct _Kova2016ProfileButtons {
	guint8 report_id; /* KOVA2016_REPORT_ID_PROFILE_BUTTONS */
	guint8 size; /* always 0x4b */
	guint8 profile_index; /* range 0-4 */
	RoccatButton buttons[KOVA2016_PROFILE_BUTTON_NUM];
} __attribute__ ((packed));

typedef enum {
	KOVA2016_BUTTON_INDEX_LEFT = 0,
	KOVA2016_BUTTON_INDEX_RIGHT,
	KOVA2016_BUTTON_INDEX_MIDDLE,
	KOVA2016_BUTTON_INDEX_SMART_CAST_LEFT,
	KOVA2016_BUTTON_INDEX_FORWARD_LEFT,
	KOVA2016_BUTTON_INDEX_BACKWARD_LEFT,
	KOVA2016_BUTTON_INDEX_SMART_CAST_RIGHT,
	KOVA2016_BUTTON_INDEX_FORWARD_RIGHT,
	KOVA2016_BUTTON_INDEX_BACKWARD_RIGHT,
	KOVA2016_BUTTON_INDEX_TOP,
	KOVA2016_BUTTON_INDEX_WHEEL_UP,
	KOVA2016_BUTTON_INDEX_WHEEL_DOWN,
	KOVA2016_BUTTON_INDEX_SHIFT_LEFT,
	KOVA2016_BUTTON_INDEX_SHIFT_RIGHT,
	KOVA2016_BUTTON_INDEX_SHIFT_MIDDLE,
	KOVA2016_BUTTON_INDEX_SHIFT_SMART_CAST_LEFT,
	KOVA2016_BUTTON_INDEX_SHIFT_FORWARD_LEFT,
	KOVA2016_BUTTON_INDEX_SHIFT_BACKWARD_LEFT,
	KOVA2016_BUTTON_INDEX_SHIFT_SMART_CAST_RIGHT,
	KOVA2016_BUTTON_INDEX_SHIFT_FORWARD_RIGHT,
	KOVA2016_BUTTON_INDEX_SHIFT_BACKWARD_RIGHT,
	KOVA2016_BUTTON_INDEX_SHIFT_TOP,
	KOVA2016_BUTTON_INDEX_SHIFT_WHEEL_UP,
	KOVA2016_BUTTON_INDEX_SHIFT_WHEEL_DOWN,
} Kova2016ButtonIndex;

Kova2016ProfileButtons const *kova2016_profile_buttons_default(void);

gboolean kova2016_profile_buttons_write(RoccatDevice *device, guint profile_index, Kova2016ProfileButtons *profile_buttons, GError **error);
Kova2016ProfileButtons *kova2016_profile_buttons_read(RoccatDevice *device, guint profile_index, GError **error);

gboolean kova2016_profile_buttons_equal(Kova2016ProfileButtons const *left, Kova2016ProfileButtons const *right);
void kova2016_profile_buttons_copy(Kova2016ProfileButtons *destination, Kova2016ProfileButtons const *source);

G_END_DECLS

#endif
