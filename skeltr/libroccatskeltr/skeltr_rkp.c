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

#include "skeltr_rkp.h"
#include "skeltr_device.h"
#include "skeltr_key_relations.h"
#include "skeltr_rkp_keys_easyzone.h"
#include "skeltr_rkp_sound_feedback.h"
#include "roccat_notificator.h"
#include "roccat_swarm_rmp.h"
#include "roccat_talk.h"
#include "i18n-lib.h"

typedef struct {
	guint8 unknown1[4];
	guint8 type;
	guint8 unknown2[70];
} __attribute__ ((packed)) SkeltrRkpTalk;

static void write_header(SkeltrProfileData const *profile_data, gpointer *data, gsize *length) {
	RoccatSwarmRmpHeader *header;

	header = (RoccatSwarmRmpHeader *)roccat_swarm_rmp_write_with_size(data, length, sizeof(RoccatSwarmRmpHeader));
	roccat_swarm_rmp_header_set_profile_name(header, gaminggear_profile_data_get_name(GAMINGGEAR_PROFILE_DATA(profile_data)));
	header->volume = 50; // TODO tie to notification_volume
	header->profile_autoswitch = 0;
}

static void write_sound_feedback(SkeltrProfileData const *profile_data, gpointer *data, gsize *length) {
	SkeltrRkpSoundFeedback *buffer;

	buffer = (SkeltrRkpSoundFeedback *)roccat_swarm_rmp_write_with_size(data, length, sizeof(SkeltrRkpSoundFeedback));

	buffer->typing_sound = skeltr_profile_data_get_sound_feedback_typing(profile_data);

	buffer->wakeup = (skeltr_profile_data_get_wakeup_notification_type(profile_data) != ROCCAT_NOTIFICATION_TYPE_OFF) ?
			SKELTR_RKP_SOUND_FEEDBACK_NOTIFICATION_ON : SKELTR_RKP_SOUND_FEEDBACK_NOTIFICATION_OFF;
	buffer->sleep = (skeltr_profile_data_get_sleep_notification_type(profile_data) != ROCCAT_NOTIFICATION_TYPE_OFF) ?
			SKELTR_RKP_SOUND_FEEDBACK_NOTIFICATION_ON : SKELTR_RKP_SOUND_FEEDBACK_NOTIFICATION_OFF;
	buffer->macro_record = (skeltr_profile_data_get_macro_record_notification_type(profile_data) != ROCCAT_NOTIFICATION_TYPE_OFF) ?
			SKELTR_RKP_SOUND_FEEDBACK_NOTIFICATION_ON : SKELTR_RKP_SOUND_FEEDBACK_NOTIFICATION_OFF;
	buffer->profile_switch = (skeltr_profile_data_get_profile_switch_notification_type(profile_data) != ROCCAT_NOTIFICATION_TYPE_OFF) ?
			SKELTR_RKP_SOUND_FEEDBACK_NOTIFICATION_ON : SKELTR_RKP_SOUND_FEEDBACK_NOTIFICATION_OFF;
}

static void write_macros(SkeltrProfileData const *profile_data, gpointer *data, gsize *length) {
	SkeltrMacro *macro;
	guint key_index;

	roccat_swarm_rmp_write_bigendian32(data, length, SKELTR_KEYS_NUM); // FIXME check
	for (key_index = 0; key_index < SKELTR_KEYS_NUM; ++key_index) {
		macro = (SkeltrMacro *)roccat_swarm_rmp_write_with_size(data, length, sizeof(SkeltrMacro));

		if (skeltr_key_relations[key_index].macro_index != SKELTR_KEY_RELATION_INVALID)
			skeltr_macro_copy(macro, skeltr_profile_data_get_macro(profile_data, skeltr_key_relations[key_index].macro_index));

		/* That's not quite right, the Windows version doesn't set key index */
		skeltr_macro_finalize(macro, 0, key_index);
	}
}

static void write_keys_function(SkeltrProfileData const *profile_data, gpointer *data, gsize *length) {
	SkeltrKeysFunction *buffer;

	buffer = (SkeltrKeysFunction *)roccat_swarm_rmp_write_with_size(data, length, sizeof(SkeltrKeysFunction));
	skeltr_keys_function_copy(buffer, skeltr_profile_data_get_keys_function(profile_data));
}

