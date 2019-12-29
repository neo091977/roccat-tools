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

#include "koneplus.h"
#include "roccat_helper.h"

static guint16 koneplus_profile_buttons_calc_checksum(KoneplusProfileButtons const *profile_buttons) {
	return ROCCAT_BYTESUM_PARTIALLY(profile_buttons, KoneplusProfileButtons, report_id, checksum);
}

static void koneplus_profile_buttons_set_checksum(KoneplusProfileButtons *profile_buttons) {
	guint16 checksum = koneplus_profile_buttons_calc_checksum(profile_buttons);
	profile_buttons->checksum = GUINT16_TO_LE(checksum);
}

static void koneplus_profile_buttons_finalize(KoneplusProfileButtons *profile_buttons, guint profile_index) {
	g_assert(profile_index < KONEPLUS_PROFILE_NUM);
	profile_buttons->profile_index = profile_index;
	koneplus_profile_buttons_set_checksum(profile_buttons);
}

gboolean koneplus_profile_buttons_write(RoccatDevice *koneplus, guint profile_index, KoneplusProfileButtons *profile_buttons, GError **error) {
	g_assert(profile_index < KONEPLUS_PROFILE_NUM);
	koneplus_profile_buttons_finalize(profile_buttons, profile_index);
	return koneplus_device_write(koneplus, (gchar const *)profile_buttons, sizeof(KoneplusProfileButtons), error);
}

KoneplusProfileButtons *koneplus_profile_buttons_read(RoccatDevice *koneplus, guint profile_index, GError **error) {
	KoneplusProfileButtons *profile_buttons;

	g_assert(profile_index < KONEPLUS_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(koneplus));

	if (!koneplus_select(koneplus, profile_index, KONEPLUS_CONTROL_REQUEST_PROFILE_BUTTONS, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(koneplus));
		return NULL;
	}

	profile_buttons = (KoneplusProfileButtons *)koneplus_device_read(koneplus, KONEPLUS_REPORT_ID_PROFILE_BUTTONS, sizeof(KoneplusProfileButtons), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(koneplus));
	return profile_buttons;
}

gboolean koneplus_profile_buttons_equal(KoneplusProfileButtons const *left, KoneplusProfileButtons const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, KoneplusProfileButtons, buttons[0], checksum);
	return equal ? FALSE : TRUE;
}
