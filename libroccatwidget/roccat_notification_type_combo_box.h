#ifndef __ROCCAT_NOTIFICATION_TYPE_COMBO_BOX_H__
#define __ROCCAT_NOTIFICATION_TYPE_COMBO_BOX_H__

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

GtkWidget *roccat_notification_type_combo_box_new(void);
gint roccat_notification_type_combo_box_get_value(GtkComboBox *combo);
void roccat_notification_type_combo_box_set_value(GtkComboBox *combo, gint value);

G_END_DECLS

#endif
