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

#include "skeltr_profile_data.h"
#include "skeltr.h"
#include "skeltr_rkp_sound_feedback.h"
#include "roccat_swarm_rmp.h"
#include "roccat_helper.h"
#include "roccat_talk.h"
#include "roccat_notificator.h"
#include "i18n-lib.h"
#include <glib/gstdio.h>
#include <string.h>
#include <errno.h>

#define SKELTR_PROFILE_DATA_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), SKELTR_PROFILE_DATA_TYPE, SkeltrProfileDataPrivate))

typedef struct _SkeltrProfileDataPrivateData SkeltrProfileDataPrivateData;
typedef struct _SkeltrProfileDataPrivateDataEventhandler SkeltrProfileDataPrivateDataEventhandler;
typedef struct _SkeltrProfileDataPrivateDataHardware SkeltrProfileDataPrivateDataHardware;

struct _SkeltrProfileDataPrivateDataEventhandler {
	gboolean modified;
	gchar profile_name[ROCCAT_SWARM_RMP_PROFILE_NAME_LENGTH + 1];
	RoccatSwarmGamefile gamefile_names[GAMINGGEAR_PROFILE_DATA_GAMEFILE_NUM];
	RoccatSwarmOpener openers[SKELTR_MACRO_NUM];
	gint profile_index;
	guint16 capslock_talk_target;
	RoccatTimer timers[SKELTR_MACRO_NUM];
	guint sound_feedback_typing; // SkeltrRkpSoundFeedbackTypingSound
	guint wakeup_notification_type; // RoccatNotificationType
	guint sleep_notification_type; // RoccatNotificationType
	guint timer_notification_type; // RoccatNotificationType
	guint macro_record_notification_type; // RoccatNotificationType
	guint profile_switch_notification_type; // RoccatNotificationType
	gdouble notification_volume;
};

struct _SkeltrProfileDataPrivateDataHardware {
	gboolean modified_key_mask;
	SkeltrKeyMask key_mask;

	gboolean modified_keys_easyzone;
	SkeltrKeysEasyzone keys_easyzone;

	gboolean modified_keys_extra;
	SkeltrKeysExtra keys_extra;

	gboolean modified_keys_function;
	SkeltrKeysFunction keys_function;

	gboolean modified_keys_macro;
	SkeltrKeysMacro keys_macro;

	gboolean modified_keys_primary;
	SkeltrKeysPrimary keys_primary;

	gboolean modified_keys_thumbster;
	SkeltrKeysThumbster keys_thumbster;

	gboolean modified_light;
	SkeltrLight light;

	gboolean modified_macros[SKELTR_MACRO_NUM];
	SkeltrMacro macros[SKELTR_MACRO_NUM];
};

struct _SkeltrProfileDataPrivateData {
	SkeltrProfileDataPrivateDataEventhandler eventhandler;
	SkeltrProfileDataPrivateDataHardware hardware;
};

struct _SkeltrProfileDataPrivate {
	SkeltrProfileDataPrivateData *original;
	SkeltrProfileDataPrivateData *updated;
};

G_DEFINE_TYPE(SkeltrProfileData, skeltr_profile_data, GAMINGGEAR_PROFILE_DATA_TYPE);

static RoccatButton *get_key_for_macro_index(SkeltrProfileDataPrivateData *data, guint macro_index) {
	if (skeltr_macro_index_is_keys_easyzone(macro_index))
		return &data->hardware.keys_easyzone.keys[skeltr_macro_index_to_keys_easyzone_index(macro_index)];
	else if (skeltr_macro_index_is_keys_macro(macro_index))
		return &data->hardware.keys_macro.keys[skeltr_macro_index_to_keys_macro_index(macro_index)];
	else if (skeltr_macro_index_is_keys_thumbster(macro_index))
		return &data->hardware.keys_thumbster.keys[skeltr_macro_index_to_keys_thumbster_index(macro_index)];
	else
		return NULL;
}

RoccatButton const *skeltr_profile_data_get_key_for_macro_index(SkeltrProfileData const *profile_data, guint macro_index) {
	return get_key_for_macro_index(profile_data->priv->updated, macro_index);
}

static gboolean is_macro(GaminggearProfileData *self, guint macro_index) {
	RoccatButton const *key = skeltr_profile_data_get_key_for_macro_index(SKELTR_PROFILE_DATA(self), macro_index);
	if (!key)
		return FALSE;
	return key->type == SKELTR_KEY_TYPE_MACRO;
}

static gboolean skeltr_profile_data_private_data_eventhandler_get_modified(SkeltrProfileDataPrivateDataEventhandler const *data) {
	return data->modified;
}

static void skeltr_profile_data_private_data_eventhandler_set_modified(SkeltrProfileDataPrivateDataEventhandler *data, gboolean state) {
	data->modified = state;
}

static gboolean skeltr_profile_data_private_data_hardware_get_modified(SkeltrProfileDataPrivateDataHardware const *data) {
	guint i;
	
	if (data->modified_key_mask ||
			data->modified_keys_easyzone ||
			data->modified_keys_extra ||
			data->modified_keys_function ||
			data->modified_keys_macro ||
			data->modified_keys_primary ||
			data->modified_keys_thumbster ||
			data->modified_light)
		return TRUE;
		
	for (i = 0; i < SKELTR_MACRO_NUM; ++i)
		if (data->modified_macros[i])
			return TRUE;

	return FALSE;
}

static void skeltr_profile_data_private_data_hardware_set_modified(SkeltrProfileDataPrivateDataHardware *data, gboolean state) {
	guint i;
	
	data->modified_key_mask = state;
	data->modified_keys_easyzone = state;
	data->modified_keys_extra = state;
	data->modified_keys_function = state;
	data->modified_keys_macro = state;
	data->modified_keys_primary = state;
	data->modified_keys_thumbster = state;
	data->modified_light = state;

	for (i = 0; i < SKELTR_MACRO_NUM; ++i)
		data->modified_macros[i] = state;
}

