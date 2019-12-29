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

#include "ryosmkfx_rkp.h"
#include "ryosmkfx_device.h"
#include "ryosmkfx.h"
#include "roccat_swarm_rmp.h"
#include "roccat_helper.h"
#include "roccat_talk.h"
#include "roccat.h"
#include "g_roccat_helper.h"

typedef struct _RyosmkfxRkpMacro RyosmkfxRkpMacro;
typedef struct _RyosmkfxRkpTalk RyosmkfxRkpTalk;

enum {
	RYOSMKFX_RKP_TALK_NAME_LENGTH = 20, /* utf16 */
};

struct _RyosmkfxRkpMacro {
	guint8 unused1[3];
	RyosmkfxMacro macro;
	guint8 unused2[2];
} __attribute__ ((packed));

struct _RyosmkfxRkpTalk {
	guint16 vendor_id;
	guint16 product_id;
	guint8 unknown1;
	guint16 name[RYOSMKFX_RKP_TALK_NAME_LENGTH];
	guint8 unknown2[30];
} __attribute__ ((packed));

static void ryosmkfx_rkp_talk_set_name(RyosmkfxRkpTalk *talk, gchar const *name) {
	gunichar2 *string;
	glong items;
	string = g_utf8_to_utf16(name, -1L, NULL, &items, NULL);
	items = MIN(RYOSMKFX_RKP_TALK_NAME_LENGTH, items);
	memcpy(&talk->name, string, items * sizeof(gunichar2));
	g_free(string);
}

static gboolean read_header(RyosmkfxProfileData *profile_data, gconstpointer *data) {
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
	ryosmkfx_profile_data_eventhandler_set_profile_name(&profile_data->eventhandler, string);
	g_free(string);

	*data += size;
	return TRUE;
}

static gboolean read_gamefiles(RyosmkfxProfileData *profile_data, gconstpointer *data) {
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
		ryosmkfx_profile_data_eventhandler_set_gamefile_name(&profile_data->eventhandler, i, gamefile);
		g_free(gamefile);
	}

	return TRUE;
}

static gboolean read_sound_feedback(RyosmkfxProfileData *profile_data, gconstpointer *data) {
	guint32 size;
	guint16 __attribute__((__unused__)) sound_feedback;

	size = roccat_swarm_rmp_read_bigendian32(data);
	if (size != 2) {
		g_warning("%u != sizeof(sound_feedback)", size);
		return FALSE;
	}

	sound_feedback = *(guint16 const *)data;

	*data += size;
	return TRUE;
}

static gboolean read_openers(RyosmkfxProfileData *profile_data, gconstpointer *data) {
	guint32 count;
	guint32 button_index;
	guint i;
	gchar *string;

	count = roccat_swarm_rmp_read_bigendian32(data);
	for (i = 0; i < count; ++i) {
		button_index = roccat_swarm_rmp_read_bigendian32(data);

		string = roccat_swarm_rmp_read_bigendian_string_with_size(data);
		ryosmkfx_profile_data_eventhandler_set_opener(&profile_data->eventhandler, button_index, string);
		g_free(string);
	}
	return TRUE;
}

static gboolean read_keys_primary(RyosmkfxProfileData *profile_data, gconstpointer *data) {
	guint32 size;

	size = roccat_swarm_rmp_read_bigendian32(data);
	if (size != sizeof(RyosKeysPrimary)) {
		g_warning("%u != sizeof(RyosKeysPrimary)", size);
		return FALSE;
	}

	ryosmkfx_profile_data_hardware_set_keys_primary(&profile_data->hardware, (RyosKeysPrimary const *)*data);

	*data += size;
	return TRUE;
}

static gboolean read_keys_function(RyosmkfxProfileData *profile_data, gconstpointer *data) {
	guint32 size;

	size = roccat_swarm_rmp_read_bigendian32(data);
	if (size != sizeof(RyosKeysFunction)) {
		g_warning("%u != sizeof(RyosKeysFunction)", size);
		return FALSE;
	}

	ryosmkfx_profile_data_hardware_set_keys_function(&profile_data->hardware, (RyosKeysFunction const *)*data);

	*data += size;
	return TRUE;
}

