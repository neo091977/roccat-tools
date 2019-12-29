#ifndef __ROCCAT_SOVA_PROFILE_DATA_H__
#define __ROCCAT_SOVA_PROFILE_DATA_H__

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
#include "sova_key_mask.h"
#include "sova_keys_function.h"
#include "sova_keys_primary.h"
#include "sova_keys_easyzone.h"
#include "sova_keys_extra.h"
#include "sova_macro.h"
#include "sova_light.h"
#include "roccat_timer.h"

G_BEGIN_DECLS

#define SOVA_PROFILE_DATA_TYPE (sova_profile_data_get_type())
#define SOVA_PROFILE_DATA(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), SOVA_PROFILE_DATA_TYPE, SovaProfileData))
#define IS_SOVA_PROFILE_DATA(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), SOVA_PROFILE_DATA_TYPE))
#define SOVA_PROFILE_DATA_CLASS(obj) (G_TYPE_CHECK_CLASS_CAST((obj), SOVA_PROFILE_DATA_TYPE, SovaProfileDataClass))
#define IS_SOVA_PROFILE_DATA_CLASS(obj) (G_TYPE_CHECK_CLASS_TYPE((obj), SOVA_PROFILE_DATA_TYPE))

typedef struct _SovaProfileData SovaProfileData;
typedef struct _SovaProfileDataClass SovaProfileDataClass;
typedef struct _SovaProfileDataPrivate SovaProfileDataPrivate;

struct _SovaProfileData {
	GaminggearProfileData parent;
	SovaProfileDataPrivate *priv;
};

struct _SovaProfileDataClass {
	GaminggearProfileDataClass parent_class;
};

GType sova_profile_data_get_type(void);

SovaProfileData *sova_profile_data_new(void);
gboolean sova_profile_data_load(SovaProfileData *self, guint profile_index, GError **error);

gdouble sova_profile_data_get_sound_feedback_volume(SovaProfileData const *profile_data);
void sova_profile_data_set_sound_feedback_volume(SovaProfileData *profile_data, gdouble volume);

guint sova_profile_data_get_sound_feedback_typing(SovaProfileData const *profile_data);
void sova_profile_data_set_sound_feedback_typing(SovaProfileData *profile_data, guint typing);

guint sova_profile_data_get_wakeup_notification_type(SovaProfileData const *profile_data);
void sova_profile_data_set_wakeup_notification_type(SovaProfileData *profile_data, guint wakeup);

guint sova_profile_data_get_sleep_notification_type(SovaProfileData const *profile_data);
void sova_profile_data_set_sleep_notification_type(SovaProfileData *profile_data, guint sleep);

guint sova_profile_data_get_timer_notification_type(SovaProfileData const *profile_data);
void sova_profile_data_set_timer_notification_type(SovaProfileData *profile_data, guint type);

guint sova_profile_data_get_macro_record_notification_type(SovaProfileData const *profile_data);
void sova_profile_data_set_macro_record_notification_type(SovaProfileData *profile_data, guint type);

guint sova_profile_data_get_profile_switch_notification_type(SovaProfileData const *profile_data);
void sova_profile_data_set_profile_switch_notification_type(SovaProfileData *profile_data, guint type);

SovaKeysExtra const *sova_profile_data_get_keys_extra(SovaProfileData const *profile_data);
void sova_profile_data_set_keys_extra(SovaProfileData *profile_data, SovaKeysExtra const *keys_extra);

SovaLight const *sova_profile_data_get_light(SovaProfileData const *profile_data);
void sova_profile_data_set_light(SovaProfileData *profile_data, SovaLight const *light);

SovaKeyMask const *sova_profile_data_get_key_mask(SovaProfileData const *profile_data);
void sova_profile_data_set_key_mask(SovaProfileData *profile_data, SovaKeyMask const *key_mask);

SovaKeysFunction const *sova_profile_data_get_keys_function(SovaProfileData const *profile_data);
void sova_profile_data_set_keys_function(SovaProfileData *profile_data, SovaKeysFunction const *keys_function);

SovaKeysEasyzone const *sova_profile_data_get_keys_easyzone(SovaProfileData const *profile_data);
void sova_profile_data_set_keys_easyzone(SovaProfileData *profile_data, SovaKeysEasyzone const *keys_easyzone);

SovaKeysPrimary const *sova_profile_data_get_keys_primary(SovaProfileData const *profile_data);
void sova_profile_data_set_keys_primary(SovaProfileData *profile_data, SovaKeysPrimary const *keys_primary);

SovaMacro const *sova_profile_data_get_macro(SovaProfileData const *profile_data, guint index);
void sova_profile_data_set_macro(SovaProfileData *profile_data, guint index, SovaMacro const *macro);

gchar const *sova_profile_data_get_opener(SovaProfileData const *profile_data, guint index);
void sova_profile_data_set_opener(SovaProfileData *profile_data, guint index, gchar const *opener);

RoccatTimer const *sova_profile_data_get_timer(SovaProfileData const *profile_data, guint index);
void sova_profile_data_set_timer(SovaProfileData *profile_data, guint index, RoccatTimer const *timer);

guint16 sova_profile_data_get_capslock_talk_target(SovaProfileData const *profile_data);
void sova_profile_data_set_capslock_talk_target(SovaProfileData *profile_data, guint16 target);

void sova_profile_data_set_brightness_from_hardware(SovaProfileData *profile_data, guint brightnes);
void sova_profile_data_set_macro_from_hardware(SovaProfileData *profile_data, guint index, SovaMacro const *macro);

G_END_DECLS

#endif
