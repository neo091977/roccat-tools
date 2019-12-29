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

#include "suora_profile_data.h"
#include "suora_key_relations.h"
#include "roccat_swarm_rmp.h"
#include "roccat_helper.h"
#include "g_roccat_helper.h"
#include "i18n-lib.h"
#include <glib/gstdio.h>
#include <string.h>
#include <errno.h>

#define SUORA_PROFILE_DATA_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), SUORA_PROFILE_DATA_TYPE, SuoraProfileDataPrivate))

typedef struct _SuoraProfileDataPrivateData SuoraProfileDataPrivateData;
typedef struct _SuoraProfileDataPrivateDataEventhandler SuoraProfileDataPrivateDataEventhandler;
typedef struct _SuoraProfileDataPrivateDataHardware SuoraProfileDataPrivateDataHardware;

struct _SuoraProfileDataPrivateDataEventhandler {
	gboolean modified;
	gchar profile_name[ROCCAT_SWARM_RMP_PROFILE_NAME_LENGTH + 1];
	RoccatSwarmGamefile gamefile_names[ROCCAT_SWARM_RMP_GAMEFILE_NUM];
	gint profile_index;

	guint sound_feedback;
	gdouble notification_volume;
};

struct _SuoraProfileDataPrivateDataHardware {
	gboolean modified_illumination;
	SuoraIllumination illumination;

	gboolean modified_keys;
	SuoraRkpKeys keys;

	gboolean modified_macros[SUORA_MACRO_NUM];
	SuoraRkpMacro macros[SUORA_MACRO_NUM];
};

struct _SuoraProfileDataPrivateData {
	SuoraProfileDataPrivateDataEventhandler eventhandler;
	SuoraProfileDataPrivateDataHardware hardware;
};

struct _SuoraProfileDataPrivate {
	gchar *path;
	SuoraProfileDataPrivateData *original;
	SuoraProfileDataPrivateData *updated;
};

G_DEFINE_TYPE(SuoraProfileData, suora_profile_data, GAMINGGEAR_PROFILE_DATA_TYPE);

static gboolean suora_profile_data_private_data_eventhandler_get_modified(SuoraProfileDataPrivateDataEventhandler const *data) {
	return data->modified;
}

static void suora_profile_data_private_data_eventhandler_set_modified(SuoraProfileDataPrivateDataEventhandler *data, gboolean state) {
	data->modified = state;
}

static gboolean suora_profile_data_private_data_hardware_get_modified(SuoraProfileDataPrivateDataHardware const *data) {
	guint i;

	if (data->modified_illumination ||
			data->modified_keys)
		return TRUE;

	for (i = 0; i < SUORA_MACRO_NUM; ++i)
		if (data->modified_macros[i])
			return TRUE;

	return FALSE;
}

static void suora_profile_data_private_data_hardware_set_modified(SuoraProfileDataPrivateDataHardware *data, gboolean state) {
	guint i;

	data->modified_illumination = state;
	data->modified_keys = state;

	for (i = 0; i < SUORA_MACRO_NUM; ++i)
		data->modified_macros[i] = state;
}

static gboolean suora_profile_data_get_modified(GaminggearProfileData *self) {
	SuoraProfileDataPrivate *priv = SUORA_PROFILE_DATA(self)->priv;

	if (suora_profile_data_private_data_eventhandler_get_modified(&priv->updated->eventhandler))
		return TRUE;

	return suora_profile_data_private_data_hardware_get_modified(&priv->updated->hardware);
}

static void suora_profile_data_reset(GaminggearProfileData *self) {
	SuoraProfileDataPrivate *priv = SUORA_PROFILE_DATA(self)->priv;
	*priv->updated = *priv->original;
}

static gchar const *suora_profile_data_get_name(GaminggearProfileData *self) {
	return SUORA_PROFILE_DATA(self)->priv->updated->eventhandler.profile_name;
}

static void suora_profile_data_set_name(GaminggearProfileData *self, gchar const *new_name) {
	SuoraProfileDataPrivate *priv = SUORA_PROFILE_DATA(self)->priv;

	if (strncmp(priv->updated->eventhandler.profile_name, new_name, ROCCAT_SWARM_RMP_PROFILE_NAME_LENGTH)) {
		g_strlcpy(priv->updated->eventhandler.profile_name, new_name, ROCCAT_SWARM_RMP_PROFILE_NAME_LENGTH + 1);
		priv->updated->eventhandler.modified = TRUE;
	}
}

gchar const *suora_profile_data_get_gamefile(GaminggearProfileData *self, guint index) {
	return SUORA_PROFILE_DATA(self)->priv->updated->eventhandler.gamefile_names[index];
}

