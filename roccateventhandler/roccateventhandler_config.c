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

#include "roccateventhandler_config.h"
#include "roccat_eventhandler_host.h"
#include <string.h>

static gchar const * const group_name = "Eventhandler";
static gchar const * const scan_interval_key = "ScanInterval";
static gchar const * const start_quiet_key = "StartQuiet";
static gchar const * const sound_driver_key = "SoundDriver";
static gchar const * const driver_state_key = "DriverState";

gint roccateventhandler_configuration_get_scan_interval(RoccatKeyFile *config) {
	return roccat_key_file_get_integer_with_default(config, group_name, scan_interval_key, 2);
}

gboolean roccateventhandler_configuration_get_start_quiet(RoccatKeyFile *config) {
	return roccat_key_file_get_boolean_with_default(config, group_name, start_quiet_key, FALSE);
}

gchar *roccateventhandler_configuration_get_sound_driver(RoccatKeyFile *config) {
	return roccat_key_file_get_string_with_default(config, group_name, sound_driver_key, "pulse");
}

guint roccateventhandler_configuration_get_driver_state(RoccatKeyFile *config) {
	gchar *string = roccat_key_file_get_string_with_default(config, group_name, driver_state_key, "unused");
	guint retval;

	if (!strcmp(string, "on"))
		retval = ROCCATEVENTHANDLER_CONFIGURATION_DRIVER_STATE_ON;
	else if (!strcmp(string, "off"))
		retval = ROCCATEVENTHANDLER_CONFIGURATION_DRIVER_STATE_OFF;
	else
		retval = ROCCATEVENTHANDLER_CONFIGURATION_DRIVER_STATE_UNUSED;

	g_free(string);
	return retval;
}
