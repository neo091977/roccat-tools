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

#include "luaconfig_configuration_dialog.h"
#include "lua.h"
#include "roccat_warning_dialog.h"
#include "i18n.h"

gboolean luaconfig_configuration_save(GtkWindow *parent, RoccatKeyFile *config) {
	GError *error = NULL;

	lua_configuration_save(config, &error);
	if (error) {
		roccat_warning_dialog(GTK_WINDOW(parent), _("Could not save configuration"), error->message);
		g_clear_error(&error);
		return FALSE;
	}

	return TRUE;
}
