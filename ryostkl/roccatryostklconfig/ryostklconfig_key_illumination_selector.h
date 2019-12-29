#ifndef __ROCCAT_RYOSTKLCONFIG_KEY_ILLUMINATION_SELECTOR_H__
#define __ROCCAT_RYOSTKLCONFIG_KEY_ILLUMINATION_SELECTOR_H__

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

#define RYOSTKLCONFIG_KEY_ILLUMINATION_SELECTOR_TYPE (ryostklconfig_key_illumination_selector_get_type())
#define RYOSTKLCONFIG_KEY_ILLUMINATION_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), RYOSTKLCONFIG_KEY_ILLUMINATION_SELECTOR_TYPE, RyostklconfigKeyIlluminationSelector))
#define IS_RYOSTKLCONFIG_KEY_ILLUMINATION_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), RYOSTKLCONFIG_KEY_ILLUMINATION_SELECTOR_TYPE))

typedef struct _RyostklconfigKeyIlluminationSelector RyostklconfigKeyIlluminationSelector;

GType ryostklconfig_key_illumination_selector_get_type(void);
GtkWidget *ryostklconfig_key_illumination_selector_new(void);

void ryostklconfig_key_illumination_selector_set_value_blocked(RyostklconfigKeyIlluminationSelector *illumination_selector, guint16 value);
guint16 ryostklconfig_key_illumination_selector_get_value(RyostklconfigKeyIlluminationSelector *illumination_selector);

void ryostklconfig_key_illumination_selector_set_limited(RyostklconfigKeyIlluminationSelector *illumination_selector, gboolean limited);

G_END_DECLS

#endif
