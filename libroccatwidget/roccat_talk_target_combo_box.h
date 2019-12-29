#ifndef __ROCCAT_TALK_TARGET_COMBO_BOX_H__
#define __ROCCAT_TALK_TARGET_COMBO_BOX_H__

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

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define ROCCAT_TALK_TARGET_COMBO_BOX_TYPE (roccat_talk_target_combo_box_get_type())
#define ROCCAT_TALK_TARGET_COMBO_BOX(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ROCCAT_TALK_TARGET_COMBO_BOX_TYPE, RoccatTalkTargetComboBox))
#define IS_ROCCAT_TALK_TARGET_COMBO_BOX(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ROCCAT_TALK_TARGET_COMBO_BOX_TYPE))

typedef struct _RoccatTalkTargetComboBox RoccatTalkTargetComboBox;
typedef struct _RoccatTalkTargetComboBoxPrivate RoccatTalkTargetComboBoxPrivate;

struct _RoccatTalkTargetComboBox {
	GtkComboBox combo;
	RoccatTalkTargetComboBoxPrivate *priv;
};

GType roccat_talk_target_combo_box_get_type(void);
GtkWidget *roccat_talk_target_combo_box_new(guint source_talk_device_type);

guint roccat_talk_target_combo_box_get_value(RoccatTalkTargetComboBox *talk_target_combo_box);
void roccat_talk_target_combo_box_set_value(RoccatTalkTargetComboBox *talk_target_combo_box, guint value);

guint roccat_talk_target_combo_box_get_source_talk_device_type(RoccatTalkTargetComboBox *talk_target_combo_box);
void roccat_talk_target_combo_box_set_source_talk_device_type(RoccatTalkTargetComboBox *talk_target_combo_box, guint value);

gchar const *roccat_talk_target_combo_box_get_text_for_value(guint talk_device);

G_END_DECLS

#endif
