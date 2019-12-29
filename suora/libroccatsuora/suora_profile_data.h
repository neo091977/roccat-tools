#ifndef __ROCCAT_SUORA_PROFILE_DATA_H__
#define __ROCCAT_SUORA_PROFILE_DATA_H__

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

#include <gaminggear/gaminggear_profile_data.h>
#include "suora_illumination.h"
#include "suora_rkp_keys.h"
#include "suora_rkp_macro.h"

G_BEGIN_DECLS

#define SUORA_PROFILE_DATA_TYPE (suora_profile_data_get_type())
#define SUORA_PROFILE_DATA(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), SUORA_PROFILE_DATA_TYPE, SuoraProfileData))
#define IS_SUORA_PROFILE_DATA(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), SUORA_PROFILE_DATA_TYPE))
#define SUORA_PROFILE_DATA_CLASS(obj) (G_TYPE_CHECK_CLASS_CAST((obj), SUORA_PROFILE_DATA_TYPE, SuoraProfileDataClass))
#define IS_SUORA_PROFILE_DATA_CLASS(obj) (G_TYPE_CHECK_CLASS_TYPE((obj), SUORA_PROFILE_DATA_TYPE))

typedef struct _SuoraProfileData SuoraProfileData;
typedef struct _SuoraProfileDataClass SuoraProfileDataClass;
typedef struct _SuoraProfileDataPrivate SuoraProfileDataPrivate;

struct _SuoraProfileData {
	GaminggearProfileData parent;
	SuoraProfileDataPrivate *priv;
};

struct _SuoraProfileDataClass {
	GaminggearProfileDataClass parent_class;
};

typedef enum {
	SUORA_PROFILE_DATA_SOUND_FEEDBACK_NONE = 0,
	SUORA_PROFILE_DATA_SOUND_FEEDBACK_CLICK = 1,
	SUORA_PROFILE_DATA_SOUND_FEEDBACK_TYPEWRITER = 2,
	SUORA_PROFILE_DATA_SOUND_FEEDBACK_BEAM = 3,
	SUORA_PROFILE_DATA_SOUND_FEEDBACK_SCI_FI = 4,
} SuoraProfileDataSoundFeedback;

typedef enum {
	SUORA_PROFILE_DATA_GAME_MODE_UNMODIFIED = 0,
	SUORA_PROFILE_DATA_GAME_MODE_ON = 1,
	SUORA_PROFILE_DATA_GAME_MODE_OFF = 2,
} SuoraProfileDataGameMode;

GType suora_profile_data_get_type(void);

SuoraProfileData *suora_profile_data_new(void);

guint suora_profile_data_get_sound_feedback(SuoraProfileData const *profile_data);
void suora_profile_data_set_sound_feedback(SuoraProfileData *profile_data, guint new_value);

gdouble suora_profile_data_get_volume(SuoraProfileData const *profile_data);
void suora_profile_data_set_volume(SuoraProfileData *profile_data, gdouble new_value);

SuoraRkpKeys const *suora_profile_data_get_keys(SuoraProfileData const *profile_data);
void suora_profile_data_set_keys(SuoraProfileData *profile_data, SuoraRkpKeys const *keys);

SuoraIllumination const *suora_profile_data_get_illumination(SuoraProfileData const *profile_data);
void suora_profile_data_set_illumination(SuoraProfileData *profile_data, SuoraIllumination const *illumination);

SuoraRkpMacro const *suora_profile_data_get_macro(SuoraProfileData const *profile_data, guint index);
void suora_profile_data_set_macro(SuoraProfileData *profile_data, guint index, SuoraRkpMacro const *macro);

gchar *suora_profile_data_path(void);

G_END_DECLS

#endif
