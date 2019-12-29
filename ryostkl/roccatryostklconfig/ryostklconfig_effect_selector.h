#ifndef __ROCCAT_RYOSTKLCONFIG_EFFECT_SELECTOR_H__
#define __ROCCAT_RYOSTKLCONFIG_EFFECT_SELECTOR_H__

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

#define RYOSTKLCONFIG_EFFECT_SELECTOR_TYPE (ryostklconfig_effect_selector_get_type())
#define RYOSTKLCONFIG_EFFECT_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), RYOSTKLCONFIG_EFFECT_SELECTOR_TYPE, RyostklconfigEffectSelector))
#define IS_RYOSTKLCONFIG_EFFECT_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), RYOSTKLCONFIG_EFFECT_SELECTOR_TYPE))

typedef struct _RyostklconfigEffectSelector RyostklconfigEffectSelector;

GType ryostklconfig_effect_selector_get_type(void);
GtkWidget *ryostklconfig_effect_selector_new(void);

void ryostklconfig_effect_selector_set_effect_blocked(RyostklconfigEffectSelector *effect_selector, guint effect);
guint ryostklconfig_effect_selector_get_effect(RyostklconfigEffectSelector *effect_selector);

void ryostklconfig_effect_selector_set_delay_time_blocked(RyostklconfigEffectSelector *effect_selector, guint delay_time);
guint ryostklconfig_effect_selector_get_delay_time(RyostklconfigEffectSelector *effect_selector);

void ryostklconfig_effect_selector_set_fade_time_blocked(RyostklconfigEffectSelector *effect_selector, guint fade_time);
guint ryostklconfig_effect_selector_get_fade_time(RyostklconfigEffectSelector *effect_selector);

G_END_DECLS

#endif
