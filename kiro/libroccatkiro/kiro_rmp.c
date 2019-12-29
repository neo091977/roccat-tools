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

#include "kiro_rmp.h"
#include "kiro_device.h"
#include "roccat_swarm_rmp.h"
#include "roccat_helper.h"
#include "g_roccat_helper.h"

static gboolean read_header(KiroProfileData *profile_data, gconstpointer *data) {
	guint32 size;
	RoccatSwarmRmpHeader const *header;
	gchar *string;
	
	size = roccat_swarm_rmp_read_bigendian32(data);
	if (size != sizeof(RoccatSwarmRmpHeader)) {
		g_warning("%u != sizeof(RoccatSwarmRmpHeader)", size);
		return FALSE;
	}
	
	header = (RoccatSwarmRmpHeader const *)*data;

	string = roccat_swarm_rmp_header_get_profile_name(header);
	kiro_profile_data_eventhandler_set_profile_name(&profile_data->eventhandler, string);
	g_free(string);
	
	*data += size;
	return TRUE;
}

static gboolean read_openers(KiroProfileData *profile_data, gconstpointer *data) {
	guint32 count;
	guint32 button_index;
	guint i;
	gchar *string;

	count = roccat_swarm_rmp_read_bigendian32(data);
	if (count > KIRO_BUTTON_NUM) {
		g_warning("%u > KIRO_BUTTON_NUM", count);
		return FALSE;
	}

	for (i = 0; i < count; ++i) {
		button_index = roccat_swarm_rmp_read_bigendian32(data);

		string = roccat_swarm_rmp_read_bigendian_string_with_size(data);
		kiro_profile_data_eventhandler_set_opener(&profile_data->eventhandler, button_index, string);
		g_free(string);
	}
	return TRUE;
}

static gboolean read_profile(KiroProfileData *profile_data, gconstpointer *data) {
	guint32 size;

	size = roccat_swarm_rmp_read_bigendian32(data);
	if (size != sizeof(KiroProfile)) {
		g_warning("%u != sizeof(KiroProfile)", size);
		return FALSE;
	}

	kiro_profile_data_hardware_set_profile(&profile_data->hardware, (KiroProfile const *)*data);

	*data += size;
	return TRUE;
}

KiroProfileData *kiro_rmp_to_profile_data(gconstpointer rmp, gsize length) {
	KiroProfileData *profile_data;
	gconstpointer pointer;
	
	profile_data = kiro_profile_data_new();
	pointer = rmp;
	
	if (!roccat_swarm_rmp_read_unknown(&pointer)) goto error;
	if (!read_header(profile_data, &pointer)) goto error;
	if (!roccat_swarm_rmp_read_unknown(&pointer)) goto error; /* gamefiles */
	if (!roccat_swarm_rmp_read_unknown(&pointer)) goto error; /* sound_feedback */
	if (!roccat_swarm_rmp_read_unknown(&pointer)) goto error; /* timer */
	if (!roccat_swarm_rmp_read_unknown_with_count(&pointer)) goto error;
	if (!roccat_swarm_rmp_read_pictures(&pointer)) goto error;
	if (!read_openers(profile_data, &pointer)) goto error;
	if (!roccat_swarm_rmp_read_unknown(&pointer)) goto error;
	if (!roccat_swarm_rmp_read_unknown(&pointer)) goto error;
	if (!read_profile(profile_data, &pointer)) goto error;
	if (!roccat_swarm_rmp_read_unknown(&pointer)) goto error;

	if (pointer > rmp + length)
		g_warning("%zu > %zu", (gsize)(pointer - rmp), length);

	return profile_data;
error:
	g_free(profile_data);
	return NULL;
}

static void write_header(KiroProfileData const *profile_data, gpointer *data, gsize *length) {
	RoccatSwarmRmpHeader *header;
	
	header = (RoccatSwarmRmpHeader *)roccat_swarm_rmp_write_with_size(data, length, sizeof(RoccatSwarmRmpHeader));
	roccat_swarm_rmp_header_set_profile_name(header, profile_data->eventhandler.profile_name);
	/* default values */
	header->volume = 50;
	header->profile_autoswitch = 0;

}

static void write_profile(KiroProfileData const *profile_data, gpointer *data, gsize *length) {
	KiroProfile *profile;
	
	profile = (KiroProfile *)roccat_swarm_rmp_write_with_size(data, length, sizeof(KiroProfile));
	kiro_profile_copy(profile, &profile_data->hardware.profile);
}

static void write_openers(KiroProfileData const *profile_data, gpointer *data, gsize *length) {
	guint type;
	guint count;
	guint i;
	
	count = 0;
	for (i = 0; i < KIRO_BUTTON_NUM; ++i) {
		type = profile_data->hardware.profile.buttons[i].type;
		if (type == KIRO_BUTTON_TYPE_OPEN_APPLICATION) { // FIXME more types possible?
			++count;
		}
	}
	roccat_swarm_rmp_write_bigendian32(data, length, count);

	for (i = 0; i < KIRO_BUTTON_NUM; ++i) {
		type = profile_data->hardware.profile.buttons[i].type;
		if (type == KIRO_BUTTON_TYPE_OPEN_APPLICATION) {
			roccat_swarm_rmp_write_bigendian32(data, length, i);
			roccat_swarm_rmp_write_bigendian_string_with_size(data, length, (gchar const *)profile_data->eventhandler.openers[i], ROCCAT_SWARM_RMP_OPENER_LENGTH);
		}
	}
}

gpointer kiro_profile_data_to_rmp(KiroProfileData const *profile_data, gsize *length) {
	gpointer rmp = NULL;
	gsize local_length = 0;
	
	roccat_swarm_rmp_write_unknown(&rmp, &local_length, 0);
	write_header(profile_data, &rmp, &local_length);
	roccat_swarm_rmp_write_unknown(&rmp, &local_length, 0x500); /* gamefiles */
	roccat_swarm_rmp_write_unknown(&rmp, &local_length, 0); /* sound feedback */
	roccat_swarm_rmp_write_unknown(&rmp, &local_length, 0); /* timer */
	roccat_swarm_rmp_write_unknown_with_count(&rmp, &local_length, 0, 0);
	roccat_swarm_rmp_write_unknown(&rmp, &local_length, 0); /* pictures */
	write_openers(profile_data, &rmp, &local_length);
	roccat_swarm_rmp_write_unknown(&rmp, &local_length, 0);
	roccat_swarm_rmp_write_unknown(&rmp, &local_length, 0);
	write_profile(profile_data, &rmp, &local_length);
	roccat_swarm_rmp_write_unknown(&rmp, &local_length, 0);
	
	*length = local_length;
	return rmp;
}
