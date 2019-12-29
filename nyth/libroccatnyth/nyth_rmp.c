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

#include "nyth_rmp.h"
#include "nyth_device.h"
#include "roccat_swarm_rmp.h"
#include "roccat_helper.h"
#include "g_roccat_helper.h"

typedef struct _NythRmpMacro NythRmpMacro;

struct _NythRmpMacro {
	guint8 report_id;
	guint16 size;
	NythMacro macro;
	guint16 checksum;
} __attribute__ ((packed));

static void nyth_rmp_macro_set_size(NythRmpMacro *macro, guint16 value) {
	macro->size = GUINT16_TO_LE(value);
}

static gboolean read_header(NythProfileData *profile_data, gconstpointer *data) {
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
	nyth_profile_data_eventhandler_set_profile_name(&profile_data->eventhandler, string);
	g_free(string);
	
	*data += size;
	return TRUE;
}

static gboolean read_gamefiles(NythProfileData *profile_data, gconstpointer *data) {
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
		nyth_profile_data_eventhandler_set_gamefile_name(&profile_data->eventhandler, i, gamefile);
		g_free(gamefile);
	}

	return TRUE;
}

static gboolean read_sound_feedback(NythProfileData *profile_data, gconstpointer *data) {
	guint32 size;
	guint16 __attribute__((__unused__)) sound_feedback;

	size = roccat_swarm_rmp_read_bigendian32(data);
	if (size != 2) {
		g_warning("%u != sizeof(sound_feedback)", size);
		return FALSE;
	}

	sound_feedback = *(guint16 const *)data;
	// TODO unused

	*data += size;
	return TRUE;
}

static gboolean read_openers(NythProfileData *profile_data, gconstpointer *data) {
	guint32 count;
	guint32 button_index;
	guint i;
	gchar *string;

	count = roccat_swarm_rmp_read_bigendian32(data);
	if (count > NYTH_PROFILE_BUTTON_NUM) {
		g_warning("%u > NYTH_PROFILE_BUTTON_NUM", count);
		return FALSE;
	}

	for (i = 0; i < count; ++i) {
		button_index = roccat_swarm_rmp_read_bigendian32(data);

		string = roccat_swarm_rmp_read_bigendian_string_with_size(data);
		nyth_profile_data_eventhandler_set_opener(&profile_data->eventhandler, button_index, string);
		g_free(string);
	}
	return TRUE;
}

static gboolean read_profile_buttons(NythProfileData *profile_data, gconstpointer *data) {
	guint32 size;

	size = roccat_swarm_rmp_read_bigendian32(data);
	if (size != sizeof(NythProfileButtons)) {
		g_warning("%u != sizeof(NythProfileButtons)", size);
		return FALSE;
	}

	nyth_profile_data_hardware_set_profile_buttons(&profile_data->hardware, (NythProfileButtons const *)*data);

	*data += size;
	return TRUE;
}

static gboolean read_profile_settings(NythProfileData *profile_data, gconstpointer *data) {
	guint32 size;

	size = roccat_swarm_rmp_read_bigendian32(data);
	if (size != sizeof(NythProfileSettings)) {
		g_warning("%u != sizeof(NythProfileSettings)", size);
		return FALSE;
	}

	nyth_profile_data_hardware_set_profile_settings(&profile_data->hardware, (NythProfileSettings const *)*data);

	*data += size;
	return TRUE;
}

static gboolean read_button_set(NythProfileData *profile_data, gconstpointer *data) {
	guint32 size;

	size = roccat_swarm_rmp_read_bigendian32(data);
	if (size != sizeof(NythButtonSet)) {
		g_warning("%u != sizeof(NythButtonSet)", size);
		return FALSE;
	}

	nyth_profile_data_hardware_set_button_set(&profile_data->hardware, (NythButtonSet const *)*data);

	*data += size;
	return TRUE;
}

