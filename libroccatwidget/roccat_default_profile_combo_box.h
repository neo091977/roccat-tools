#ifndef __ROCCAT_DEFAULT_PROFILE_COMBO_BOX_H__
#define __ROCCAT_DEFAULT_PROFILE_COMBO_BOX_H__

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

#define ROCCAT_DEFAULT_PROFILE_COMBO_BOX_TYPE (roccat_default_profile_combo_box_get_type())
#define ROCCAT_DEFAULT_PROFILE_COMBO_BOX(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ROCCAT_DEFAULT_PROFILE_COMBO_BOX_TYPE, RoccatDefaultProfileComboBox))
#define IS_ROCCAT_DEFAULT_PROFILE_COMBO_BOX(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ROCCAT_DEFAULT_PROFILE_COMBO_BOX_TYPE))

typedef struct _RoccatDefaultProfileComboBox RoccatDefaultProfileComboBox;

GType roccat_default_profile_combo_box_get_type(void);
GtkWidget *roccat_default_profile_combo_box_new(guint max_profile_number);

guint roccat_default_profile_combo_box_get_value(RoccatDefaultProfileComboBox *combo_box);
void roccat_default_profile_combo_box_set_value(RoccatDefaultProfileComboBox *combo_box, guint new_value);

G_END_DECLS

#endif