static gboolean skeltr_profile_data_get_modified(GaminggearProfileData *self) {
	SkeltrProfileDataPrivateData *updated = SKELTR_PROFILE_DATA(self)->priv->updated;
	
	if (skeltr_profile_data_private_data_eventhandler_get_modified(&updated->eventhandler))
		return TRUE;
	
	return skeltr_profile_data_private_data_hardware_get_modified(&updated->hardware);
}

static void skeltr_profile_data_reset(GaminggearProfileData *self) {
	SkeltrProfileDataPrivate *priv = SKELTR_PROFILE_DATA(self)->priv;
	*priv->updated = *priv->original;
}

static gchar const *skeltr_profile_data_get_name(GaminggearProfileData *self) {
	return SKELTR_PROFILE_DATA(self)->priv->updated->eventhandler.profile_name;
}

static void skeltr_profile_data_set_name(GaminggearProfileData *self, gchar const *new_name) {
	SkeltrProfileDataPrivateDataEventhandler *eventhandler = &SKELTR_PROFILE_DATA(self)->priv->updated->eventhandler;

	if (strncmp(eventhandler->profile_name, new_name, ROCCAT_SWARM_RMP_PROFILE_NAME_LENGTH)) {
		g_strlcpy(eventhandler->profile_name, new_name, ROCCAT_SWARM_RMP_PROFILE_NAME_LENGTH + 1);
		eventhandler->modified = TRUE;
	}
}

static gchar const *skeltr_profile_data_get_gamefile(GaminggearProfileData *self, guint index) {
	return SKELTR_PROFILE_DATA(self)->priv->updated->eventhandler.gamefile_names[index];
}

static void skeltr_profile_data_set_gamefile(GaminggearProfileData *self, guint index, gchar const *new_gamefile) {
	SkeltrProfileDataPrivateDataEventhandler *eventhandler = &SKELTR_PROFILE_DATA(self)->priv->updated->eventhandler;

	if (strncmp(eventhandler->gamefile_names[index], new_gamefile, ROCCAT_SWARM_RMP_GAMEFILE_LENGTH)) {
		g_strlcpy(eventhandler->gamefile_names[index], new_gamefile, ROCCAT_SWARM_RMP_GAMEFILE_LENGTH);
		eventhandler->modified = TRUE;
	}
}

static gint skeltr_profile_data_get_hardware_index(GaminggearProfileData *self) {
	return SKELTR_PROFILE_DATA(self)->priv->updated->eventhandler.profile_index;
}

static void skeltr_profile_data_set_hardware_index(GaminggearProfileData *self, gint new_index) {
	SkeltrProfileDataPrivateData *updated = SKELTR_PROFILE_DATA(self)->priv->updated;

	if (updated->eventhandler.profile_index != new_index) {
		updated->eventhandler.profile_index = new_index;
		skeltr_profile_data_private_data_eventhandler_set_modified(&updated->eventhandler, TRUE);
		skeltr_profile_data_private_data_hardware_set_modified(&updated->hardware, TRUE);
		// FIXME set profile_index also in original data?
	}
}

static gchar *skeltr_profile_data_path(void) {
	gchar *base = roccat_profile_dir();
	gchar *dir = g_build_path("/", base, "skeltr", NULL);
	g_free(base);
	return dir;
}

static gboolean skeltr_profile_dir_create_if_needed(GError **error) {
	gchar *dir;
	gboolean retval;

	dir = skeltr_profile_data_path();
	retval = roccat_profile_dir_create_if_needed(dir, error);
	g_free(dir);
	return retval;
}

static gchar *skeltr_build_profile_path(guint profile_index) {
	gchar *dir = skeltr_profile_data_path();
	gchar *filename = g_strdup_printf("actual%i", profile_index);
	gchar *path = g_build_path("/", dir, filename, NULL);
	g_free(filename);
	g_free(dir);
	return path;
}

static gboolean skeltr_profile_data_load_with_path(GaminggearProfileData *self, gchar const *path, GError **error) {
	SkeltrProfileDataPrivate *priv = SKELTR_PROFILE_DATA(self)->priv;
	gchar *data;
	gsize length;

	if (!g_file_get_contents(path, &data, &length, error))
		return FALSE;

	if (length != sizeof(SkeltrProfileDataPrivateDataEventhandler)) {
		g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_INVAL, _("Data has wrong length, %zu instead of %zu"),
				(gsize)length, (gsize)sizeof(SkeltrProfileDataPrivateDataEventhandler));
		g_free(data);
		return FALSE;
	}

	priv->updated->eventhandler = *(SkeltrProfileDataPrivateDataEventhandler *)data;

	skeltr_profile_data_private_data_eventhandler_set_modified(&priv->updated->eventhandler, FALSE);
	skeltr_profile_data_private_data_hardware_set_modified(&priv->updated->hardware, FALSE);
	*priv->original = *priv->updated;

	return TRUE;
}

gboolean skeltr_profile_data_load(SkeltrProfileData *self, guint profile_index, GError **error) {
	gchar *path;
	gboolean retval;

	self->priv->updated->eventhandler.profile_index = profile_index;

	path = skeltr_build_profile_path(profile_index);
	/* Not calling skeltr_profile_data_load_with_path directly to get error checking from GaminggearProfileData */
	retval = gaminggear_profile_data_load(GAMINGGEAR_PROFILE_DATA(self), path, error);
	g_free(path);

	return retval;
}

