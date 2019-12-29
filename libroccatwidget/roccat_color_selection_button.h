#ifndef __ROCCAT_COLOR_SELECTION_BUTTON_H__
#define __ROCCAT_COLOR_SELECTION_BUTTON_H__

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

/*
 * RoccatColorSelectionButton works like GtkColorButton but provides same look and
 * feel as RoccatColorPaletteButton
 */

#include "roccat_color_button.h"
#include "roccat_color_selection_dialog.h"
#include "roccat_light.h"

G_BEGIN_DECLS

#define ROCCAT_COLOR_SELECTION_BUTTON_TYPE (roccat_color_selection_button_get_type())
#define ROCCAT_COLOR_SELECTION_BUTTON_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ROCCAT_COLOR_SELECTION_BUTTON_TYPE, RoccatColorSelectionButtonClass))
#define IS_ROCCAT_COLOR_SELECTION_BUTTON_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ROCCAT_COLOR_SELECTION_BUTTON_TYPE))
#define ROCCAT_COLOR_SELECTION_BUTTON(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ROCCAT_COLOR_SELECTION_BUTTON_TYPE, RoccatColorSelectionButton))
#define IS_ROCCAT_COLOR_SELECTION_BUTTON(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ROCCAT_COLOR_SELECTION_BUTTON_TYPE))

typedef struct _RoccatColorSelectionButtonClass RoccatColorSelectionButtonClass;
typedef struct _RoccatColorSelectionButton RoccatColorSelectionButton;
typedef struct _RoccatColorSelectionButtonPrivate RoccatColorSelectionButtonPrivate;

struct _RoccatColorSelectionButtonClass {
	RoccatColorButtonClass parent_class;
};

struct _RoccatColorSelectionButton {
	RoccatColorButton parent;
	RoccatColorSelectionButtonPrivate *priv;
};

GType roccat_color_selection_button_get_type(void);
GtkWidget *roccat_color_selection_button_new(void);

void roccat_color_selection_button_set_view_type(RoccatColorSelectionButton *button, RoccatColorSelectionDialogType type);

void roccat_color_selection_button_set_palette_dimensions(RoccatColorSelectionButton *button, size_t length, guint width);
void roccat_color_selection_button_set_palette_colors(RoccatColorSelectionButton *button, GdkColor const *colors, size_t length);
void roccat_color_selection_button_set_palette_index(RoccatColorSelectionButton *button, gint index);
gint roccat_color_selection_button_get_palette_index(RoccatColorSelectionButton *button);
gboolean roccat_color_selection_button_get_palette_color(RoccatColorSelectionButton *button, GdkColor *color);

void roccat_color_selection_button_set_custom_color(RoccatColorSelectionButton *button, GdkColor const *color);
void roccat_color_selection_button_get_custom_color(RoccatColorSelectionButton *button, GdkColor *color);

/* Returns active color regardless of active type */
gboolean roccat_color_selection_button_get_color(RoccatColorSelectionButton *button, GdkColor *color);

void roccat_color_selection_button_set_active_type(RoccatColorSelectionButton *button, RoccatColorSelectionDialogType type);
RoccatColorSelectionDialogType roccat_color_selection_button_get_active_type(RoccatColorSelectionButton *button);

void roccat_color_selection_button_set_roccat_light(RoccatColorSelectionButton *button, RoccatLight const *light);
void roccat_color_selection_button_get_roccat_light(RoccatColorSelectionButton *button, RoccatLight *light);

G_END_DECLS

#endif
