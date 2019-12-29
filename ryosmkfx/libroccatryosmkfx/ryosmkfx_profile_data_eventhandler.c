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
#include "ryosmkfx_profile_data_eventhandler.h"
#include "config.h"
#include "i18n-lib.h"

static gchar *ryosmkfx_profile_dir(void) {
	gchar *base = roccat_profile_dir();
	gchar *dir = g_build_path("/", base, "ryosmkfx", NULL);
	g_free(base);
	return dir;
}

static gboolean ryosmkfx_profile_dir_create_if_needed(GError **error) {
	gchar *dir;
	gboolean retval;

	dir = ryosmkfx_profile_dir();
	retval = roccat_profile_dir_create_if_needed(dir, error);
	g_free(dir);
	return retval;
}

static gchar *ryosmkfx_build_profile_path(guint profile_index) {
	gchar *dir = ryosmkfx_profile_dir();
	gchar *filename = g_strdup_printf("actual%i", profile_index);
	gchar *path = g_build_path("/", dir, filename, NULL);
	g_free(filename);
	g_free(dir);
	return path;
}

void ryosmkfx_profile_data_eventhandler_set_profile_name(RyosmkfxProfileDataEventhandler *profile_data, gchar const *new_name) {
	if (strncmp(profile_data->profile_name, new_name, ROCCAT_SWARM_RMP_PROFILE_NAME_LENGTH)) {
		g_strlcpy(profile_data->profile_name, new_name, ROCCAT_SWARM_RMP_PROFILE_NAME_LENGTH + 1);
		profile_data->modified = TRUE;
	}
}

void ryosmkfx_profile_data_eventhandler_set_effect_script_name(RyosmkfxProfileDataEventhandler *profile_data, gchar const *new_name) {
	if (!new_name)
		return;

	if (!strncmp(profile_data->effect_script, new_name, RYOS_EFFECT_SCRIPT_NAME_LENGTH - 1))
		return;

	g_strlcpy(profile_data->effect_script, new_name, RYOS_EFFECT_SCRIPT_NAME_LENGTH);
	profile_data->modified = TRUE;
}

void ryosmkfx_profile_data_eventhandler_set_gamefile_name(RyosmkfxProfileDataEventhandler *profile_data, guint index, gchar const *new_name) {
	if (strncmp(profile_data->gamefile_names[index], new_name, ROCCAT_SWARM_RMP_GAMEFILE_LENGTH)) {
		g_strlcpy(profile_data->gamefile_names[index], new_name, ROCCAT_SWARM_RMP_GAMEFILE_LENGTH);
		profile_data->modified = TRUE;
	}
}

void ryosmkfx_profile_data_eventhandler_set_timer(RyosmkfxProfileDataEventhandler *profile_data, guint index, RoccatTimer const *timer) {
	if (!roccat_timer_equal(&profile_data->timers[index], timer)) {
		roccat_timer_copy(&profile_data->timers[index], timer);
		profile_data->modified = TRUE;
	}
}

void ryosmkfx_profile_data_eventhandler_set_opener(RyosmkfxProfileDataEventhandler *profile_data, guint index, gchar const *new_opener) {
	if (strncmp(profile_data->openers[index], new_opener, ROCCAT_SWARM_RMP_OPENER_LENGTH)) {
		g_strlcpy(profile_data->openers[index], new_opener, ROCCAT_SWARM_RMP_OPENER_LENGTH);
		profile_data->modified = TRUE;
	}
}

void ryosmkfx_profile_data_eventhandler_set_talk_target(RyosmkfxProfileDataEventhandler *profile_data, guint index, guint16 new_target) {
	if (profile_data->talk_targets[index] != new_target) {
		profile_data->talk_targets[index] = new_target;
		profile_data->modified = TRUE;
	}
}

