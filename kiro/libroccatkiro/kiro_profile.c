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

#include "kiro_profile.h"
#include "kiro_device.h"
#include "roccat_helper.h"

static guint8 const default_profile[sizeof(KiroProfile)] = {
	0x06, 0x34, 0x06, 0x1F, 0x08, 0x10, 0x20, 0x30, 0x50, 0x01, 0x33, 0x01, 0x00, 0x03, 0x02, 0x0D,
	0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x02, 0x00, 0x00, 0x03, 0x00, 0x00, 0x07, 0x00, 0x00,
	0x08, 0x00, 0x00, 0x64, 0x00, 0x00, 0x65, 0x00, 0x00, 0x14, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x0E,
	0x00, 0x00, 0x6B, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00
};

KiroProfile const *kiro_profile_default(void) {
	return (KiroProfile const *)default_profile;
}

static guint16 kiro_profile_calc_checksum(KiroProfile const *profile) {
	return ROCCAT_BYTESUM_PARTIALLY(profile, KiroProfile, report_id, checksum);
}

static void kiro_profile_set_checksum(KiroProfile *profile) {
	profile->checksum = GUINT16_TO_LE(kiro_profile_calc_checksum(profile));
}

static void kiro_profile_finalize(KiroProfile *profile) {
	profile->report_id = KIRO_REPORT_ID_PROFILE;
	profile->size = 0x34; // FIXME sizeof(KiroProfile)
	kiro_profile_set_checksum(profile);
}

KiroProfile *kiro_profile_read(RoccatDevice *device, GError **error) {
	return (KiroProfile *)kiro_device_read(device, KIRO_REPORT_ID_PROFILE, sizeof(KiroProfile), error);
}

gboolean kiro_profile_write(RoccatDevice *device, KiroProfile *profile, GError **error) {
	kiro_profile_finalize(profile);
	return kiro_device_write(device, (gchar const *)profile, sizeof(KiroProfile), error);
}

gboolean kiro_profile_equal(KiroProfile const *left, KiroProfile const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, KiroProfile, size, checksum);
	return equal ? FALSE : TRUE;
}

void kiro_profile_copy(KiroProfile *destination, KiroProfile const *source) {
	memcpy(destination, source, sizeof(KiroProfile));
}
