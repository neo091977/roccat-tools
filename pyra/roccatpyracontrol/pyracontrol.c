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

#include "pyra.h"
#include "roccat_firmware.h"
#include "roccat_helper.h"
#include "roccat_secure.h"
#include "g_dbus_roccat_helper.h"
#include "config.h"
#include "i18n.h"
#include <stdlib.h>

static gboolean parameter_just_print_version = FALSE;
static gboolean parameter_just_print_actual_profile = FALSE;
static gboolean parameter_just_reset = FALSE;
static gboolean parameter_read_firmware = FALSE;
static gint parameter_activate_profile = -1;
static gint parameter_load = -1;
static gint parameter_save = -1;
static gchar *parameter_in_rmp = NULL;
static gchar *parameter_out_rmp = NULL;

static GOptionEntry entries[] = {
	{ "activate-profile", 'a', 0, G_OPTION_ARG_INT, &parameter_activate_profile, N_("activate profile NUMBER"), N_("NUMBER") },
	{ "read-firmware", 'f', 0, G_OPTION_ARG_NONE, &parameter_read_firmware, N_("read firmware version"), NULL },
	{ "load", 'l', 0, G_OPTION_ARG_INT, &parameter_load, N_("load profile NUMBER"), N_("NUMBER") },
	{ "read-rmp", 'r', 0, G_OPTION_ARG_FILENAME, &parameter_in_rmp, N_("read from rmp-file FILENAME"), N_("FILENAME") },
	{ "save", 's', 0, G_OPTION_ARG_INT, &parameter_save, N_("save as profile NUMBER"), N_("NUMBER") },
	{ "version", 'v', 0, G_OPTION_ARG_NONE, &parameter_just_print_version, N_("print version"), NULL },
	{ "write-rmp", 'w', 0, G_OPTION_ARG_FILENAME, &parameter_out_rmp, N_("write to rmp-file FILENAME"), N_("FILENAME") },
	{ "actual-profile", 0, 0, G_OPTION_ARG_NONE, &parameter_just_print_actual_profile, N_("print actual profile"), NULL },
	{ "reset", 0, 0, G_OPTION_ARG_NONE, &parameter_just_reset, N_("reset to factory defaults"), NULL },
	{ NULL }
};

static gboolean parse_profile_number_parameter(gint number, GError **error) {
	if (number < PYRA_PROFILE_MIN + 1 || number > PYRA_PROFILE_MAX + 1) {
		g_set_error(error, G_OPTION_ERROR, G_OPTION_ERROR_BAD_VALUE, _("Profile numbers have to be in range [1-5]"));
		return FALSE;
	}
	return TRUE;
}

static gboolean post_parse_func(GOptionContext *context, GOptionGroup *group, gpointer data, GError **error) {
	guint just_counter = 0;

	if (parameter_just_print_version) ++just_counter;
	if (parameter_just_print_actual_profile) ++just_counter;
	if (parameter_just_reset) ++just_counter;
	if (just_counter > 1) {
		g_set_error(error, G_OPTION_ERROR, G_OPTION_ERROR_FAILED, _("Just give one of -v, --actual-profile, --reset"));
		return FALSE;
	}

	if (parameter_activate_profile != -1 && !parse_profile_number_parameter(parameter_activate_profile, error))
		return FALSE;

	if (parameter_load != -1 && !parse_profile_number_parameter(parameter_load, error))
		return FALSE;

	if (parameter_save != -1 && !parse_profile_number_parameter(parameter_save, error))
		return FALSE;

	if ((parameter_load != -1 || parameter_in_rmp) && !(parameter_out_rmp || parameter_save != -1)) {
		g_set_error(error, G_OPTION_ERROR, G_OPTION_ERROR_FAILED, _("If -r or -l is given, -w or -s is needed"));
	}

	if (parameter_load != -1 && parameter_in_rmp) {
		g_set_error(error, G_OPTION_ERROR, G_OPTION_ERROR_FAILED, _("-r and -l are mutual exclusive"));
		return FALSE;
	}

	return TRUE;
}

static GOptionContext *commandline_parse(int *argc, char ***argv) {
	GOptionContext *context = NULL;
	GError *error = NULL;
	gchar *string;

	string = g_strdup_printf(_("- controls extended capabilities of Roccat %s mice"), PYRA_DEVICE_NAME_COMBINED);
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
	g_free(parameter_in_rmp);
	g_free(parameter_out_rmp);
	g_option_context_free(context);
}

