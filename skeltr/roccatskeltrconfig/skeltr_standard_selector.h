#ifndef __ROCCAT_SKELTR_STANDARD_SELECTOR_H__
#define __ROCCAT_SKELTR_STANDARD_SELECTOR_H__

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

#include "skeltr_key_combo_box.h"

G_BEGIN_DECLS

#define SKELTR_STANDARD_SELECTOR_TYPE (skeltr_standard_selector_get_type())
#define SKELTR_STANDARD_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), SKELTR_STANDARD_SELECTOR_TYPE, SkeltrStandardSelector))
#define IS_SKELTR_STANDARD_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), SKELTR_STANDARD_SELECTOR_TYPE))

typedef struct _SkeltrStandardSelector SkeltrStandardSelector;

GType skeltr_standard_selector_get_type(void);
GtkWidget *skeltr_standard_selector_new(SkeltrKeyComboBoxData *standard_basepointer, SkeltrKeyComboBoxData *secondary_basepointer);

void skeltr_standard_selector_set_index(SkeltrStandardSelector *selector, gint index, guint standard_mask, guint standard_standard, guint secondary_mask, guint secondary_standard);
gint skeltr_standard_selector_get_index(SkeltrStandardSelector *selector);

void skeltr_standard_selector_set_secondary_text(SkeltrStandardSelector *selector, gchar const *text);

void skeltr_standard_selector_update(SkeltrStandardSelector *selector);

gchar *skeltr_standard_selector_get_text_for_data(SkeltrStandardSelector *selector, SkeltrKeyComboBoxData const *data);

G_END_DECLS

#endif
