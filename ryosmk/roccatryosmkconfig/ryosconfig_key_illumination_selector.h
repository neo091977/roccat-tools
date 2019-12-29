#ifndef __ROCCAT_RYOSCONFIG_KEY_ILLUMINATION_SELECTOR_H__
#define __ROCCAT_RYOSCONFIG_KEY_ILLUMINATION_SELECTOR_H__

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

#define RYOSCONFIG_KEY_ILLUMINATION_SELECTOR_TYPE (ryosconfig_key_illumination_selector_get_type())
#define RYOSCONFIG_KEY_ILLUMINATION_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), RYOSCONFIG_KEY_ILLUMINATION_SELECTOR_TYPE, RyosconfigKeyIlluminationSelector))
#define IS_RYOSCONFIG_KEY_ILLUMINATION_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), RYOSCONFIG_KEY_ILLUMINATION_SELECTOR_TYPE))

typedef struct _RyosconfigKeyIlluminationSelector RyosconfigKeyIlluminationSelector;

GType ryosconfig_key_illumination_selector_get_type(void);
GtkWidget *ryosconfig_key_illumination_selector_new(void);

void ryosconfig_key_illumination_selector_set_value_blocked(RyosconfigKeyIlluminationSelector *illumination_selector, guint8 value);
guint8 ryosconfig_key_illumination_selector_get_value(RyosconfigKeyIlluminationSelector *illumination_selector);

G_END_DECLS

#endif
