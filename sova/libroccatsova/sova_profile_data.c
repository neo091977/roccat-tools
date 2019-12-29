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

#include "sova_profile_data.h"
#include "sova_key_relations.h"
#include "sova_rkp_sound_feedback.h"
#include "roccat_swarm_rmp.h"
#include "roccat_helper.h"
#include "roccat_talk.h"
#include "roccat_notificator.h"
#include "g_roccat_helper.h"
#include "i18n-lib.h"
#include <glib/gstdio.h>
#include <string.h>
#include <errno.h>

#define SOVA_PROFILE_DATA_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), SOVA_PROFILE_DATA_TYPE, SovaProfileDataPrivate))

typedef struct _SovaProfileDataPrivateData SovaProfileDataPrivateData;
typedef struct _SovaProfileDataPrivateDataEventhandler SovaProfileDataPrivateDataEventhandler;
typedef struct _SovaProfileDataPrivateDataHardware SovaProfileDataPrivateDataHardware;

struct _SovaProfileDataPrivateDataEventhandler {
	gboolean modified;
	gchar profile_name[ROCCAT_SWARM_RMP_PROFILE_NAME_LENGTH + 1];
	RoccatSwarmGamefile gamefile_names[GAMINGGEAR_PROFILE_DATA_GAMEFILE_NUM];
	RoccatSwarmOpener openers[SOVA_MACRO_NUM];
	gint profile_index;
	guint16 capslock_talk_target;
	RoccatTimer timers[SOVA_MACRO_NUM];
	guint sound_feedback_typing; // SovaRkpSoundFeedbackTypingSound
	guint wakeup_notification_type; // RoccatNotificationType
	guint sleep_notification_type; // RoccatNotificationType
	guint timer_notification_type; // RoccatNotificationType
	guint macro_record_notification_type; // RoccatNotificationType
	guint profile_switch_notification_type; // RoccatNotificationType
	gdouble notification_volume;
};

struct _SovaProfileDataPrivateDataHardware {
	gboolean modified_keys_extra;
	SovaKeysExtra keys_extra;

	gboolean modified_light;
	SovaLight light;

	gboolean modified_key_mask;
	SovaKeyMask key_mask;

	gboolean modified_keys_primary;
	SovaKeysPrimary keys_primary;

	gboolean modified_keys_function;
	SovaKeysFunction keys_function;

	gboolean modified_keys_easyzone;
	SovaKeysEasyzone keys_easyzone;

	gboolean modified_macros[SOVA_MACRO_NUM];
	SovaMacro macros[SOVA_MACRO_NUM];
};

struct _SovaProfileDataPrivateData {
	SovaProfileDataPrivateDataEventhandler eventhandler;
	SovaProfileDataPrivateDataHardware hardware;
};

struct _SovaProfileDataPrivate {
	SovaProfileDataPrivateData *original;
	SovaProfileDataPrivateData *updated;
};

G_DEFINE_TYPE(SovaProfileData, sova_profile_data, GAMINGGEAR_PROFILE_DATA_TYPE);

static gboolean sova_profile_data_private_data_eventhandler_get_modified(SovaProfileDataPrivateDataEventhandler const *data) {
	return data->modified;
}

static void sova_profile_data_private_data_eventhandler_set_modified(SovaProfileDataPrivateDataEventhandler *data, gboolean state) {
	data->modified = state;
}

static gboolean sova_profile_data_private_data_hardware_get_modified(SovaProfileDataPrivateDataHardware const *data) {
	guint i;

	if (data->modified_keys_extra ||
			data->modified_keys_primary ||
			data->modified_keys_function ||
			data->modified_keys_easyzone ||
			data->modified_key_mask ||
			data->modified_light)
		return TRUE;

	for (i = 0; i < SOVA_MACRO_NUM; ++i)
		if (data->modified_macros[i])
			return TRUE;

	return FALSE;
}

static void sova_profile_data_private_data_hardware_set_modified(SovaProfileDataPrivateDataHardware *data, gboolean state) {
	guint i;

	data->modified_keys_extra = state;
	data->modified_keys_primary = state;
	data->modified_keys_function = state;
	data->modified_keys_easyzone = state;
	data->modified_key_mask = state;
	data->modified_light = state;

	for (i = 0; i < SOVA_MACRO_NUM; ++i)
		data->modified_macros[i] = state;
}

static gboolean sova_profile_data_get_modified(GaminggearProfileData *self) {
	SovaProfileDataPrivate *priv = SOVA_PROFILE_DATA(self)->priv;

	if (sova_profile_data_private_data_eventhandler_get_modified(&priv->updated->eventhandler))
		return TRUE;

	return sova_profile_data_private_data_hardware_get_modified(&priv->updated->hardware);
}