static void write_keys_extra(SkeltrProfileData const *profile_data, gpointer *data, gsize *length) {
	SkeltrKeysExtra *buffer;

	buffer = (SkeltrKeysExtra *)roccat_swarm_rmp_write_with_size(data, length, sizeof(SkeltrKeysExtra));
	skeltr_keys_extra_copy(buffer, skeltr_profile_data_get_keys_extra(profile_data));
}

static void write_keys_thumbster(SkeltrProfileData const *profile_data, gpointer *data, gsize *length) {
	SkeltrKeysThumbster *buffer;

	buffer = (SkeltrKeysThumbster *)roccat_swarm_rmp_write_with_size(data, length, sizeof(SkeltrKeysThumbster));
	skeltr_keys_thumbster_copy(buffer, skeltr_profile_data_get_keys_thumbster(profile_data));
}

static void write_keys_primary(SkeltrProfileData const *profile_data, gpointer *data, gsize *length) {
	SkeltrKeysPrimary *buffer;

	buffer = (SkeltrKeysPrimary *)roccat_swarm_rmp_write_with_size(data, length, sizeof(SkeltrKeysPrimary));
	skeltr_keys_primary_copy(buffer, skeltr_profile_data_get_keys_primary(profile_data));
}

static void write_keys_macro(SkeltrProfileData const *profile_data, gpointer *data, gsize *length) {
	SkeltrKeysMacro *buffer;

	buffer = (SkeltrKeysMacro *)roccat_swarm_rmp_write_with_size(data, length, sizeof(SkeltrKeysMacro));
	skeltr_keys_macro_copy(buffer, skeltr_profile_data_get_keys_macro(profile_data));
}

static void write_keys_easyzone(SkeltrProfileData const *profile_data, gpointer *data, gsize *length) {
	SkeltrRkpKeysEasyzone *buffer;

	buffer = (SkeltrRkpKeysEasyzone *)roccat_swarm_rmp_write_with_size(data, length, sizeof(SkeltrRkpKeysEasyzone));
	skeltr_keys_easyzone_to_rkp_keys_easyzone(buffer, skeltr_profile_data_get_keys_easyzone(profile_data));
}

static void write_light(SkeltrProfileData const *profile_data, gpointer *data, gsize *length) {
	SkeltrLight *buffer;

	buffer = (SkeltrLight *)roccat_swarm_rmp_write_with_size(data, length, sizeof(SkeltrLight));
	skeltr_light_copy(buffer, skeltr_profile_data_get_light(profile_data));
}

static void write_gamefiles(SkeltrProfileData const *profile_data, gpointer *data, gsize *length) {
	guint i;
	gsize size;

	size = ROCCAT_SWARM_RMP_GAMEFILE_NUM * ROCCAT_SWARM_RMP_GAMEFILE_LENGTH * sizeof(gunichar2);
	roccat_swarm_rmp_write_bigendian32(data, length, size);
	for (i = 0; i < ROCCAT_SWARM_RMP_GAMEFILE_NUM; ++i)
		roccat_swarm_rmp_write_bigendian_string(data, length, gaminggear_profile_data_get_gamefile(GAMINGGEAR_PROFILE_DATA(profile_data), i), ROCCAT_SWARM_RMP_GAMEFILE_LENGTH);
}

void write_unknown_with_count_count(gpointer *data, gsize *length, gsize count1, gsize count2, gsize size) {
	guint i;

	roccat_swarm_rmp_write_bigendian32(data, length, count1);

	for (i = 0; i < count1; ++i)
		roccat_swarm_rmp_write_unknown_with_count(data, length, count2, size);
}

static void write_talk(SkeltrProfileData const *profile_data, gpointer *data, gsize *length) {
	SkeltrRkpTalk *buffer;

	if (skeltr_profile_data_get_keys_extra(profile_data)->capslock == TRUE) { // FIXME
		/* Only caps-lock button is easyshift/talk capable */
		roccat_swarm_rmp_write_bigendian32(data, length, 1);
		roccat_swarm_rmp_write_bigendian32(data, length, 0x39);
		buffer = (SkeltrRkpTalk *)roccat_swarm_rmp_write_with_size(data, length, sizeof(SkeltrRkpTalk));
		buffer->type = TRUE; // FIXME
	} else
		roccat_swarm_rmp_write_bigendian32(data, length, 0);
}

static void write_key_mask(SkeltrProfileData const *profile_data, gpointer *data, gsize *length) {
	SkeltrKeyMask *buffer;

	buffer = (SkeltrKeyMask *)roccat_swarm_rmp_write_with_size(data, length, sizeof(SkeltrKeyMask));
	skeltr_key_mask_copy(buffer, skeltr_profile_data_get_key_mask(profile_data));
}

