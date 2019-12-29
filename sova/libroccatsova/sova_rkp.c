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

#include "sova_rkp.h"
#include "sova_device.h"
#include "sova_key_relations.h"
#include "sova_key_mask.h"
#include "sova_keys_easyzone.h"
#include "sova_keys_function.h"
#include "sova_keys_primary.h"
#include "sova_rkp_sound_feedback.h"
#include "sova_light.h"
#include "sova_macro.h"
#include "roccat_talk.h"
#include "roccat_swarm_rmp.h"
#include "roccat_notificator.h"
#include <string.h>

typedef struct {
	guint8 unknown[8];
} __attribute__ ((packed)) SovaRkpKeysExtra;

typedef struct {
	guint8 unknown1[4];
	guint8 type;
	guint8 unknown2[70];
} __attribute__ ((packed)) SovaRkpTalk;

static void write_header(SovaProfileData const *profile_data, gpointer *data, gsize *length) {
	RoccatSwarmRmpHeader *header;

	header = (RoccatSwarmRmpHeader *)roccat_swarm_rmp_write_with_size(data, length, sizeof(RoccatSwarmRmpHeader));
	roccat_swarm_rmp_header_set_profile_name(header, gaminggear_profile_data_get_name(GAMINGGEAR_PROFILE_DATA(profile_data)));
	header->volume = 50; // TODO tie to notification_volume
	header->profile_autoswitch = 0;
}

static void write_macros(SovaProfileData const *profile_data, gpointer *data, gsize *length) {
	SovaMacro *macro;
	guint macro_index;

	roccat_swarm_rmp_write_bigendian32(data, length, SOVA_MACRO_NUM);
	for (macro_index = 0; macro_index < SOVA_MACRO_NUM; ++macro_index) {
		macro = (SovaMacro *)roccat_swarm_rmp_write_with_size(data, length, sizeof(SovaMacro));
		sova_macro_copy(macro, sova_profile_data_get_macro(profile_data, macro_index));
		sova_macro_finalize(macro, 0, macro_index);
	}
}

static void write_keys_function(SovaProfileData const *profile_data, gpointer *data, gsize *length) {
	SovaKeysFunction *buffer;

	buffer = (SovaKeysFunction *)roccat_swarm_rmp_write_with_size(data, length, sizeof(SovaKeysFunction));
	sova_keys_function_copy(buffer, sova_profile_data_get_keys_function(profile_data));
}

static void write_keys_primary(SovaProfileData const *profile_data, gpointer *data, gsize *length) {
	SovaKeysPrimary *buffer;

	buffer = (SovaKeysPrimary *)roccat_swarm_rmp_write_with_size(data, length, sizeof(SovaKeysPrimary));
	sova_keys_primary_copy(buffer, sova_profile_data_get_keys_primary(profile_data));
}

static void write_keys_easyzone(SovaProfileData const *profile_data, gpointer *data, gsize *length) {
	SovaKeysEasyzone *buffer;

	buffer = (SovaKeysEasyzone *)roccat_swarm_rmp_write_with_size(data, length, sizeof(SovaKeysEasyzone));
	sova_keys_easyzone_copy(buffer, sova_profile_data_get_keys_easyzone(profile_data));
}

static void write_key_mask(SovaProfileData const *profile_data, gpointer *data, gsize *length) {
	SovaKeyMask *buffer;

	buffer = (SovaKeyMask *)roccat_swarm_rmp_write_with_size(data, length, sizeof(SovaKeyMask));
	sova_key_mask_copy(buffer, sova_profile_data_get_key_mask(profile_data));
}

static void write_keys_extra(SovaProfileData const *profile_data, gpointer *data, gsize *length) {
	SovaRkpKeysExtra *buffer;

	buffer = (SovaRkpKeysExtra *)roccat_swarm_rmp_write_with_size(data, length, sizeof(SovaRkpKeysExtra));
	memcpy(buffer, sova_profile_data_get_keys_extra(profile_data), sizeof(SovaRkpKeysExtra));
}