static void sova_profile_data_reset(GaminggearProfileData *self) {
	SovaProfileDataPrivate *priv = SOVA_PROFILE_DATA(self)->priv;
	*priv->updated = *priv->original;
}

static gchar const *sova_profile_data_get_name(GaminggearProfileData *self) {
	return SOVA_PROFILE_DATA(self)->priv->updated->eventhandler.profile_name;
}

static void sova_profile_data_set_name(GaminggearProfileData *self, gchar const *new_name) {
	SovaProfileDataPrivate *priv = SOVA_PROFILE_DATA(self)->priv;

	if (strncmp(priv->updated->eventhandler.profile_name, new_name, ROCCAT_SWARM_RMP_PROFILE_NAME_LENGTH)) {
		g_strlcpy(priv->updated->eventhandler.profile_name, new_name, ROCCAT_SWARM_RMP_PROFILE_NAME_LENGTH + 1);
		priv->updated->eventhandler.modified = TRUE;
	}
}

static gchar const *sova_profile_data_get_gamefile(GaminggearProfileData *self, guint index) {
	return SOVA_PROFILE_DATA(self)->priv->updated->eventhandler.gamefile_names[index];
}

static void sova_profile_data_set_gamefile(GaminggearProfileData *self, guint index, gchar const *new_gamefile) {
	SovaProfileDataPrivate *priv = SOVA_PROFILE_DATA(self)->priv;

	if (strncmp(priv->updated->eventhandler.gamefile_names[index], new_gamefile, ROCCAT_SWARM_RMP_GAMEFILE_LENGTH)) {
		g_strlcpy(priv->updated->eventhandler.gamefile_names[index], new_gamefile, ROCCAT_SWARM_RMP_GAMEFILE_LENGTH);
		priv->updated->eventhandler.modified = TRUE;
	}
}

static gint sova_profile_data_get_hardware_index(GaminggearProfileData *self) {
	return SOVA_PROFILE_DATA(self)->priv->updated->eventhandler.profile_index;
}

static void sova_profile_data_set_hardware_index(GaminggearProfileData *self, gint new_index) {
	SovaProfileDataPrivate *priv = SOVA_PROFILE_DATA(self)->priv;

	if (priv->updated->eventhandler.profile_index != new_index) {
		priv->updated->eventhandler.profile_index = new_index;
		sova_profile_data_private_data_eventhandler_set_modified(&priv->updated->eventhandler, TRUE);
		sova_profile_data_private_data_hardware_set_modified(&priv->updated->hardware, TRUE);
		// FIXME set profile_index also in original data?
	}
}

static gchar *sova_profile_data_path(void) {
	gchar *base = roccat_profile_dir();
	gchar *dir = g_build_path("/", base, "sova", NULL);
	g_free(base);
	return dir;
}

static gboolean sova_profile_dir_create_if_needed(GError **error) {
	gchar *dir;
	gboolean retval;

	dir = sova_profile_data_path();
	retval = roccat_profile_dir_create_if_needed(dir, error);
	g_free(dir);
	return retval;
}

static gchar *sova_build_profile_path(guint profile_index) {
	gchar *dir = sova_profile_data_path();
	gchar *filename = g_strdup_printf("actual%i", profile_index);
	gchar *path = g_build_path("/", dir, filename, NULL);
	g_free(filename);
	g_free(dir);
	return path;
}

static gboolean sova_profile_data_load_with_path(GaminggearProfileData *self, gchar const *path, GError **error) {
	SovaProfileDataPrivate *priv = SOVA_PROFILE_DATA(self)->priv;
	gchar *data;
	gsize length;

	if (!g_file_get_contents(path, &data, &length, error))
		return FALSE;

	if (length != sizeof(SovaProfileDataPrivateDataEventhandler)) {
		g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_INVAL, _("Data has wrong length, %zu instead of %zu"),
				(gsize)length, (gsize)sizeof(SovaProfileDataPrivateDataEventhandler));
		g_free(data);
		return FALSE;
	}

	priv->updated->eventhandler = *(SovaProfileDataPrivateDataEventhandler *)data;

	sova_profile_data_private_data_eventhandler_set_modified(&priv->updated->eventhandler, FALSE);
	sova_profile_data_private_data_hardware_set_modified(&priv->updated->hardware, FALSE);
	*priv->original = *priv->updated;

	return TRUE;
}

gboolean sova_profile_data_load(SovaProfileData *self, guint profile_index, GError **error) {
	gchar *path;
	gboolean retval;

	self->priv->updated->eventhandler.profile_index = profile_index;

	path = sova_build_profile_path(profile_index);
	/* Not calling sova_profile_data_load_with_path directly to get error checking from GaminggearProfileData */
	retval = gaminggear_profile_data_load(GAMINGGEAR_PROFILE_DATA(self), path, error);
	g_free(path);

	return retval;
}

