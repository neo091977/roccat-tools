#ifndef __ROCCAT_SOVA_PRIMARY_SELECTOR_H__
#define __ROCCAT_SOVA_PRIMARY_SELECTOR_H__

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

#include "sova_key_combo_box.h"

G_BEGIN_DECLS

#define SOVA_PRIMARY_SELECTOR_TYPE (sova_primary_selector_get_type())
#define SOVA_PRIMARY_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), SOVA_PRIMARY_SELECTOR_TYPE, SovaPrimarySelector))
#define IS_SOVA_PRIMARY_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), SOVA_PRIMARY_SELECTOR_TYPE))

typedef struct _SovaPrimarySelector SovaPrimarySelector;

GType sova_primary_selector_get_type(void);
GtkWidget *sova_primary_selector_new(SovaKeyComboBoxData *basepointer);

void sova_primary_selector_set_index(SovaPrimarySelector *selector, gint index, guint mask, guint standard);
gint sova_primary_selector_get_index(SovaPrimarySelector *selector);

void sova_primary_selector_update(SovaPrimarySelector *selector);

gchar *sova_primary_selector_get_text_for_data(SovaPrimarySelector *selector, SovaKeyComboBoxData const *data);

G_END_DECLS

#endif