static gboolean read_macros(NythProfileData *profile_data, gconstpointer *data) {
	NythRmpMacro const *rmp_macro;
	guint32 count;
	guint32 size;
	guint i;

	count = roccat_swarm_rmp_read_bigendian32(data);
	if (count != NYTH_PROFILE_BUTTON_NUM) {
		g_warning("%u != NYTH_PROFILE_BUTTON_NUM", count);
		return FALSE;
	}
	
	for (i = 0; i < count; ++i) {
		size = roccat_swarm_rmp_read_bigendian32(data);
		if (size != sizeof(NythRmpMacro)) {
			g_warning("%u != sizeof(NythRmpMacro)", size);
			return FALSE;
		}
		
		rmp_macro = (NythRmpMacro const *)*data;
		nyth_profile_data_hardware_set_macro(&profile_data->hardware, i, &rmp_macro->macro);
		*data += size;
	}

	return TRUE;
}

NythProfileData *nyth_rmp_to_profile_data(gconstpointer rmp, gsize length) {
	NythProfileData *profile_data;
	gconstpointer pointer;
	
	profile_data = nyth_profile_data_new();
	pointer = rmp;
	
	if (!roccat_swarm_rmp_read_unknown(&pointer)) goto error;
	if (!read_header(profile_data, &pointer)) goto error;
	if (!read_gamefiles(profile_data, &pointer)) goto error;
	if (!read_sound_feedback(profile_data, &pointer)) goto error;
	if (!roccat_swarm_rmp_read_timers(&pointer)) goto error;
	if (!roccat_swarm_rmp_read_unknown_with_count(&pointer)) goto error;
	if (!roccat_swarm_rmp_read_pictures(&pointer)) goto error;
	if (!read_openers(profile_data, &pointer)) goto error;
	if (!roccat_swarm_rmp_read_unknown(&pointer)) goto error;
	if (!roccat_swarm_rmp_read_unknown(&pointer)) goto error;
	if (!read_profile_buttons(profile_data, &pointer)) goto error;
	if (!read_profile_settings(profile_data, &pointer)) goto error;
	if (!read_button_set(profile_data, &pointer)) goto error;
	if (!roccat_swarm_rmp_read_unknown(&pointer)) goto error;
	if (!read_macros(profile_data, &pointer)) goto error;

	if (pointer > rmp + length)
		g_warning("%zu > %zu", (gsize)(pointer - rmp), length);

	return profile_data;
error:
	g_free(profile_data);
	return NULL;
}

static void write_header(NythProfileData const *profile_data, gpointer *data, gsize *length) {
	RoccatSwarmRmpHeader *header;
	
	header = (RoccatSwarmRmpHeader *)roccat_swarm_rmp_write_with_size(data, length, sizeof(RoccatSwarmRmpHeader));
	roccat_swarm_rmp_header_set_profile_name(header, profile_data->eventhandler.profile_name);
	/* default values */
	header->volume = 50;
	header->profile_autoswitch = 0;

}

static void write_profile_settings(NythProfileData const *profile_data, gpointer *data, gsize *length) {
	NythProfileSettings *settings;
	
	settings = (NythProfileSettings *)roccat_swarm_rmp_write_with_size(data, length, sizeof(NythProfileSettings));
	nyth_profile_settings_copy(settings, &profile_data->hardware.profile_settings);
}

static void write_profile_buttons(NythProfileData const *profile_data, gpointer *data, gsize *length) {
	NythProfileButtons *buttons;
	
	buttons = (NythProfileButtons *)roccat_swarm_rmp_write_with_size(data, length, sizeof(NythProfileButtons));
	nyth_profile_buttons_copy(buttons, &profile_data->hardware.profile_buttons);
}

static void write_button_set(NythProfileData const *profile_data, gpointer *data, gsize *length) {
	NythButtonSet *button_set;
	
	button_set = (NythButtonSet *)roccat_swarm_rmp_write_with_size(data, length, sizeof(NythButtonSet));
	nyth_button_set_copy(button_set, &profile_data->hardware.button_set);
}

static void write_sound_feedback(NythProfileData const *profile_data, gpointer *data, gsize *length) {
	guint16 *sound_feedback;

	sound_feedback = (guint16 *)roccat_swarm_rmp_write_with_size(data, length, sizeof(guint16));
	*sound_feedback = 0; /* default value */
}

static void write_gamefiles(NythProfileData const *profile_data, gpointer *data, gsize *length) {
	guint i;
	gsize size;
	
	size = ROCCAT_SWARM_RMP_GAMEFILE_NUM * ROCCAT_SWARM_RMP_GAMEFILE_LENGTH * sizeof(gunichar2);
	roccat_swarm_rmp_write_bigendian32(data, length, size);
	for (i = 0; i < ROCCAT_SWARM_RMP_GAMEFILE_NUM; ++i)
		roccat_swarm_rmp_write_bigendian_string(data, length, profile_data->eventhandler.gamefile_names[i], ROCCAT_SWARM_RMP_GAMEFILE_LENGTH);
}

