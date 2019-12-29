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

#include "nyth_button_set.h"
#include "nyth_device.h"
#include "roccat_helper.h"
#include <string.h>

static guint8 const default_button_set[sizeof(NythButtonSet)] = {
	0x0A, 0x1E, 0x00, 0x42, 0x75, 0x74, 0x74, 0x6F, 0x6E, 0x20, 0x50, 0x72, 0x65, 0x73, 0x65, 0x74,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x0F
};

NythButtonSet const *nyth_button_set_default(void) {
	return (NythButtonSet const *)default_button_set;
}

NythButtonSet *nyth_button_set_read(RoccatDevice *device, guint profile_index, GError **error) {
	NythButtonSet *button_set;
	gaminggear_device_lock(GAMINGGEAR_DEVICE(device));

	if (!nyth_select(device, profile_index, NYTH_CONTROL_DATA_INDEX_NONE, NYTH_CONTROL_REQUEST_BUTTON_SET_PRESET, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));
		return NULL;
	}

	button_set = (NythButtonSet *)nyth_device_read(device, NYTH_REPORT_ID_BUTTON_SET, sizeof(NythButtonSet), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));
	return button_set;
}

static void nyth_button_set_finalize(NythButtonSet *button_set, guint profile_index) {
	button_set->report_id = NYTH_REPORT_ID_BUTTON_SET;
	button_set->profile_index = profile_index;
	button_set->size = sizeof(NythButtonSet);
}

gboolean nyth_button_set_write(RoccatDevice *device, guint profile_index, NythButtonSet *button_set, GError **error) {
	nyth_button_set_finalize(button_set, profile_index);
	return nyth_device_write(device, (gchar const *)button_set, sizeof(NythButtonSet), error);
}

/* Removing the trailing 0 if string is too long */
void nyth_button_set_set_name(NythButtonSet *button_set, gchar const *new_name) {
	strncpy((gchar *)(button_set->name), new_name, NYTH_BUTTON_SET_NAME_LENGTH);
}

gchar *nyth_button_set_get_name(NythButtonSet const *button_set) {
	return strndup((gchar *)(button_set->name), NYTH_BUTTON_SET_NAME_LENGTH);
}

gboolean nyth_button_set_equal(NythButtonSet const *left, NythButtonSet const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY_TO_END(left, right, NythButtonSet, name[0]);
	return equal ? FALSE : TRUE;
}

void nyth_button_set_copy(NythButtonSet *destination, NythButtonSet const *source) {
	memcpy(destination, source, sizeof(NythButtonSet));
}
