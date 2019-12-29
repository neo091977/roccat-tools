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
#include "roccat_secure.h"
#include "roccat_config.h"
#include "roccat_helper.h"
#include "roccat.h"
#include "roccat_eventhandler_application.h"
#include "g_dbus_roccat_helper.h"
#include "config.h"
#include "i18n.h"
#include <glib.h>
#include <stdlib.h>

static gboolean parameter_verbose = FALSE;
static gboolean parameter_just_print_config_dir = FALSE;
static gboolean parameter_just_print_profile_dir = FALSE;

static GOptionEntry entries[] = {
	{ "config-dir", 'c', 0, G_OPTION_ARG_NONE, &parameter_just_print_config_dir, N_("print config path"), NULL },
	{ "profile-dir", 'p', 0, G_OPTION_ARG_NONE, &parameter_just_print_profile_dir, N_("print profile path"), NULL },
	{ "verbose", 'v', 0, G_OPTION_ARG_NONE, &parameter_verbose, N_("print informative messages"), NULL },
	{ NULL }
};

static gboolean post_parse_func(GOptionContext *context, GOptionGroup *group, gpointer data, GError **error) {
	guint just_counter = 0;

	if (parameter_just_print_config_dir) ++just_counter;
	if (parameter_just_print_profile_dir) ++just_counter;
	if (just_counter > 1) {
		g_set_error(error, G_OPTION_ERROR, G_OPTION_ERROR_FAILED, _("Just give one of -c or -p"));
		return FALSE;
	}

	return TRUE;
}

static GOptionContext *commandline_parse(int *argc, char ***argv) {
	GOptionContext *context = NULL;
	GError *error = NULL;

	context = g_option_context_new(_("- handles extended capabilities of Roccat devices"));

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
	g_option_context_free(context);
}

static void null_logger(gchar const *log_domain, GLogLevelFlags log_level, gchar const *message, gpointer user_data) {}

static void print_config_dir(void) {
	gchar *path = roccat_configuration_dir();
	g_print("%s", path);
	g_free(path);
}

static void print_profile_dir(void) {
	gchar *path = roccat_profile_dir();
	g_print("%s", path);
	g_free(path);
}

int main(int argc, char **argv) {
	GOptionContext *context;
	GMainLoop *loop;
	gint retval = EXIT_SUCCESS;
	RoccatEventhandlerApplication *app;
	gboolean unique;
	gboolean result;
	GError *error = NULL;

	roccat_secure();
	roccat_textdomain();

#if !(GLIB_CHECK_VERSION(2, 36, 0))
	g_type_init();
#endif

	context = commandline_parse(&argc, &argv);

	g_debug(_("Version: %s"), VERSION_STRING);

	if (parameter_just_print_config_dir) {
		print_config_dir();
		goto exit_1;
	}

	if (parameter_just_print_profile_dir) {
		print_profile_dir();
		goto exit_1;
	}

	result = dbus_roccat_register_service(ROCCAT_DBUS_SERVER_NAME, &unique, &error);
	if (!result) {
		g_warning(_("Could not register dbus service: %s"), error->message);
		g_clear_error(&error);
		retval = EXIT_FAILURE;
		goto exit_1;
	}
	if (!unique) {
		g_warning(_("There is already an instance running"));
		retval = EXIT_FAILURE;
		goto exit_1;
	}

	if (!parameter_verbose)
		g_log_set_handler(NULL, G_LOG_LEVEL_INFO, null_logger, NULL);

	app = roccat_eventhandler_application_new();

	loop = g_main_loop_new(NULL, FALSE);
	g_main_loop_run(loop);

	g_object_unref(app);
exit_1:
	commandline_free(context);
	exit(retval);
}
