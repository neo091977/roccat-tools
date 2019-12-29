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

#include "ryostkl_rkp.h"
#include "ryos_rkp.h"
#include "roccat_helper.h"
#include "roccat_notificator.h"
#include "i18n-lib.h"
#include <string.h>

static gdouble ryostkl_rkp_volume_to_db(guint8 rkp_volume) {
	return (double)(-30 + 10 * rkp_volume);
}

static guint8 ryostkl_db_to_rkp_volume(gdouble db) {
	return MAX(MIN((int)db / 10 + 3, RYOS_RKP_SOUND_FEEDBACK_VOLUME_MAX), RYOS_RKP_SOUND_FEEDBACK_VOLUME_MIN);
}

static gchar *ryostkl_rkp_get_profile_name(RyostklRkp const *rkp) {
	return g_utf16_to_utf8((gunichar2 const *)(rkp->profile_name), RYOS_RKP_PROFILE_NAME_LENGTH, NULL, NULL, NULL);
}

static void ryostkl_rkp_set_profile_name(RyostklRkp *rkp, gchar const *name) {
	gunichar2 *string;
	glong items;

	string = g_utf8_to_utf16(name, -1L, NULL, &items, NULL);
	if (string == NULL)
		return;

	/* make sure target has trailing 0 */
	items = MIN(RYOS_RKP_PROFILE_NAME_LENGTH - 1, items);
	string[items] = 0;

	memset(&rkp->profile_name, 0, RYOS_RKP_PROFILE_NAME_LENGTH * 2);
	memcpy(&rkp->profile_name, string, items * 2);

	g_free(string);
}

static gchar *ryostkl_rkp_get_gamefile_name(RyostklRkp const *rkp, guint index) {
	return g_utf16_to_utf8((gunichar2 const *)(rkp->gamefile_names[index]), RYOS_RKP_PROFILE_GAMEFILE_LENGTH, NULL, NULL, NULL);
}

static void ryostkl_rkp_set_gamefile_name(RyostklRkp *rkp, guint index, gchar const *name) {
	gunichar2 *string;
	glong items;

	string = g_utf8_to_utf16(name, -1L, NULL, &items, NULL);
	if (string == NULL)
		return;

	items = MIN(RYOS_RKP_PROFILE_GAMEFILE_LENGTH - 1, items);
	string[items] = 0;

	memset(&rkp->gamefile_names[index], 0, RYOS_RKP_PROFILE_GAMEFILE_LENGTH * 2);
	memcpy(&rkp->gamefile_names[index], string, items * 2);

	g_free(string);
}

RyostklRkp *ryostkl_profile_data_to_rkp(RyostklProfileData const *profile_data) {
	RyostklRkp *rkp;
	guint i;
	
	rkp = (RyostklRkp *)g_malloc0(sizeof(RyostklRkp));
	
	rkp->keys_primary = profile_data->hardware.keys_primary;
	rkp->keys_function = profile_data->hardware.keys_function;
	rkp->keys_thumbster = profile_data->hardware.keys_thumbster;
	rkp->keys_extra = profile_data->hardware.keys_extra;
	rkp->keys_easyzone = profile_data->hardware.keys_easyzone;
	rkp->key_mask = profile_data->hardware.key_mask;
	rkp->light = profile_data->hardware.light;
	
	for (i = 0; i < RYOS_RKP_KEYS_NUM; ++i) {
		rkp->macros[i] = profile_data->hardware.macros[i];
		rkp->timers[i] = profile_data->eventhandler.timers[i];
		rkp->launchers[i] = profile_data->eventhandler.launchers[i];
		rkp->led_macros[i] = profile_data->eventhandler.led_macros[i];
	}
	
	for (i = 0; i < RYOS_RKP_TALK_NUM; ++i)
		rkp->talks[i] = profile_data->eventhandler.talks[i];

	for (i = 0; i < RYOS_RKP_PROFILE_GAMEFILE_NUM; ++i)
		ryostkl_rkp_set_gamefile_name(rkp, i, profile_data->eventhandler.gamefile_names[i]);
	
	ryostkl_rkp_set_profile_name(rkp, profile_data->eventhandler.profile_name);

	roccat_set_bit8(&rkp->sound_feedback_mask, RYOS_RKP_SOUND_FEEDBACK_MASK_BIT_MACRO_LIVE_RECORDING, profile_data->eventhandler.live_recording_notification_type != ROCCAT_NOTIFICATION_TYPE_OFF);
	roccat_set_bit8(&rkp->sound_feedback_mask, RYOS_RKP_SOUND_FEEDBACK_MASK_BIT_PROFILE_SWITCH, profile_data->eventhandler.profile_notification_type != ROCCAT_NOTIFICATION_TYPE_OFF);

	rkp->sound_feedback_volume = ryostkl_db_to_rkp_volume(profile_data->eventhandler.notification_volume);
	rkp->led_volume_indicator = profile_data->eventhandler.led_volume_indicator ? RYOS_RKP_DATA_LED_VOLUME_INDICATOR_ON : RYOS_RKP_DATA_LED_VOLUME_INDICATOR_OFF;

	rkp->light_layer_automatic_0 = profile_data->hardware.light_layer_automatic[0];
	rkp->light_layer_manual_0 = profile_data->hardware.light_layer_manual[0];
	
	// TODO rkp->_14

	rkp->character_repeat_delay = 2;
	rkp->character_repeat_rate = 31;
	rkp->unknown1 = 2;

	return rkp;
}

