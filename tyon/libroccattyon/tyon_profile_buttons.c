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

#include "tyon.h"
#include "tyon_profile_buttons.h"
#include "tyon_device.h"
#include "roccat_helper.h"

void tyon_profile_buttons_finalize(TyonProfileButtons *profile_buttons, guint profile_index) {
	g_assert(profile_index < TYON_PROFILE_NUM);
	profile_buttons->profile_index = profile_index;
}

gboolean tyon_profile_buttons_write(RoccatDevice *device, guint profile_index, TyonProfileButtons *profile_buttons, GError **error) {
	g_assert(profile_index < TYON_PROFILE_NUM);
	tyon_profile_buttons_finalize(profile_buttons, profile_index);
	return tyon_device_write(device, (gchar const *)profile_buttons, sizeof(TyonProfileButtons), error);
}

TyonProfileButtons *tyon_profile_buttons_read(RoccatDevice *device, guint profile_index, GError **error) {
	TyonProfileButtons *profile_buttons;

	g_assert(profile_index < TYON_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(device));

	if (!tyon_select(device, profile_index, TYON_CONTROL_DATA_INDEX_NONE, TYON_CONTROL_REQUEST_PROFILE_BUTTONS, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));
		return NULL;
	}

	profile_buttons = (TyonProfileButtons *)tyon_device_read(device, TYON_REPORT_ID_PROFILE_BUTTONS, sizeof(TyonProfileButtons), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));
	return profile_buttons;
}

gboolean tyon_profile_buttons_equal(TyonProfileButtons const *left, TyonProfileButtons const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY_TO_END(left, right, TyonProfileButtons, buttons[0]);
	return equal ? FALSE : TRUE;
}
