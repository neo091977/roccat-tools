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
#include "nyth_profile_buttons.h"
#include "nyth_device.h"
#include "roccat_helper.h"

static guint8 const default_profile_buttons[sizeof(NythProfileButtons)] = {
	0x07, 0x7B, 0x00, 0x01, 0x00, 0x00, 0x02, 0x00, 0x00, 0x03, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x0E,
	0x00, 0x00, 0x41, 0x00, 0x00, 0x09, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x05, 0x00, 0x1E, 0x05, 0x00,
	0x1F, 0x05, 0x00, 0x20, 0x05, 0x00, 0x21, 0x05, 0x00, 0x22, 0x05, 0x00, 0x23, 0x05, 0x00, 0x24,
	0x05, 0x00, 0x25, 0x05, 0x00, 0x26, 0x05, 0x00, 0x27, 0x05, 0x00, 0x2D, 0x05, 0x00, 0x2E, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x25, 0x00, 0x00, 0x26, 0x00, 0x00, 0x27, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x63, 0x00, 0x00, 0x62, 0x00, 0x00, 0x05, 0x02, 0x1E, 0x05, 0x02, 0x1F, 0x05, 0x02, 0x20,
	0x05, 0x02, 0x21, 0x05, 0x02, 0x22, 0x05, 0x02, 0x23, 0x05, 0x02, 0x24, 0x05, 0x02, 0x25, 0x05,
	0x02, 0x26, 0x05, 0x02, 0x27, 0x05, 0x02, 0x2D, 0x05, 0x02, 0x2E
};

NythProfileButtons const *nyth_profile_buttons_default(void) {
	return (NythProfileButtons const *)default_profile_buttons;
}

static void nyth_profile_buttons_finalize(NythProfileButtons *profile_buttons, guint profile_index) {
	g_assert(profile_index < NYTH_PROFILE_NUM);
	profile_buttons->report_id = NYTH_REPORT_ID_PROFILE_BUTTONS;
	profile_buttons->size = sizeof(NythProfileButtons);
	profile_buttons->profile_index = profile_index;
}

gboolean nyth_profile_buttons_write(RoccatDevice *device, guint profile_index, NythProfileButtons *profile_buttons, GError **error) {
	g_assert(profile_index < NYTH_PROFILE_NUM);
	nyth_profile_buttons_finalize(profile_buttons, profile_index);
	return nyth_device_write(device, (gchar const *)profile_buttons, sizeof(NythProfileButtons), error);
}

NythProfileButtons *nyth_profile_buttons_read(RoccatDevice *device, guint profile_index, GError **error) {
	NythProfileButtons *profile_buttons;

	g_assert(profile_index < NYTH_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(device));

	if (!nyth_select(device, profile_index, NYTH_CONTROL_DATA_INDEX_NONE, NYTH_CONTROL_REQUEST_PROFILE_BUTTONS, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));
		return NULL;
	}

	profile_buttons = (NythProfileButtons *)nyth_device_read(device, NYTH_REPORT_ID_PROFILE_BUTTONS, sizeof(NythProfileButtons), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));
	return profile_buttons;
}

gboolean nyth_profile_buttons_equal(NythProfileButtons const *left, NythProfileButtons const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY_TO_END(left, right, NythProfileButtons, buttons[0]);
	return equal ? FALSE : TRUE;
}

void nyth_profile_buttons_copy(NythProfileButtons *destination, NythProfileButtons const *source) {
	memcpy(destination, source, sizeof(NythProfileButtons));
}
