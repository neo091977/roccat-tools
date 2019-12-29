#ifndef __ROCCAT_COLOR_PALETTE_SELECTOR_H__
#define __ROCCAT_COLOR_PALETTE_SELECTOR_H__

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

#define ROCCAT_COLOR_PALETTE_SELECTOR_TYPE (roccat_color_palette_selector_get_type())
#define ROCCAT_COLOR_PALETTE_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ROCCAT_COLOR_PALETTE_SELECTOR_TYPE, RoccatColorPaletteSelector))
#define IS_ROCCAT_COLOR_PALETTE_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ROCCAT_COLOR_PALETTE_SELECTOR_TYPE))

typedef struct _RoccatColorPaletteSelector RoccatColorPaletteSelector;
typedef struct _RoccatColorPaletteSelectorPrivate RoccatColorPaletteSelectorPrivate;

struct _RoccatColorPaletteSelector {
	GtkTable parent;
	RoccatColorPaletteSelectorPrivate *priv;
};

GType roccat_color_palette_selector_get_type(void);

GtkWidget *roccat_color_palette_selector_new(void);
GtkWidget *roccat_color_palette_selector_new_with_colors(GdkColor const *colors, size_t length, guint width);

void roccat_color_palette_selector_set_dimensions(RoccatColorPaletteSelector *color_palette_selector, size_t length, guint width);
void roccat_color_palette_selector_set_colors(RoccatColorPaletteSelector *color_palette_selector, GdkColor const *colors, size_t length);
void roccat_color_palette_selector_set_index(RoccatColorPaletteSelector *color_palette_selector, gint index);

gint roccat_color_palette_selector_get_index(RoccatColorPaletteSelector *color_palette_selector);
gboolean roccat_color_palette_selector_get_color(RoccatColorPaletteSelector *color_palette_selector, GdkColor *color);
gboolean roccat_color_palette_selector_get_color_for_index(RoccatColorPaletteSelector *color_palette_selector, gint index, GdkColor *color);

G_END_DECLS

#endif
