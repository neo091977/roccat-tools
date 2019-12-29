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

#include "konepure.h"
#include "roccat_firmware.h"
#include "roccat_srom.h"
#include "roccat_helper.h"
#include "roccat_secure.h"
#include "g_dbus_roccat_helper.h"
#include "config.h"
#include "i18n.h"
#include <stdlib.h>

static gboolean parameter_just_activate_driver_state = FALSE;
static gboolean parameter_just_deactivate_driver_state = FALSE;
static gchar *parameter_just_update_firmware = NULL;
static gchar *parameter_just_update_srom = NULL;
static gchar *parameter_in_rmp = NULL;
static gchar *parameter_out_rmp = NULL;
static gboolean parameter_just_print_version = FALSE;
static gboolean parameter_just_print_actual_profile = FALSE;
static gboolean parameter_just_reset = FALSE;
static gboolean parameter_read_firmware = FALSE;
static gint parameter_load = -1;
static gint parameter_save = -1;
static gint parameter_activate_profile = -1;
static gint parameter_sensor_register = -1;
static gboolean parameter_sensor_read = FALSE;
static gint parameter_sensor_write = -1;

static GOptionEntry entries[] = {
		{ "activate-profile", 'a', 0, G_OPTION_ARG_INT, &parameter_activate_profile, N_("activate profile NUMBER"), N_("NUMBER") },
		{ "driver-off", 0, 0, G_OPTION_ARG_NONE, &parameter_just_deactivate_driver_state, N_("deactivate Windows driver state"), NULL },
		{ "driver-on", 0, 0, G_OPTION_ARG_NONE, &parameter_just_activate_driver_state, N_("activate Windows driver state"), NULL },
		{ "read-firmware", 'f', 0, G_OPTION_ARG_NONE, &parameter_read_firmware, N_("read firmware version"), NULL },
		{ "load", 'l', 0, G_OPTION_ARG_INT, &parameter_load, N_("load profile NUMBER"), N_("NUMBER") },
		{ "read-rmp", 'r', 0, G_OPTION_ARG_FILENAME, &parameter_in_rmp, N_("read from rmp-file FILENAME"), N_("FILENAME") },
		{ "save", 's', 0, G_OPTION_ARG_INT, &parameter_save, N_("save as profile NUMBER"), N_("NUMBER") },
		{ "firmware", 0, 0, G_OPTION_ARG_FILENAME, &parameter_just_update_firmware, N_("update firmware with FILENAME"), N_("FILENAME") },
		{ "version", 'v', 0, G_OPTION_ARG_NONE, &parameter_just_print_version, N_("print version"), NULL },
		{ "write-rmp", 'w', 0, G_OPTION_ARG_FILENAME, &parameter_out_rmp, N_("write to rmp-file FILENAME"), N_("FILENAME") },
		{ "actual-profile", 0, 0, G_OPTION_ARG_NONE, &parameter_just_print_actual_profile, N_("print actual profile"), NULL },
		{ "reset", 0, 0, G_OPTION_ARG_NONE, &parameter_just_reset, N_("reset to factory defaults"), NULL },
		{ "sensor-register", 0, 0, G_OPTION_ARG_INT, &parameter_sensor_register, N_("sensor register"), NULL },
		{ "sensor-read", 0, 0, G_OPTION_ARG_NONE, &parameter_sensor_read, N_("read value from sensor register"), NULL },
		{ "sensor-write", 0, 0, G_OPTION_ARG_INT, &parameter_sensor_write, N_("write VALUE to sensor register"), N_("VALUE") },
		{ "srom", 0, 0, G_OPTION_ARG_FILENAME, &parameter_just_update_srom, N_("update sensor rom with FILENAME"), N_("FILENAME") },
		{ NULL }
};