static gboolean read_keys_macro(RyosmkfxProfileData *profile_data, gconstpointer *data) {
	guint32 size;

	size = roccat_swarm_rmp_read_bigendian32(data);
	if (size != sizeof(RyosKeysMacro)) {
		g_warning("%u != sizeof(RyosKeysMacro)", size);
		return FALSE;
	}

	ryosmkfx_profile_data_hardware_set_keys_macro(&profile_data->hardware, (RyosKeysMacro const *)*data);

	*data += size;
	return TRUE;
}

static gboolean read_keys_thumbster(RyosmkfxProfileData *profile_data, gconstpointer *data) {
	guint32 size;

	size = roccat_swarm_rmp_read_bigendian32(data);
	if (size != sizeof(RyosKeysThumbster)) {
		g_warning("%u != sizeof(RyosKeysThumbster)", size);
		return FALSE;
	}

	ryosmkfx_profile_data_hardware_set_keys_thumbster(&profile_data->hardware, (RyosKeysThumbster const *)*data);

	*data += size;
	return TRUE;
}

static gboolean read_keys_extra(RyosmkfxProfileData *profile_data, gconstpointer *data) {
	guint32 size;

	size = roccat_swarm_rmp_read_bigendian32(data);
	if (size != sizeof(RyosKeysExtra)) {
		g_warning("%u != sizeof(RyosKeysExtra)", size);
		return FALSE;
	}

	ryosmkfx_profile_data_hardware_set_keys_extra(&profile_data->hardware, (RyosKeysExtra const *)*data);

	*data += size;
	return TRUE;
}

static gboolean read_keys_easyzone(RyosmkfxProfileData *profile_data, gconstpointer *data) {
	guint32 size;

	size = roccat_swarm_rmp_read_bigendian32(data);
	if (size != sizeof(RyosKeysEasyzone)) {
		g_warning("%u != sizeof(RyosKeysEasyzone)", size);
		return FALSE;
	}

	ryosmkfx_profile_data_hardware_set_keys_easyzone(&profile_data->hardware, (RyosKeysEasyzone const *)*data);

	*data += size;
	return TRUE;
}

static gboolean read_key_mask(RyosmkfxProfileData *profile_data, gconstpointer *data) {
	guint32 size;

	size = roccat_swarm_rmp_read_bigendian32(data);
	if (size != sizeof(RyosKeyMask)) {
		g_warning("%u != sizeof(RyosKeyMask)", size);
		return FALSE;
	}

	ryosmkfx_profile_data_hardware_set_key_mask(&profile_data->hardware, (RyosKeyMask const *)*data);

	*data += size;
	return TRUE;
}

static gboolean read_light(RyosmkfxProfileData *profile_data, gconstpointer *data) {
	guint32 size;

	size = roccat_swarm_rmp_read_bigendian32(data);
	if (size != sizeof(RyosmkfxLight)) {
		g_warning("%u != sizeof(RyosmkfxLight)", size);
		return FALSE;
	}

	ryosmkfx_profile_data_hardware_set_light(&profile_data->hardware, (RyosmkfxLight const *)*data);

	*data += size;
	return TRUE;
}

static gboolean read_talk(RyosmkfxProfileData *profile_data, gconstpointer *data) {
	guint32 count;
	guint32 button_index;
	guint32 size;

	guint i;

	count = roccat_swarm_rmp_read_bigendian32(data);
	for (i = 0; i < count; ++i) {
		button_index = roccat_swarm_rmp_read_bigendian32(data);
		size = roccat_swarm_rmp_read_bigendian32(data);
		if (size != sizeof(RyosmkfxRkpTalk)) {
			g_warning("%u != sizeof(RyosmkfxRkpTalk)", size);
			return FALSE;
		}

		ryosmkfx_profile_data_eventhandler_set_talk_target(&profile_data->eventhandler, button_index, ((RyosmkfxRkpTalk const *)*data)->product_id);
		*data += size;
	}

	return TRUE;
}

