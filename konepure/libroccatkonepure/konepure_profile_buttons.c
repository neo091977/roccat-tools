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

#include "konepure.h"
#include "roccat_helper.h"

static guint16 konepure_profile_buttons_calc_checksum(KonepureProfileButtons const *profile_buttons) {
	return ROCCAT_BYTESUM_PARTIALLY(profile_buttons, KonepureProfileButtons, report_id, checksum);
}

static void konepure_profile_buttons_set_checksum(KonepureProfileButtons *profile_buttons) {
	guint16 checksum = konepure_profile_buttons_calc_checksum(profile_buttons);
	profile_buttons->checksum = GUINT16_TO_LE(checksum);
}

static void konepure_profile_buttons_finalize(KonepureProfileButtons *profile_buttons, guint profile_index) {
	g_assert(profile_index < KONEPLUS_PROFILE_NUM);
	profile_buttons->profile_number = profile_index;
	konepure_profile_buttons_set_checksum(profile_buttons);
}

gboolean konepure_profile_buttons_write(RoccatDevice *konepure, guint profile_index, KonepureProfileButtons *profile_buttons, GError **error) {
	g_assert(profile_index < KONEPLUS_PROFILE_NUM);
	konepure_profile_buttons_finalize(profile_buttons, profile_index);
	return koneplus_device_write(konepure, (gchar const *)profile_buttons, sizeof(KonepureProfileButtons), error);
}

KonepureProfileButtons *konepure_profile_buttons_read(RoccatDevice *konepure, guint profile_index, GError **error) {
	KonepureProfileButtons *profile_buttons;

	g_assert(profile_index < KONEPLUS_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(konepure));

	if (!koneplus_select(konepure, profile_index, KONEPLUS_CONTROL_REQUEST_PROFILE_BUTTONS, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(konepure));
		return NULL;
	}

	profile_buttons = (KonepureProfileButtons *)koneplus_device_read(konepure, KONEPLUS_REPORT_ID_PROFILE_BUTTONS, sizeof(KonepureProfileButtons), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(konepure));
	return profile_buttons;
}

gboolean konepure_profile_buttons_equal(KonepureProfileButtons const *left, KonepureProfileButtons const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, KonepureProfileButtons, buttons[0], checksum);
	return equal ? FALSE : TRUE;
}
