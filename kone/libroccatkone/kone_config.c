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

#include "kone_internal.h"
#include "g_roccat_helper.h"
#include "roccat_notificator.h"
#include "roccat_config.h"

static gchar const * const group_name = "KoneGUI";
static gchar const * const rmp_save_path_key = "RmpFilePath";
static gchar const * const only_notify_on_osd_key = "OnlyNotifyOnOSD";
static gchar const * const profile_notification_type_key = "ProfileNotificationType";
static gchar const * const cpi_notification_type_key = "CpiNotificationType";
static gchar const * const default_profile_number_key = "DefaultProfileNumber";
static gchar const * const notification_volume_key = "NotificationVolume";

/*!
 * \brief Returns path of configuration file
 * \retval Path that has to be freed with g_free()
 */
static gchar *configuration_path(void) {
	gchar *dir = roccat_configuration_dir();
	gchar *path = g_build_filename(dir, "kone.ini", NULL);
	g_free(dir);
	return path;
}

RoccatKeyFile *kone_configuration_load(void) {
	RoccatKeyFile *config;
	gchar *path;

	path = configuration_path();
	config = roccat_key_file_load(path);
	g_free(path);

	return config;
}

void kone_configuration_free(RoccatKeyFile *config) {
	if (config == NULL)
		return;

	roccat_key_file_free(config);
}

gboolean kone_configuration_save(RoccatKeyFile *config, GError **error) {
	if (config == NULL)
		return TRUE;

	return roccat_key_file_save(config, error);
}

gchar *kone_configuration_get_rmp_save_path(RoccatKeyFile *config) {
	return roccat_key_file_get_string_with_default(config, group_name, rmp_save_path_key, g_get_home_dir());
}

void kone_configuration_set_rmp_save_path(RoccatKeyFile *config, gchar const *path) {
	roccat_key_file_set_string(config, group_name, rmp_save_path_key, path);
}

gboolean kone_configuration_get_only_notify_on_osd(RoccatKeyFile *config) {
	return roccat_key_file_get_boolean_with_default(config, group_name, only_notify_on_osd_key, TRUE);
}

void kone_configuration_set_only_notify_on_osd(RoccatKeyFile *config, gboolean new_value) {
	roccat_key_file_set_boolean(config, group_name, only_notify_on_osd_key, new_value);
}

gint kone_configuration_get_profile_notification_type(RoccatKeyFile *config) {
	return roccat_key_file_get_integer_with_default(config, group_name, profile_notification_type_key, ROCCAT_NOTIFICATION_TYPE_OSD);
}

void kone_configuration_set_profile_notification_type(RoccatKeyFile *config, gint new_value) {
	roccat_key_file_set_integer(config, group_name, profile_notification_type_key, new_value);
}

gint kone_configuration_get_cpi_notification_type(RoccatKeyFile *config) {
	return roccat_key_file_get_integer_with_default(config, group_name, cpi_notification_type_key, ROCCAT_NOTIFICATION_TYPE_OSD);
}

void kone_configuration_set_cpi_notification_type(RoccatKeyFile *config, gint new_value) {
	roccat_key_file_set_integer(config, group_name, cpi_notification_type_key, new_value);
}

gdouble kone_configuration_get_notification_volume(RoccatKeyFile *config) {
	return roccat_key_file_get_double_with_default(config, group_name, notification_volume_key, 0);
}

void kone_configuration_set_notification_volume(RoccatKeyFile *config, gdouble new_value) {
	roccat_key_file_set_double(config, group_name, notification_volume_key, new_value);
}

gint kone_configuration_get_default_profile_number(RoccatKeyFile *config) {
	return roccat_key_file_get_integer_with_default(config, group_name, default_profile_number_key, 0);
}

void kone_configuration_set_default_profile_number(RoccatKeyFile *config, gint new_value) {
	roccat_key_file_set_integer(config, group_name, default_profile_number_key, new_value);
}