static gboolean skeltr_profile_data_save(GaminggearProfileData *self, GError **error) {
	SkeltrProfileDataPrivate *priv = SKELTR_PROFILE_DATA(self)->priv;
	gchar *path;
	gboolean retval;

	if (!skeltr_profile_dir_create_if_needed(error))
		return FALSE;

	path = skeltr_build_profile_path(priv->updated->eventhandler.profile_index);
	retval = roccat_profile_write_with_path(path, (gchar const *)&priv->updated->eventhandler, sizeof(SkeltrProfileDataPrivateDataEventhandler), error);
	g_free(path);

	if (retval)
		priv->updated->eventhandler.modified = FALSE;

	/* Copy everything since device might not be present. */
	*priv->original = *priv->updated;

	return retval;
}

static gboolean skeltr_profile_data_remove(GaminggearProfileData *self, GError **error) {
	SkeltrProfileDataPrivate *priv = SKELTR_PROFILE_DATA(self)->priv;
	gchar *path;

	path = skeltr_build_profile_path(priv->updated->eventhandler.profile_index);
	if (g_unlink(path) == -1)
		g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(errno), _("Could not remove file %1$s: %2$s"), path, g_strerror(errno));
	g_free(path);

	g_clear_pointer(&priv->original, g_free);
	g_clear_pointer(&priv->updated, g_free);

	return TRUE;
}

static gboolean skeltr_profile_data_write_key_mask(GaminggearProfileData *self, RoccatDevice *device, GError **error) {
	SkeltrProfileDataPrivateData *updated = SKELTR_PROFILE_DATA(self)->priv->updated;
	gboolean retval;

	if (!updated->hardware.modified_key_mask)
		return TRUE;

	retval = skeltr_key_mask_write(device, updated->eventhandler.profile_index, &updated->hardware.key_mask, error);

	if (retval)
		updated->hardware.modified_key_mask = FALSE;

	return retval;
}

static gboolean skeltr_profile_data_write_keys_easyzone(GaminggearProfileData *self, RoccatDevice *device, GError **error) {
	SkeltrProfileDataPrivateData *updated = SKELTR_PROFILE_DATA(self)->priv->updated;
	gboolean retval;

	if (!updated->hardware.modified_keys_easyzone)
		return TRUE;

	retval = skeltr_keys_easyzone_write(device, updated->eventhandler.profile_index, &updated->hardware.keys_easyzone, error);

	if (retval)
		updated->hardware.modified_keys_easyzone = FALSE;

	return retval;
}

static gboolean skeltr_profile_data_write_keys_extra(GaminggearProfileData *self, RoccatDevice *device, GError **error) {
	SkeltrProfileDataPrivateData *updated = SKELTR_PROFILE_DATA(self)->priv->updated;
	gboolean retval;

	if (!updated->hardware.modified_keys_extra)
		return TRUE;

	retval = skeltr_keys_extra_write(device, updated->eventhandler.profile_index, &updated->hardware.keys_extra, error);

	if (retval)
		updated->hardware.modified_keys_extra = FALSE;

	return retval;
}

static gboolean skeltr_profile_data_write_keys_function(GaminggearProfileData *self, RoccatDevice *device, GError **error) {
	SkeltrProfileDataPrivateData *updated = SKELTR_PROFILE_DATA(self)->priv->updated;
	gboolean retval;

	if (!updated->hardware.modified_keys_function)
		return TRUE;

	retval = skeltr_keys_function_write(device, updated->eventhandler.profile_index, &updated->hardware.keys_function, error);

	if (retval)
		updated->hardware.modified_keys_function = FALSE;

	return retval;
}

static gboolean skeltr_profile_data_write_keys_macro(GaminggearProfileData *self, RoccatDevice *device, GError **error) {
	SkeltrProfileDataPrivateData *updated = SKELTR_PROFILE_DATA(self)->priv->updated;
	gboolean retval;

	if (!updated->hardware.modified_keys_macro)
		return TRUE;

	retval = skeltr_keys_macro_write(device, updated->eventhandler.profile_index, &updated->hardware.keys_macro, error);

	if (retval)
		updated->hardware.modified_keys_macro = FALSE;

	return retval;
}

static gboolean skeltr_profile_data_write_keys_primary(GaminggearProfileData *self, RoccatDevice *device, GError **error) {
	SkeltrProfileDataPrivateData *updated = SKELTR_PROFILE_DATA(self)->priv->updated;
	gboolean retval;

	if (!updated->hardware.modified_keys_primary)
		return TRUE;

	retval = skeltr_keys_primary_write(device, updated->eventhandler.profile_index, &updated->hardware.keys_primary, error);

	if (retval)
		updated->hardware.modified_keys_primary = FALSE;

	return retval;
}

static gboolean skeltr_profile_data_write_keys_thumbster(GaminggearProfileData *self, RoccatDevice *device, GError **error) {
	SkeltrProfileDataPrivateData *updated = SKELTR_PROFILE_DATA(self)->priv->updated;
	gboolean retval;

	if (!updated->hardware.modified_keys_thumbster)
		return TRUE;

	retval = skeltr_keys_thumbster_write(device, updated->eventhandler.profile_index, &updated->hardware.keys_thumbster, error);

	if (retval)
		updated->hardware.modified_keys_thumbster = FALSE;

	return retval;
}

static gboolean skeltr_profile_data_write_light(GaminggearProfileData *self, RoccatDevice *device, GError **error) {
	SkeltrProfileDataPrivateData *updated = SKELTR_PROFILE_DATA(self)->priv->updated;
	gboolean retval;

	if (!updated->hardware.modified_light)
		return TRUE;

	retval = skeltr_light_write(device, updated->eventhandler.profile_index, &updated->hardware.light, error);

	if (retval)
		updated->hardware.modified_light = FALSE;

	return retval;
}

