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
#include "kova2016_profile_buttons.h"
#include "kova2016_device.h"
#include "roccat_helper.h"

static guint8 const default_profile_buttons[sizeof(Kova2016ProfileButtons)] = {
	0x07, 0x4B, 0x00, 0x01, 0x00, 0x00, 0x02, 0x00, 0x00, 0x03, 0x00, 0x00, 0x64, 0x00, 0x00, 0xD0,
	0x00, 0x00, 0x41, 0x00, 0x00, 0x65, 0x00, 0x00, 0x67, 0x00, 0x00, 0x66, 0x00, 0x00, 0x14, 0x00,
	0x00, 0x0D, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x08, 0x00, 0x00, 0x07, 0x00, 0x00, 0x25, 0x00, 0x00,
	0x21, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x62, 0x00, 0x00, 0x63,
	0x00, 0x00, 0x10, 0x00, 0x00, 0x26, 0x00, 0x00, 0x27, 0x00, 0x00
};

Kova2016ProfileButtons const *kova2016_profile_buttons_default(void) {
	return (Kova2016ProfileButtons const *)default_profile_buttons;
}

static void kova2016_profile_buttons_finalize(Kova2016ProfileButtons *profile_buttons, guint profile_index) {
	g_assert(profile_index < KOVA2016_PROFILE_NUM);
	profile_buttons->report_id = KOVA2016_REPORT_ID_PROFILE_BUTTONS;
	profile_buttons->size = sizeof(Kova2016ProfileButtons);
	profile_buttons->profile_index = profile_index;
}

gboolean kova2016_profile_buttons_write(RoccatDevice *device, guint profile_index, Kova2016ProfileButtons *profile_buttons, GError **error) {
	g_assert(profile_index < KOVA2016_PROFILE_NUM);
	kova2016_profile_buttons_finalize(profile_buttons, profile_index);
	return kova2016_device_write(device, (gchar const *)profile_buttons, sizeof(Kova2016ProfileButtons), error);
}

Kova2016ProfileButtons *kova2016_profile_buttons_read(RoccatDevice *device, guint profile_index, GError **error) {
	Kova2016ProfileButtons *profile_buttons;

	g_assert(profile_index < KOVA2016_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(device));

	if (!kova2016_select(device, profile_index, KOVA2016_CONTROL_DATA_INDEX_NONE, KOVA2016_CONTROL_REQUEST_PROFILE_BUTTONS, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));
		return NULL;
	}

	profile_buttons = (Kova2016ProfileButtons *)kova2016_device_read(device, KOVA2016_REPORT_ID_PROFILE_BUTTONS, sizeof(Kova2016ProfileButtons), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));
	return profile_buttons;
}

gboolean kova2016_profile_buttons_equal(Kova2016ProfileButtons const *left, Kova2016ProfileButtons const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY_TO_END(left, right, Kova2016ProfileButtons, buttons[0]);
	return equal ? FALSE : TRUE;
}

void kova2016_profile_buttons_copy(Kova2016ProfileButtons *destination, Kova2016ProfileButtons const *source) {
	memcpy(destination, source, sizeof(Kova2016ProfileButtons));
}
