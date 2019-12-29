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

#include "nyth_firmware.h"
#include "nyth_dbus_services.h"
#include "nyth_device.h"
#include "nyth_device_state.h"
#include "nyth_info.h"
#include "nyth_profile.h"
#include "nyth.h"
#include "roccat_helper.h"
#include "roccat_secure.h"
#include "config.h"
#include "i18n.h"
#include <stdlib.h>

static gboolean parameter_just_activate_driver_state = FALSE;
static gboolean parameter_just_deactivate_driver_state = FALSE;
static gchar *parameter_just_update_firmware = NULL;
static gboolean parameter_just_print_actual_profile = FALSE;
static gboolean parameter_just_print_version = FALSE;
static gboolean parameter_just_reset = FALSE;
static gboolean parameter_read_firmware = FALSE;
static gint parameter_activate_profile = -1;

static GOptionEntry entries[] = {
	{ "activate-profile", 'a', 0, G_OPTION_ARG_INT, &parameter_activate_profile, N_("activate profile NUMBER"), N_("NUMBER") },
	{ "actual-profile", 0, 0, G_OPTION_ARG_NONE, &parameter_just_print_actual_profile, N_("print actual profile"), NULL },
	{ "driver-off", 0, 0, G_OPTION_ARG_NONE, &parameter_just_deactivate_driver_state, N_("deactivate Windows driver state"), NULL },
	{ "driver-on", 0, 0, G_OPTION_ARG_NONE, &parameter_just_activate_driver_state, N_("activate Windows driver state"), NULL },
	{ "firmware", 0, 0, G_OPTION_ARG_FILENAME, &parameter_just_update_firmware, N_("update firmware with FILENAME"), N_("FILENAME") },
	{ "read-firmware", 'f', 0, G_OPTION_ARG_NONE, &parameter_read_firmware, N_("read firmware version"), NULL },
	{ "reset", 0, 0, G_OPTION_ARG_NONE, &parameter_just_reset, N_("reset to factory defaults"), NULL },
	{ "version", 'v', 0, G_OPTION_ARG_NONE, &parameter_just_print_version, N_("print version"), NULL },
	{ NULL }
};

static gboolean parse_profile_number_parameter(gint number, GError **error) {
	if (number < 1 || number > NYTH_PROFILE_NUM) {
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
		g_set_error(error, G_OPTION_ERROR, G_OPTION_ERROR_FAILED, _("Just give one of -v, --actual-profile, --driver-off, --driver-on, --reset, --firmware"));
		return FALSE;
	}

	if (parameter_activate_profile != -1 && !parse_profile_number_parameter(parameter_activate_profile, error))
		return FALSE;

	return TRUE;
}

static GOptionContext *commandline_parse(int *argc, char ***argv) {
	GOptionContext *context = NULL;
	GError *error = NULL;
	gchar *string;

	string = g_strdup_printf(_("- controls extended capabilities of Roccat %s mice"), NYTH_DEVICE_NAME_COMBINED);
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
	g_option_context_free(context);
}

static gboolean print_firmware(RoccatDevice *nyth, GError **error) {
	guint firmware_version;
	gchar *firmware_string;

	firmware_version = nyth_firmware_version_read(nyth, error);
	if (*error)
		return FALSE;

	firmware_string = roccat_firmware_version_to_string(firmware_version);
	g_print("%s\n", firmware_string);
	g_free(firmware_string);

	return TRUE;
}

static gboolean reset(RoccatDevice *nyth, GError **error) {
	DBusGProxy *proxy;
	guint i;

	if (!nyth_reset(nyth, error))
		return FALSE;

	proxy = nyth_dbus_proxy_new();
	if (!proxy)
		return TRUE;

	for (i = 0; i < NYTH_PROFILE_NUM; ++i)
		nyth_dbus_emit_profile_data_changed_outside(proxy, i + 1);
	dbus_roccat_proxy_free(proxy);

	return TRUE;
}

int main(int argc, char **argv) {
	GOptionContext *context;
	RoccatDevice *nyth;
	NythProfile *profile;
	GError *local_error = NULL;
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

	nyth = nyth_device_first();
	if (nyth == NULL) {
		g_critical(_("No %s found."), NYTH_DEVICE_NAME_COMBINED);
		retval = EXIT_FAILURE;
		goto exit1;
	}

	if (parameter_just_activate_driver_state) {
		if (!nyth_device_state_write(nyth, NYTH_DEVICE_STATE_STATE_ON, &local_error)) {
			g_critical(_("Could not activate driver state: %s"), local_error->message);
			retval = EXIT_FAILURE;
		}
		goto exit2;
	}

	if (parameter_just_deactivate_driver_state) {
		if (!nyth_device_state_write(nyth, NYTH_DEVICE_STATE_STATE_OFF, &local_error)) {
			g_critical(_("Could not deactivate driver state: %s"), local_error->message);
			retval = EXIT_FAILURE;
		}
		goto exit2;
	}

	if (parameter_just_reset) {
		if (!reset(nyth, &local_error)) {
			g_critical(_("Could not reset device: %s"), local_error->message);
			retval = EXIT_FAILURE;
		}
		goto exit2;
	}

	if (parameter_just_update_firmware) {
		if (!roccat_firmware_update_console(nyth, NYTH_INTERFACE_MOUSE, parameter_just_update_firmware,
				NYTH_FIRMWARE_SIZE, ROCCAT_FIRMWARE_NUMBER_DEFAULT,
				NYTH_FIRMWARE_UPDATE_WAIT_PREPARE, NYTH_FIRMWARE_UPDATE_WAIT_INIT_DATA, NYTH_FIRMWARE_UPDATE_WAIT_DATA, NYTH_FIRMWARE_UPDATE_FINAL_WRITE_CHECK)) {
			retval = EXIT_FAILURE;
		}
		goto exit2;
	}

	if (parameter_read_firmware) {
		if (!print_firmware(nyth, &local_error)) {
			g_critical(_("Could not print firmware version: %s"), local_error->message);
			retval = EXIT_FAILURE;
			goto exit2;
		}
	}

	profile = nyth_profile_read(nyth, &local_error);
	if (!profile) {
		g_critical(_("Could not read profile: %s"), local_error->message);
		retval = EXIT_FAILURE;
		goto exit2;
	}

	if (parameter_just_print_actual_profile) {
		g_print("%i\n", profile->index + 1);
		goto exit3;
	}

	if (parameter_activate_profile != -1) {
		profile->index = parameter_activate_profile - 1;
		if (!nyth_profile_write(nyth, profile, &local_error)) {
			g_critical(_("Could not activate profile %i: %s"), parameter_activate_profile, local_error->message);
			retval = EXIT_FAILURE;
			goto exit3;
		}
		nyth_dbus_emit_profile_changed_outside_instant(parameter_activate_profile);
	}

exit3:
	g_free(profile);
exit2:
	g_object_unref(G_OBJECT(nyth));
exit1:
	commandline_free(context);
	g_clear_error(&local_error);
	exit(retval);
}
