#ifndef __ROCCAT_SKELTR_PRIMARY_SELECTOR_H__
#define __ROCCAT_SKELTR_PRIMARY_SELECTOR_H__

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

#define SKELTR_PRIMARY_SELECTOR_TYPE (skeltr_primary_selector_get_type())
#define SKELTR_PRIMARY_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), SKELTR_PRIMARY_SELECTOR_TYPE, SkeltrPrimarySelector))
#define IS_SKELTR_PRIMARY_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), SKELTR_PRIMARY_SELECTOR_TYPE))

typedef struct _SkeltrPrimarySelector SkeltrPrimarySelector;

GType skeltr_primary_selector_get_type(void);
GtkWidget *skeltr_primary_selector_new(SkeltrKeyComboBoxData *basepointer);

void skeltr_primary_selector_set_index(SkeltrPrimarySelector *selector, gint index, guint mask, guint standard);
gint skeltr_primary_selector_get_index(SkeltrPrimarySelector *selector);

void skeltr_primary_selector_update(SkeltrPrimarySelector *selector);

gchar *skeltr_primary_selector_get_text_for_data(SkeltrPrimarySelector *selector, SkeltrKeyComboBoxData const *data);

G_END_DECLS

#endif