static gboolean sova_profile_data_save(GaminggearProfileData *self, GError **error) {
	SovaProfileDataPrivate *priv = SOVA_PROFILE_DATA(self)->priv;
	gchar *path;
	gboolean retval;

	if (!sova_profile_dir_create_if_needed(error))
		return FALSE;

	path = sova_build_profile_path(priv->updated->eventhandler.profile_index);
	retval = roccat_profile_write_with_path(path, (gchar const *)&priv->updated->eventhandler, sizeof(SovaProfileDataPrivateDataEventhandler), error);
	g_free(path);

	if (retval)
		priv->updated->eventhandler.modified = FALSE;

	/* Copy everything since device might not be present. */
	*priv->original = *priv->updated;

	return retval;
}

static gboolean sova_profile_data_remove(GaminggearProfileData *self, GError **error) {
	SovaProfileDataPrivate *priv = SOVA_PROFILE_DATA(self)->priv;
	gchar *path;

	path = sova_build_profile_path(priv->updated->eventhandler.profile_index);
	if (g_unlink(path) == -1)
		g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(errno), _("Could not remove file %1$s: %2$s"), path, g_strerror(errno));
	g_free(path);

	g_clear_pointer(&priv->original, g_free);
	g_clear_pointer(&priv->updated, g_free);

	return TRUE;
}

static gboolean sova_profile_data_write_keys_extra(GaminggearProfileData *self, RoccatDevice *device, GError **error) {
	SovaProfileDataPrivate *priv = SOVA_PROFILE_DATA(self)->priv;
	gboolean retval;

	if (!priv->updated->hardware.modified_keys_extra)
		return TRUE;

	retval = sova_keys_extra_write(device, priv->updated->eventhandler.profile_index, &priv->updated->hardware.keys_extra, error);

	if (retval)
		priv->updated->hardware.modified_keys_extra = FALSE;

	return retval;
}

static gboolean sova_profile_data_write_key_mask(GaminggearProfileData *self, RoccatDevice *device, GError **error) {
	SovaProfileDataPrivate *priv = SOVA_PROFILE_DATA(self)->priv;
	gboolean retval;

	if (!priv->updated->hardware.modified_key_mask)
		return TRUE;

	retval = sova_key_mask_write(device, priv->updated->eventhandler.profile_index, &priv->updated->hardware.key_mask, error);

	if (retval)
		priv->updated->hardware.modified_key_mask = FALSE;

	return retval;
}

static gboolean sova_profile_data_write_keys_function(GaminggearProfileData *self, RoccatDevice *device, GError **error) {
	SovaProfileDataPrivate *priv = SOVA_PROFILE_DATA(self)->priv;
	gboolean retval;

	if (!priv->updated->hardware.modified_keys_function)
		return TRUE;

	retval = sova_keys_function_write(device, priv->updated->eventhandler.profile_index, &priv->updated->hardware.keys_function, error);

	if (retval)
		priv->updated->hardware.modified_keys_function = FALSE;

	return retval;
}

static gboolean sova_profile_data_write_keys_easyzone(GaminggearProfileData *self, RoccatDevice *device, GError **error) {
	SovaProfileDataPrivate *priv = SOVA_PROFILE_DATA(self)->priv;
	gboolean retval;

	if (!priv->updated->hardware.modified_keys_easyzone)
		return TRUE;

	retval = sova_keys_easyzone_write(device, priv->updated->eventhandler.profile_index, &priv->updated->hardware.keys_easyzone, error);

	if (retval)
		priv->updated->hardware.modified_keys_easyzone = FALSE;

	return retval;
}

static gboolean sova_profile_data_write_keys_primary(GaminggearProfileData *self, RoccatDevice *device, GError **error) {
	SovaProfileDataPrivate *priv = SOVA_PROFILE_DATA(self)->priv;
	gboolean retval;

	if (!priv->updated->hardware.modified_keys_primary)
		return TRUE;

	retval = sova_keys_primary_write(device, priv->updated->eventhandler.profile_index, &priv->updated->hardware.keys_primary, error);

	if (retval)
		priv->updated->hardware.modified_keys_primary = FALSE;

	return retval;
}

static gboolean sova_profile_data_write_light(GaminggearProfileData *self, RoccatDevice *device, GError **error) {
	SovaProfileDataPrivate *priv = SOVA_PROFILE_DATA(self)->priv;
	gboolean retval;

	if (!priv->updated->hardware.modified_light)
		return TRUE;

	retval = sova_light_write(device, priv->updated->eventhandler.profile_index, &priv->updated->hardware.light, error);

	if (retval)
		priv->updated->hardware.modified_light = FALSE;

	return retval;
}