static gboolean skeltr_profile_data_write_macros(GaminggearProfileData *self, RoccatDevice *device, GError **error) {
	SkeltrProfileDataPrivateData *updated = SKELTR_PROFILE_DATA(self)->priv->updated;
	guint macro_index;

	for (macro_index = 0; macro_index < SKELTR_MACRO_NUM; ++macro_index) {
		if (!updated->hardware.modified_macros[macro_index])
			continue;

		if (!is_macro(self, macro_index))
			continue;
		
		if (!skeltr_macro_write(device, updated->eventhandler.profile_index, macro_index, &updated->hardware.macros[macro_index], error))
			return FALSE;

		/* Always set macro unmodified */
		updated->hardware.modified_macros[macro_index] = FALSE;
	}

	return TRUE;
}

static gboolean skeltr_profile_data_write(GaminggearProfileData *self, GaminggearDevice *device, GError **error) {
	SkeltrProfileDataPrivate *priv = SKELTR_PROFILE_DATA(self)->priv;
	RoccatDevice *roccat_device = ROCCAT_DEVICE(device);

	if (!skeltr_profile_data_write_key_mask(self, roccat_device, error)) return FALSE;
	if (!skeltr_profile_data_write_keys_easyzone(self, roccat_device, error)) return FALSE;
	if (!skeltr_profile_data_write_keys_extra(self, roccat_device, error)) return FALSE;
	if (!skeltr_profile_data_write_keys_function(self, roccat_device, error)) return FALSE;
	if (!skeltr_profile_data_write_keys_macro(self, roccat_device, error)) return FALSE;
	if (!skeltr_profile_data_write_keys_primary(self, roccat_device, error)) return FALSE;
	if (!skeltr_profile_data_write_keys_thumbster(self, roccat_device, error)) return FALSE;
	if (!skeltr_profile_data_write_light(self, roccat_device, error)) return FALSE;
	if (!skeltr_profile_data_write_macros(self, roccat_device, error)) return FALSE;

	priv->original->hardware = priv->updated->hardware;

	return TRUE;
}

static gboolean skeltr_profile_data_read_key_mask(GaminggearProfileData *self, RoccatDevice *device, GError **error) {
	SkeltrProfileDataPrivateData *updated = SKELTR_PROFILE_DATA(self)->priv->updated;
	SkeltrKeyMask *key_mask = skeltr_key_mask_read(device, updated->eventhandler.profile_index, error);

	if (key_mask == NULL)
		return FALSE;
	skeltr_key_mask_copy(&updated->hardware.key_mask, key_mask);
	g_free(key_mask);

	updated->hardware.modified_key_mask = FALSE;

	return TRUE;
}

static gboolean skeltr_profile_data_read_keys_easyzone(GaminggearProfileData *self, RoccatDevice *device, GError **error) {
	SkeltrProfileDataPrivateData *updated = SKELTR_PROFILE_DATA(self)->priv->updated;
	SkeltrKeysEasyzone *keys_easyzone = skeltr_keys_easyzone_read(device, updated->eventhandler.profile_index, error);

	if (keys_easyzone == NULL)
		return FALSE;
	skeltr_keys_easyzone_copy(&updated->hardware.keys_easyzone, keys_easyzone);
	g_free(keys_easyzone);

	updated->hardware.modified_keys_easyzone = FALSE;

	return TRUE;
}

static gboolean skeltr_profile_data_read_keys_extra(GaminggearProfileData *self, RoccatDevice *device, GError **error) {
	SkeltrProfileDataPrivateData *updated = SKELTR_PROFILE_DATA(self)->priv->updated;
	SkeltrKeysExtra *keys_extra = skeltr_keys_extra_read(device, updated->eventhandler.profile_index, error);

	if (keys_extra == NULL)
		return FALSE;
	skeltr_keys_extra_copy(&updated->hardware.keys_extra, keys_extra);
	g_free(keys_extra);

	updated->hardware.modified_keys_extra = FALSE;

	return TRUE;
}

static gboolean skeltr_profile_data_read_keys_function(GaminggearProfileData *self, RoccatDevice *device, GError **error) {
	SkeltrProfileDataPrivateData *updated = SKELTR_PROFILE_DATA(self)->priv->updated;
	SkeltrKeysFunction *keys_function = skeltr_keys_function_read(device, updated->eventhandler.profile_index, error);

	if (keys_function == NULL)
		return FALSE;
	skeltr_keys_function_copy(&updated->hardware.keys_function, keys_function);
	g_free(keys_function);

	updated->hardware.modified_keys_function = FALSE;

	return TRUE;
}

static gboolean skeltr_profile_data_read_keys_macro(GaminggearProfileData *self, RoccatDevice *device, GError **error) {
	SkeltrProfileDataPrivateData *updated = SKELTR_PROFILE_DATA(self)->priv->updated;
	SkeltrKeysMacro *keys_macro = skeltr_keys_macro_read(device, updated->eventhandler.profile_index, error);

	if (keys_macro == NULL)
		return FALSE;
	skeltr_keys_macro_copy(&updated->hardware.keys_macro, keys_macro);
	g_free(keys_macro);

	updated->hardware.modified_keys_macro = FALSE;

	return TRUE;
}

static gboolean skeltr_profile_data_read_keys_primary(GaminggearProfileData *self, RoccatDevice *device, GError **error) {
	SkeltrProfileDataPrivateData *updated = SKELTR_PROFILE_DATA(self)->priv->updated;
	SkeltrKeysPrimary *keys_primary = skeltr_keys_primary_read(device, updated->eventhandler.profile_index, error);

	if (keys_primary == NULL)
		return FALSE;
	skeltr_keys_primary_copy(&updated->hardware.keys_primary, keys_primary);
	g_free(keys_primary);

	updated->hardware.modified_keys_primary = FALSE;

	return TRUE;
}