static gboolean print_actual_profile(RoccatDevice *pyra, GError **error) {
	guint profile_index = pyra_actual_profile_read(pyra, error);
	if (*error)
		return FALSE;
	g_print("%i\n", profile_index + 1);
	return TRUE;
}

static gboolean print_firmware(RoccatDevice *pyra, GError **error) {
	gchar *firmware_string;
	guint firmware_version;

	firmware_version = pyra_firmware_version_read(pyra, error);
	if (*error)
		return FALSE;

	firmware_string = roccat_firmware_version_to_string(firmware_version);
	g_print("%s\n", firmware_string);
	g_free(firmware_string);
	return TRUE;
}

static gboolean reset(RoccatDevice *pyra, GError **error) {
	DBusGProxy *proxy;
	guint i;

	if (!pyra_reset(pyra, error))
		return FALSE;

	proxy = pyra_dbus_proxy_new();
	if (!proxy)
		return TRUE;

	for (i = 0; i < PYRA_PROFILE_NUM; ++i)
		pyra_dbus_emit_profile_data_changed_outside(proxy, i + 1);
	dbus_roccat_proxy_free(proxy);

	return TRUE;
}

int main(int argc, char **argv) {
	GOptionContext *context;
	RoccatDevice *pyra;
	GError *error = NULL;
	int retval = EXIT_SUCCESS;
	PyraRMP *rmp = NULL;

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

	pyra = pyra_device_first();
	if (pyra == NULL) {
		g_critical(_("No %s found."), PYRA_DEVICE_NAME_COMBINED);
		retval = EXIT_FAILURE;
		goto exit1;
	}

	if (parameter_just_print_actual_profile) {
		if (!print_actual_profile(pyra, &error)) {
			g_critical(_("Could not print actual profile: %s"), error->message);
			retval = EXIT_FAILURE;
		}
		goto exit2;
	}

	if (parameter_just_reset) {
		if (!reset(pyra, &error)) {
			g_critical(_("Could not reset device: %s"), error->message);
			retval = EXIT_FAILURE;
		}
		goto exit2;
	}

	if (parameter_read_firmware) {
		if (!print_firmware(pyra, &error)) {
			g_critical(_("Could not print firmware version: %s"), error->message);
			retval = EXIT_FAILURE;
			goto exit2;
		}
	}

	if (parameter_load != -1) {
		rmp = pyra_rmp_load(pyra, parameter_load - 1, &error);
		if (!rmp) {
			g_critical(_("Could not load profile %i: %s"), parameter_load, error->message);
			retval = EXIT_FAILURE;
			goto exit2;
		}
	} else if (parameter_in_rmp) {
		rmp = pyra_rmp_read_with_path(parameter_in_rmp, &error);
		if (!rmp) {
			g_critical(_("Could not read rmp %s: %s"), parameter_in_rmp, error->message);
			retval = EXIT_FAILURE;
			goto exit2;
		}
	} else
		rmp = pyra_rmp_dup(pyra_default_rmp());

	pyra_rmp_set_modified(rmp);

	if (parameter_save != -1) {
		if (!pyra_rmp_save(pyra, rmp, parameter_save - 1, &error)) {
			g_critical(_("Could not save profile %i: %s"), parameter_save, error->message);
			retval = EXIT_FAILURE;
			goto exit3;
		}
		pyra_dbus_emit_profile_data_changed_outside_instant(parameter_save);
	}

	if (parameter_out_rmp) {
		if (!pyra_rmp_write_with_path(parameter_out_rmp, rmp, &error)) {
			g_critical(_("Could not write rmp %s: %s"), parameter_out_rmp, error->message);
			retval = EXIT_FAILURE;
			goto exit3;
		}
	}

	if (parameter_activate_profile != -1) {
		if (!pyra_actual_profile_write(pyra, parameter_activate_profile -1, &error)) {
			g_critical(_("Could not activate profile %i: %s"), parameter_activate_profile, error->message);
			retval = EXIT_FAILURE;
			goto exit3;
		}
		pyra_dbus_emit_profile_changed_outside_instant(parameter_activate_profile);
	}

exit3:
	pyra_rmp_free(rmp);
exit2:
	g_object_unref(G_OBJECT(pyra));
exit1:
	commandline_free(context);
	g_clear_error(&error);
	exit(retval);
}
