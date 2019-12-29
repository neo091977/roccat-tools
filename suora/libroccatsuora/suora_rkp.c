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

#include "suora_rkp.h"
#include "suora_device.h"
#include "suora_key_relations.h"
#include "suora_rkp_keys.h"
#include "roccat_swarm_rmp.h"

typedef struct _SuoraRkpIllumination SuoraRkpIllumination;
typedef struct _SuoraRkpSoundFeedback SuoraRkpSoundFeedback;

struct _SuoraRkpIllumination {
	guint8 unused;
	SuoraFeatureBuffer feature_buffer;
} __attribute__ ((packed));

struct _SuoraRkpSoundFeedback {
	guint8 unused1;
	guint8 feedback;
	guint8 unused2;
} __attribute__ ((packed));

static void write_header(SuoraProfileData const *profile_data, gpointer *data, gsize *length) {
	RoccatSwarmRmpHeader *header;

	header = (RoccatSwarmRmpHeader *)roccat_swarm_rmp_write_with_size(data, length, sizeof(RoccatSwarmRmpHeader));
	roccat_swarm_rmp_header_set_profile_name(header, gaminggear_profile_data_get_name(GAMINGGEAR_PROFILE_DATA(profile_data)));
	header->volume = 50; // TODO tie to notification_volume
	header->profile_autoswitch = 0;
}

static void write_sound_feedback(SuoraProfileData const *profile_data, gpointer *data, gsize *length) {
	SuoraRkpSoundFeedback *sound_feedback;

	sound_feedback = (SuoraRkpSoundFeedback *)roccat_swarm_rmp_write_with_size(data, length, sizeof(SuoraRkpSoundFeedback));
	sound_feedback->feedback = suora_profile_data_get_sound_feedback(profile_data);
}

static void write_macros(SuoraProfileData const *profile_data, gpointer *data, gsize *length) {
	SuoraRkpMacro *rkp_macro;
	guint key_index;

	roccat_swarm_rmp_write_bigendian32(data, length, SUORA_KEYS_NUM);
	for (key_index = 0; key_index < SUORA_KEYS_NUM; ++key_index) {
		rkp_macro = (SuoraRkpMacro *)roccat_swarm_rmp_write_with_size(data, length, sizeof(SuoraRkpMacro));

		if (suora_key_relations[key_index].macro_index != SUORA_KEY_RELATION_INVALID)
			suora_rkp_macro_copy(rkp_macro, suora_profile_data_get_macro(profile_data, suora_key_relations[key_index].macro_index));

		/* That's not quite right, the Windows version doesn't set key index */
		suora_rkp_macro_finalize(rkp_macro, key_index);
	}
}

static void write_keys(SuoraProfileData const *profile_data, gpointer *data, gsize *length) {
	SuoraRkpKeys *buffer;

	buffer = (SuoraRkpKeys *)roccat_swarm_rmp_write_with_size(data, length, sizeof(SuoraRkpKeys));
	suora_rkp_keys_copy(buffer, suora_profile_data_get_keys(profile_data));
}

static void write_illumination(SuoraProfileData const *profile_data, gpointer *data, gsize *length) {
	SuoraRkpIllumination *buffer;
	SuoraFeatureBuffer *feature_buffer;
	SuoraIllumination *illumination_buffer;
	
	buffer = (SuoraRkpIllumination *)roccat_swarm_rmp_write_with_size(data, length, sizeof(SuoraRkpIllumination));
	feature_buffer = &buffer->feature_buffer;
	illumination_buffer = (SuoraIllumination *)&feature_buffer->payload;
	
	feature_buffer->action = SUORA_FEATURE_REQUEST_ILLUMINATION; // FIXME get/set?
	suora_illumination_copy(illumination_buffer, suora_profile_data_get_illumination(profile_data));
	suora_feature_buffer_set_checksum(feature_buffer);
}

static void write_gamefiles(SuoraProfileData const *profile_data, gpointer *data, gsize *length) {
	guint i;
	gsize size;

	size = ROCCAT_SWARM_RMP_GAMEFILE_NUM * ROCCAT_SWARM_RMP_GAMEFILE_LENGTH * sizeof(gunichar2);
	roccat_swarm_rmp_write_bigendian32(data, length, size);
	for (i = 0; i < ROCCAT_SWARM_RMP_GAMEFILE_NUM; ++i)
		roccat_swarm_rmp_write_bigendian_string(data, length, gaminggear_profile_data_get_gamefile(GAMINGGEAR_PROFILE_DATA(profile_data), i), ROCCAT_SWARM_RMP_GAMEFILE_LENGTH);
}

gpointer suora_profile_data_to_rkp(SuoraProfileData const *profile_data, gsize *length) {
	gpointer rkp = NULL;
	gsize local_length = 0;

	roccat_swarm_rmp_write_unknown(&rkp, &local_length, 0); // 4
	write_header(profile_data, &rkp, &local_length);
	write_sound_feedback(profile_data, &rkp, &local_length);
	write_illumination(profile_data, &rkp, &local_length);
	roccat_swarm_rmp_write_unknown(&rkp, &local_length, 0); // pictures
	write_keys(profile_data, &rkp, &local_length);
	write_macros(profile_data, &rkp, &local_length);
	write_gamefiles(profile_data, &rkp, &local_length);

	*length = local_length;
	return rkp;
}