static gboolean skeltr_profile_data_read_keys_thumbster(GaminggearProfileData *self, RoccatDevice *device, GError **error) {
	SkeltrProfileDataPrivateData *updated = SKELTR_PROFILE_DATA(self)->priv->updated;
	SkeltrKeysThumbster *keys_thumbster = skeltr_keys_thumbster_read(device, updated->eventhandler.profile_index, error);

	if (keys_thumbster == NULL)
		return FALSE;
	skeltr_keys_thumbster_copy(&updated->hardware.keys_thumbster, keys_thumbster);
	g_free(keys_thumbster);

	updated->hardware.modified_keys_thumbster = FALSE;

	return TRUE;
}

static gboolean skeltr_profile_data_read_light(GaminggearProfileData *self, RoccatDevice *device, GError **error) {
	SkeltrProfileDataPrivateData *updated = SKELTR_PROFILE_DATA(self)->priv->updated;
	SkeltrLight *light = skeltr_light_read(device, updated->eventhandler.profile_index, error);

	if (light == NULL)
		return FALSE;
	skeltr_light_copy(&updated->hardware.light, light);
	g_free(light);

	updated->hardware.modified_light = FALSE;

	return TRUE;
}

static gboolean skeltr_profile_data_read_macros(GaminggearProfileData *self, RoccatDevice *device, GError **error) {
	SkeltrProfileDataPrivateData *updated = SKELTR_PROFILE_DATA(self)->priv->updated;
	guint macro_index;
	SkeltrMacro *macro;

	for (macro_index = 0; macro_index < SKELTR_MACRO_NUM; ++macro_index) {
		if (is_macro(self, macro_index)) {
			macro = skeltr_macro_read(device, updated->eventhandler.profile_index, macro_index, error);
			if (!macro)
				return FALSE;

			skeltr_macro_copy(&updated->hardware.macros[macro_index], macro);
		}

		updated->hardware.modified_macros[macro_index] = FALSE;
	}

	return TRUE;
}

static gboolean skeltr_profile_data_read(GaminggearProfileData *self, GaminggearDevice *device, GError **error) {
	SkeltrProfileDataPrivate *priv = SKELTR_PROFILE_DATA(self)->priv;
	RoccatDevice *roccat_device = ROCCAT_DEVICE(device);

	if (!skeltr_profile_data_read_key_mask(self, roccat_device, error)) return FALSE;
	if (!skeltr_profile_data_read_keys_easyzone(self, roccat_device, error)) return FALSE;
	if (!skeltr_profile_data_read_keys_extra(self, roccat_device, error)) return FALSE;
	if (!skeltr_profile_data_read_keys_function(self, roccat_device, error)) return FALSE;
	if (!skeltr_profile_data_read_keys_macro(self, roccat_device, error)) return FALSE;
	if (!skeltr_profile_data_read_keys_primary(self, roccat_device, error)) return FALSE;
	if (!skeltr_profile_data_read_keys_thumbster(self, roccat_device, error)) return FALSE;
	if (!skeltr_profile_data_read_light(self, roccat_device, error)) return FALSE;
	if (!skeltr_profile_data_read_macros(self, roccat_device, error)) return FALSE;

	priv->original->hardware = priv->updated->hardware;

	return TRUE;
}

static gboolean skeltr_profile_data_equal(GaminggearProfileData *self, GaminggearProfileData *other) {
	SkeltrProfileDataPrivate *self_priv = SKELTR_PROFILE_DATA(self)->priv;
	SkeltrProfileDataPrivate *other_priv = SKELTR_PROFILE_DATA(other)->priv;

	return (memcmp(self_priv->updated, other_priv->updated, sizeof(SkeltrProfileDataPrivateData)) == 0) ? TRUE : FALSE;
}

static void skeltr_profile_data_copy(GaminggearProfileData *self, GaminggearProfileData *other) {
	SkeltrProfileDataPrivate *self_priv = SKELTR_PROFILE_DATA(self)->priv;
	SkeltrProfileDataPrivate *other_priv = SKELTR_PROFILE_DATA(other)->priv;

	GAMINGGEAR_PROFILE_DATA_CLASS(skeltr_profile_data_parent_class)->copy(self, other);
	*self_priv->updated = *other_priv->updated;
	*self_priv->original = *other_priv->original;
}

static void skeltr_profile_data_discriminate(GaminggearProfileData *self) {
	SkeltrProfileDataPrivate *self_priv = SKELTR_PROFILE_DATA(self)->priv;
	gchar *new_name;

	GAMINGGEAR_PROFILE_DATA_CLASS(skeltr_profile_data_parent_class)->discriminate(self);

	new_name = g_strdup_printf("%s_new", skeltr_profile_data_get_name(self));
	skeltr_profile_data_set_name(self, new_name);
	g_free(new_name);

	self_priv->updated->eventhandler.profile_index = GAMINGGEAR_PROFILE_DATA_GAMEFILE_PROFILE_INVALID;

	skeltr_profile_data_private_data_eventhandler_set_modified(&self_priv->updated->eventhandler, TRUE);
	skeltr_profile_data_private_data_hardware_set_modified(&self_priv->updated->hardware, TRUE);

	*self_priv->original = *self_priv->updated;
}