static void write_light(SovaProfileData const *profile_data, gpointer *data, gsize *length) {
	SovaLight *buffer;

	buffer = (SovaLight *)roccat_swarm_rmp_write_with_size(data, length, sizeof(SovaLight));
	sova_light_copy(buffer, sova_profile_data_get_light(profile_data));
}

static void write_gamefiles(SovaProfileData const *profile_data, gpointer *data, gsize *length) {
	guint i;
	gsize size;

	size = GAMINGGEAR_PROFILE_DATA_GAMEFILE_NUM * ROCCAT_SWARM_RMP_GAMEFILE_LENGTH * sizeof(gunichar2);
	roccat_swarm_rmp_write_bigendian32(data, length, size);
	for (i = 0; i < GAMINGGEAR_PROFILE_DATA_GAMEFILE_NUM; ++i)
		roccat_swarm_rmp_write_bigendian_string(data, length, gaminggear_profile_data_get_gamefile(GAMINGGEAR_PROFILE_DATA(profile_data), i), ROCCAT_SWARM_RMP_GAMEFILE_LENGTH);
}

static gboolean should_write_opener(SovaProfileData const *profile_data, guint index) {
	guint type = sova_profile_data_get_keys_easyzone(profile_data)->keys[index].type;

	if (type == SOVA_KEY_TYPE_OPEN_APP || type == SOVA_KEY_TYPE_OPEN_DOCUMENT || type == SOVA_KEY_TYPE_OPEN_FOLDER || type == SOVA_KEY_TYPE_OPEN_WEBSITE)
		return TRUE;
	return FALSE;
}

static void write_openers(SovaProfileData const *profile_data, gpointer *data, gsize *length) {
	guint count;
	guint i;

	count = 0;
	for (i = 0; i < SOVA_MACRO_NUM; ++i) {
		if (should_write_opener(profile_data, i))
			++count;
	}
	roccat_swarm_rmp_write_bigendian32(data, length, count);

	for (i = 0; i < SOVA_MACRO_NUM; ++i) {
		if (should_write_opener(profile_data, i)) {
			roccat_swarm_rmp_write_bigendian32(data, length, i);
			roccat_swarm_rmp_write_bigendian_string_with_size(data, length, sova_profile_data_get_opener(profile_data, i), ROCCAT_SWARM_RMP_OPENER_LENGTH);
		}
	}
}

static void write_talk(SovaProfileData const *profile_data, gpointer *data, gsize *length) {
	SovaRkpTalk *buffer;

	if (sova_profile_data_get_keys_extra(profile_data)->capslock == TRUE) { // FIXME
		/* Only caps-lock button is easyshift/talk capable */
		roccat_swarm_rmp_write_bigendian32(data, length, 1);
		roccat_swarm_rmp_write_bigendian32(data, length, 0x39);
		buffer = (SovaRkpTalk *)roccat_swarm_rmp_write_with_size(data, length, sizeof(SovaRkpTalk));
		buffer->type = TRUE; // FIXME
	} else
		roccat_swarm_rmp_write_bigendian32(data, length, 0);
}

static void write_sound_feedback(SovaProfileData const *profile_data, gpointer *data, gsize *length) {
	SovaRkpSoundFeedback *buffer;

	buffer = (SovaRkpSoundFeedback *)roccat_swarm_rmp_write_with_size(data, length, sizeof(SovaRkpSoundFeedback));

	buffer->typing_sound = sova_profile_data_get_sound_feedback_typing(profile_data);

	buffer->wakeup = (sova_profile_data_get_wakeup_notification_type(profile_data) != ROCCAT_NOTIFICATION_TYPE_OFF) ?
			SOVA_RKP_SOUND_FEEDBACK_NOTIFICATION_ON : SOVA_RKP_SOUND_FEEDBACK_NOTIFICATION_OFF;
	buffer->sleep = (sova_profile_data_get_sleep_notification_type(profile_data) != ROCCAT_NOTIFICATION_TYPE_OFF) ?
			SOVA_RKP_SOUND_FEEDBACK_NOTIFICATION_ON : SOVA_RKP_SOUND_FEEDBACK_NOTIFICATION_OFF;
	buffer->macro_record = (sova_profile_data_get_macro_record_notification_type(profile_data) != ROCCAT_NOTIFICATION_TYPE_OFF) ?
			SOVA_RKP_SOUND_FEEDBACK_NOTIFICATION_ON : SOVA_RKP_SOUND_FEEDBACK_NOTIFICATION_OFF;
	buffer->profile_switch = (sova_profile_data_get_profile_switch_notification_type(profile_data) != ROCCAT_NOTIFICATION_TYPE_OFF) ?
			SOVA_RKP_SOUND_FEEDBACK_NOTIFICATION_ON : SOVA_RKP_SOUND_FEEDBACK_NOTIFICATION_OFF;
}

