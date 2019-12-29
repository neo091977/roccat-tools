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

#include "kone_device_scanner.h"
#include "kone.h"
#include "roccat_helper.h"
#include "roccat_secure.h"
#include "config.h"
#include "i18n.h"
#include <stdlib.h>

static gchar *parameter_in_rmp = NULL;
static gchar *parameter_out_rmp = NULL;
static gint parameter_load = -1;
static gint parameter_save = -1;
static gboolean parameter_just_print_path = FALSE;
static gboolean parameter_read_firmware = FALSE;
static gboolean parameter_read_weight = FALSE;
static gboolean parameter_just_calibrate_tcu = FALSE;
static gboolean parameter_just_print_version = FALSE;
static gboolean parameter_just_print_actual_profile = FALSE;
static gboolean parameter_just_print_actual_cpi = FALSE;
static gint parameter_activate_profile = -1;

static GOptionEntry entries[] = {
		{ "read-rmp", 'r', 0, G_OPTION_ARG_FILENAME, &parameter_in_rmp, N_("read from rmp-file FILENAME"), N_("FILENAME") },
		{ "write-rmp", 'w', 0, G_OPTION_ARG_FILENAME, &parameter_out_rmp, N_("write to rmp-file FILENAME"), N_("FILENAME") },
		{ "load", 'l', 0, G_OPTION_ARG_INT, &parameter_load, N_("load profile NUMBER"), N_("NUMBER") },
		{ "save", 's', 0, G_OPTION_ARG_INT, &parameter_save, N_("save as profile NUMBER"), N_("NUMBER") },
		{ "path", 'p', 0, G_OPTION_ARG_NONE, &parameter_just_print_path, N_("print path in sysfs"), NULL },
		{ "read-firmware", 'f', 0, G_OPTION_ARG_NONE, &parameter_read_firmware, N_("read firmware version"), NULL },
		{ "read-weight", 'g', 0, G_OPTION_ARG_NONE, &parameter_read_weight, N_("read fast-clip weight"), NULL },
		{ "calibrate", 'c', 0, G_OPTION_ARG_NONE, &parameter_just_calibrate_tcu, N_("calibrate and activate TCU"), NULL },
		{ "version", 'v', 0, G_OPTION_ARG_NONE, &parameter_just_print_version, N_("print version"), NULL },
		{ "activate-profile", 'a', 0, G_OPTION_ARG_INT, &parameter_activate_profile, N_("activate profile NUMBER"), N_("NUMBER") },
		{ "actual-profile", 0, 0, G_OPTION_ARG_NONE, &parameter_just_print_actual_profile, N_("print actual profile"), NULL },
		{ "actual-cpi", 0, 0, G_OPTION_ARG_NONE, &parameter_just_print_actual_cpi, N_("print actual CPI"), NULL },
		{ NULL }
};

static gboolean parse_profile_number_parameter(gint number, GError **error) {
	if (number < KONE_PROFILE_MIN || number > KONE_PROFILE_MAX) {
		g_set_error(error, G_OPTION_ERROR, G_OPTION_ERROR_BAD_VALUE, _("Profile numbers have to be in range [1-5]"));
		return FALSE;
	}
	return TRUE;
}

static gboolean post_parse_func(GOptionContext *context, GOptionGroup *group, gpointer data, GError **error) {
	guint counter;

	counter = 0;
	if (parameter_just_print_path) ++counter;
	if (parameter_just_calibrate_tcu) ++counter;
	if (parameter_just_print_version) ++counter;
	if (parameter_just_print_actual_profile) ++counter;
	if (parameter_just_print_actual_cpi) ++counter;
	if (counter > 1) {
		g_set_error(error, G_OPTION_ERROR, G_OPTION_ERROR_FAILED, _("Just give one of -m, -e, -p, -c, -v, --actual-profile, --actual-cpi"));
		return FALSE;
	}

	counter = 0;
	if (parameter_load != -1) ++counter;
	if (parameter_in_rmp) ++counter;
	if (counter > 1) {
		g_set_error(error, G_OPTION_ERROR, G_OPTION_ERROR_FAILED, _("-r and -l are mutual exclusive"));
		return FALSE;
	}

	if (parameter_activate_profile != -1 && !parse_profile_number_parameter(parameter_activate_profile, error))
		return FALSE;

	if (parameter_load != -1 && !parse_profile_number_parameter(parameter_load, error))
		return FALSE;

	if (parameter_save != -1 && !parse_profile_number_parameter(parameter_save, error))
		return FALSE;

	/*
	 * profile_load is allowed without writing afterwards, since warning about
	 * differing profiles is useful in itself.
	 */
	if ((parameter_in_rmp || (parameter_load != -1)) &&
			!(parameter_out_rmp || (parameter_save != -1))) {
		g_set_error(error, G_OPTION_ERROR, G_OPTION_ERROR_FAILED, _("If -r or -l is given, -w or -s is needed"));
		return FALSE;
	}

	return TRUE;
}