RyostklProfileData *ryostkl_rkp_to_profile_data(RyostklRkp const *rkp) {
	RyostklProfileData *profile_data;
	guint i;
	gchar *string;
	
	profile_data = (RyostklProfileData *)g_malloc0(sizeof(RyostklProfileData));
	
	profile_data->hardware.keys_primary = rkp->keys_primary;
	profile_data->hardware.keys_function = rkp->keys_function;
	profile_data->hardware.keys_thumbster = rkp->keys_thumbster;
	profile_data->hardware.keys_extra = rkp->keys_extra;
	profile_data->hardware.keys_easyzone = rkp->keys_easyzone;
	profile_data->hardware.key_mask = rkp->key_mask;
	profile_data->hardware.light = rkp->light;
	
	for (i = 0; i < RYOS_RKP_KEYS_NUM; ++i) {
		profile_data->hardware.macros[i] = rkp->macros[i];
		profile_data->eventhandler.timers[i] = rkp->timers[i];
		profile_data->eventhandler.launchers[i] = rkp->launchers[i];
		profile_data->eventhandler.led_macros[i] = rkp->led_macros[i];
	}

	for (i = 0; i < RYOS_RKP_TALK_NUM; ++i)
		profile_data->eventhandler.talks[i] = rkp->talks[i];

	string = ryostkl_rkp_get_profile_name(rkp);
	ryostkl_profile_data_eventhandler_set_profile_name(&profile_data->eventhandler, string);
	g_free(string);
	
	for (i = 0; i < RYOS_RKP_PROFILE_GAMEFILE_NUM; ++i) {
		string = ryostkl_rkp_get_gamefile_name(rkp, i);
		ryostkl_profile_data_eventhandler_set_gamefile_name(&profile_data->eventhandler, i, string);
		g_free(string);
	}

	profile_data->eventhandler.live_recording_notification_type = roccat_get_bit8(rkp->sound_feedback_mask, RYOS_RKP_SOUND_FEEDBACK_MASK_BIT_MACRO_LIVE_RECORDING) ?
			ROCCAT_NOTIFICATION_TYPE_AUDIO : ROCCAT_NOTIFICATION_TYPE_OFF;
	profile_data->eventhandler.profile_notification_type = roccat_get_bit8(rkp->sound_feedback_mask, RYOS_RKP_SOUND_FEEDBACK_MASK_BIT_PROFILE_SWITCH) ?
			ROCCAT_NOTIFICATION_TYPE_AUDIO : ROCCAT_NOTIFICATION_TYPE_OFF;

	profile_data->eventhandler.notification_volume = ryostkl_rkp_volume_to_db(rkp->sound_feedback_volume);
	profile_data->eventhandler.led_volume_indicator = rkp->led_volume_indicator == RYOS_RKP_DATA_LED_VOLUME_INDICATOR_ON ? TRUE : FALSE;
	
	profile_data->hardware.light_layer_automatic[0] = rkp->light_layer_automatic_0;
	profile_data->hardware.light_layer_manual[0] = rkp->light_layer_manual_0;
	
	return profile_data;
}

RyostklRkp *ryostkl_rkp_read_with_path(gchar const *path, GError **error) {
	RyostklRkp *rkp;
	gsize length;
	gboolean result;

	result = g_file_get_contents(path, (gchar **)&rkp, &length, error);

	if (!result)
		return NULL;

	if (length != sizeof(RyostklRkp)) {
		g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_INVAL, _("Data has wrong length, %zu instead of %zu"),
				(gsize)length, (gsize)sizeof(RyostklRkp));
		g_free(rkp);
		return NULL;
	}

	return rkp;
}

gboolean ryostkl_rkp_write_with_path(gchar const *path, RyostklRkp const *rkp, GError **error) {
	return roccat_profile_write_with_path(path, (gchar const *)rkp, sizeof(RyostklRkp), error);
}
