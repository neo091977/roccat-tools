#ifndef __ROCCAT_RYOS_STANDARD_SELECTOR_H__
#define __ROCCAT_RYOS_STANDARD_SELECTOR_H__

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

#include "ryos_key_combo_box.h"

G_BEGIN_DECLS

#define RYOS_STANDARD_SELECTOR_TYPE (ryos_standard_selector_get_type())
#define RYOS_STANDARD_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), RYOS_STANDARD_SELECTOR_TYPE, RyosStandardSelector))
#define IS_RYOS_STANDARD_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), RYOS_STANDARD_SELECTOR_TYPE))

typedef struct _RyosStandardSelector RyosStandardSelector;

GType ryos_standard_selector_get_type(void);
GtkWidget *ryos_standard_selector_new(RyosKeyComboBoxData *standard_basepointer, RyosKeyComboBoxData *secondary_basepointer);

void ryos_standard_selector_set_index(RyosStandardSelector *selector, gint index, guint standard_mask, guint standard_standard, guint secondary_mask, guint secondary_standard);
gint ryos_standard_selector_get_index(RyosStandardSelector *selector);

void ryos_standard_selector_set_secondary_text(RyosStandardSelector *selector, gchar const *text);

void ryos_standard_selector_update(RyosStandardSelector *selector);

gchar *ryos_standard_selector_get_text_for_data(RyosStandardSelector *selector, RyosKeyComboBoxData const *data);

G_END_DECLS

#endif