static gboolean should_write_opener(SkeltrProfileData const *profile_data, guint macro_index) {
	RoccatButton const *button;

	button = skeltr_profile_data_get_key_for_macro_index(profile_data, macro_index);
	if (!button)
		return FALSE;

	return button->type == SKELTR_KEY_TYPE_OPEN_APP || button->type == SKELTR_KEY_TYPE_OPEN_DOCUMENT ||
			button->type == SKELTR_KEY_TYPE_OPEN_FOLDER || button->type == SKELTR_KEY_TYPE_OPEN_WEBSITE;
}

static void write_openers(SkeltrProfileData const *profile_data, gpointer *data, gsize *length) {
	guint count;
	guint i;

	count = 0;
	for (i = 0; i < SKELTR_MACRO_NUM; ++i) {
		if (should_write_opener(profile_data, i))
			++count;
	}
	roccat_swarm_rmp_write_bigendian32(data, length, count);

	for (i = 0; i < SKELTR_MACRO_NUM; ++i) {
		if (should_write_opener(profile_data, i)) {
			roccat_swarm_rmp_write_bigendian32(data, length, i);
			roccat_swarm_rmp_write_bigendian_string_with_size(data, length, skeltr_profile_data_get_opener(profile_data, i), ROCCAT_SWARM_RMP_OPENER_LENGTH);
		}
	}
}

gpointer skeltr_profile_data_to_rkp(SkeltrProfileData const *profile_data, gsize *length) {
	gpointer rkp = NULL;
	gsize local_length = 0;

	roccat_swarm_rmp_write_unknown(&rkp, &local_length, 0);
	write_header(profile_data, &rkp, &local_length);
	write_gamefiles(profile_data, &rkp, &local_length);
	write_openers(profile_data, &rkp, &local_length);
	roccat_swarm_rmp_write_unknown(&rkp, &local_length, 0);
	roccat_swarm_rmp_write_timers(&rkp, &local_length);
	roccat_swarm_rmp_write_unknown_with_count(&rkp, &local_length, 0, 0);
	roccat_swarm_rmp_write_unknown(&rkp, &local_length, 0); // pictures
	write_talk(profile_data, &rkp, &local_length);
	write_keys_function(profile_data, &rkp, &local_length);
	write_keys_macro(profile_data, &rkp, &local_length);
	write_keys_thumbster(profile_data, &rkp, &local_length);
	write_keys_extra(profile_data, &rkp, &local_length);
	write_keys_easyzone(profile_data, &rkp, &local_length);
	write_key_mask(profile_data, &rkp, &local_length);
	write_macros(profile_data, &rkp, &local_length);
	roccat_swarm_rmp_write_unknown(&rkp, &local_length, 0);
	write_sound_feedback(profile_data, &rkp, &local_length);
	write_keys_primary(profile_data, &rkp, &local_length);
	write_light(profile_data, &rkp, &local_length);
	write_unknown_with_count_count(&rkp, &local_length, 0, 0, 0);
	roccat_swarm_rmp_write_unknown(&rkp, &local_length, 0);

	*length = local_length;
	return rkp;
}