static gboolean sova_profile_data_write_macros(GaminggearProfileData *self, RoccatDevice *device, GError **error) {
	SovaProfileDataPrivate *priv = SOVA_PROFILE_DATA(self)->priv;
	guint macro_index;

	for (macro_index = 0; macro_index < SOVA_MACRO_NUM; ++macro_index) {
		if (!priv->updated->hardware.modified_macros[macro_index])
			continue;

		if (priv->updated->hardware.keys_easyzone.keys[macro_index].type == SOVA_KEY_TYPE_MACRO) {
			if (!sova_macro_write(device, priv->updated->eventhandler.profile_index, macro_index, &priv->updated->hardware.macros[macro_index], error))
				return FALSE;
		}

		/* Always set macro unmodified */
		priv->updated->hardware.modified_macros[macro_index] = FALSE;
	}

	return TRUE;
}

static gboolean sova_profile_data_write(GaminggearProfileData *self, GaminggearDevice *device, GError **error) {
	SovaProfileDataPrivate *priv = SOVA_PROFILE_DATA(self)->priv;
	RoccatDevice *roccat_device = ROCCAT_DEVICE(device);

	if (!sova_profile_data_write_keys_extra(self, roccat_device, error)) return FALSE;
	if (!sova_profile_data_write_key_mask(self, roccat_device, error)) return FALSE;
	if (!sova_profile_data_write_keys_primary(self, roccat_device, error)) return FALSE;
	if (!sova_profile_data_write_keys_function(self, roccat_device, error)) return FALSE;
	if (!sova_profile_data_write_keys_easyzone(self, roccat_device, error)) return FALSE;
	if (!sova_profile_data_write_light(self, roccat_device, error)) return FALSE;
	if (!sova_profile_data_write_macros(self, roccat_device, error)) return FALSE;

	priv->original->hardware = priv->updated->hardware;

	return TRUE;
}

static gboolean sova_profile_data_read_keys_extra(GaminggearProfileData *self, RoccatDevice *device, GError **error) {
	SovaProfileDataPrivate *priv = SOVA_PROFILE_DATA(self)->priv;
	SovaKeysExtra *keys_extra = sova_keys_extra_read(device, priv->updated->eventhandler.profile_index, error);

	if (keys_extra == NULL)
		return FALSE;
	sova_keys_extra_copy(&priv->updated->hardware.keys_extra, keys_extra);
	g_free(keys_extra);

	priv->updated->hardware.modified_keys_extra = FALSE;

	return TRUE;
}

static gboolean sova_profile_data_read_key_mask(GaminggearProfileData *self, RoccatDevice *device, GError **error) {
	SovaProfileDataPrivate *priv = SOVA_PROFILE_DATA(self)->priv;
	SovaKeyMask *key_mask = sova_key_mask_read(device, priv->updated->eventhandler.profile_index, error);

	if (key_mask == NULL)
		return FALSE;
	sova_key_mask_copy(&priv->updated->hardware.key_mask, key_mask);
	g_free(key_mask);

	priv->updated->hardware.modified_key_mask = FALSE;

	return TRUE;
}

static gboolean sova_profile_data_read_keys_function(GaminggearProfileData *self, RoccatDevice *device, GError **error) {
	SovaProfileDataPrivate *priv = SOVA_PROFILE_DATA(self)->priv;
	SovaKeysFunction *keys_function = sova_keys_function_read(device, priv->updated->eventhandler.profile_index, error);

	if (keys_function == NULL)
		return FALSE;
	sova_keys_function_copy(&priv->updated->hardware.keys_function, keys_function);
	g_free(keys_function);

	priv->updated->hardware.modified_keys_function = FALSE;

	return TRUE;
}

static gboolean sova_profile_data_read_keys_easyzone(GaminggearProfileData *self, RoccatDevice *device, GError **error) {
	SovaProfileDataPrivate *priv = SOVA_PROFILE_DATA(self)->priv;
	SovaKeysEasyzone *keys_easyzone = sova_keys_easyzone_read(device, priv->updated->eventhandler.profile_index, error);

	if (keys_easyzone == NULL)
		return FALSE;
	sova_keys_easyzone_copy(&priv->updated->hardware.keys_easyzone, keys_easyzone);
	g_free(keys_easyzone);

	priv->updated->hardware.modified_keys_easyzone = FALSE;

	return TRUE;
}

static gboolean sova_profile_data_read_keys_primary(GaminggearProfileData *self, RoccatDevice *device, GError **error) {
	SovaProfileDataPrivate *priv = SOVA_PROFILE_DATA(self)->priv;
	SovaKeysPrimary *keys_primary = sova_keys_primary_read(device, priv->updated->eventhandler.profile_index, error);

	if (keys_primary == NULL)
		return FALSE;
	sova_keys_primary_copy(&priv->updated->hardware.keys_primary, keys_primary);
	g_free(keys_primary);

	priv->updated->hardware.modified_keys_primary = FALSE;

	return TRUE;
}

