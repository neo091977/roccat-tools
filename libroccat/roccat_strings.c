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

#include "roccat_strings.h"
#include "roccat_firmware.h"
#include "i18n-lib.h"

gchar const *roccat_string_gamefile_frame_tooltip(void) {
	return _("Select an executable or give a window name.\n"
			"Accepts Pearl compatible regular expressions.");
}

gchar const *roccat_string_default_profile_number_tooltip(void) {
	return _("Number of profile that gets activated when no gamefile setting is applied.");
}

gchar const *roccat_string_firmware_liability(void) {
	return _("Please keep in mind that you might loose your warranty and neither I nor Roccat\n"
			"take liability if you brick your device by flashing it using my firmware or\n"
			"SROM updater.");
}

gchar *roccat_string_old_firmware_version(guint has, guint needs) {
	gchar *has_string = roccat_firmware_version_to_string(has);
	gchar *needs_string = roccat_firmware_version_to_string(needs);
	gchar *string;

	string = g_strdup_printf(_("Some functionality might not work with your actual version %s, please update to at least %s."), has_string, needs_string);

	g_free(has_string);
	g_free(needs_string);

	return string;
}
