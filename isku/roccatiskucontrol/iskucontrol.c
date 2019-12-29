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

#include "isku.h"
#include "roccat_firmware.h"
#include "roccat_helper.h"
#include "roccat_secure.h"
#include "g_dbus_roccat_helper.h"
#include "config.h"
#include "i18n.h"
#include <stdlib.h>

static gboolean parameter_just_activate_driver_state = FALSE;
static gboolean parameter_just_deactivate_driver_state = FALSE;
static gchar *parameter_just_update_firmware = NULL;
static gboolean parameter_just_print_version = FALSE;
static gboolean parameter_just_print_actual_profile = FALSE;
static gboolean parameter_just_reset = FALSE;
static gboolean parameter_read_firmware = FALSE;
static gint parameter_activate_profile = -1;
static gint parameter_load = -1;
static gint parameter_save = -1;
static gchar *parameter_in_rkp = NULL;
static gchar *parameter_out_rkp = NULL;

static GOptionEntry entries[] = {
	{ "activate-profile", 'a', 0, G_OPTION_ARG_INT, &parameter_activate_profile, N_("activate profile NUMBER"), N_("NUMBER") },
	{ "driver-off", 0, 0, G_OPTION_ARG_NONE, &parameter_just_deactivate_driver_state, N_("deactivate Windows driver state"), NULL },
	{ "driver-on", 0, 0, G_OPTION_ARG_NONE, &parameter_just_activate_driver_state, N_("activate Windows driver state"), NULL },
	{ "read-firmware", 'f', 0, G_OPTION_ARG_NONE, &parameter_read_firmware, N_("read firmware version"), NULL },
	{ "load", 'l', 0, G_OPTION_ARG_INT, &parameter_load, N_("load profile NUMBER"), N_("NUMBER") },
	{ "read-rkp", 'r', 0, G_OPTION_ARG_FILENAME, &parameter_in_rkp, N_("read from rkp-file FILENAME"), N_("FILENAME") },
	{ "save", 's', 0, G_OPTION_ARG_INT, &parameter_save, N_("save as profile NUMBER"), N_("NUMBER") },
	{ "firmware", 0, 0, G_OPTION_ARG_FILENAME, &parameter_just_update_firmware, N_("update firmware with FILENAME"), N_("FILENAME") },
	{ "version", 'v', 0, G_OPTION_ARG_NONE, &parameter_just_print_version, N_("print version"), NULL },
	{ "write-rkp", 'w', 0, G_OPTION_ARG_FILENAME, &parameter_out_rkp, N_("write to rkp-file FILENAME"), N_("FILENAME") },
	{ "actual-profile", 0, 0, G_OPTION_ARG_NONE, &parameter_just_print_actual_profile, N_("print actual profile"), NULL },
	{ "reset", 0, 0, G_OPTION_ARG_NONE, &parameter_just_reset, N_("reset to factory defaults"), NULL },
	{ NULL }
};

static gboolean parse_profile_number_parameter(gint number, GError **error) {
	if (number < 1 || number > ISKU_PROFILE_NUM) {
		g_set_error(error, G_OPTION_ERROR, G_OPTION_ERROR_BAD_VALUE, _("Profile numbers have to be in range [1-5]"));
		return FALSE;
	}
	return TRUE;
}

static gboolean post_parse_func(GOptionContext *context, GOptionGroup *group, gpointer data, GError **error) {
	guint just_counter = 0;

	if (parameter_just_activate_driver_state) ++just_counter;
	if (parameter_just_deactivate_driver_state) ++just_counter;
	if (parameter_just_update_firmware) ++just_counter;
	if (parameter_just_print_version) ++just_counter;
	if (parameter_just_print_actual_profile) ++just_counter;
	if (parameter_just_reset) ++just_counter;
	if (just_counter > 1) {
		g_set_error(error, G_OPTION_ERROR, G_OPTION_ERROR_FAILED, _("Just give one of -v, --actual-profile, --reset, --driver-off, --driver-on, --firmware"));
		return FALSE;
	}

	if (parameter_activate_profile != -1 && !parse_profile_number_parameter(parameter_activate_profile, error))
		return FALSE;

	if (parameter_load != -1 && !parse_profile_number_parameter(parameter_load, error))
		return FALSE;

	if (parameter_save != -1 && !parse_profile_number_parameter(parameter_save, error))
		return FALSE;

	if ((parameter_load != -1 || parameter_in_rkp) && !(parameter_out_rkp || parameter_save != -1)) {
		g_set_error(error, G_OPTION_ERROR, G_OPTION_ERROR_FAILED, _("If -r or -l is given, -w or -s is needed"));
		return FALSE;
	}

	if (parameter_load != -1 && parameter_in_rkp) {
		g_set_error(error, G_OPTION_ERROR, G_OPTION_ERROR_FAILED, _("-r and -l are mutual exclusive"));
		return FALSE;
	}

	return TRUE;
}

static GOptionContext *commandline_parse(int *argc, char ***argv) {
	GOptionContext *context = NULL;
	GError *error = NULL;
	gchar *string;

	string = g_strdup_printf(_("- controls extended capabilities of Roccat %s keyboards"), ISKU_DEVICE_NAME);
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
	g_free(parameter_just_update_firmware);
	g_free(parameter_in_rkp);
	g_free(parameter_out_rkp);
	g_option_context_free(context);
}

static gboolean print_actual_profile(RoccatDevice *isku, GError **error) {
	guint profile = isku_actual_profile_read(isku, error);
	if (*error)
		return FALSE;
	g_print("%i\n", profile + 1);
	return TRUE;
}

