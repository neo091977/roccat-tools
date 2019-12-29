#ifndef __ROCCAT_KEY_BUTTON_H__
#define __ROCCAT_KEY_BUTTON_H__

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

typedef gchar *(*roccat_key_button_name_converter)(guint8 hid);

#define ROCCAT_KEY_BUTTON_TYPE (roccat_key_button_get_type())
#define ROCCAT_KEY_BUTTON(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ROCCAT_KEY_BUTTON_TYPE, RoccatKeyButton))
#define IS_ROCCAT_KEY_BUTTON(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ROCCAT_KEY_BUTTON_TYPE))

typedef struct _RoccatKeyButton RoccatKeyButton;

GType roccat_key_button_get_type(void);
GtkWidget *roccat_key_button_new(RoccatKeyButton *group, guint hid_usage_id, roccat_key_button_name_converter converter);
void roccat_key_button_set_label(RoccatKeyButton *key_button, guint hid_usage_id);
void roccat_key_button_set_light(RoccatKeyButton *key_button, gboolean state);
void roccat_key_button_set_color(RoccatKeyButton *key_button, GdkColor const *color);
void roccat_key_button_set_intensity(RoccatKeyButton *key_button, gfloat intensity);

G_END_DECLS

#endif
