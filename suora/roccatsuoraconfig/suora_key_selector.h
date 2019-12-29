#ifndef __ROCCAT_SUORA_KEY_SELECTOR_H__
#define __ROCCAT_SUORA_KEY_SELECTOR_H__

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

#include "suora_key_combo_box.h"

G_BEGIN_DECLS

#define SUORA_KEY_SELECTOR_TYPE (suora_key_selector_get_type())
#define SUORA_KEY_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), SUORA_KEY_SELECTOR_TYPE, SuoraKeySelector))
#define IS_SUORA_KEY_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), SUORA_KEY_SELECTOR_TYPE))

typedef struct _SuoraKeySelector SuoraKeySelector;

GType suora_key_selector_get_type(void);
GtkWidget *suora_key_selector_new(SuoraKeyComboBoxData *basepointer);

void suora_key_selector_set_index(SuoraKeySelector *selector, gint index, guint mask, guint standard);
gint suora_key_selector_get_index(SuoraKeySelector *selector);

void suora_key_selector_update(SuoraKeySelector *selector);

gchar *suora_key_selector_get_text_for_data(SuoraKeySelector *selector, SuoraKeyComboBoxData const *data);

G_END_DECLS

#endif