static gboolean sova_profile_data_read_light(GaminggearProfileData *self, RoccatDevice *device, GError **error) {
	SovaProfileDataPrivate *priv = SOVA_PROFILE_DATA(self)->priv;
	SovaLight *light = sova_light_read(device, priv->updated->eventhandler.profile_index, error);

	if (light == NULL)
		return FALSE;
	sova_light_copy(&priv->updated->hardware.light, light);
	g_free(light);

	priv->updated->hardware.modified_light = FALSE;

	return TRUE;
}

static gboolean sova_profile_data_read_macros(GaminggearProfileData *self, RoccatDevice *device, GError **error) {
	SovaProfileDataPrivate *priv = SOVA_PROFILE_DATA(self)->priv;
	guint macro_index;
	SovaMacro *macro;

	for (macro_index = 0; macro_index < SOVA_MACRO_NUM; ++macro_index) {
		if (priv->updated->hardware.keys_easyzone.keys[macro_index].type == SOVA_KEY_TYPE_MACRO) {
			macro = sova_macro_read(device, priv->updated->eventhandler.profile_index, macro_index, error);
			if (!macro)
				return FALSE;

			sova_macro_copy(&priv->updated->hardware.macros[macro_index], macro);
		}

		priv->updated->hardware.modified_macros[macro_index] = FALSE;
	}

	return TRUE;
}

static gboolean sova_profile_data_read(GaminggearProfileData *self, GaminggearDevice *device, GError **error) {
	SovaProfileDataPrivate *priv = SOVA_PROFILE_DATA(self)->priv;
	RoccatDevice *roccat_device = ROCCAT_DEVICE(device);

	if (!sova_profile_data_read_keys_extra(self, roccat_device, error)) return FALSE;
	if (!sova_profile_data_read_key_mask(self, roccat_device, error)) return FALSE;
	if (!sova_profile_data_read_keys_function(self, roccat_device, error)) return FALSE;
	if (!sova_profile_data_read_keys_easyzone(self, roccat_device, error)) return FALSE;
	if (!sova_profile_data_read_keys_primary(self, roccat_device, error)) return FALSE;
	if (!sova_profile_data_read_light(self, roccat_device, error)) return FALSE;
	if (!sova_profile_data_read_macros(self, roccat_device, error)) return FALSE;

	priv->original->hardware = priv->updated->hardware;

	return TRUE;
}

static gboolean sova_profile_data_equal(GaminggearProfileData *self, GaminggearProfileData *other) {
	SovaProfileDataPrivate *self_priv = SOVA_PROFILE_DATA(self)->priv;
	SovaProfileDataPrivate *other_priv = SOVA_PROFILE_DATA(other)->priv;

	return (memcmp(self_priv->updated, other_priv->updated, sizeof(SovaProfileDataPrivateData)) == 0) ? TRUE : FALSE;
}

static void sova_profile_data_copy(GaminggearProfileData *self, GaminggearProfileData *other) {
	SovaProfileDataPrivate *self_priv = SOVA_PROFILE_DATA(self)->priv;
	SovaProfileDataPrivate *other_priv = SOVA_PROFILE_DATA(other)->priv;

	GAMINGGEAR_PROFILE_DATA_CLASS(sova_profile_data_parent_class)->copy(self, other);
	*self_priv->updated = *other_priv->updated;
	*self_priv->original = *other_priv->original;
}

static void sova_profile_data_discriminate(GaminggearProfileData *self) {
	SovaProfileDataPrivate *self_priv = SOVA_PROFILE_DATA(self)->priv;
	gchar *new_name;

	GAMINGGEAR_PROFILE_DATA_CLASS(sova_profile_data_parent_class)->discriminate(self);

	new_name = g_strdup_printf("%s_new", sova_profile_data_get_name(self));
	sova_profile_data_set_name(self, new_name);
	g_free(new_name);

	self_priv->updated->eventhandler.profile_index = GAMINGGEAR_PROFILE_DATA_GAMEFILE_PROFILE_INVALID;

	sova_profile_data_private_data_eventhandler_set_modified(&self_priv->updated->eventhandler, TRUE);
	sova_profile_data_private_data_hardware_set_modified(&self_priv->updated->hardware, TRUE);

	*self_priv->original = *self_priv->updated;
}