static void write_macros(NythProfileData const *profile_data, gpointer *data, gsize *length) {
	NythRmpMacro *rmp_macro;
	guint i;
	
	/* The Windows version doesn't support the compressed format with button_index
	 * for macro section (nor button_index contained in macro structure).
	 * Always all macros are written, regardless of usage which increases the filesize massively.
	 */
	roccat_swarm_rmp_write_bigendian32(data, length, NYTH_PROFILE_BUTTON_NUM);
	for (i = 0; i < NYTH_PROFILE_BUTTON_NUM; ++i) {
		rmp_macro = (NythRmpMacro *)roccat_swarm_rmp_write_with_size(data, length, sizeof(NythRmpMacro));

		/* Windows version writes the wrong report id 0x0e used by Isku* and Ryos* */
		rmp_macro->report_id = NYTH_REPORT_ID_MACRO;

		nyth_rmp_macro_set_size(rmp_macro, sizeof(NythRmpMacro));
		memcpy(&rmp_macro->macro, &profile_data->hardware.macros[i], sizeof(NythMacro));
	}
}

static void write_openers(NythProfileData const *profile_data, gpointer *data, gsize *length) {
	guint type;
	guint count;
	guint i;
	
	count = 0;
	for (i = 0; i < NYTH_PROFILE_BUTTON_NUM; ++i) {
		type = profile_data->hardware.profile_buttons.buttons[i].type;
		if (type == NYTH_BUTTON_TYPE_OPEN_APPLICATION || type == NYTH_BUTTON_TYPE_OPEN_DOCUMENT || type == NYTH_BUTTON_TYPE_OPEN_FOLDER || type == NYTH_BUTTON_TYPE_OPEN_WEBSITE) {
			++count;
		}
	}
	roccat_swarm_rmp_write_bigendian32(data, length, count);

	for (i = 0; i < NYTH_PROFILE_BUTTON_NUM; ++i) {
		type = profile_data->hardware.profile_buttons.buttons[i].type;
		if (type == NYTH_BUTTON_TYPE_OPEN_APPLICATION || type == NYTH_BUTTON_TYPE_OPEN_DOCUMENT || type == NYTH_BUTTON_TYPE_OPEN_FOLDER || type == NYTH_BUTTON_TYPE_OPEN_WEBSITE) {
			roccat_swarm_rmp_write_bigendian32(data, length, i);
			roccat_swarm_rmp_write_bigendian_string_with_size(data, length, (gchar const *)profile_data->eventhandler.openers[i], ROCCAT_SWARM_RMP_OPENER_LENGTH);
		}
	}
}

gpointer nyth_profile_data_to_rmp(NythProfileData const *profile_data, gsize *length) {
	gpointer rmp = NULL;
	gsize local_length = 0;
	
	roccat_swarm_rmp_write_unknown(&rmp, &local_length, 0); // FIXME 4
	write_header(profile_data, &rmp, &local_length);
	write_gamefiles(profile_data, &rmp, &local_length);
	write_sound_feedback(profile_data, &rmp, &local_length);
	roccat_swarm_rmp_write_timers(&rmp, &local_length);
	roccat_swarm_rmp_write_unknown_with_count(&rmp, &local_length, 0, 0); // FIXME 5 x 4
	roccat_swarm_rmp_write_unknown(&rmp, &local_length, 0); // FIXME pictures
	write_openers(profile_data, &rmp, &local_length);
	roccat_swarm_rmp_write_unknown(&rmp, &local_length, 0);
	roccat_swarm_rmp_write_unknown(&rmp, &local_length, 0); // FIXME 4
	write_profile_buttons(profile_data, &rmp, &local_length);
	write_profile_settings(profile_data, &rmp, &local_length);
	write_button_set(profile_data, &rmp, &local_length);
	roccat_swarm_rmp_write_unknown(&rmp, &local_length, 0x30);
	write_macros(profile_data, &rmp, &local_length);
	
	*length = local_length;
	return rmp;
}
