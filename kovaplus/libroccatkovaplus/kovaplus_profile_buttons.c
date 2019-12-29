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

#include "kovaplus.h"
#include "roccat_helper.h"

static guint16 kovaplus_profile_buttons_calc_checksum(KovaplusProfileButtons const *profile_buttons) {
	return ROCCAT_BYTESUM_PARTIALLY(profile_buttons, KovaplusProfileButtons, report_id, checksum);
}

void kovaplus_profile_buttons_set_checksum(KovaplusProfileButtons *profile_buttons) {
	profile_buttons->checksum = GUINT16_TO_LE(kovaplus_profile_buttons_calc_checksum(profile_buttons));
}

void kovaplus_profile_buttons_finalize(KovaplusProfileButtons *profile_buttons, guint profile_index) {
	g_assert(profile_index < KOVAPLUS_PROFILE_NUM);
	profile_buttons->report_id = KOVAPLUS_REPORT_ID_PROFILE_BUTTONS;
	profile_buttons->size = sizeof(KovaplusProfileButtons);
	profile_buttons->profile_index = profile_index;
	kovaplus_profile_buttons_set_checksum(profile_buttons);
}

gboolean kovaplus_profile_buttons_write(RoccatDevice *kovaplus, guint profile_index, KovaplusProfileButtons *profile_buttons, GError **error) {
	gboolean result;
	g_assert(profile_index < KOVAPLUS_PROFILE_NUM);
	kovaplus_profile_buttons_finalize(profile_buttons, profile_index);
	result = kovaplus_device_write(kovaplus, (gchar const *)profile_buttons, sizeof(KovaplusProfileButtons), error);
	return result;
}

KovaplusProfileButtons *kovaplus_profile_buttons_read(RoccatDevice *kovaplus, guint profile_number, GError **error) {
	KovaplusProfileButtons *buttons;

	g_assert(profile_number < KOVAPLUS_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(kovaplus));

	if (!kovaplus_select(kovaplus, profile_number, KOVAPLUS_CONTROL_REQUEST_PROFILE_BUTTONS, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(kovaplus));
		return NULL;
	}

	buttons = (KovaplusProfileButtons *)kovaplus_device_read(kovaplus, KOVAPLUS_REPORT_ID_PROFILE_BUTTONS,
			sizeof(KovaplusProfileButtons), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(kovaplus));

	return buttons;
}
