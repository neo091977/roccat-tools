#ifndef __ROCCAT_RYOS_LAYER_MODE_SELECTOR_H__
#define __ROCCAT_RYOS_LAYER_MODE_SELECTOR_H__

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

#define RYOS_LAYER_MODE_SELECTOR_TYPE (ryos_layer_mode_selector_get_type())
#define RYOS_LAYER_MODE_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), RYOS_LAYER_MODE_SELECTOR_TYPE, RyosLayerModeSelector))
#define IS_RYOS_LAYER_MODE_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), RYOS_LAYER_MODE_SELECTOR_TYPE))

typedef struct _RyosLayerModeSelector RyosLayerModeSelector;

GType ryos_layer_mode_selector_get_type(void);
GtkWidget *ryos_layer_mode_selector_new(void);

void ryos_layer_mode_selector_set_value(RyosLayerModeSelector *mode_selector, guint mode);
guint ryos_layer_mode_selector_get_value(RyosLayerModeSelector *mode_selector);

G_END_DECLS

#endif