static GOptionContext *commandline_parse(int *argc, char ***argv) {
	GOptionContext *context = NULL;
	GError *error = NULL;
	gchar *string;

	string = g_strdup_printf(_("- controls extended capabilities of Roccat %s mice"), KONE_DEVICE_NAME);
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

static gboolean print_firmware(RoccatDevice *kone, GError **error) {
	gchar *firmware_string;
	guint firmware_version = kone_firmware_version_read(kone, error);
	if (*error)
		return FALSE;

	firmware_string = kone_firmware_version_to_string(firmware_version);
	g_print("%s\n", firmware_string);
	g_free(firmware_string);
	return TRUE;
}

static gboolean print_weight(RoccatDevice *kone, GError **error) {
	guint weight = kone_weight_read(kone, error);
	if (*error)
		return FALSE;
	g_print("%ig\n", weight);
	return TRUE;
}

static gboolean print_actual_cpi(RoccatDevice *kone, GError **error) {
	guint cpi = kone_actual_dpi_read(kone, error);
	if (*error)
		return FALSE;
	g_print("%i\n", cpi);
	return TRUE;
}

static gboolean print_actual_profile(RoccatDevice *kone, GError **error) {
	guint profile_number = kone_actual_profile_read(kone, error);
	if (*error)
		return FALSE;
	g_print("%i\n", profile_number);
	return TRUE;
}

static gboolean user_decision(gchar *text) {
	gchar decision;
	int retval;

	g_print("%s", text);
	g_print(" (y|n)");
	retval = scanf("%c", &decision);
	if (retval != 1)
		return FALSE;
	fflush(stdin);
	return g_ascii_tolower(decision) == 'y';
}

static gboolean calibrate(RoccatDevice *kone, GError **error) {
	if (user_decision(_("To calibrate, you have to keep your hands off your Kone for 5 seconds."
			" Do you want to continue?"))) {
		g_print(_("Please wait..."));
		if (!kone_calibrate(kone, error))
			return FALSE;

		g_print(_("Done.\n"));
	} else
		g_print(_("Calibration was aborted on user request.\n"));
	return TRUE;
}

int main(int argc, char **argv) {
	GOptionContext *context;
	RoccatDevice *kone;
	GError *error = NULL;
	KoneRMP *rmp = NULL;
	int retval = EXIT_SUCCESS;

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

	kone = kone_device_first();
	if (kone == NULL) {
		g_critical(_("No %s found."), KONE_DEVICE_NAME);
		retval = EXIT_FAILURE;
		goto exit1;
	}

	if (parameter_just_print_path) {
		g_print("%s", roccat_device_get_syspath(kone));
		goto exit2;
	}

	if (parameter_just_print_actual_profile) {
		if (!print_actual_profile(kone, &error)) {
			g_critical(_("Could not print actual profile: %s"), error->message);
			retval = EXIT_FAILURE;
		}
		goto exit2;
	}

	if (parameter_just_print_actual_cpi) {
		if (!print_actual_cpi(kone, &error)) {
			g_critical(_("Could not print actual CPI: %s"), error->message);
			retval = EXIT_FAILURE;
		}
		goto exit2;
	}

	if (parameter_just_calibrate_tcu) {
		if (!calibrate(kone, &error)) {
			g_critical(_("Could not calibrate TCU: %s"), error->message);
			retval = EXIT_FAILURE;
		}
		goto exit2;
	}

	if (parameter_read_firmware) {
		if (!print_firmware(kone, &error)) {
			g_critical(_("Could not print firmware version: %s"), error->message);
			retval = EXIT_FAILURE;
			goto exit2;
		}
	}

	if (parameter_read_weight) {
		if (!print_weight(kone, &error)) {
			g_critical(_("Could not print weight: %s"), error->message);
			retval = EXIT_FAILURE;
			goto exit2;
		}
	}

	if (parameter_load != -1) {
		rmp = kone_rmp_load(kone, parameter_load, &error);
		if (!rmp) {
			g_critical(_("Could not load profile %i: %s"), parameter_load, error->message);
			retval = EXIT_FAILURE;
			goto exit2;
		}
	} else if (parameter_in_rmp) {
		rmp = kone_rmp_read_with_path(parameter_in_rmp, &error);
		if (!rmp) {
			g_critical(_("Could not read rmp %s: %s"), parameter_in_rmp, error->message);
			retval = EXIT_FAILURE;
			goto exit2;
		}
	} else
		rmp = kone_rmp_dup(kone_default_rmp());

	kone_rmp_set_modified(rmp);

	if (parameter_save != -1) {
		if (!kone_rmp_save(kone, rmp, parameter_save, &error)) {
			g_critical(_("Could not save profile %i: %s"), parameter_save, error->message);
			retval = EXIT_FAILURE;
			goto exit3;
		}
		kone_dbus_emit_profile_data_changed_outside_instant(parameter_save);
	}

	if (parameter_out_rmp) {
		if (!kone_rmp_write_with_path(parameter_out_rmp, rmp, &error)) {
			g_critical(_("Could not write rmp %s: %s"), parameter_out_rmp, error->message);
			retval = EXIT_FAILURE;
			goto exit3;
		}
	}

	if (parameter_activate_profile != -1) {
		if (!kone_profile_activate(kone, parameter_activate_profile, &error)) {
			g_critical(_("Could not activate profile %i: %s"), parameter_activate_profile, error->message);
			retval = EXIT_FAILURE;
			goto exit3;
		}
		kone_dbus_emit_profile_changed_outside_instant(parameter_activate_profile);
	}

exit3:
	kone_rmp_free(rmp);
exit2:
	g_object_unref(G_OBJECT(kone));
exit1:
	commandline_free(context);
	g_clear_error(&error);
	exit(retval);
}