static gboolean read_stored_lights(RyosmkfxProfileData *profile_data, gconstpointer *data) {
	guint32 size;

	size = roccat_swarm_rmp_read_bigendian32(data);
	if (size != sizeof(RyosmkfxStoredLights)) {
		g_warning("%u != sizeof(RyosmkfxStoredLights)", size);
		return FALSE;
	}

	ryosmkfx_profile_data_hardware_set_stored_lights(&profile_data->hardware, (RyosmkfxStoredLights const *)*data);

	*data += size;
	return TRUE;
}

static gboolean read_macros(RyosmkfxProfileData *profile_data, gconstpointer *data) {
	RyosmkfxRkpMacro const *rkp_macro;
	guint32 count;
	guint32 size;
	guint i;

	count = roccat_swarm_rmp_read_bigendian32(data);
	if (count != RYOS_RKP_KEYS_NUM) {
		g_warning("%u != RYOS_RKP_KEYS_NUM", count);
		return FALSE;
	}

	for (i = 0; i < count; ++i) {
		size = roccat_swarm_rmp_read_bigendian32(data);
		if (size != sizeof(RyosmkfxRkpMacro)) {
			g_warning("%u != sizeof(RyosmkfxRkpMacro)", size);
			return FALSE;
		}

		rkp_macro = (RyosmkfxRkpMacro const *)*data;

		/* TODO should I use i or rkp_macro->macro.button_index ? */
		ryosmkfx_profile_data_hardware_set_macro(&profile_data->hardware, i, &rkp_macro->macro);
		*data += size;
	}

	return TRUE;
}

RyosmkfxProfileData *ryosmkfx_rkp_to_profile_data(gconstpointer rkp, gsize length) {
	RyosmkfxProfileData *profile_data;
	gconstpointer pointer;

	profile_data = ryosmkfx_profile_data_new();
	pointer = rkp;

	if (!roccat_swarm_rmp_read_unknown(&pointer)) goto error;
	if (!read_header(profile_data, &pointer)) goto error;
	if (!read_gamefiles(profile_data, &pointer)) goto error;
	if (!read_openers(profile_data, &pointer)) goto error;
	if (!read_sound_feedback(profile_data, &pointer)) goto error;
	if (!roccat_swarm_rmp_read_timers(&pointer)) goto error;
	if (!roccat_swarm_rmp_read_unknown_with_count(&pointer)) goto error;
	if (!roccat_swarm_rmp_read_pictures(&pointer)) goto error;
	if (!read_talk(profile_data, &pointer)) goto error;
	if (!read_keys_primary(profile_data, &pointer)) goto error;
	if (!read_keys_function(profile_data, &pointer)) goto error;
	if (!read_keys_macro(profile_data, &pointer)) goto error;
	if (!read_keys_thumbster(profile_data, &pointer)) goto error;
	if (!read_keys_extra(profile_data, &pointer)) goto error;
	if (!read_keys_easyzone(profile_data, &pointer)) goto error;
	if (!read_key_mask(profile_data, &pointer)) goto error;
	if (!read_macros(profile_data, &pointer)) goto error;
	if (!roccat_swarm_rmp_read_unknown(&pointer)) goto error;
	if (!read_light(profile_data, &pointer)) goto error;
	if (!read_stored_lights(profile_data, &pointer)) goto error;
	if (!roccat_swarm_rmp_read_unknown_with_count(&pointer)) goto error;
	if (!roccat_swarm_rmp_read_unknown(&pointer)) goto error; // color presets

	if (pointer > rkp + length)
		g_warning("%zu > %zu", (gsize)(pointer - rkp), length);

	return profile_data;
error:
	g_free(profile_data);
	return NULL;
}