static void sova_profile_data_init(SovaProfileData *profile_data) {
	SovaProfileDataPrivate *priv = SOVA_PROFILE_DATA_GET_PRIVATE(profile_data);
	profile_data->priv = priv;

	priv->updated = (SovaProfileDataPrivateData *)g_malloc0(sizeof(SovaProfileDataPrivateData));

	sova_profile_data_set_name(GAMINGGEAR_PROFILE_DATA(profile_data), "Default");
	priv->updated->eventhandler.profile_index = GAMINGGEAR_PROFILE_DATA_GAMEFILE_PROFILE_INVALID;
	priv->updated->eventhandler.notification_volume = 0.0;
	priv->updated->eventhandler.capslock_talk_target = ROCCAT_TALK_DEVICE_MOUSE;
	priv->updated->eventhandler.sound_feedback_typing = SOVA_RKP_SOUND_FEEDBACK_TYPING_SOUND_OFF;
	priv->updated->eventhandler.wakeup_notification_type = ROCCAT_NOTIFICATION_TYPE_OFF;
	priv->updated->eventhandler.sleep_notification_type = ROCCAT_NOTIFICATION_TYPE_OFF;
	priv->updated->eventhandler.timer_notification_type = ROCCAT_NOTIFICATION_TYPE_OFF;
	priv->updated->eventhandler.macro_record_notification_type = ROCCAT_NOTIFICATION_TYPE_OFF;
	priv->updated->eventhandler.profile_switch_notification_type = ROCCAT_NOTIFICATION_TYPE_OFF;
	sova_profile_data_private_data_eventhandler_set_modified(&priv->updated->eventhandler, TRUE);

	sova_keys_extra_copy(&priv->updated->hardware.keys_extra, sova_keys_extra_default());
	sova_key_mask_copy(&priv->updated->hardware.key_mask, sova_key_mask_default());
	sova_keys_function_copy(&priv->updated->hardware.keys_function, sova_keys_function_default());
	sova_keys_primary_copy(&priv->updated->hardware.keys_primary, sova_keys_primary_default());
	sova_keys_easyzone_copy(&priv->updated->hardware.keys_easyzone, sova_keys_easyzone_default());
	sova_light_copy(&priv->updated->hardware.light, sova_light_default());
	sova_profile_data_private_data_hardware_set_modified(&priv->updated->hardware, TRUE);

	priv->original = (SovaProfileDataPrivateData *)g_malloc0(sizeof(SovaProfileDataPrivateData));

	*priv->original = *priv->updated;
}

static void sova_profile_data_finalize(GObject *object) {
	SovaProfileDataPrivate *priv = SOVA_PROFILE_DATA(object)->priv;

	g_clear_pointer(&priv->original, g_free);
	g_clear_pointer(&priv->updated, g_free);

	G_OBJECT_CLASS(sova_profile_data_parent_class)->finalize(object);
}

static void sova_profile_data_class_init(SovaProfileDataClass *klass) {
	GObjectClass *gobject_class = (GObjectClass*)klass;
	GaminggearProfileDataClass *parent_class = GAMINGGEAR_PROFILE_DATA_CLASS(klass);

	gobject_class->finalize = sova_profile_data_finalize;

	parent_class->get_name = sova_profile_data_get_name;
	parent_class->set_name = sova_profile_data_set_name;
	parent_class->get_gamefile = sova_profile_data_get_gamefile;
	parent_class->set_gamefile = sova_profile_data_set_gamefile;
	parent_class->get_hardware_index = sova_profile_data_get_hardware_index;
	parent_class->set_hardware_index = sova_profile_data_set_hardware_index;
	parent_class->load = sova_profile_data_load_with_path;
	parent_class->save = sova_profile_data_save;
	parent_class->remove = sova_profile_data_remove;
	parent_class->read = sova_profile_data_read;
	parent_class->write = sova_profile_data_write;
	parent_class->get_modified = sova_profile_data_get_modified;
	parent_class->reset = sova_profile_data_reset;
	parent_class->copy = sova_profile_data_copy;
	parent_class->equal = sova_profile_data_equal;
	parent_class->discriminate = sova_profile_data_discriminate;

	g_type_class_add_private(klass, sizeof(SovaProfileDataPrivate));
}

SovaProfileData *sova_profile_data_new(void) {
	return SOVA_PROFILE_DATA(g_object_new(SOVA_PROFILE_DATA_TYPE, NULL));
}

gdouble sova_profile_data_get_sound_feedback_volume(SovaProfileData const *profile_data) {
	return profile_data->priv->updated->eventhandler.notification_volume;
}

void sova_profile_data_set_sound_feedback_volume(SovaProfileData *profile_data, gdouble volume) {
	SovaProfileDataPrivateDataEventhandler *eventhandler = &profile_data->priv->updated->eventhandler;

	if (eventhandler->notification_volume != volume) {
		eventhandler->notification_volume = volume;
		eventhandler->modified = TRUE;
	}
}

guint sova_profile_data_get_sound_feedback_typing(SovaProfileData const *profile_data) {
	return profile_data->priv->updated->eventhandler.sound_feedback_typing;
}

void sova_profile_data_set_sound_feedback_typing(SovaProfileData *profile_data, guint typing) {
	SovaProfileDataPrivateDataEventhandler *eventhandler = &profile_data->priv->updated->eventhandler;

	if (eventhandler->sound_feedback_typing != typing) {
		eventhandler->sound_feedback_typing = typing;
		eventhandler->modified = TRUE;
	}
}