static void skeltr_profile_data_init(SkeltrProfileData *profile_data) {
	SkeltrProfileDataPrivate *priv = SKELTR_PROFILE_DATA_GET_PRIVATE(profile_data);
	profile_data->priv = priv;

	priv->updated = (SkeltrProfileDataPrivateData *)g_malloc0(sizeof(SkeltrProfileDataPrivateData));

	skeltr_profile_data_set_name(GAMINGGEAR_PROFILE_DATA(profile_data), "Default");
	priv->updated->eventhandler.profile_index = GAMINGGEAR_PROFILE_DATA_GAMEFILE_PROFILE_INVALID;
	priv->updated->eventhandler.notification_volume = 0.0;
	priv->updated->eventhandler.capslock_talk_target = ROCCAT_TALK_DEVICE_MOUSE;
	priv->updated->eventhandler.sound_feedback_typing = SKELTR_RKP_SOUND_FEEDBACK_TYPING_SOUND_OFF;
	priv->updated->eventhandler.wakeup_notification_type = ROCCAT_NOTIFICATION_TYPE_OFF;
	priv->updated->eventhandler.sleep_notification_type = ROCCAT_NOTIFICATION_TYPE_OFF;
	priv->updated->eventhandler.timer_notification_type = ROCCAT_NOTIFICATION_TYPE_OFF;
	priv->updated->eventhandler.macro_record_notification_type = ROCCAT_NOTIFICATION_TYPE_OFF;
	priv->updated->eventhandler.profile_switch_notification_type = ROCCAT_NOTIFICATION_TYPE_OFF;
	skeltr_profile_data_private_data_eventhandler_set_modified(&priv->updated->eventhandler, TRUE);

	skeltr_keys_extra_copy(&priv->updated->hardware.keys_extra, skeltr_keys_extra_default());
	skeltr_key_mask_copy(&priv->updated->hardware.key_mask, skeltr_key_mask_default());
	skeltr_keys_function_copy(&priv->updated->hardware.keys_function, skeltr_keys_function_default());
	skeltr_keys_primary_copy(&priv->updated->hardware.keys_primary, skeltr_keys_primary_default());
	skeltr_keys_easyzone_copy(&priv->updated->hardware.keys_easyzone, skeltr_keys_easyzone_default());
	skeltr_light_copy(&priv->updated->hardware.light, skeltr_light_default());
	skeltr_profile_data_private_data_hardware_set_modified(&priv->updated->hardware, TRUE);

	priv->original = (SkeltrProfileDataPrivateData *)g_malloc0(sizeof(SkeltrProfileDataPrivateData));

	*priv->original = *priv->updated;
}

static void skeltr_profile_data_finalize(GObject *object) {
	SkeltrProfileDataPrivate *priv = SKELTR_PROFILE_DATA(object)->priv;

	g_clear_pointer(&priv->original, g_free);
	g_clear_pointer(&priv->updated, g_free);

	G_OBJECT_CLASS(skeltr_profile_data_parent_class)->finalize(object);
}

static void skeltr_profile_data_class_init(SkeltrProfileDataClass *klass) {
	GObjectClass *gobject_class = (GObjectClass*)klass;
	GaminggearProfileDataClass *parent_class = GAMINGGEAR_PROFILE_DATA_CLASS(klass);

	gobject_class->finalize = skeltr_profile_data_finalize;

	parent_class->get_name = skeltr_profile_data_get_name;
	parent_class->set_name = skeltr_profile_data_set_name;
	parent_class->get_gamefile = skeltr_profile_data_get_gamefile;
	parent_class->set_gamefile = skeltr_profile_data_set_gamefile;
	parent_class->get_hardware_index = skeltr_profile_data_get_hardware_index;
	parent_class->set_hardware_index = skeltr_profile_data_set_hardware_index;
	parent_class->load = skeltr_profile_data_load_with_path;
	parent_class->save = skeltr_profile_data_save;
	parent_class->remove = skeltr_profile_data_remove;
	parent_class->read = skeltr_profile_data_read;
	parent_class->write = skeltr_profile_data_write;
	parent_class->get_modified = skeltr_profile_data_get_modified;
	parent_class->reset = skeltr_profile_data_reset;
	parent_class->copy = skeltr_profile_data_copy;
	parent_class->equal = skeltr_profile_data_equal;
	parent_class->discriminate = skeltr_profile_data_discriminate;

	g_type_class_add_private(klass, sizeof(SkeltrProfileDataPrivate));
}

SkeltrProfileData *skeltr_profile_data_new(void) {
	return SKELTR_PROFILE_DATA(g_object_new(SKELTR_PROFILE_DATA_TYPE, NULL));
}

gboolean skeltr_profile_data_load(SkeltrProfileData *self, guint profile_index, GError **error);

gdouble skeltr_profile_data_get_sound_feedback_volume(SkeltrProfileData const *profile_data) {
	return profile_data->priv->updated->eventhandler.notification_volume;
}

void skeltr_profile_data_set_sound_feedback_volume(SkeltrProfileData *profile_data, gdouble volume) {
	SkeltrProfileDataPrivateDataEventhandler *eventhandler = &profile_data->priv->updated->eventhandler;

	if (eventhandler->notification_volume != volume) {
		eventhandler->notification_volume = volume;
		eventhandler->modified = TRUE;
	}
}

guint skeltr_profile_data_get_sound_feedback_typing(SkeltrProfileData const *profile_data) {
	return profile_data->priv->updated->eventhandler.sound_feedback_typing;
}

void skeltr_profile_data_set_sound_feedback_typing(SkeltrProfileData *profile_data, guint typing) {
	SkeltrProfileDataPrivateDataEventhandler *eventhandler = &profile_data->priv->updated->eventhandler;

	if (eventhandler->sound_feedback_typing != typing) {
		eventhandler->sound_feedback_typing = typing;
		eventhandler->modified = TRUE;
	}
}

guint skeltr_profile_data_get_wakeup_notification_type(SkeltrProfileData const *profile_data) {
	return profile_data->priv->updated->eventhandler.wakeup_notification_type;
}

