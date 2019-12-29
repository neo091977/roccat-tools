#ifndef __ROCCAT_RYOS_LAYOUT_SELECTOR_H__
#define __ROCCAT_RYOS_LAYOUT_SELECTOR_H__

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

#define RYOS_LAYOUT_SELECTOR_TYPE (ryos_layout_selector_get_type())
#define RYOS_LAYOUT_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), RYOS_LAYOUT_SELECTOR_TYPE, RyosLayoutSelector))
#define IS_RYOS_LAYOUT_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), RYOS_LAYOUT_SELECTOR_TYPE))

typedef struct _RyosLayoutSelector RyosLayoutSelector;

GType ryos_layout_selector_get_type(void);
GtkWidget *ryos_layout_selector_new(void);

void ryos_layout_selector_set_value(RyosLayoutSelector *layout_selector, gchar const *layout);
gchar *ryos_layout_selector_get_value(RyosLayoutSelector *layout_selector);

G_END_DECLS

#endif