gpointer sova_profile_data_to_rkp(SovaProfileData const *profile_data, gsize *length) {
	gpointer rkp = NULL;
	gsize local_length = 0;

	roccat_swarm_rmp_write_unknown(&rkp, &local_length, 0);
	write_header(profile_data, &rkp, &local_length);
	write_gamefiles(profile_data, &rkp, &local_length);
	write_openers(profile_data, &rkp, &local_length);
	roccat_swarm_rmp_write_unknown(&rkp, &local_length, 0);
	roccat_swarm_rmp_write_timers(&rkp, &local_length);
	roccat_swarm_rmp_write_unknown_with_count(&rkp, &local_length, 0, 0);
	roccat_swarm_rmp_write_unknown_with_count(&rkp, &local_length, 0, 0); // pictures
	write_talk(profile_data, &rkp, &local_length);
	write_keys_function(profile_data, &rkp, &local_length);
	write_keys_easyzone(profile_data, &rkp, &local_length);
	write_key_mask(profile_data, &rkp, &local_length);
	write_macros(profile_data, &rkp, &local_length);
	roccat_swarm_rmp_write_unknown(&rkp, &local_length, 0);
	write_sound_feedback(profile_data, &rkp, &local_length);
	write_keys_extra(profile_data, &rkp, &local_length);
	write_keys_primary(profile_data, &rkp, &local_length);
	write_light(profile_data, &rkp, &local_length);
	roccat_swarm_rmp_write_unknown(&rkp, &local_length, 0);

	*length = local_length;
	return rkp;
}