void suora_profile_data_set_gamefile(GaminggearProfileData *self, guint index, gchar const *new_gamefile) {
	SuoraProfileDataPrivate *priv = SUORA_PROFILE_DATA(self)->priv;

	if (strncmp(priv->updated->eventhandler.gamefile_names[index], new_gamefile, ROCCAT_SWARM_RMP_GAMEFILE_LENGTH)) {
		g_strlcpy(priv->updated->eventhandler.gamefile_names[index], new_gamefile, ROCCAT_SWARM_RMP_GAMEFILE_LENGTH);
		priv->updated->eventhandler.modified = TRUE;
	}
}

gint suora_profile_data_get_hardware_index(GaminggearProfileData *self) {
	return SUORA_PROFILE_DATA(self)->priv->updated->eventhandler.profile_index;
}

void suora_profile_data_set_hardware_index(GaminggearProfileData *self, gint new_index) {
	SuoraProfileDataPrivate *priv = SUORA_PROFILE_DATA(self)->priv;

	if (priv->updated->eventhandler.profile_index != new_index) {
		priv->updated->eventhandler.profile_index = new_index;
		suora_profile_data_private_data_eventhandler_set_modified(&priv->updated->eventhandler, TRUE);
		suora_profile_data_private_data_hardware_set_modified(&priv->updated->hardware, TRUE);
		// FIXME set profile_index also in original data?
	}
}

static gboolean suora_profile_data_load(GaminggearProfileData *self, gchar const *path, GError **error) {
	SuoraProfileDataPrivate *priv = SUORA_PROFILE_DATA(self)->priv;
	gchar *data;
	gsize length;

	g_free(priv->path);
	priv->path = g_strdup(path);

	if (!g_file_get_contents(path, &data, &length, error))
		return FALSE;

	if (length != sizeof(SuoraProfileDataPrivateData)) {
		g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_INVAL, _("Data has wrong length, %zu instead of %zu"),
				(gsize)length, (gsize)sizeof(SuoraProfileDataPrivateData));
		g_free(data);
		return FALSE;
	}

	g_free(priv->updated);
	priv->updated = (SuoraProfileDataPrivateData *)data;

	suora_profile_data_private_data_eventhandler_set_modified(&priv->updated->eventhandler, FALSE);
	suora_profile_data_private_data_hardware_set_modified(&priv->updated->hardware, FALSE);
	*priv->original = *priv->updated;

	return TRUE;
}

gchar *suora_profile_data_path(void) {
	gchar *base = roccat_profile_dir();
	gchar *dir = g_build_path("/", base, "suora", NULL);
	g_free(base);
	return dir;
}

static gboolean suora_profile_dir_create_if_needed(GError **error) {
	gchar *dir;
	gboolean retval;

	dir = suora_profile_data_path();
	retval = roccat_profile_dir_create_if_needed(dir, error);
	g_free(dir);
	return retval;
}

static gchar *suora_build_profile_path(void) {
	gchar *dir = suora_profile_data_path();
	gchar *path;
	guint count;

	count = 0;
	while (TRUE) {
		path = g_strdup_printf("%s/profile_%u", dir, count);
		if (!g_file_test(path, G_FILE_TEST_EXISTS)) {
			g_free(dir);
			return path;
		}
		g_free(path);
		++count;
	};

	g_free(dir);
	g_free(path);
	return NULL;
}

static gboolean suora_profile_data_save(GaminggearProfileData *self, GError **error) {
	SuoraProfileDataPrivate *priv = SUORA_PROFILE_DATA(self)->priv;
	gboolean retval;

	if (!suora_profile_dir_create_if_needed(error))
		return FALSE;

	if (!priv->path)
		priv->path = suora_build_profile_path();

	retval = roccat_profile_write_with_path(priv->path, (gchar const *)priv->updated, sizeof(SuoraProfileDataPrivateData), error);

	if (retval)
		priv->updated->eventhandler.modified = FALSE;

	/* Copy everything since device might not be present. */
	*priv->original = *priv->updated;

	return retval;
}

static gboolean suora_profile_data_remove(GaminggearProfileData *self, GError **error) {
	SuoraProfileDataPrivate *priv = SUORA_PROFILE_DATA(self)->priv;

	if (priv->path) {
		if (g_unlink(priv->path) == -1) {
			g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(errno), _("Could not remove file %1$s: %2$s"), priv->path, g_strerror(errno));
		}
		g_clear_pointer(&priv->path, g_free);
	}

	g_clear_pointer(&priv->original, g_free);
	g_clear_pointer(&priv->updated, g_free);

	return TRUE;
}