guint sova_profile_data_get_wakeup_notification_type(SovaProfileData const *profile_data) {
	return profile_data->priv->updated->eventhandler.wakeup_notification_type;
}

void sova_profile_data_set_wakeup_notification_type(SovaProfileData *profile_data, guint wakeup) {
	SovaProfileDataPrivateDataEventhandler *eventhandler = &profile_data->priv->updated->eventhandler;

	if (eventhandler->wakeup_notification_type != wakeup) {
		eventhandler->wakeup_notification_type = wakeup;
		eventhandler->modified = TRUE;
	}
}

guint sova_profile_data_get_sleep_notification_type(SovaProfileData const *profile_data) {
	return profile_data->priv->updated->eventhandler.sleep_notification_type;
}

void sova_profile_data_set_sleep_notification_type(SovaProfileData *profile_data, guint sleep) {
	SovaProfileDataPrivateDataEventhandler *eventhandler = &profile_data->priv->updated->eventhandler;

	if (eventhandler->sleep_notification_type != sleep) {
		eventhandler->sleep_notification_type = sleep;
		eventhandler->modified = TRUE;
	}
}

guint sova_profile_data_get_timer_notification_type(SovaProfileData const *profile_data) {
	return profile_data->priv->updated->eventhandler.timer_notification_type;
}

void sova_profile_data_set_timer_notification_type(SovaProfileData *profile_data, guint type) {
	SovaProfileDataPrivateDataEventhandler *eventhandler = &profile_data->priv->updated->eventhandler;

	if (eventhandler->timer_notification_type != type) {
		eventhandler->timer_notification_type = type;
		eventhandler->modified = TRUE;
	}
}

guint sova_profile_data_get_macro_record_notification_type(SovaProfileData const *profile_data) {
	return profile_data->priv->updated->eventhandler.macro_record_notification_type;
}

void sova_profile_data_set_macro_record_notification_type(SovaProfileData *profile_data, guint type) {
	SovaProfileDataPrivateDataEventhandler *eventhandler = &profile_data->priv->updated->eventhandler;

	if (eventhandler->macro_record_notification_type != type) {
		eventhandler->macro_record_notification_type = type;
		eventhandler->modified = TRUE;
	}
}

guint sova_profile_data_get_profile_switch_notification_type(SovaProfileData const *profile_data) {
	return profile_data->priv->updated->eventhandler.profile_switch_notification_type;
}

void sova_profile_data_set_profile_switch_notification_type(SovaProfileData *profile_data, guint type) {
	SovaProfileDataPrivateDataEventhandler *eventhandler = &profile_data->priv->updated->eventhandler;

	if (eventhandler->profile_switch_notification_type != type) {
		eventhandler->profile_switch_notification_type = type;
		eventhandler->modified = TRUE;
	}
}

SovaKeysExtra const *sova_profile_data_get_keys_extra(SovaProfileData const *profile_data) {
	return &profile_data->priv->updated->hardware.keys_extra;
}

void sova_profile_data_set_keys_extra(SovaProfileData *profile_data, SovaKeysExtra const *keys_extra) {
	SovaProfileDataPrivateDataHardware *hardware = &profile_data->priv->updated->hardware;

	if (!sova_keys_extra_equal(&hardware->keys_extra, keys_extra)) {
		sova_keys_extra_copy(&hardware->keys_extra, keys_extra);
		hardware->modified_keys_extra = TRUE;
	}
}

SovaLight const *sova_profile_data_get_light(SovaProfileData const *profile_data) {
	return &profile_data->priv->updated->hardware.light;
}

void sova_profile_data_set_light(SovaProfileData *profile_data, SovaLight const *light) {
	SovaProfileDataPrivateDataHardware *hardware = &profile_data->priv->updated->hardware;

	if (!sova_light_equal(&hardware->light, light)) {
		sova_light_copy(&hardware->light, light);
		hardware->modified_light = TRUE;
	}
}

SovaKeyMask const *sova_profile_data_get_key_mask(SovaProfileData const *profile_data) {
	return &profile_data->priv->updated->hardware.key_mask;
}

void sova_profile_data_set_key_mask(SovaProfileData *profile_data, SovaKeyMask const *key_mask) {
	SovaProfileDataPrivateDataHardware *hardware = &profile_data->priv->updated->hardware;

	if (!sova_key_mask_equal(&hardware->key_mask, key_mask)) {
		sova_key_mask_copy(&hardware->key_mask, key_mask);
		hardware->modified_key_mask = TRUE;
	}
}

SovaKeysFunction const *sova_profile_data_get_keys_function(SovaProfileData const *profile_data) {
	return &profile_data->priv->updated->hardware.keys_function;
}