static void write_header(RyosmkfxProfileData const *profile_data, gpointer *data, gsize *length) {
	RoccatSwarmRmpHeader *header;

	header = (RoccatSwarmRmpHeader *)roccat_swarm_rmp_write_with_size(data, length, sizeof(RoccatSwarmRmpHeader));
	roccat_swarm_rmp_header_set_profile_name(header, profile_data->eventhandler.profile_name);
	/* default values */
	header->volume = 50;
	header->profile_autoswitch = 0;
	header->unknown4[0] = 1;
}

static gboolean should_write_talk(RyosmkfxProfileDataHardware const *hardware, guint talk_index) {
	guint type = RYOS_KEY_TYPE_DISABLED;

	if (ryos_macro_index_is_keys_easyzone(talk_index))
		type = hardware->keys_easyzone.keys[ryos_macro_index_to_keys_easyzone_index(talk_index)].type;
	else if (ryos_macro_index_is_keys_macro(talk_index))
		type = hardware->keys_macro.keys[ryos_macro_index_to_keys_macro_index(talk_index)].type;
	else if (ryos_macro_index_is_keys_thumbster(talk_index))
		type = hardware->keys_thumbster.keys[ryos_macro_index_to_keys_thumbster_index(talk_index)].type;

	return type == RYOS_KEY_TYPE_TALK_EASYSHIFT ||
			type == RYOS_KEY_TYPE_TALK_EASYSHIFT_LOCK ||
			type == RYOS_KEY_TYPE_TALK_EASYAIM_1 ||
			type == RYOS_KEY_TYPE_TALK_EASYAIM_2 ||
			type == RYOS_KEY_TYPE_TALK_EASYAIM_3 ||
			type == RYOS_KEY_TYPE_TALK_EASYAIM_4 ||
			type == RYOS_KEY_TYPE_TALK_EASYAIM_5;
}

static void write_talk(RyosmkfxProfileData const *profile_data, gpointer *data, gsize *length) {
	RyosmkfxRkpTalk *buffer;
	guint count;
	guint i;

	count = 0;
	for (i = 0; i < RYOS_RKP_KEYS_NUM; ++i) {
		if (should_write_talk(&profile_data->hardware, i))
			++count;
	}
	roccat_swarm_rmp_write_bigendian32(data, length, count);

	for (i = 0; i < RYOS_RKP_KEYS_NUM; ++i) {
		if (should_write_talk(&profile_data->hardware, i)) {
			roccat_swarm_rmp_write_bigendian32(data, length, i);
			buffer = (RyosmkfxRkpTalk *)roccat_swarm_rmp_write_with_size(data, length, sizeof(RyosmkfxRkpTalk));
			ryosmkfx_rkp_talk_set_name(buffer, roccat_talk_device_get_text_static(profile_data->eventhandler.talk_targets[i]));
			buffer->vendor_id = GUINT16_TO_LE(USB_VENDOR_ID_ROCCAT);
			buffer->product_id = GUINT16_TO_LE(profile_data->eventhandler.talk_targets[i]);
		}
	}
}

static void write_keys_primary(RyosmkfxProfileData const *profile_data, gpointer *data, gsize *length) {
	RyosKeysPrimary *buffer;

	buffer = (RyosKeysPrimary *)roccat_swarm_rmp_write_with_size(data, length, sizeof(RyosKeysPrimary));
	ryos_keys_primary_copy(buffer, &profile_data->hardware.keys_primary);
}

static void write_keys_function(RyosmkfxProfileData const *profile_data, gpointer *data, gsize *length) {
	RyosKeysFunction *buffer;

	buffer = (RyosKeysFunction *)roccat_swarm_rmp_write_with_size(data, length, sizeof(RyosKeysFunction));
	ryos_keys_function_copy(buffer, &profile_data->hardware.keys_function);
}

static void write_keys_macro(RyosmkfxProfileData const *profile_data, gpointer *data, gsize *length) {
	RyosKeysMacro *buffer;

	buffer = (RyosKeysMacro *)roccat_swarm_rmp_write_with_size(data, length, sizeof(RyosKeysMacro));
	ryos_keys_macro_copy(buffer, &profile_data->hardware.keys_macro);
}

