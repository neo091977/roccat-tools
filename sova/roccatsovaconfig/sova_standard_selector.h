#ifndef __ROCCAT_SOVA_STANDARD_SELECTOR_H__
#define __ROCCAT_SOVA_STANDARD_SELECTOR_H__

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

#define SOVA_STANDARD_SELECTOR_TYPE (sova_standard_selector_get_type())
#define SOVA_STANDARD_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), SOVA_STANDARD_SELECTOR_TYPE, SovaStandardSelector))
#define IS_SOVA_STANDARD_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), SOVA_STANDARD_SELECTOR_TYPE))

typedef struct _SovaStandardSelector SovaStandardSelector;

GType sova_standard_selector_get_type(void);
GtkWidget *sova_standard_selector_new(SovaKeyComboBoxData *standard_basepointer, SovaKeyComboBoxData *secondary_basepointer);

void sova_standard_selector_set_index(SovaStandardSelector *selector, gint index, guint standard_mask, guint standard_standard, guint secondary_mask, guint secondary_standard);
gint sova_standard_selector_get_index(SovaStandardSelector *selector);

void sova_standard_selector_set_secondary_text(SovaStandardSelector *selector, gchar const *text);

void sova_standard_selector_update(SovaStandardSelector *selector);

gchar *sova_standard_selector_get_text_for_data(SovaStandardSelector *selector, SovaKeyComboBoxData const *data);

G_END_DECLS

#endif
