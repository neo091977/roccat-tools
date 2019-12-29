#ifndef __ROCCAT_SKELTR_PROFILE_DATA_H__
#define __ROCCAT_SKELTR_PROFILE_DATA_H__

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
#include "skeltr_key_mask.h"
#include "skeltr_keys_easyzone.h"
#include "skeltr_keys_extra.h"
#include "skeltr_keys_function.h"
#include "skeltr_keys_macro.h"
#include "skeltr_keys_primary.h"
#include "skeltr_keys_thumbster.h"
#include "skeltr_light.h"
#include "skeltr_macro.h"
#include "roccat_timer.h"

G_BEGIN_DECLS

#define SKELTR_PROFILE_DATA_TYPE (skeltr_profile_data_get_type())
#define SKELTR_PROFILE_DATA(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), SKELTR_PROFILE_DATA_TYPE, SkeltrProfileData))
#define IS_SKELTR_PROFILE_DATA(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), SKELTR_PROFILE_DATA_TYPE))
#define SKELTR_PROFILE_DATA_CLASS(obj) (G_TYPE_CHECK_CLASS_CAST((obj), SKELTR_PROFILE_DATA_TYPE, SkeltrProfileDataClass))
#define IS_SKELTR_PROFILE_DATA_CLASS(obj) (G_TYPE_CHECK_CLASS_TYPE((obj), SKELTR_PROFILE_DATA_TYPE))

typedef struct _SkeltrProfileData SkeltrProfileData;
typedef struct _SkeltrProfileDataClass SkeltrProfileDataClass;
typedef struct _SkeltrProfileDataPrivate SkeltrProfileDataPrivate;

struct _SkeltrProfileData {
	GaminggearProfileData parent;
	SkeltrProfileDataPrivate *priv;
};

struct _SkeltrProfileDataClass {
	GaminggearProfileDataClass parent_class;
};

GType skeltr_profile_data_get_type(void);

SkeltrProfileData *skeltr_profile_data_new(void);
gboolean skeltr_profile_data_load(SkeltrProfileData *self, guint profile_index, GError **error);

RoccatButton const *skeltr_profile_data_get_key_for_macro_index(SkeltrProfileData const *profile_data, guint macro_index);

gdouble skeltr_profile_data_get_sound_feedback_volume(SkeltrProfileData const *profile_data);
void skeltr_profile_data_set_sound_feedback_volume(SkeltrProfileData *profile_data, gdouble volume);

guint skeltr_profile_data_get_sound_feedback_typing(SkeltrProfileData const *profile_data);
void skeltr_profile_data_set_sound_feedback_typing(SkeltrProfileData *profile_data, guint typing);

guint skeltr_profile_data_get_wakeup_notification_type(SkeltrProfileData const *profile_data);
void skeltr_profile_data_set_wakeup_notification_type(SkeltrProfileData *profile_data, guint wakeup);

guint skeltr_profile_data_get_sleep_notification_type(SkeltrProfileData const *profile_data);
void skeltr_profile_data_set_sleep_notification_type(SkeltrProfileData *profile_data, guint sleep);

guint skeltr_profile_data_get_timer_notification_type(SkeltrProfileData const *profile_data);
void skeltr_profile_data_set_timer_notification_type(SkeltrProfileData *profile_data, guint type);

guint skeltr_profile_data_get_macro_record_notification_type(SkeltrProfileData const *profile_data);
void skeltr_profile_data_set_macro_record_notification_type(SkeltrProfileData *profile_data, guint type);

guint skeltr_profile_data_get_profile_switch_notification_type(SkeltrProfileData const *profile_data);
void skeltr_profile_data_set_profile_switch_notification_type(SkeltrProfileData *profile_data, guint type);

SkeltrKeyMask const *skeltr_profile_data_get_key_mask(SkeltrProfileData const *profile_data);
void skeltr_profile_data_set_key_mask(SkeltrProfileData *profile_data, SkeltrKeyMask const *key_mask);

SkeltrKeysEasyzone const *skeltr_profile_data_get_keys_easyzone(SkeltrProfileData const *profile_data);
void skeltr_profile_data_set_keys_easyzone(SkeltrProfileData *profile_data, SkeltrKeysEasyzone const *keys_easyzone);

SkeltrKeysExtra const *skeltr_profile_data_get_keys_extra(SkeltrProfileData const *profile_data);
void skeltr_profile_data_set_keys_extra(SkeltrProfileData *profile_data, SkeltrKeysExtra const *keys_extra);

SkeltrKeysFunction const *skeltr_profile_data_get_keys_function(SkeltrProfileData const *profile_data);
void skeltr_profile_data_set_keys_function(SkeltrProfileData *profile_data, SkeltrKeysFunction const *keys_function);

SkeltrKeysMacro const *skeltr_profile_data_get_keys_macro(SkeltrProfileData const *profile_data);
void skeltr_profile_data_set_keys_macro(SkeltrProfileData *profile_data, SkeltrKeysMacro const *keys_macro);

SkeltrKeysPrimary const *skeltr_profile_data_get_keys_primary(SkeltrProfileData const *profile_data);
void skeltr_profile_data_set_keys_primary(SkeltrProfileData *profile_data, SkeltrKeysPrimary const *keys_primary);

SkeltrKeysThumbster const *skeltr_profile_data_get_keys_thumbster(SkeltrProfileData const *profile_data);
void skeltr_profile_data_set_keys_thumbster(SkeltrProfileData *profile_data, SkeltrKeysThumbster const *keys_thumbster);

SkeltrLight const *skeltr_profile_data_get_light(SkeltrProfileData const *profile_data);
void skeltr_profile_data_set_light(SkeltrProfileData *profile_data, SkeltrLight const *light);

SkeltrMacro const *skeltr_profile_data_get_macro(SkeltrProfileData const *profile_data, guint index);
void skeltr_profile_data_set_macro(SkeltrProfileData *profile_data, guint index, SkeltrMacro const *macro);

gchar const *skeltr_profile_data_get_opener(SkeltrProfileData const *profile_data, guint index);
void skeltr_profile_data_set_opener(SkeltrProfileData *profile_data, guint index, gchar const *opener);

RoccatTimer const *skeltr_profile_data_get_timer(SkeltrProfileData const *profile_data, guint index);
void skeltr_profile_data_set_timer(SkeltrProfileData *profile_data, guint index, RoccatTimer const *timer);

guint16 skeltr_profile_data_get_capslock_talk_target(SkeltrProfileData const *profile_data);
void skeltr_profile_data_set_capslock_talk_target(SkeltrProfileData *profile_data, guint16 target);

void skeltr_profile_data_set_brightness_from_hardware(SkeltrProfileData *profile_data, guint brightnes);
void skeltr_profile_data_set_macro_from_hardware(SkeltrProfileData *profile_data, guint index, SkeltrMacro const *macro);

G_END_DECLS

#endif