static gboolean is_key_macro(SuoraRkpKeys const *keys, guint macro_index) {
	gint keys_index = suora_key_relation_find_by_macro_index(macro_index);
	if (keys_index == SUORA_KEY_RELATION_INVALID) {
		// FIXME error
		return FALSE;
	}
	return (keys->keys[keys_index].type == SUORA_RKP_KEY_TYPE_MACRO) ? TRUE : FALSE;
}

static gboolean suora_profile_data_write_illumination(GaminggearProfileData *self, RoccatDevice *device, GError **error) {
	SuoraProfileDataPrivate *priv = SUORA_PROFILE_DATA(self)->priv;
	gboolean retval;

	if (!priv->updated->hardware.modified_illumination)
		return TRUE;

	retval = suora_illumination_write(device, &priv->updated->hardware.illumination, error);

	if (retval)
		priv->updated->hardware.modified_illumination = FALSE;

	return retval;
}

static gboolean suora_profile_data_write_keys(GaminggearProfileData *self, RoccatDevice *device, GError **error) {
	SuoraProfileDataPrivate *priv = SUORA_PROFILE_DATA(self)->priv;
	SuoraKeys keys;
	gboolean retval;

	if (!priv->updated->hardware.modified_keys)
		return TRUE;

	suora_rkp_keys_to_keys(&keys, &priv->updated->hardware.keys);
	retval = suora_keys_write(device, &keys, error);

	if (retval)
		priv->updated->hardware.modified_keys = FALSE;

	return retval;
}

static gboolean suora_profile_data_write_macros(GaminggearProfileData *self, RoccatDevice *device, GError **error) {
	SuoraProfileDataPrivate *priv = SUORA_PROFILE_DATA(self)->priv;
	guint macro_index;
	SuoraMacro macro;

	for (macro_index = 0; macro_index < SUORA_MACRO_NUM; ++macro_index) {
		if (!priv->updated->hardware.modified_macros[macro_index])
			continue;

		if (is_key_macro(&priv->updated->hardware.keys, macro_index)) {
			if (!suora_rkp_macro_to_macro(&priv->updated->hardware.macros[macro_index], &macro, error))
				return FALSE;

			if (!suora_macro_write(device, macro_index, &macro, error))
				return FALSE;
		}

		/* Always set macro unmodified */
		priv->updated->hardware.modified_macros[macro_index] = FALSE;
	}

	return TRUE;
}

static gboolean suora_profile_data_write(GaminggearProfileData *self, GaminggearDevice *device, GError **error) {
	SuoraProfileDataPrivate *priv = SUORA_PROFILE_DATA(self)->priv;
	RoccatDevice *roccat_device = ROCCAT_DEVICE(device);

	if (!suora_profile_data_write_illumination(self, roccat_device, error)) return FALSE;
	if (!suora_profile_data_write_keys(self, roccat_device, error)) return FALSE;
	if (!suora_profile_data_write_macros(self, roccat_device, error)) return FALSE;

	priv->original->hardware = priv->updated->hardware;

	return TRUE;
}

static gboolean suora_profile_data_read_illumination(GaminggearProfileData *self, RoccatDevice *device, GError **error) {
	SuoraProfileDataPrivate *priv = SUORA_PROFILE_DATA(self)->priv;
	SuoraIllumination *illumination = suora_illumination_read(device, error);

	if (illumination == NULL)
		return FALSE;
	suora_illumination_copy(&priv->updated->hardware.illumination, illumination);
	g_free(illumination);

	priv->updated->hardware.modified_illumination = FALSE;

	return TRUE;
}

static gboolean suora_profile_data_read_keys(GaminggearProfileData *self, RoccatDevice *device, GError **error) {
	SuoraProfileDataPrivate *priv = SUORA_PROFILE_DATA(self)->priv;
	SuoraKeys *keys;
	SuoraRkpKeys rkp_keys;

	keys = suora_keys_read(device, error);
	if (keys == NULL)
		return FALSE;

	suora_keys_to_rkp_keys(&rkp_keys, keys);
	g_free(keys);
	suora_rkp_keys_copy(&priv->updated->hardware.keys, &rkp_keys);

	priv->updated->hardware.modified_keys = FALSE;

	return TRUE;
}

static gboolean suora_profile_data_read_macros(GaminggearProfileData *self, RoccatDevice *device, GError **error) {
	SuoraProfileDataPrivate *priv = SUORA_PROFILE_DATA(self)->priv;
	guint macro_index;
	SuoraMacro *macro;

	for (macro_index = 0; macro_index < SUORA_MACRO_NUM; ++macro_index) {
		if (is_key_macro(&priv->updated->hardware.keys, macro_index)) {
			macro = suora_macro_read(device, macro_index, error);
			if (!macro)
				return FALSE;

			suora_macro_to_rkp_macro(macro, &priv->updated->hardware.macros[macro_index]);
		}

		priv->updated->hardware.modified_macros[macro_index] = FALSE;
	}

	return TRUE;
}