static gboolean read_header(SkeltrProfileData *profile_data, gconstpointer *data) {
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

static gboolean read_sound_feedback(SkeltrProfileData *profile_data, gconstpointer *data) {
	guint32 size;
	SkeltrRkpSoundFeedback const *sound_feedback;

	size = roccat_swarm_rmp_read_bigendian32(data);
	if (size != sizeof(SkeltrRkpSoundFeedback)) {
		g_warning("%u != sizeof(SkeltrRkpSoundFeedback)", size);
		return FALSE;
	}

	sound_feedback = (SkeltrRkpSoundFeedback const *)*data;

	skeltr_profile_data_set_sound_feedback_typing(profile_data, sound_feedback->typing_sound);

	skeltr_profile_data_set_wakeup_notification_type(profile_data, (sound_feedback->wakeup == SKELTR_RKP_SOUND_FEEDBACK_NOTIFICATION_ON) ?
			ROCCAT_NOTIFICATION_TYPE_AUDIO : ROCCAT_NOTIFICATION_TYPE_OFF);
	skeltr_profile_data_set_sleep_notification_type(profile_data, (sound_feedback->sleep == SKELTR_RKP_SOUND_FEEDBACK_NOTIFICATION_ON) ?
			ROCCAT_NOTIFICATION_TYPE_AUDIO : ROCCAT_NOTIFICATION_TYPE_OFF);
	skeltr_profile_data_set_macro_record_notification_type(profile_data, (sound_feedback->macro_record == SKELTR_RKP_SOUND_FEEDBACK_NOTIFICATION_ON) ?
			ROCCAT_NOTIFICATION_TYPE_AUDIO : ROCCAT_NOTIFICATION_TYPE_OFF);
	skeltr_profile_data_set_profile_switch_notification_type(profile_data, (sound_feedback->profile_switch == SKELTR_RKP_SOUND_FEEDBACK_NOTIFICATION_ON) ?
			ROCCAT_NOTIFICATION_TYPE_AUDIO : ROCCAT_NOTIFICATION_TYPE_OFF);

	*data += size;
	return TRUE;
}

static gboolean read_macros(SkeltrProfileData *profile_data, gconstpointer *data) {
	SkeltrMacro const *macro;
	guint32 count;
	guint32 size;
	guint key_index;

	count = roccat_swarm_rmp_read_bigendian32(data);
	if (count != SKELTR_KEYS_NUM) {
		g_warning("%u != SKELTR_KEYS_NUM", count);
		return FALSE;
	}

	for (key_index = 0; key_index < count; ++key_index) {
		size = roccat_swarm_rmp_read_bigendian32(data);
		if (size != sizeof(SkeltrMacro)) {
			g_warning("%u != sizeof(SkeltrMacro)", size);
			return FALSE;
		}

		macro = (SkeltrMacro const *)*data;
		*data += size;

		if (skeltr_key_relations[key_index].macro_index == SKELTR_KEY_RELATION_INVALID)
			continue;

		skeltr_profile_data_set_macro(profile_data, skeltr_key_relations[key_index].macro_index, macro);
	}

	return TRUE;
}

static gboolean read_gamefiles(SkeltrProfileData *profile_data, gconstpointer *data) {
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

static gboolean read_keys_easyzone(SkeltrProfileData *profile_data, gconstpointer *data) {
	guint32 size;
	SkeltrKeysEasyzone keys_easyzone;

	size = roccat_swarm_rmp_read_bigendian32(data);
	if (size != sizeof(SkeltrRkpKeysEasyzone)) {
		g_warning("%u != sizeof(SkeltrRkpKeysEasyzone)", size);
		return FALSE;
	}

	skeltr_rkp_keys_easyzone_to_keys_easyzone(&keys_easyzone, (SkeltrRkpKeysEasyzone const *)*data);
	skeltr_profile_data_set_keys_easyzone(profile_data, &keys_easyzone);

	*data += size;
	return TRUE;
}

static gboolean read_keys_function(SkeltrProfileData *profile_data, gconstpointer *data) {
	guint32 size;

	size = roccat_swarm_rmp_read_bigendian32(data);
	if (size != sizeof(SkeltrKeysFunction)) {
		g_warning("%u != sizeof(SkeltrKeysFunction)", size);
		return FALSE;
	}

	skeltr_profile_data_set_keys_function(profile_data, (SkeltrKeysFunction const *)*data);

	*data += size;
	return TRUE;
}

static gboolean read_keys_macro(SkeltrProfileData *profile_data, gconstpointer *data) {
	guint32 size;

	size = roccat_swarm_rmp_read_bigendian32(data);
	if (size != sizeof(SkeltrKeysMacro)) {
		g_warning("%u != sizeof(SkeltrKeysMacro)", size);
		return FALSE;
	}

	skeltr_profile_data_set_keys_macro(profile_data, (SkeltrKeysMacro const *)*data);

	*data += size;
	return TRUE;
}

static gboolean read_keys_thumbster(SkeltrProfileData *profile_data, gconstpointer *data) {
	guint32 size;

	size = roccat_swarm_rmp_read_bigendian32(data);
	if (size != sizeof(SkeltrKeysThumbster)) {
		g_warning("%u != sizeof(SkeltrKeysThumbster)", size);
		return FALSE;
	}

	skeltr_profile_data_set_keys_thumbster(profile_data, (SkeltrKeysThumbster const *)*data);

	*data += size;
	return TRUE;
}

static gboolean read_keys_extra(SkeltrProfileData *profile_data, gconstpointer *data) {
	guint32 size;

	size = roccat_swarm_rmp_read_bigendian32(data);
	if (size != sizeof(SkeltrKeysExtra)) {
		g_warning("%u != sizeof(SkeltrKeysExtra)", size);
		return FALSE;
	}

	skeltr_profile_data_set_keys_extra(profile_data, (SkeltrKeysExtra const *)*data);

	*data += size;
	return TRUE;
}

static gboolean read_keys_primary(SkeltrProfileData *profile_data, gconstpointer *data) {
	guint32 size;

	size = roccat_swarm_rmp_read_bigendian32(data);
	if (size != sizeof(SkeltrKeysPrimary)) {
		g_warning("%u != sizeof(SkeltrKeysPrimary)", size);
		return FALSE;
	}

	skeltr_profile_data_set_keys_primary(profile_data, (SkeltrKeysPrimary const *)*data);

	*data += size;
	return TRUE;
}

static gboolean read_light(SkeltrProfileData *profile_data, gconstpointer *data) {
	guint32 size;

	size = roccat_swarm_rmp_read_bigendian32(data);
	if (size != sizeof(SkeltrLight)) {
		g_warning("%u != sizeof(SkeltrLight)", size);
		return FALSE;
	}

	skeltr_profile_data_set_light(profile_data, (SkeltrLight const *)*data);

	*data += size;
	return TRUE;
}

static gboolean read_unknown_with_count_count(gconstpointer *data) {
	guint32 count;
	guint32 i;

	count = roccat_swarm_rmp_read_bigendian32(data);
	g_debug(_("Rmp unknown count 0x%08x {"), count);

	for (i = 0; i < count; ++i)
		if (!roccat_swarm_rmp_read_unknown_with_count(data))
			return FALSE;

	g_debug(_("}"));

	return TRUE;
}

static gboolean read_openers(SkeltrProfileData *profile_data, gconstpointer *data) {
	guint32 count;
	guint32 button_index;
	guint i;
	gchar *string;

	count = roccat_swarm_rmp_read_bigendian32(data);
	for (i = 0; i < count; ++i) {
		button_index = roccat_swarm_rmp_read_bigendian32(data);

		string = roccat_swarm_rmp_read_bigendian_string_with_size(data);
		skeltr_profile_data_set_opener(profile_data, button_index, string);
		g_free(string);
	}
	return TRUE;
}

static gboolean read_talk(SkeltrProfileData *profile_data, gconstpointer *data) {
	guint32 count;
	guint32 button_index;
	guint32 size;
	SkeltrRkpTalk const *talk;

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
	if (size != sizeof(SkeltrRkpTalk)) {
		g_warning("%u != sizeof(SkeltrRkpTalk)", size);
		return FALSE;
	}

	talk = (SkeltrRkpTalk const *)*data;

	if (talk->type == TRUE) // FIXME
		skeltr_profile_data_set_capslock_talk_target(profile_data, ROCCAT_TALK_DEVICE_MOUSE);

	*data += size;

	return TRUE;
}

static gboolean read_key_mask(SkeltrProfileData *profile_data, gconstpointer *data) {
	guint32 size;

	size = roccat_swarm_rmp_read_bigendian32(data);
	if (size != sizeof(SkeltrKeyMask)) {
		g_warning("%u != sizeof(SkeltrKeyMask)", size);
		return FALSE;
	}

	skeltr_profile_data_set_key_mask(profile_data, (SkeltrKeyMask const *)*data);

	*data += size;
	return TRUE;
}

gboolean skeltr_rkp_to_profile_data(SkeltrProfileData *profile_data, gconstpointer rkp, gsize length) {
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
	if (!read_keys_macro(profile_data, &pointer)) goto error;
	if (!read_keys_thumbster(profile_data, &pointer)) goto error;
	if (!read_keys_extra(profile_data, &pointer)) goto error;
	if (!read_keys_easyzone(profile_data, &pointer)) goto error;
	if (!read_key_mask(profile_data, &pointer)) goto error;
	if (!read_macros(profile_data, &pointer)) goto error;
	if (!roccat_swarm_rmp_read_unknown(&pointer)) goto error;
	if (!read_sound_feedback(profile_data, &pointer)) goto error;
	if (!read_keys_primary(profile_data, &pointer)) goto error;
	if (!read_light(profile_data, &pointer)) goto error;
	if (!read_unknown_with_count_count(&pointer)) goto error;
	if (!roccat_swarm_rmp_read_unknown(&pointer)) goto error;

	if (pointer > rkp + length)
		g_warning("%zu > %zu", (gsize)(pointer - rkp), length);

	return TRUE;
error:
	return FALSE;
}