void skeltr_profile_data_set_wakeup_notification_type(SkeltrProfileData *profile_data, guint wakeup) {
	SkeltrProfileDataPrivateDataEventhandler *eventhandler = &profile_data->priv->updated->eventhandler;

	if (eventhandler->wakeup_notification_type != wakeup) {
		eventhandler->wakeup_notification_type = wakeup;
		eventhandler->modified = TRUE;
	}
}

guint skeltr_profile_data_get_sleep_notification_type(SkeltrProfileData const *profile_data) {
	return profile_data->priv->updated->eventhandler.sleep_notification_type;
}

void skeltr_profile_data_set_sleep_notification_type(SkeltrProfileData *profile_data, guint sleep) {
	SkeltrProfileDataPrivateDataEventhandler *eventhandler = &profile_data->priv->updated->eventhandler;

	if (eventhandler->sleep_notification_type != sleep) {
		eventhandler->sleep_notification_type = sleep;
		eventhandler->modified = TRUE;
	}
}

guint skeltr_profile_data_get_timer_notification_type(SkeltrProfileData const *profile_data) {
	return profile_data->priv->updated->eventhandler.timer_notification_type;
}

void skeltr_profile_data_set_timer_notification_type(SkeltrProfileData *profile_data, guint type) {
	SkeltrProfileDataPrivateDataEventhandler *eventhandler = &profile_data->priv->updated->eventhandler;

	if (eventhandler->timer_notification_type != type) {
		eventhandler->timer_notification_type = type;
		eventhandler->modified = TRUE;
	}
}

guint skeltr_profile_data_get_macro_record_notification_type(SkeltrProfileData const *profile_data) {
	return profile_data->priv->updated->eventhandler.macro_record_notification_type;
}

void skeltr_profile_data_set_macro_record_notification_type(SkeltrProfileData *profile_data, guint type) {
	SkeltrProfileDataPrivateDataEventhandler *eventhandler = &profile_data->priv->updated->eventhandler;

	if (eventhandler->macro_record_notification_type != type) {
		eventhandler->macro_record_notification_type = type;
		eventhandler->modified = TRUE;
	}
}

guint skeltr_profile_data_get_profile_switch_notification_type(SkeltrProfileData const *profile_data) {
	return profile_data->priv->updated->eventhandler.profile_switch_notification_type;
}

void skeltr_profile_data_set_profile_switch_notification_type(SkeltrProfileData *profile_data, guint type) {
	SkeltrProfileDataPrivateDataEventhandler *eventhandler = &profile_data->priv->updated->eventhandler;

	if (eventhandler->profile_switch_notification_type != type) {
		eventhandler->profile_switch_notification_type = type;
		eventhandler->modified = TRUE;
	}
}

SkeltrKeyMask const *skeltr_profile_data_get_key_mask(SkeltrProfileData const *profile_data) {
	return &profile_data->priv->updated->hardware.key_mask;
}

void skeltr_profile_data_set_key_mask(SkeltrProfileData *profile_data, SkeltrKeyMask const *key_mask) {
	SkeltrProfileDataPrivateDataHardware *hardware = &profile_data->priv->updated->hardware;

	if (!skeltr_key_mask_equal(&hardware->key_mask, key_mask)) {
		skeltr_key_mask_copy(&hardware->key_mask, key_mask);
		hardware->modified_key_mask = TRUE;
	}
}

SkeltrKeysEasyzone const *skeltr_profile_data_get_keys_easyzone(SkeltrProfileData const *profile_data) {
	return &profile_data->priv->updated->hardware.keys_easyzone;
}

void skeltr_profile_data_set_keys_easyzone(SkeltrProfileData *profile_data, SkeltrKeysEasyzone const *keys_easyzone) {
	SkeltrProfileDataPrivateDataHardware *hardware = &profile_data->priv->updated->hardware;

	if (!skeltr_keys_easyzone_equal(&hardware->keys_easyzone, keys_easyzone)) {
		skeltr_keys_easyzone_copy(&hardware->keys_easyzone, keys_easyzone);
		hardware->modified_keys_easyzone = TRUE;
	}
}

SkeltrKeysExtra const *skeltr_profile_data_get_keys_extra(SkeltrProfileData const *profile_data) {
	return &profile_data->priv->updated->hardware.keys_extra;
}

void skeltr_profile_data_set_keys_extra(SkeltrProfileData *profile_data, SkeltrKeysExtra const *keys_extra) {
	SkeltrProfileDataPrivateDataHardware *hardware = &profile_data->priv->updated->hardware;

	if (!skeltr_keys_extra_equal(&hardware->keys_extra, keys_extra)) {
		skeltr_keys_extra_copy(&hardware->keys_extra, keys_extra);
		hardware->modified_keys_extra = TRUE;
	}
}

SkeltrKeysFunction const *skeltr_profile_data_get_keys_function(SkeltrProfileData const *profile_data) {
	return &profile_data->priv->updated->hardware.keys_function;
}

void skeltr_profile_data_set_keys_function(SkeltrProfileData *profile_data, SkeltrKeysFunction const *keys_function) {
	SkeltrProfileDataPrivateDataHardware *hardware = &profile_data->priv->updated->hardware;

	if (!skeltr_keys_function_equal(&hardware->keys_function, keys_function)) {
		skeltr_keys_function_copy(&hardware->keys_function, keys_function);
		hardware->modified_keys_function = TRUE;
	}
}

SkeltrKeysMacro const *skeltr_profile_data_get_keys_macro(SkeltrProfileData const *profile_data) {
	return &profile_data->priv->updated->hardware.keys_macro;
}