void sova_profile_data_set_keys_function(SovaProfileData *profile_data, SovaKeysFunction const *keys_function) {
	SovaProfileDataPrivateDataHardware *hardware = &profile_data->priv->updated->hardware;

	if (!sova_keys_function_equal(&hardware->keys_function, keys_function)) {
		sova_keys_function_copy(&hardware->keys_function, keys_function);
		hardware->modified_keys_function = TRUE;
	}
}

SovaKeysEasyzone const *sova_profile_data_get_keys_easyzone(SovaProfileData const *profile_data) {
	return &profile_data->priv->updated->hardware.keys_easyzone;
}

void sova_profile_data_set_keys_easyzone(SovaProfileData *profile_data, SovaKeysEasyzone const *keys_easyzone) {
	SovaProfileDataPrivateDataHardware *hardware = &profile_data->priv->updated->hardware;

	if (!sova_keys_easyzone_equal(&hardware->keys_easyzone, keys_easyzone)) {
		sova_keys_easyzone_copy(&hardware->keys_easyzone, keys_easyzone);
		hardware->modified_keys_easyzone = TRUE;
	}
}

SovaKeysPrimary const *sova_profile_data_get_keys_primary(SovaProfileData const *profile_data) {
	return &profile_data->priv->updated->hardware.keys_primary;
}

void sova_profile_data_set_keys_primary(SovaProfileData *profile_data, SovaKeysPrimary const *keys_primary) {
	SovaProfileDataPrivateDataHardware *hardware = &profile_data->priv->updated->hardware;

	if (!sova_keys_primary_equal(&hardware->keys_primary, keys_primary)) {
		sova_keys_primary_copy(&hardware->keys_primary, keys_primary);
		hardware->modified_keys_primary = TRUE;
	}
}

SovaMacro const *sova_profile_data_get_macro(SovaProfileData const *profile_data, guint index) {
	return &profile_data->priv->updated->hardware.macros[index];
}

void sova_profile_data_set_macro(SovaProfileData *profile_data, guint index, SovaMacro const *macro) {
	SovaProfileDataPrivateDataHardware *hardware = &profile_data->priv->updated->hardware;

	if (!sova_macro_equal(&hardware->macros[index], macro)) {
		sova_macro_copy(&hardware->macros[index], macro);
		hardware->modified_macros[index] = TRUE;
	}
}

gchar const *sova_profile_data_get_opener(SovaProfileData const *profile_data, guint index) {
	return profile_data->priv->updated->eventhandler.openers[index];
}

void sova_profile_data_set_opener(SovaProfileData *profile_data, guint index, gchar const *opener) {
	SovaProfileDataPrivateDataEventhandler *eventhandler = &profile_data->priv->updated->eventhandler;

	if (strncmp(eventhandler->openers[index], opener, ROCCAT_SWARM_RMP_OPENER_LENGTH)) {
		g_strlcpy(eventhandler->openers[index], opener, ROCCAT_SWARM_RMP_OPENER_LENGTH);
		eventhandler->modified = TRUE;
	}
}

RoccatTimer const *sova_profile_data_get_timer(SovaProfileData const *profile_data, guint index) {
	return &profile_data->priv->updated->eventhandler.timers[index];
}

void sova_profile_data_set_timer(SovaProfileData *profile_data, guint index, RoccatTimer const *timer) {
	SovaProfileDataPrivateDataEventhandler *eventhandler = &profile_data->priv->updated->eventhandler;

	if (!roccat_timer_equal(&eventhandler->timers[index], timer)) {
		roccat_timer_copy(&eventhandler->timers[index], timer);
		eventhandler->modified = TRUE;
	}
}

guint16 sova_profile_data_get_capslock_talk_target(SovaProfileData const *profile_data) {
	return profile_data->priv->updated->eventhandler.capslock_talk_target;
}

void sova_profile_data_set_capslock_talk_target(SovaProfileData *profile_data, guint16 target) {
	SovaProfileDataPrivateDataEventhandler *eventhandler = &profile_data->priv->updated->eventhandler;

	if (eventhandler->capslock_talk_target != target) {
		eventhandler->capslock_talk_target = target;
		eventhandler->modified = TRUE;
	}
}

void sova_profile_data_set_brightness_from_hardware(SovaProfileData *profile_data, guint brightness) {
	profile_data->priv->updated->hardware.light.brightness = brightness;
	profile_data->priv->original->hardware.light.brightness = brightness;
}

void sova_profile_data_set_macro_from_hardware(SovaProfileData *profile_data, guint index, SovaMacro const *macro) {
	profile_data->priv->updated->hardware.keys_easyzone.keys[index].type = SOVA_KEY_TYPE_MACRO;
	sova_macro_copy(&profile_data->priv->updated->hardware.macros[index], macro);
	profile_data->priv->original->hardware.keys_easyzone.keys[index].type = SOVA_KEY_TYPE_MACRO;
	sova_macro_copy(&profile_data->priv->original->hardware.macros[index], macro);
}