static void write_keys_thumbster(RyosmkfxProfileData const *profile_data, gpointer *data, gsize *length) {
	RyosKeysThumbster *buffer;

	buffer = (RyosKeysThumbster *)roccat_swarm_rmp_write_with_size(data, length, sizeof(RyosKeysThumbster));
	ryos_keys_thumbster_copy(buffer, &profile_data->hardware.keys_thumbster);
}

static void write_keys_extra(RyosmkfxProfileData const *profile_data, gpointer *data, gsize *length) {
	RyosKeysExtra *buffer;

	buffer = (RyosKeysExtra *)roccat_swarm_rmp_write_with_size(data, length, sizeof(RyosKeysExtra));
	ryos_keys_extra_copy(buffer, &profile_data->hardware.keys_extra);
}

static void write_keys_easyzone(RyosmkfxProfileData const *profile_data, gpointer *data, gsize *length) {
	RyosKeysEasyzone *buffer;

	buffer = (RyosKeysEasyzone *)roccat_swarm_rmp_write_with_size(data, length, sizeof(RyosKeysEasyzone));
	ryos_keys_easyzone_copy(buffer, &profile_data->hardware.keys_easyzone);
}

static void write_key_mask(RyosmkfxProfileData const *profile_data, gpointer *data, gsize *length) {
	RyosKeyMask *buffer;

	buffer = (RyosKeyMask *)roccat_swarm_rmp_write_with_size(data, length, sizeof(RyosKeyMask));
	ryos_key_mask_copy(buffer, &profile_data->hardware.key_mask);
}

static void write_light(RyosmkfxProfileData const *profile_data, gpointer *data, gsize *length) {
	RyosmkfxLight *buffer;

	buffer = (RyosmkfxLight *)roccat_swarm_rmp_write_with_size(data, length, sizeof(RyosmkfxLight));
	ryosmkfx_light_copy(buffer, &profile_data->hardware.light);
}

static void write_stored_lights(RyosmkfxProfileData const *profile_data, gpointer *data, gsize *length) {
	RyosmkfxStoredLights *buffer;

	buffer = (RyosmkfxStoredLights *)roccat_swarm_rmp_write_with_size(data, length, sizeof(RyosmkfxStoredLights));
	ryosmkfx_stored_lights_copy(buffer, &profile_data->hardware.stored_lights);
}

static void write_sound_feedback(RyosmkfxProfileData const *profile_data, gpointer *data, gsize *length) {
	guint16 *sound_feedback;

	sound_feedback = (guint16 *)roccat_swarm_rmp_write_with_size(data, length, sizeof(guint16));
	*sound_feedback = 0; /* default value */
}

static void write_gamefiles(RyosmkfxProfileData const *profile_data, gpointer *data, gsize *length) {
	guint i;
	gsize size;

	size = ROCCAT_SWARM_RMP_GAMEFILE_NUM * ROCCAT_SWARM_RMP_GAMEFILE_LENGTH * sizeof(gunichar2);
	roccat_swarm_rmp_write_bigendian32(data, length, size);
	for (i = 0; i < ROCCAT_SWARM_RMP_GAMEFILE_NUM; ++i)
		roccat_swarm_rmp_write_bigendian_string(data, length, profile_data->eventhandler.gamefile_names[i], ROCCAT_SWARM_RMP_GAMEFILE_LENGTH);
}

static void write_macros(RyosmkfxProfileData const *profile_data, gpointer *data, gsize *length) {
	RyosmkfxRkpMacro *rkp_macro;
	guint i;

	/* The Windows version doesn't support the compressed format with button_index
	 * for macro section (nor button_index contained in macro structure).
	 * Always all macros are written, regardless of usage which increases the filesize massively.
	 */
	roccat_swarm_rmp_write_bigendian32(data, length, RYOS_RKP_KEYS_NUM);
	for (i = 0; i < RYOS_RKP_KEYS_NUM; ++i) {
		rkp_macro = (RyosmkfxRkpMacro *)roccat_swarm_rmp_write_with_size(data, length, sizeof(RyosmkfxRkpMacro));
		ryosmkfx_macro_copy(&rkp_macro->macro, &profile_data->hardware.macros[i]);
	}
}

