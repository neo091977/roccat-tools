#ifndef __ROCCAT_COLOR_BUTTON_H__
#define __ROCCAT_COLOR_BUTTON_H__

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

/* RoccatColorButton is a button looking like GtkColorButton with a colored
 * area in the middle.
 * Just setting the background of a button doesn't work with some dark themes.
 */

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define ROCCAT_COLOR_BUTTON_TYPE (roccat_color_button_get_type())
#define ROCCAT_COLOR_BUTTON(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ROCCAT_COLOR_BUTTON_TYPE, RoccatColorButton))
#define IS_ROCCAT_COLOR_BUTTON(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ROCCAT_COLOR_BUTTON_TYPE))

typedef struct _RoccatColorButtonClass RoccatColorButtonClass;
typedef struct _RoccatColorButton RoccatColorButton;
typedef struct _RoccatColorButtonPrivate RoccatColorButtonPrivate;

struct _RoccatColorButtonClass {
	GtkButtonClass parent_class;
};

struct _RoccatColorButton {
	GtkButton parent;
	RoccatColorButtonPrivate *priv;
};

GType roccat_color_button_get_type(void);
GtkWidget *roccat_color_button_new(void);
GtkWidget *roccat_color_button_new_with_color(GdkColor const *color);

void roccat_color_button_set_default_color(RoccatColorButton *color_button);
void roccat_color_button_set_color(RoccatColorButton *color_button, GdkColor const *color);
void roccat_color_button_get_color(RoccatColorButton *color_button, GdkColor *color);

G_END_DECLS

#endif