void ryosmkfx_profile_data_eventhandler_set_led_macro(RyosmkfxProfileDataEventhandler *profile_data, guint index, RyosLedMacro *led_macro) {
	if (!ryos_led_macro_equal(&profile_data->led_macros[index], led_macro)) {
		ryos_led_macro_copy(&profile_data->led_macros[index], led_macro);
		profile_data->modified = TRUE;
	}
}

static RyosmkfxProfileDataEventhandler *ryosmkfx_profile_data_eventhandler_read_with_path(gchar const *path, GError **error) {
	RyosmkfxProfileDataEventhandler *profile_data;
	gsize length;
	gboolean result;

	result = g_file_get_contents(path, (gchar **)&profile_data, &length, error);

	if (!result)
		return NULL;

	if (length != sizeof(RyosmkfxProfileDataEventhandler)) {
		g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_INVAL, _("Eventhandler datafile has wrong size: %zu instead of %zu"),
				(gsize)length, (gsize)sizeof(RyosmkfxProfileDataEventhandler));
		g_free(profile_data);
		return NULL;
	}

	return profile_data;
}

static gboolean ryosmkfx_profile_data_eventhandler_write_with_path(gchar const *path, RyosmkfxProfileDataEventhandler const *profile_data, GError **error) {
	return roccat_profile_write_with_path(path, (gchar const *)profile_data, sizeof(RyosmkfxProfileDataEventhandler), error);
}

gboolean ryosmkfx_profile_data_eventhandler_save(RyosmkfxProfileDataEventhandler *profile_data, guint profile_index, GError **error) {
	gchar *config_path;
	gboolean retval;

	if (!ryosmkfx_profile_dir_create_if_needed(error))
		return FALSE;

	config_path = ryosmkfx_build_profile_path(profile_index);
	retval = ryosmkfx_profile_data_eventhandler_write_with_path(config_path, profile_data, error);
	if (retval)
		ryosmkfx_profile_data_eventhandler_set_unmodified(profile_data);
	g_free(config_path);
	return retval;
}

void ryosmkfx_profile_data_eventhandler_update_with_default(RyosmkfxProfileDataEventhandler *profile_data) {
	gchar *script;

	profile_data->timer_notification_type = ROCCAT_NOTIFICATION_TYPE_OSD;
	profile_data->profile_notification_type = ROCCAT_NOTIFICATION_TYPE_OSD;
	profile_data->live_recording_notification_type = ROCCAT_NOTIFICATION_TYPE_OSD;
	profile_data->notification_volume = 0.0;

	script = g_build_path("/", RYOS_EFFECT_MODULES_PATH, "ripple.lua", NULL);
	ryosmkfx_profile_data_eventhandler_set_effect_script_name(profile_data, script);
	g_free(script);

	ryosmkfx_profile_data_eventhandler_set_modified(profile_data);
}

gboolean ryosmkfx_profile_data_eventhandler_update(RyosmkfxProfileDataEventhandler *profile_data, guint profile_index, GError **error) {
	RyosmkfxProfileDataEventhandler *temp;
	gchar *config_path;

	config_path = ryosmkfx_build_profile_path(profile_index);
	temp = ryosmkfx_profile_data_eventhandler_read_with_path(config_path, error);
	g_free(config_path);

	if (!temp)
		return FALSE;

	memcpy(profile_data, temp, sizeof(RyosmkfxProfileDataEventhandler));
	g_free(temp);

	ryosmkfx_profile_data_eventhandler_set_unmodified(profile_data);

	return TRUE;
}

void ryosmkfx_profile_data_eventhandler_set_modified(RyosmkfxProfileDataEventhandler *profile_data) {
	profile_data->modified = TRUE;
}

void ryosmkfx_profile_data_eventhandler_set_unmodified(RyosmkfxProfileDataEventhandler *profile_data) {
	profile_data->modified = FALSE;
}

gboolean ryosmkfx_profile_data_eventhandler_get_modified(RyosmkfxProfileDataEventhandler const *profile_data) {
	return profile_data->modified;
}
