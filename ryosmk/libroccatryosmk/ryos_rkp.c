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

#include "ryos_rkp.h"
#include "ryos_default_rkp.h"
#include "ryos_rkp_accessors.h"
#include "roccat_helper.h"
#include "roccat_notificator.h"
#include "roccat.h"
#include "i18n-lib.h"
#include <string.h>

static gdouble ryos_rkp_volume_to_db(guint8 rkp_volume) {
	return (double)(-30 + 10 * rkp_volume);
}

static guint8 ryos_db_to_rkp_volume(gdouble db) {
	return MAX(MIN((int)db / 10 + 3, RYOS_RKP_SOUND_FEEDBACK_VOLUME_MAX), RYOS_RKP_SOUND_FEEDBACK_VOLUME_MIN);
}

RyosRkp *ryos_rkp_read_with_path(gchar const *path, GError **error) {
	RyosRkp *rkp;
	gsize length;
	gboolean result;

	result = g_file_get_contents(path, (gchar **)&rkp, &length, error);

	if (!result)
		return NULL;

	if (length != sizeof(RyosRkp)) {
		g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_INVAL, _("Data has wrong length, %zu instead of %zu"),
				(gsize)length, (gsize)sizeof(RyosRkp));
		g_free(rkp);
		return NULL;
	}

	return rkp;
}

gboolean ryos_rkp_write_with_path(gchar const *path, RyosRkp *rkp, GError **error) {
	return roccat_profile_write_with_path(path, (gchar const *)rkp, sizeof(RyosRkp), error);
}

RyosRkp *ryos_profile_data_to_rkp(RyosProfileData const *profile_data) {
	RyosRkp *rkp;
	guint i;

	rkp = (RyosRkp *)g_malloc0(sizeof(RyosRkp));

	rkp->keys_primary = profile_data->hardware.keys_primary;
	rkp->keys_macro = profile_data->hardware.keys_macro;
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
		ryos_rkp_set_gamefile_name(rkp, i, profile_data->eventhandler.gamefile_names[i]);

	ryos_rkp_set_profile_name(rkp, profile_data->eventhandler.profile_name);

	roccat_set_bit8(&rkp->sound_feedback_mask, RYOS_RKP_SOUND_FEEDBACK_MASK_BIT_MACRO_LIVE_RECORDING, profile_data->eventhandler.live_recording_notification_type != ROCCAT_NOTIFICATION_TYPE_OFF);
	roccat_set_bit8(&rkp->sound_feedback_mask, RYOS_RKP_SOUND_FEEDBACK_MASK_BIT_PROFILE_SWITCH, profile_data->eventhandler.profile_notification_type != ROCCAT_NOTIFICATION_TYPE_OFF);

	rkp->sound_feedback_volume = ryos_db_to_rkp_volume(profile_data->eventhandler.notification_volume);
	rkp->led_volume_indicator = profile_data->eventhandler.led_volume_indicator ? RYOS_RKP_DATA_LED_VOLUME_INDICATOR_ON : RYOS_RKP_DATA_LED_VOLUME_INDICATOR_OFF;

	rkp->lights_automatic = profile_data->hardware.stored_lights_automatic;
	rkp->lights_manual = profile_data->hardware.stored_lights_manual;

	rkp->character_repeat_delay = 2;
	rkp->character_repeat_rate = 31;
	rkp->unknown1 = 2;

	return rkp;
}

RyosProfileData *ryos_rkp_to_profile_data(RyosRkp const *rkp) {
	RyosProfileData *profile_data;
	guint i;
	gchar *string;

	profile_data = (RyosProfileData *)g_malloc0(sizeof(RyosProfileData));

	profile_data->hardware.keys_primary = rkp->keys_primary;
	profile_data->hardware.keys_macro = rkp->keys_macro;
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

	string = ryos_rkp_get_profile_name(rkp);
	ryos_profile_data_eventhandler_set_profile_name(&profile_data->eventhandler, string);
	g_free(string);

	for (i = 0; i < RYOS_RKP_PROFILE_GAMEFILE_NUM; ++i) {
		string = ryos_rkp_get_gamefile_name(rkp, i);
		ryos_profile_data_eventhandler_set_gamefile_name(&profile_data->eventhandler, i, string);
		g_free(string);
	}

	profile_data->eventhandler.live_recording_notification_type = roccat_get_bit8(rkp->sound_feedback_mask, RYOS_RKP_SOUND_FEEDBACK_MASK_BIT_MACRO_LIVE_RECORDING) ?
			ROCCAT_NOTIFICATION_TYPE_AUDIO : ROCCAT_NOTIFICATION_TYPE_OFF;
	profile_data->eventhandler.profile_notification_type = roccat_get_bit8(rkp->sound_feedback_mask, RYOS_RKP_SOUND_FEEDBACK_MASK_BIT_PROFILE_SWITCH) ?
			ROCCAT_NOTIFICATION_TYPE_AUDIO : ROCCAT_NOTIFICATION_TYPE_OFF;

	profile_data->eventhandler.notification_volume = ryos_rkp_volume_to_db(rkp->sound_feedback_volume);
	profile_data->eventhandler.led_volume_indicator = rkp->led_volume_indicator == RYOS_RKP_DATA_LED_VOLUME_INDICATOR_ON ? TRUE : FALSE;

	profile_data->hardware.stored_lights_automatic = rkp->lights_automatic;
	profile_data->hardware.stored_lights_manual = rkp->lights_manual;

	return profile_data;
}
