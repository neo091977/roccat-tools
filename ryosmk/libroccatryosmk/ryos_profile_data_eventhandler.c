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

#include "roccat_helper.h"
#include "roccat_notificator.h"
#include "ryos_profile_data_eventhandler.h"
#include "config.h"
#include "i18n-lib.h"

static gchar *ryos_profile_dir(void) {
	gchar *base = roccat_profile_dir();
	gchar *dir = g_build_path("/", base, "ryosmk", NULL);
	g_free(base);
	return dir;
}

static gboolean ryos_profile_dir_create_if_needed(GError **error) {
	gchar *dir;
	gboolean retval;

	dir = ryos_profile_dir();
	retval = roccat_profile_dir_create_if_needed(dir, error);
	g_free(dir);
	return retval;
}

static gchar *ryos_build_profile_path(guint profile_index) {
	gchar *dir = ryos_profile_dir();
	gchar *filename = g_strdup_printf("actual%i", profile_index);
	gchar *path = g_build_path("/", dir, filename, NULL);
	g_free(filename);
	g_free(dir);
	return path;
}

void ryos_profile_data_eventhandler_set_profile_name(RyosProfileDataEventhandler *profile_data, gchar const *new_name) {
	if (strncmp(profile_data->profile_name, new_name, ROCCAT_SWARM_RMP_PROFILE_NAME_LENGTH)) {
		g_strlcpy(profile_data->profile_name, new_name, ROCCAT_SWARM_RMP_PROFILE_NAME_LENGTH + 1);
		profile_data->modified = TRUE;
	}
}

void ryos_profile_data_eventhandler_set_effect_script_name(RyosProfileDataEventhandler *profile_data, gchar const *new_name) {
	if (!new_name)
		return;

	if (!strncmp(profile_data->effect_script, new_name, RYOS_EFFECT_SCRIPT_NAME_LENGTH - 1))
		return;

	g_strlcpy(profile_data->effect_script, new_name, RYOS_EFFECT_SCRIPT_NAME_LENGTH);
	profile_data->modified = TRUE;
}

void ryos_profile_data_eventhandler_set_gamefile_name(RyosProfileDataEventhandler *profile_data, guint index, gchar const *new_name) {
	if (strncmp(profile_data->gamefile_names[index], new_name, ROCCAT_SWARM_RMP_GAMEFILE_LENGTH)) {
		g_strlcpy(profile_data->gamefile_names[index], new_name, ROCCAT_SWARM_RMP_GAMEFILE_LENGTH);
		profile_data->modified = TRUE;
	}
}

void ryos_profile_data_eventhandler_set_timer(RyosProfileDataEventhandler *profile_data, guint index, RyosRkpTimer const *timer) {
	if (!ryos_rkp_timer_equal(&profile_data->timers[index], timer)) {
		ryos_rkp_timer_copy(&profile_data->timers[index], timer);
		profile_data->modified = TRUE;
	}
}

void ryos_profile_data_eventhandler_set_quicklaunch(RyosProfileDataEventhandler *profile_data, guint index, RyosRkpQuicklaunch const *launcher) {
	if (!ryos_rkp_quicklaunch_equal(&profile_data->launchers[index], launcher)) {
		ryos_rkp_quicklaunch_copy(&profile_data->launchers[index], launcher);
		profile_data->modified = TRUE;
	}
}

void ryos_profile_data_eventhandler_set_talk(RyosProfileDataEventhandler *profile_data, guint index, RyosRkpTalk const *talk) {
	if (!ryos_rkp_talk_equal(&profile_data->talks[index], talk)) {
		ryos_rkp_talk_copy(&profile_data->talks[index], talk);
		profile_data->modified = TRUE;
	}
}

void ryos_profile_data_eventhandler_set_led_macro(RyosProfileDataEventhandler *profile_data, guint index, RyosLedMacro const *led_macro) {
	if (!ryos_led_macro_equal(&profile_data->led_macros[index], led_macro)) {
		ryos_led_macro_copy(&profile_data->led_macros[index], led_macro);
		profile_data->modified = TRUE;
	}
}

static RyosProfileDataEventhandler *ryos_profile_data_eventhandler_read_with_path(gchar const *path, GError **error) {
	RyosProfileDataEventhandler *profile_data;
	gsize length;
	gboolean result;

	result = g_file_get_contents(path, (gchar **)&profile_data, &length, error);

	if (!result)
		return NULL;

	if (length != sizeof(RyosProfileDataEventhandler)) {
		g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_INVAL, _("Eventhandler datafile has wrong size: %zu instead of %zu"),
				(gsize)length, (gsize)sizeof(RyosProfileDataEventhandler));
		g_free(profile_data);
		return NULL;
	}

	return profile_data;
}

static gboolean ryos_profile_data_eventhandler_write_with_path(gchar const *path, RyosProfileDataEventhandler const *profile_data, GError **error) {
	return roccat_profile_write_with_path(path, (gchar const *)profile_data, sizeof(RyosProfileDataEventhandler), error);
}

gboolean ryos_profile_data_eventhandler_save(RyosProfileDataEventhandler *profile_data, guint profile_index, GError **error) {
	gchar *config_path;
	gboolean retval;

	if (!ryos_profile_dir_create_if_needed(error))
		return FALSE;

	config_path = ryos_build_profile_path(profile_index);
	retval = ryos_profile_data_eventhandler_write_with_path(config_path, profile_data, error);
	if (retval)
		ryos_profile_data_eventhandler_set_unmodified(profile_data);
	g_free(config_path);
	return retval;
}

void ryos_profile_data_eventhandler_update_with_default(RyosProfileDataEventhandler *profile_data) {
	gchar *script;

	profile_data->timer_notification_type = ROCCAT_NOTIFICATION_TYPE_OSD;
	profile_data->profile_notification_type = ROCCAT_NOTIFICATION_TYPE_OSD;
	profile_data->live_recording_notification_type = ROCCAT_NOTIFICATION_TYPE_OSD;
	profile_data->notification_volume = 0.0;
	profile_data->led_volume_indicator = FALSE;

	script = g_build_path("/", RYOS_EFFECT_MODULES_PATH, "ripple.lua", NULL);
	ryos_profile_data_eventhandler_set_effect_script_name(profile_data, script);
	g_free(script);

	ryos_profile_data_eventhandler_set_modified(profile_data);
}

gboolean ryos_profile_data_eventhandler_update(RyosProfileDataEventhandler *profile_data, guint profile_index, GError **error) {
	RyosProfileDataEventhandler *temp;
	gchar *config_path;

	config_path = ryos_build_profile_path(profile_index);
	temp = ryos_profile_data_eventhandler_read_with_path(config_path, error);
	g_free(config_path);

	if (!temp)
		return FALSE;

	memcpy(profile_data, temp, sizeof(RyosProfileDataEventhandler));
	g_free(temp);
	ryos_profile_data_eventhandler_set_unmodified(profile_data);

	return TRUE;
}

void ryos_profile_data_eventhandler_set_modified(RyosProfileDataEventhandler *profile_data) {
	profile_data->modified = TRUE;
}

void ryos_profile_data_eventhandler_set_unmodified(RyosProfileDataEventhandler *profile_data) {
	profile_data->modified = FALSE;
}