void skeltr_profile_data_set_keys_macro(SkeltrProfileData *profile_data, SkeltrKeysMacro const *keys_macro) {
	SkeltrProfileDataPrivateDataHardware *hardware = &profile_data->priv->updated->hardware;

	if (!skeltr_keys_macro_equal(&hardware->keys_macro, keys_macro)) {
		skeltr_keys_macro_copy(&hardware->keys_macro, keys_macro);
		hardware->modified_keys_macro = TRUE;
	}
}

SkeltrKeysPrimary const *skeltr_profile_data_get_keys_primary(SkeltrProfileData const *profile_data) {
	return &profile_data->priv->updated->hardware.keys_primary;
}

void skeltr_profile_data_set_keys_primary(SkeltrProfileData *profile_data, SkeltrKeysPrimary const *keys_primary) {
	SkeltrProfileDataPrivateDataHardware *hardware = &profile_data->priv->updated->hardware;

	if (!skeltr_keys_primary_equal(&hardware->keys_primary, keys_primary)) {
		skeltr_keys_primary_copy(&hardware->keys_primary, keys_primary);
		hardware->modified_keys_primary = TRUE;
	}
}

SkeltrKeysThumbster const *skeltr_profile_data_get_keys_thumbster(SkeltrProfileData const *profile_data) {
	return &profile_data->priv->updated->hardware.keys_thumbster;
}

void skeltr_profile_data_set_keys_thumbster(SkeltrProfileData *profile_data, SkeltrKeysThumbster const *keys_thumbster) {
	SkeltrProfileDataPrivateDataHardware *hardware = &profile_data->priv->updated->hardware;

	if (!skeltr_keys_thumbster_equal(&hardware->keys_thumbster, keys_thumbster)) {
		skeltr_keys_thumbster_copy(&hardware->keys_thumbster, keys_thumbster);
		hardware->modified_keys_thumbster = TRUE;
	}
}

SkeltrLight const *skeltr_profile_data_get_light(SkeltrProfileData const *profile_data) {
	return &profile_data->priv->updated->hardware.light;
}

void skeltr_profile_data_set_light(SkeltrProfileData *profile_data, SkeltrLight const *light) {
	SkeltrProfileDataPrivateDataHardware *hardware = &profile_data->priv->updated->hardware;

	if (!skeltr_light_equal(&hardware->light, light)) {
		skeltr_light_copy(&hardware->light, light);
		hardware->modified_light = TRUE;
	}
}

SkeltrMacro const *skeltr_profile_data_get_macro(SkeltrProfileData const *profile_data, guint index) {
	return &profile_data->priv->updated->hardware.macros[index];
}

void skeltr_profile_data_set_macro(SkeltrProfileData *profile_data, guint index, SkeltrMacro const *macro) {
	SkeltrProfileDataPrivateDataHardware *hardware = &profile_data->priv->updated->hardware;

	if (!skeltr_macro_equal(&hardware->macros[index], macro)) {
		skeltr_macro_copy(&hardware->macros[index], macro);
		hardware->modified_macros[index] = TRUE;
	}
}

gchar const *skeltr_profile_data_get_opener(SkeltrProfileData const *profile_data, guint index) {
	return profile_data->priv->updated->eventhandler.openers[index];
}

void skeltr_profile_data_set_opener(SkeltrProfileData *profile_data, guint index, gchar const *opener) {
	SkeltrProfileDataPrivateDataEventhandler *eventhandler = &profile_data->priv->updated->eventhandler;

	if (strncmp(eventhandler->openers[index], opener, ROCCAT_SWARM_RMP_OPENER_LENGTH)) {
		g_strlcpy(eventhandler->openers[index], opener, ROCCAT_SWARM_RMP_OPENER_LENGTH);
		eventhandler->modified = TRUE;
	}
}

RoccatTimer const *skeltr_profile_data_get_timer(SkeltrProfileData const *profile_data, guint index) {
	return &profile_data->priv->updated->eventhandler.timers[index];
}

void skeltr_profile_data_set_timer(SkeltrProfileData *profile_data, guint index, RoccatTimer const *timer) {
	SkeltrProfileDataPrivateDataEventhandler *eventhandler = &profile_data->priv->updated->eventhandler;

	if (!roccat_timer_equal(&eventhandler->timers[index], timer)) {
		roccat_timer_copy(&eventhandler->timers[index], timer);
		eventhandler->modified = TRUE;
	}
}

guint16 skeltr_profile_data_get_capslock_talk_target(SkeltrProfileData const *profile_data) {
	return profile_data->priv->updated->eventhandler.capslock_talk_target;
}

void skeltr_profile_data_set_capslock_talk_target(SkeltrProfileData *profile_data, guint16 target) {
	SkeltrProfileDataPrivateDataEventhandler *eventhandler = &profile_data->priv->updated->eventhandler;

	if (eventhandler->capslock_talk_target != target) {
		eventhandler->capslock_talk_target = target;
		eventhandler->modified = TRUE;
	}
}

void skeltr_profile_data_set_brightness_from_hardware(SkeltrProfileData *profile_data, guint brightness) {
	profile_data->priv->updated->hardware.light.brightness = brightness;
	profile_data->priv->original->hardware.light.brightness = brightness;
}

void skeltr_profile_data_set_macro_from_hardware(SkeltrProfileData *profile_data, guint index, SkeltrMacro const *macro) {
	RoccatButton *updated_key = get_key_for_macro_index(profile_data->priv->updated, index);
	RoccatButton *original_key = get_key_for_macro_index(profile_data->priv->original, index);
	
	if (!updated_key || !original_key)
		return;
	
	updated_key->type = SKELTR_KEY_TYPE_MACRO;
	skeltr_macro_copy(&profile_data->priv->updated->hardware.macros[index], macro);
	original_key->type = SKELTR_KEY_TYPE_MACRO;
	skeltr_macro_copy(&profile_data->priv->original->hardware.macros[index], macro);
}
