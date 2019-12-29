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

#include "lua.h"
#include "roccat_secure.h"
#include "roccat_helper.h"
#include "config.h"
#include "i18n.h"
#include <stdlib.h>

static gboolean parameter_just_print_version = FALSE;
static gchar *parameter_rmp = NULL;

static GOptionEntry entries[] = {
	{ "activate-rmp", 'a', 0, G_OPTION_ARG_FILENAME, &parameter_rmp, N_("activate rmp from FILENAME"), N_("FILENAME") },
	{ "version", 'v', 0, G_OPTION_ARG_NONE, &parameter_just_print_version, N_("print version"), NULL },
	{ NULL }
};

static gboolean post_parse_func(GOptionContext *context, GOptionGroup *group, gpointer data, GError **error) {
	guint just_counter = 0;

	if (parameter_just_print_version) ++just_counter;

	if (just_counter > 1) {
		g_set_error(error, G_OPTION_ERROR, G_OPTION_ERROR_FAILED, _("Just give one of -v"));
		return FALSE;
	}

	return TRUE;
}

static GOptionContext *commandline_parse(int *argc, char ***argv) {
	GOptionContext *context = NULL;
	GError *error = NULL;
	gchar *string;

	string = g_strdup_printf(_("- controls extended capabilities of Roccat %s mice"), LUA_DEVICE_NAME);
	context = g_option_context_new(string);
	g_free(string);

	g_option_context_add_main_entries(context, entries, NULL);
	g_option_context_set_translation_domain(context, PROJECT_NAME);

	g_option_group_set_parse_hooks(g_option_context_get_main_group(context), NULL, post_parse_func);

	if (!g_option_context_parse(context, argc, argv, &error)) {
		g_critical(_("Could not parse options: %s"), error->message);
		exit(EXIT_FAILURE);
	}
	return context;
}

static void commandline_free(GOptionContext *context) {
	g_free(parameter_rmp);
	g_option_context_free(context);
}

int main(int argc, char **argv) {
	GOptionContext *context;
	RoccatDevice *lua;
	GError *error = NULL;
	int retval = EXIT_SUCCESS;
	LuaRmp *rmp = NULL;

	roccat_secure();
	roccat_textdomain();

	context = commandline_parse(&argc, &argv);

	g_debug(_("Version: %s"), VERSION_STRING);

	if (parameter_just_print_version) {
		g_print(VERSION_STRING "\n");
		goto exit1;
	}

#if !(GLIB_CHECK_VERSION(2, 36, 0))
	g_type_init();
#endif

	lua = lua_device_first();
	if (lua == NULL) {
		g_critical(_("No %s found."), LUA_DEVICE_NAME);
		retval = EXIT_FAILURE;
		goto exit1;
	}

	if (parameter_rmp) {
		rmp = lua_rmp_read_with_path(parameter_rmp, &error);
		if (!rmp) {
			g_critical(_("Could not read rmp %s: %s"), parameter_rmp, error->message);
			retval = EXIT_FAILURE;
			goto exit2;
		}
		lua_rmp_set_modified(rmp);
		if (!lua_rmp_save(lua, rmp, &error)) {
			g_critical(_("Could not save profile: %s"), error->message);
			retval = EXIT_FAILURE;
			goto exit3;
		}
	}

exit3:
	lua_rmp_free(rmp);
exit2:
	g_object_unref(G_OBJECT(lua));
exit1:
	commandline_free(context);
	g_clear_error(&error);
	exit(retval);
}