static gboolean suora_profile_data_read(GaminggearProfileData *self, GaminggearDevice *device, GError **error) {
	SuoraProfileDataPrivate *priv = SUORA_PROFILE_DATA(self)->priv;
	RoccatDevice *roccat_device = ROCCAT_DEVICE(device);

	if (!suora_profile_data_read_illumination(self, roccat_device, error)) return FALSE;
	if (!suora_profile_data_read_keys(self, roccat_device, error)) return FALSE;
	if (!suora_profile_data_read_macros(self, roccat_device, error)) return FALSE;

	priv->original->hardware = priv->updated->hardware;

	return TRUE;
}

static gboolean suora_profile_data_equal(GaminggearProfileData *self, GaminggearProfileData *other) {
	SuoraProfileDataPrivate *self_priv = SUORA_PROFILE_DATA(self)->priv;
	SuoraProfileDataPrivate *other_priv = SUORA_PROFILE_DATA(other)->priv;

	return (memcmp(self_priv->updated, other_priv->updated, sizeof(SuoraProfileDataPrivateData)) == 0) ? TRUE : FALSE;
}

static void suora_profile_data_copy(GaminggearProfileData *self, GaminggearProfileData *other) {
	SuoraProfileDataPrivate *self_priv = SUORA_PROFILE_DATA(self)->priv;
	SuoraProfileDataPrivate *other_priv = SUORA_PROFILE_DATA(other)->priv;

	GAMINGGEAR_PROFILE_DATA_CLASS(suora_profile_data_parent_class)->copy(self, other);
	self_priv->path = g_strdup(other_priv->path);
	*self_priv->updated = *other_priv->updated;
	*self_priv->original = *other_priv->original;
}

static void suora_profile_data_discriminate(GaminggearProfileData *self) {
	SuoraProfileDataPrivate *self_priv = SUORA_PROFILE_DATA(self)->priv;
	gchar *new_name;

	GAMINGGEAR_PROFILE_DATA_CLASS(suora_profile_data_parent_class)->discriminate(self);

	g_clear_pointer(&self_priv->path, g_free); /* needs another file name */

	new_name = g_strdup_printf("%s_new", suora_profile_data_get_name(self));
	suora_profile_data_set_name(self, new_name);
	g_free(new_name);

	self_priv->updated->eventhandler.profile_index = GAMINGGEAR_PROFILE_DATA_GAMEFILE_PROFILE_INVALID;

	suora_profile_data_private_data_eventhandler_set_modified(&self_priv->updated->eventhandler, TRUE);
	suora_profile_data_private_data_hardware_set_modified(&self_priv->updated->hardware, TRUE);

	*self_priv->original = *self_priv->updated;
}

static void suora_profile_data_init(SuoraProfileData *profile_data) {
	SuoraProfileDataPrivate *priv = SUORA_PROFILE_DATA_GET_PRIVATE(profile_data);
	profile_data->priv = priv;

	priv->path = NULL;

	priv->updated = (SuoraProfileDataPrivateData *)g_malloc0(sizeof(SuoraProfileDataPrivateData));

	suora_profile_data_set_name(GAMINGGEAR_PROFILE_DATA(profile_data), "Default");
	priv->updated->eventhandler.profile_index = GAMINGGEAR_PROFILE_DATA_GAMEFILE_PROFILE_INVALID;
	priv->updated->eventhandler.sound_feedback = SUORA_PROFILE_DATA_SOUND_FEEDBACK_NONE;
	priv->updated->eventhandler.notification_volume = 0.0;
	suora_profile_data_private_data_eventhandler_set_modified(&priv->updated->eventhandler, TRUE);

	suora_rkp_keys_copy(&priv->updated->hardware.keys, suora_rkp_keys_default());
	suora_illumination_copy(&priv->updated->hardware.illumination, suora_illumination_default());
	suora_profile_data_private_data_hardware_set_modified(&priv->updated->hardware, TRUE);

	priv->original = (SuoraProfileDataPrivateData *)g_malloc0(sizeof(SuoraProfileDataPrivateData));

	*priv->original = *priv->updated;
}