static gboolean should_write_opener(RyosmkfxProfileDataHardware const *hardware, guint macro_index) {
	guint type = RYOS_KEY_TYPE_DISABLED;

	if (ryos_macro_index_is_keys_easyzone(macro_index))
		type = hardware->keys_easyzone.keys[ryos_macro_index_to_keys_easyzone_index(macro_index)].type;
	else if (ryos_macro_index_is_keys_macro(macro_index))
		type = hardware->keys_macro.keys[ryos_macro_index_to_keys_macro_index(macro_index)].type;
	else if (ryos_macro_index_is_keys_thumbster(macro_index))
		type = hardware->keys_thumbster.keys[ryos_macro_index_to_keys_thumbster_index(macro_index)].type;

	return type == RYOS_KEY_TYPE_QUICKLAUNCH || type == RYOSMKFX_KEY_TYPE_OPEN_DOCUMENT || type == RYOSMKFX_KEY_TYPE_OPEN_FOLDER || type == RYOSMKFX_KEY_TYPE_OPEN_WEBSITE;
}

static void write_openers(RyosmkfxProfileData const *profile_data, gpointer *data, gsize *length) {
	guint count;
	guint i;

	count = 0;
	for (i = 0; i < RYOS_RKP_KEYS_NUM; ++i) {
		if (should_write_opener(&profile_data->hardware, i))
			++count;
	}
	roccat_swarm_rmp_write_bigendian32(data, length, count);

	for (i = 0; i < RYOS_RKP_KEYS_NUM; ++i) {
		if (should_write_opener(&profile_data->hardware, i)) {
			roccat_swarm_rmp_write_bigendian32(data, length, i);
			roccat_swarm_rmp_write_bigendian_string_with_size(data, length, (gchar const *)profile_data->eventhandler.openers[i], ROCCAT_SWARM_RMP_GAMEFILE_LENGTH);
		}
	}
}

gpointer ryosmkfx_profile_data_to_rkp(RyosmkfxProfileData const *profile_data, gsize *length) {
	gpointer rkp = NULL;
	gsize local_length = 0;

	roccat_swarm_rmp_write_unknown(&rkp, &local_length, 0); // 4
	write_header(profile_data, &rkp, &local_length);
	write_gamefiles(profile_data, &rkp, &local_length);
	write_openers(profile_data, &rkp, &local_length);
	write_sound_feedback(profile_data, &rkp, &local_length);
	roccat_swarm_rmp_write_timers(&rkp, &local_length);
	roccat_swarm_rmp_write_unknown_with_count(&rkp, &local_length, 5, 4);
	roccat_swarm_rmp_write_unknown(&rkp, &local_length, 0); // pictures
	write_talk(profile_data, &rkp, &local_length);
	write_keys_primary(profile_data, &rkp, &local_length);
	write_keys_function(profile_data, &rkp, &local_length);
	write_keys_macro(profile_data, &rkp, &local_length);
	write_keys_thumbster(profile_data, &rkp, &local_length);
	write_keys_extra(profile_data, &rkp, &local_length);
	write_keys_easyzone(profile_data, &rkp, &local_length);
	write_key_mask(profile_data, &rkp, &local_length);
	write_macros(profile_data, &rkp, &local_length);
	roccat_swarm_rmp_write_unknown(&rkp, &local_length, 0);
	write_light(profile_data, &rkp, &local_length);
	write_stored_lights(profile_data, &rkp, &local_length);
	roccat_swarm_rmp_write_unknown_with_count(&rkp, &local_length, 120, 534);
	roccat_swarm_rmp_write_unknown(&rkp, &local_length, 508); // color presets

	*length = local_length;
	return rkp;
}