static gboolean read_header(SuoraProfileData *profile_data, gconstpointer *data) {
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
	gaminggear_profile_data_set_name(GAMINGGEAR_PROFILE_DATA(profile_data), string);
	g_free(string);
	
	*data += size;
	return TRUE;
}

static gboolean read_sound_feedback(SuoraProfileData *profile_data, gconstpointer *data) {
	guint32 size;
	SuoraRkpSoundFeedback sound_feedback;

	size = roccat_swarm_rmp_read_bigendian32(data);
	if (size != sizeof(SuoraRkpSoundFeedback)) {
		g_warning("%u != sizeof(SuoraRkpSoundFeedback)", size);
		return FALSE;
	}

	sound_feedback = *(SuoraRkpSoundFeedback const *)data;

	suora_profile_data_set_sound_feedback(profile_data, sound_feedback.feedback);
	
	*data += size;
	return TRUE;
}

static gboolean read_macros(SuoraProfileData *profile_data, gconstpointer *data) {
	SuoraRkpMacro const *rkp_macro;
	guint32 count;
	guint32 size;
	guint key_index;

	count = roccat_swarm_rmp_read_bigendian32(data);
	if (count != SUORA_KEYS_NUM) {
		g_warning("%u != SUORA_KEYS_NUM", count);
		return FALSE;
	}

	for (key_index = 0; key_index < count; ++key_index) {
		size = roccat_swarm_rmp_read_bigendian32(data);
		if (size != sizeof(SuoraRkpMacro)) {
			g_warning("%u != sizeof(SuoraRkpMacro)", size);
			return FALSE;
		}

		rkp_macro = (SuoraRkpMacro const *)*data;
		*data += size;

		if (suora_key_relations[key_index].macro_index == SUORA_KEY_RELATION_INVALID)
			continue;

		suora_profile_data_set_macro(profile_data, suora_key_relations[key_index].macro_index, rkp_macro);
	}

	return TRUE;
}

static gboolean read_gamefiles(SuoraProfileData *profile_data, gconstpointer *data) {
	guint32 size;
	gchar *gamefile;
	guint i;

	size = roccat_swarm_rmp_read_bigendian32(data);
	if (size != ROCCAT_SWARM_RMP_GAMEFILE_NUM * ROCCAT_SWARM_RMP_GAMEFILE_LENGTH * 2) {
		g_warning("%u != ROCCAT_SWARM_RMP_GAMEFILE_NUM * ROCCAT_SWARM_RMP_GAMEFILE_LENGTH * 2", size);
		return FALSE;
	}

	for (i = 0; i < ROCCAT_SWARM_RMP_GAMEFILE_NUM; ++i) {
		gamefile = roccat_swarm_rmp_read_bigendian_string(data, ROCCAT_SWARM_RMP_GAMEFILE_LENGTH);
		gaminggear_profile_data_set_gamefile(GAMINGGEAR_PROFILE_DATA(profile_data), i, gamefile);
		g_free(gamefile);
	}

	return TRUE;
}

static gboolean read_keys(SuoraProfileData *profile_data, gconstpointer *data) {
	guint32 size;

	size = roccat_swarm_rmp_read_bigendian32(data);
	if (size != sizeof(SuoraRkpKeys)) {
		g_warning("%u != sizeof(SuoraRkpKeys)", size);
		return FALSE;
	}

	suora_profile_data_set_keys(profile_data, (SuoraRkpKeys const *)*data);

	*data += size;
	return TRUE;
}

static gboolean read_illumination(SuoraProfileData *profile_data, gconstpointer *data) {
	guint32 size;
	SuoraRkpIllumination const *illumination;
	
	size = roccat_swarm_rmp_read_bigendian32(data);
	if (size != sizeof(SuoraRkpIllumination)) {
		g_warning("%u != sizeof(SuoraRkpIllumination)", size);
		return FALSE;
	}

	illumination = (SuoraRkpIllumination const *)*data;
	suora_profile_data_set_illumination(profile_data, (SuoraIllumination const *)&illumination->feature_buffer.payload);

	*data += size;
	return TRUE;
}

gboolean suora_rkp_to_profile_data(SuoraProfileData *profile_data, gconstpointer rkp, gsize length) {
	gconstpointer pointer;

	pointer = rkp;

	if (!roccat_swarm_rmp_read_unknown(&pointer)) goto error;
	if (!read_header(profile_data, &pointer)) goto error;
	if (!read_sound_feedback(profile_data, &pointer)) goto error;
	if (!read_illumination(profile_data, &pointer)) goto error;
	if (!roccat_swarm_rmp_read_pictures(&pointer)) goto error;
	if (!read_keys(profile_data, &pointer)) goto error;
	if (!read_macros(profile_data, &pointer)) goto error;
	if (!read_gamefiles(profile_data, &pointer)) goto error;

	if (pointer > rkp + length)
		g_warning("%zu > %zu", (gsize)(pointer - rkp), length);

	return TRUE;
error:
	return FALSE;
}