static gboolean parse_profile_number_parameter(gint number, GError **error) {
	if (number < 1 || number > KONEPLUS_PROFILE_NUM) {
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
	if (parameter_just_update_srom) ++just_counter;
	if (parameter_just_print_version) ++just_counter;
	if (parameter_just_print_actual_profile) ++just_counter;
	if (parameter_just_reset) ++just_counter;
	if (just_counter > 1) {
		g_set_error(error, G_OPTION_ERROR, G_OPTION_ERROR_FAILED, _("Just give one of -v, --actual-profile, --reset, --driver-off, --driver-on, --firmware, --srom"));
		return FALSE;
	}

	if (parameter_load != -1 && !parse_profile_number_parameter(parameter_load, error))
		return FALSE;

	if (parameter_save != -1 && !parse_profile_number_parameter(parameter_save, error))
		return FALSE;

	if (parameter_activate_profile != -1 && !parse_profile_number_parameter(parameter_activate_profile, error))
		return FALSE;

	if ((parameter_load != -1 || parameter_in_rmp) && !(parameter_out_rmp || parameter_save != -1)) {
		g_set_error(error, G_OPTION_ERROR, G_OPTION_ERROR_FAILED, _("If -r or -l is given, -w or -s is needed"));
		return FALSE;
	}

	if (parameter_load != -1 && parameter_in_rmp) {
		g_set_error(error, G_OPTION_ERROR, G_OPTION_ERROR_FAILED, _("-r and -l are mutual exclusive"));
		return FALSE;
	}

	if (parameter_sensor_register != -1 && (parameter_sensor_register < 0 || parameter_sensor_register > 255)) {
		g_set_error(error, G_OPTION_ERROR, G_OPTION_ERROR_FAILED, _("Sensor register out of range [0-255]"));
		return FALSE;
	}

	if (parameter_sensor_write != -1 && (parameter_sensor_write < 0 || parameter_sensor_write > 255)) {
		g_set_error(error, G_OPTION_ERROR, G_OPTION_ERROR_FAILED, _("Sensor value out of range [0-255]"));
		return FALSE;
	}

	if ((parameter_sensor_read || parameter_sensor_write != -1) && !(parameter_sensor_register != -1)) {
		g_set_error(error, G_OPTION_ERROR, G_OPTION_ERROR_FAILED, _("If --sensor-read or --sensor-write is given, --sensor-register is needed"));
		return FALSE;
	}

	return TRUE;
}

static GOptionContext *commandline_parse(int *argc, char ***argv) {
	GOptionContext *context = NULL;
	GError *error = NULL;
	gchar *string;

	string = g_strdup_printf(_("- controls extended capabilities of Roccat %s mice"), KONEPURE_DEVICE_NAME);
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
	g_free(parameter_in_rmp);
	g_free(parameter_out_rmp);
	g_option_context_free(context);
}

static gboolean print_actual_profile(RoccatDevice *konepure, GError **error) {
	guint profile_index = koneplus_actual_profile_read(konepure, error);
	if (*error)
		return FALSE;
	g_print("%i\n", profile_index + 1);
	return TRUE;
}

static gboolean print_firmware(RoccatDevice *konepure, GError **error) {
	gchar *firmware_string;
	guint firmware_version;

	firmware_version = koneplus_firmware_version_read(konepure, error);
	if (*error)
		return FALSE;

	firmware_string = roccat_firmware_version_to_string(firmware_version);
	g_print("%s\n", firmware_string);
	g_free(firmware_string);
	return TRUE;
}

static gboolean print_sensor_value(RoccatDevice *konepure, GError **error) {
	guint8 value;
	value = koneplus_sensor_read_register(konepure, parameter_sensor_register, error);
	if (*error)
		return FALSE;
	g_print("0x%02x\n", value);
	return TRUE;
}

static gboolean reset(RoccatDevice *konepure, GError **error) {
	DBusGProxy *proxy;
	guint i;

	if (!koneplus_reset(konepure, error))
		return FALSE;

	proxy = konepure_dbus_proxy_new();
	if (!proxy)
		return TRUE;

	for (i = 0; i < KONEPLUS_PROFILE_NUM; ++i)
		konepure_dbus_emit_profile_data_changed_outside(proxy, i + 1);
	dbus_roccat_proxy_free(proxy);

	return TRUE;
}

int main(int argc, char **argv) {
	GOptionContext *context;
	RoccatDevice *konepure;
	GError *local_error = NULL;
	int retval = EXIT_SUCCESS;
	KoneplusRmp *rmp = NULL;

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

	konepure = konepure_device_first();
	if (konepure == NULL) {
		g_critical(_("No %s found."), KONEPURE_DEVICE_NAME);
		retval = EXIT_FAILURE;
		goto exit1;
	}

	if (parameter_just_activate_driver_state) {
		if (!koneplus_device_state_write(konepure, KONEPLUS_DEVICE_STATE_STATE_ON, &local_error)) {
			g_critical(_("Could not activate driver state: %s"), local_error->message);
			retval = EXIT_FAILURE;
		}
		goto exit2;
	}

	if (parameter_just_deactivate_driver_state) {
		if (!koneplus_device_state_write(konepure, KONEPLUS_DEVICE_STATE_STATE_OFF, &local_error)) {
			g_critical(_("Could not deactivate driver state: %s"), local_error->message);
			retval = EXIT_FAILURE;
		}
		goto exit2;
	}

	if (parameter_just_update_firmware) {
		if (!roccat_firmware_update_console(konepure, KONEPLUS_INTERFACE_MOUSE, parameter_just_update_firmware,
				KONEPLUS_FIRMWARE_SIZE, ROCCAT_FIRMWARE_NUMBER_DEFAULT,
				ROCCAT_FIRMWARE_UPDATE_WAIT_PREPARE_DEFAULT, ROCCAT_FIRMWARE_UPDATE_WAIT_INIT_DATA_DEFAULT, ROCCAT_FIRMWARE_UPDATE_WAIT_DATA_DEFAULT, ROCCAT_FIRMWARE_UPDATE_FINAL_WRITE_CHECK_DEFAULT)) {
			retval = EXIT_FAILURE;
		}
		goto exit2;
	}

	if (parameter_just_update_srom) {
		if (!roccat_srom_update_console(konepure, KONEPLUS_INTERFACE_MOUSE, parameter_just_update_srom)) {
			retval = EXIT_FAILURE;
		}
		goto exit2;
	}

	if (parameter_just_print_actual_profile) {
		if (!print_actual_profile(konepure, &local_error)) {
			g_critical(_("Could not print actual profile: %s"), local_error->message);
			retval = EXIT_FAILURE;
		}
		goto exit2;
	}

	if (parameter_just_reset) {
		if (!reset(konepure, &local_error)) {
			g_critical(_("Could not reset device: %s"), local_error->message);
			retval = EXIT_FAILURE;
		}
		goto exit2;
	}

	if (parameter_read_firmware) {
		if (!print_firmware(konepure, &local_error)) {
			g_critical(_("Could not print firmware version: %s"), local_error->message);
			retval = EXIT_FAILURE;
			goto exit2;
		}
	}

	if (parameter_sensor_read) {
		if (!print_sensor_value(konepure, &local_error)) {
			g_critical(_("Could not read sensor register: %s"), local_error->message);
			retval = EXIT_FAILURE;
			goto exit2;
		}
	}

	if (parameter_sensor_write != -1) {
		koneplus_sensor_write_register(konepure, parameter_sensor_register, parameter_sensor_write, &local_error);
		if (local_error) {
			g_critical(_("Could not write sensor register: %s"), local_error->message);
			retval = EXIT_FAILURE;
			goto exit2;
		}
	}

	if (parameter_load != -1) {
		rmp = konepure_rmp_load(konepure, parameter_load - 1, &local_error);
		if (!rmp) {
			g_critical(_("Could not load profile %i: %s"), parameter_load, local_error->message);
			retval = EXIT_FAILURE;
			goto exit2;
		}
	} else if (parameter_in_rmp) {
		rmp = koneplus_rmp_read_with_path(parameter_in_rmp, konepure_rmp_defaults(), &local_error);
		if (!rmp) {
			g_critical(_("Could not read rmp %s: %s"), parameter_in_rmp, local_error->message);
			retval = EXIT_FAILURE;
			goto exit2;
		}
	} else
		rmp = konepure_default_rmp();

	koneplus_rmp_set_modified(rmp);

	if (parameter_save != -1) {
		if (!konepure_rmp_save(konepure, rmp, parameter_save - 1, &local_error)) {
			g_critical(_("Could not save profile %i: %s"), parameter_save, local_error->message);
			retval = EXIT_FAILURE;
			goto exit3;
		}
		konepure_dbus_emit_profile_data_changed_outside_instant(parameter_save);
	}

	if (parameter_out_rmp) {
		if (!koneplus_rmp_write_with_path(parameter_out_rmp, rmp, &local_error)) {
			g_critical(_("Could not write rmp %s: %s"), parameter_out_rmp, local_error->message);
			retval = EXIT_FAILURE;
			goto exit3;
		}
	}

	if (parameter_activate_profile != -1) {
		if (!koneplus_actual_profile_write(konepure, parameter_activate_profile -1, &local_error)) {
			g_critical(_("Could not activate profile %i: %s"), parameter_activate_profile, local_error->message);
			retval = EXIT_FAILURE;
			goto exit3;
		}
		konepure_dbus_emit_profile_changed_outside_instant(parameter_activate_profile);
	}

exit3:
	koneplus_rmp_free(rmp);
exit2:
	g_object_unref(G_OBJECT(konepure));
exit1:
	commandline_free(context);
	g_clear_error(&local_error);
	exit(retval);
}