static gboolean read_header(SovaProfileData *profile_data, gconstpointer *data) {
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

static gboolean read_macros(SovaProfileData *profile_data, gconstpointer *data) {
	SovaMacro const *macro;
	guint32 count;
	guint32 size;
	guint macro_index;

	count = roccat_swarm_rmp_read_bigendian32(data);
	if (count != SOVA_MACRO_NUM) {
		g_warning("%u != SOVA_MACRO_NUM", count);
		return FALSE;
	}

	for (macro_index = 0; macro_index < count; ++macro_index) {
		size = roccat_swarm_rmp_read_bigendian32(data);
		if (size != sizeof(SovaMacro)) {
			g_warning("%u != sizeof(SovaMacro)", size);
			return FALSE;
		}

		macro = (SovaMacro const *)*data;
		*data += size;
		sova_profile_data_set_macro(profile_data, macro_index, macro);
	}

	return TRUE;
}

static gboolean read_gamefiles(SovaProfileData *profile_data, gconstpointer *data) {
	guint32 size;
	gchar *gamefile;
	guint i;

	size = roccat_swarm_rmp_read_bigendian32(data);
	if (size != GAMINGGEAR_PROFILE_DATA_GAMEFILE_NUM * ROCCAT_SWARM_RMP_GAMEFILE_LENGTH * 2) {
		g_warning("%u != GAMINGGEAR_PROFILE_DATA_GAMEFILE_NUM * ROCCAT_SWARM_RMP_GAMEFILE_LENGTH * 2", size);
		return FALSE;
	}

	for (i = 0; i < GAMINGGEAR_PROFILE_DATA_GAMEFILE_NUM; ++i) {
		gamefile = roccat_swarm_rmp_read_bigendian_string(data, ROCCAT_SWARM_RMP_GAMEFILE_LENGTH);
		gaminggear_profile_data_set_gamefile(GAMINGGEAR_PROFILE_DATA(profile_data), i, gamefile);
		g_free(gamefile);
	}

	return TRUE;
}

static gboolean read_keys_function(SovaProfileData *profile_data, gconstpointer *data) {
	guint32 size;

	size = roccat_swarm_rmp_read_bigendian32(data);
	if (size != sizeof(SovaKeysFunction)) {
		g_warning("%u != sizeof(SovaKeysFunction)", size);
		return FALSE;
	}

	sova_profile_data_set_keys_function(profile_data, (SovaKeysFunction const *)*data);

	*data += size;
	return TRUE;
}

static gboolean read_keys_easyzone(SovaProfileData *profile_data, gconstpointer *data) {
	guint32 size;

	size = roccat_swarm_rmp_read_bigendian32(data);
	if (size != sizeof(SovaKeysEasyzone)) {
		g_warning("%u != sizeof(SovaKeysEasyzone)", size);
		return FALSE;
	}

	sova_profile_data_set_keys_easyzone(profile_data, (SovaKeysEasyzone const *)*data);

	*data += size;
	return TRUE;
}

static gboolean read_keys_primary(SovaProfileData *profile_data, gconstpointer *data) {
	guint32 size;

	size = roccat_swarm_rmp_read_bigendian32(data);
	if (size != sizeof(SovaKeysPrimary)) {
		g_warning("%u != sizeof(SovaKeysPrimary)", size);
		return FALSE;
	}

	sova_profile_data_set_keys_primary(profile_data, (SovaKeysPrimary const *)*data);

	*data += size;
	return TRUE;
}

static gboolean read_key_mask(SovaProfileData *profile_data, gconstpointer *data) {
	guint32 size;

	size = roccat_swarm_rmp_read_bigendian32(data);
	if (size != sizeof(SovaKeyMask)) {
		g_warning("%u != sizeof(SovaKeyMask)", size);
		return FALSE;
	}

	sova_profile_data_set_key_mask(profile_data, (SovaKeyMask const *)*data);

	*data += size;
	return TRUE;
}

static gboolean read_light(SovaProfileData *profile_data, gconstpointer *data) {
	guint32 size;

	size = roccat_swarm_rmp_read_bigendian32(data);
	if (size != sizeof(SovaLight)) {
		g_warning("%u != sizeof(SovaLight)", size);
		return FALSE;
	}

	sova_profile_data_set_light(profile_data, (SovaLight const *)*data);

	*data += size;
	return TRUE;
}

static gboolean read_keys_extra(SovaProfileData *profile_data, gconstpointer *data) {
	guint32 size;
	SovaKeysExtra keys_extra = {0};

	size = roccat_swarm_rmp_read_bigendian32(data);
	if (size != sizeof(SovaRkpKeysExtra)) {
		g_warning("%u != sizeof(SovaRkpKeysExtra)", size);
		return FALSE;
	}

	memcpy(&keys_extra, *data, sizeof(SovaRkpKeysExtra));
	sova_profile_data_set_keys_extra(profile_data, &keys_extra);

	*data += size;
	return TRUE;
}

static gboolean read_openers(SovaProfileData *profile_data, gconstpointer *data) {
	guint32 count;
	guint32 button_index;
	guint i;
	gchar *string;

	count = roccat_swarm_rmp_read_bigendian32(data);
	for (i = 0; i < count; ++i) {
		button_index = roccat_swarm_rmp_read_bigendian32(data);

		string = roccat_swarm_rmp_read_bigendian_string_with_size(data);
		sova_profile_data_set_opener(profile_data, button_index, string);
		g_free(string);
	}
	return TRUE;
}

static gboolean read_talk(SovaProfileData *profile_data, gconstpointer *data) {
	guint32 count;
	guint32 button_index;
	guint32 size;
	SovaRkpTalk const *talk;

	count = roccat_swarm_rmp_read_bigendian32(data);
	if (count  > 1) {
		g_warning("Unsupported number of talk entries %u > 1", count);
		return FALSE;
	}

	if (count == 0)
		return TRUE;

	button_index = roccat_swarm_rmp_read_bigendian32(data);
	if (button_index != 0x39) {
		g_warning("Talk entry has wrong button index %u instead of %u", button_index, 0x39);
		return FALSE;
	}

	size = roccat_swarm_rmp_read_bigendian32(data);
	if (size != sizeof(SovaRkpTalk)) {
		g_warning("%u != sizeof(SovaRkpTalk)", size);
		return FALSE;
	}

	talk = (SovaRkpTalk const *)*data;

	if (talk->type == TRUE) // FIXME
		sova_profile_data_set_capslock_talk_target(profile_data, ROCCAT_TALK_DEVICE_MOUSE);

	*data += size;

	return TRUE;
}

static gboolean read_sound_feedback(SovaProfileData *profile_data, gconstpointer *data) {
	guint32 size;
	SovaRkpSoundFeedback const *sound_feedback;

	size = roccat_swarm_rmp_read_bigendian32(data);
	if (size != sizeof(SovaRkpSoundFeedback)) {
		g_warning("%u != sizeof(SovaRkpSoundFeedback)", size);
		return FALSE;
	}

	sound_feedback = (SovaRkpSoundFeedback const *)*data;

	sova_profile_data_set_sound_feedback_typing(profile_data, sound_feedback->typing_sound);

	sova_profile_data_set_wakeup_notification_type(profile_data, (sound_feedback->wakeup == SOVA_RKP_SOUND_FEEDBACK_NOTIFICATION_ON) ?
			ROCCAT_NOTIFICATION_TYPE_AUDIO : ROCCAT_NOTIFICATION_TYPE_OFF);
	sova_profile_data_set_sleep_notification_type(profile_data, (sound_feedback->sleep == SOVA_RKP_SOUND_FEEDBACK_NOTIFICATION_ON) ?
			ROCCAT_NOTIFICATION_TYPE_AUDIO : ROCCAT_NOTIFICATION_TYPE_OFF);
	sova_profile_data_set_macro_record_notification_type(profile_data, (sound_feedback->macro_record == SOVA_RKP_SOUND_FEEDBACK_NOTIFICATION_ON) ?
			ROCCAT_NOTIFICATION_TYPE_AUDIO : ROCCAT_NOTIFICATION_TYPE_OFF);
	sova_profile_data_set_profile_switch_notification_type(profile_data, (sound_feedback->profile_switch == SOVA_RKP_SOUND_FEEDBACK_NOTIFICATION_ON) ?
			ROCCAT_NOTIFICATION_TYPE_AUDIO : ROCCAT_NOTIFICATION_TYPE_OFF);

	*data += size;
	return TRUE;
}

gboolean sova_rkp_to_profile_data(SovaProfileData *profile_data, gconstpointer rkp, gsize length) {
	gconstpointer pointer;

	pointer = rkp;

	if (!roccat_swarm_rmp_read_unknown(&pointer)) goto error;
	if (!read_header(profile_data, &pointer)) goto error;
	if (!read_gamefiles(profile_data, &pointer)) goto error;
	if (!read_openers(profile_data, &pointer)) goto error;
	if (!roccat_swarm_rmp_read_unknown(&pointer)) goto error;
	if (!roccat_swarm_rmp_read_timers(&pointer)) goto error;
	if (!roccat_swarm_rmp_read_unknown_with_count(&pointer)) goto error;
	if (!roccat_swarm_rmp_read_pictures(&pointer)) goto error;
	if (!read_talk(profile_data, &pointer)) goto error;
	if (!read_keys_function(profile_data, &pointer)) goto error;
	if (!read_keys_easyzone(profile_data, &pointer)) goto error;
	if (!read_key_mask(profile_data, &pointer)) goto error;
	if (!read_macros(profile_data, &pointer)) goto error;
	if (!roccat_swarm_rmp_read_unknown(&pointer)) goto error;
	if (!read_sound_feedback(profile_data, &pointer)) goto error;
	if (!read_keys_extra(profile_data, &pointer)) goto error;
	if (!read_keys_primary(profile_data, &pointer)) goto error;
	if (!read_light(profile_data, &pointer)) goto error;
	if (!roccat_swarm_rmp_read_unknown(&pointer)) goto error;

	if (pointer > rkp + length)
		g_warning("%zu > %zu", (gsize)(pointer - rkp), length);

	return TRUE;
error:
	return FALSE;
}