static gboolean print_firmware(RoccatDevice *isku, GError **error) {
	guint firmware_version;
	gchar *firmware_string;

	firmware_version = isku_firmware_version_read(isku, error);
	if (*error)
		return FALSE;

	firmware_string = roccat_firmware_version_to_string(firmware_version);
	g_print("%s\n", firmware_string);
	g_free(firmware_string);

	return TRUE;
}

static gboolean reset(RoccatDevice *isku, GError **error) {
	DBusGProxy *proxy;
	guint i;

	if (!isku_reset(isku, ISKU_RESET_FUNCTION_RESET, error))
		return FALSE;

	proxy = isku_dbus_proxy_new();
	if (!proxy)
		return TRUE;

	for (i = 0; i < ISKU_PROFILE_NUM; ++i)
		isku_dbus_emit_profile_data_changed_outside(proxy, i + 1);
	dbus_roccat_proxy_free(proxy);

	return TRUE;
}

int main(int argc, char **argv) {
	GOptionContext *context;
	RoccatDevice *isku;
	GError *local_error = NULL;
	int retval = EXIT_SUCCESS;
	IskuRkp *rkp = NULL;

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

	isku = isku_device_first();
	if (isku == NULL) {
		g_critical(_("No %s found."), ISKU_DEVICE_NAME);
		retval = EXIT_FAILURE;
		goto exit1;
	}

	if (parameter_just_activate_driver_state) {
		if (!isku_device_state_write(isku, ISKU_DEVICE_STATE_STATE_ON, &local_error)) {
			g_critical(_("Could not activate driver state: %s"), local_error->message);
			retval = EXIT_FAILURE;
		}
		goto exit2;
	}

	if (parameter_just_deactivate_driver_state) {
		if (!isku_device_state_write(isku, ISKU_DEVICE_STATE_STATE_OFF, &local_error)) {
			g_critical(_("Could not deactivate driver state: %s"), local_error->message);
			retval = EXIT_FAILURE;
		}
		goto exit2;
	}

	if (parameter_just_update_firmware) {
		if (!roccat_firmware_update_console(isku, ISKU_INTERFACE_KEYBOARD, parameter_just_update_firmware,
				ISKU_FIRMWARE_SIZE, ROCCAT_FIRMWARE_NUMBER_DEFAULT,
				ROCCAT_FIRMWARE_UPDATE_WAIT_PREPARE_DEFAULT, ROCCAT_FIRMWARE_UPDATE_WAIT_INIT_DATA_DEFAULT, ROCCAT_FIRMWARE_UPDATE_WAIT_DATA_DEFAULT, ROCCAT_FIRMWARE_UPDATE_FINAL_WRITE_CHECK_DEFAULT)) {
			retval = EXIT_FAILURE;
		}
		goto exit2;
	}

	if (parameter_just_print_actual_profile) {
		if (!print_actual_profile(isku, &local_error)) {
			g_critical(_("Could not print actual profile: %s"), local_error->message);
			retval = EXIT_FAILURE;
		}
		goto exit2;
	}

	if (parameter_just_reset) {
		if (!reset(isku, &local_error)) {
			g_critical(_("Could not reset device: %s"), local_error->message);
			retval = EXIT_FAILURE;
		}
		goto exit2;
	}

	if (parameter_read_firmware) {
		if (!print_firmware(isku, &local_error)) {
			g_critical(_("Could not print firmware version: %s"), local_error->message);
			retval = EXIT_FAILURE;
			goto exit2;
		}
	}

	if (parameter_load != -1) {
		rkp = isku_rkp_load(isku, parameter_load - 1, &local_error);
		if (!rkp) {
			g_critical(_("Could not load profile %i: %s"), parameter_load, local_error->message);
			retval = EXIT_FAILURE;
			goto exit2;
		}
	} else if (parameter_in_rkp) {
		rkp = isku_rkp_read_with_path(parameter_in_rkp, isku_rkp_defaults(), &local_error);
		if (!rkp) {
			g_critical(_("Could not read rkp %s: %s"), parameter_in_rkp, local_error->message);
			retval = EXIT_FAILURE;
			goto exit2;
		}
	} else
		rkp = isku_default_rkp();

	isku_rkp_set_modified(rkp);

	if (parameter_save != -1) {
		if (!isku_rkp_save(isku, rkp, parameter_save - 1, &local_error)) {
			g_critical(_("Could not save profile %i: %s"), parameter_save, local_error->message);
			retval = EXIT_FAILURE;
			goto exit3;
		}
		isku_dbus_emit_profile_data_changed_outside_instant(parameter_save);
	}

	if (parameter_out_rkp) {
		if (!isku_rkp_write_with_path(parameter_out_rkp, rkp, &local_error)) {
			g_critical(_("Could not write rkp %s: %s"), parameter_out_rkp, local_error->message);
			retval = EXIT_FAILURE;
			goto exit3;
		}
	}

	if (parameter_activate_profile != -1) {
		if (!isku_actual_profile_write(isku, parameter_activate_profile -1, &local_error)) {
			g_critical(_("Could not activate profile %i: %s"), parameter_activate_profile, local_error->message);
			retval = EXIT_FAILURE;
			goto exit3;
		}
		isku_dbus_emit_profile_changed_outside_instant(parameter_activate_profile);
	}

exit3:
	isku_rkp_free(rkp);
exit2:
	g_object_unref(G_OBJECT(isku));
exit1:
	commandline_free(context);
	g_clear_error(&local_error);
	exit(retval);
}