static void suora_profile_data_finalize(GObject *object) {
	SuoraProfileDataPrivate *priv = SUORA_PROFILE_DATA(object)->priv;

	g_clear_pointer(&priv->original, g_free);
	g_clear_pointer(&priv->updated, g_free);
	g_clear_pointer(&priv->path, g_free);

	G_OBJECT_CLASS(suora_profile_data_parent_class)->finalize(object);
}

static void suora_profile_data_class_init(SuoraProfileDataClass *klass) {
	GObjectClass *gobject_class = (GObjectClass*)klass;
	GaminggearProfileDataClass *parent_class = GAMINGGEAR_PROFILE_DATA_CLASS(klass);

	gobject_class->finalize = suora_profile_data_finalize;

	parent_class->get_name = suora_profile_data_get_name;
	parent_class->set_name = suora_profile_data_set_name;
	parent_class->get_gamefile = suora_profile_data_get_gamefile;
	parent_class->set_gamefile = suora_profile_data_set_gamefile;
	parent_class->get_hardware_index = suora_profile_data_get_hardware_index;
	parent_class->set_hardware_index = suora_profile_data_set_hardware_index;
	parent_class->load = suora_profile_data_load;
	parent_class->save = suora_profile_data_save;
	parent_class->remove = suora_profile_data_remove;
	parent_class->read = suora_profile_data_read;
	parent_class->write = suora_profile_data_write;
	parent_class->get_modified = suora_profile_data_get_modified;
	parent_class->reset = suora_profile_data_reset;
	parent_class->copy = suora_profile_data_copy;
	parent_class->equal = suora_profile_data_equal;
	parent_class->discriminate = suora_profile_data_discriminate;

	g_type_class_add_private(klass, sizeof(SuoraProfileDataPrivate));
}

SuoraProfileData *suora_profile_data_new(void) {
	return SUORA_PROFILE_DATA(g_object_new(SUORA_PROFILE_DATA_TYPE, NULL));
}

guint suora_profile_data_get_sound_feedback(SuoraProfileData const *profile_data) {
	return profile_data->priv->updated->eventhandler.sound_feedback;
}

void suora_profile_data_set_sound_feedback(SuoraProfileData *profile_data, guint new_value) {
	SuoraProfileDataPrivate *priv = profile_data->priv;

	if (priv->updated->eventhandler.sound_feedback != new_value) {
		priv->updated->eventhandler.sound_feedback = new_value;
		priv->updated->eventhandler.modified = TRUE;
	}
}

gdouble suora_profile_data_get_volume(SuoraProfileData const *profile_data) {
	return profile_data->priv->updated->eventhandler.notification_volume;
}

void suora_profile_data_set_volume(SuoraProfileData *profile_data, gdouble new_value) {
	SuoraProfileDataPrivate *priv = profile_data->priv;

	if (priv->updated->eventhandler.notification_volume != new_value) {
		priv->updated->eventhandler.notification_volume = new_value;
		priv->updated->eventhandler.modified = TRUE;
	}
}

SuoraRkpKeys const *suora_profile_data_get_keys(SuoraProfileData const *profile_data) {
	return &profile_data->priv->updated->hardware.keys;
}

void suora_profile_data_set_keys(SuoraProfileData *profile_data, SuoraRkpKeys const *keys) {
	SuoraProfileDataPrivate *priv = profile_data->priv;

	if (!suora_rkp_keys_equal(&priv->updated->hardware.keys, keys)) {
		suora_rkp_keys_copy(&priv->updated->hardware.keys, keys);
		priv->updated->hardware.modified_keys = TRUE;
	}
}

SuoraIllumination const *suora_profile_data_get_illumination(SuoraProfileData const *profile_data) {
	return &profile_data->priv->updated->hardware.illumination;
}

void suora_profile_data_set_illumination(SuoraProfileData *profile_data, SuoraIllumination const *illumination) {
	SuoraProfileDataPrivate *priv = profile_data->priv;

	if (!suora_illumination_equal(&priv->updated->hardware.illumination, illumination)) {
		suora_illumination_copy(&priv->updated->hardware.illumination, illumination);
		priv->updated->hardware.modified_illumination = TRUE;
	}
}

SuoraRkpMacro const *suora_profile_data_get_macro(SuoraProfileData const *profile_data, guint index) {
	return &profile_data->priv->updated->hardware.macros[index];
}

void suora_profile_data_set_macro(SuoraProfileData *profile_data, guint index, SuoraRkpMacro const *macro) {
	SuoraProfileDataPrivate *priv = profile_data->priv;

	if (!suora_rkp_macro_equal(&priv->updated->hardware.macros[index], macro)) {
		suora_rkp_macro_copy(&priv->updated->hardware.macros[index], macro);
		priv->updated->hardware.modified_macros[index] = TRUE;
	}
}
