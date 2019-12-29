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

#include "kone_internal.h"
#include "roccat_device_sysfs.h"
#include "roccat_helper.h"
#include <string.h>

guint16 kone_profile_calc_checksum(KoneProfile const *profile) {
	return ROCCAT_BYTESUM_PARTIALLY(profile, KoneProfile, size, checksum);
}

void kone_profile_set_checksum(KoneProfile *profile) {
	guint16 checksum = kone_profile_calc_checksum(profile);
	profile->checksum = GUINT16_TO_LE(checksum);
}

static gchar *kone_build_profile_name(guint profile_number) {
	return g_strdup_printf("profile%i", profile_number);
}

KoneProfile *kone_profile_read(RoccatDevice *kone, guint profile_number, GError **error) {
	gchar *name = kone_build_profile_name(profile_number);
	KoneProfile *result;
	g_assert(profile_number >= 1 && profile_number <= 5);
	result = (KoneProfile *)roccat_device_sysfs_read(kone, name, sizeof(KoneProfile), error);
	g_free(name);
	return result;
}

gboolean kone_profile_write(RoccatDevice *kone, guint profile_number, KoneProfile *profile, GError **error) {
	gboolean result;
	g_assert(profile_number >= 1 && profile_number <= 5);
	gchar *name = kone_build_profile_name(profile_number);
	kone_profile_finalize(profile, profile_number);
	result = roccat_device_sysfs_write(kone, name, (gchar const *)profile, sizeof(KoneProfile), error);
	g_free(name);
	return result;
}

void kone_profile_finalize(KoneProfile *profile, guint profile_number) {
	g_assert(profile_number >= 1 && profile_number <= 5);
	profile->profile = profile_number;
	kone_profile_set_checksum(profile);
}
