#ifndef __ROCCAT_LIGHT_SELECTOR_H__
#define __ROCCAT_LIGHT_SELECTOR_H__

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

#define ROCCAT_LIGHT_SELECTOR_TYPE (roccat_light_selector_get_type())
#define ROCCAT_LIGHT_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ROCCAT_LIGHT_SELECTOR_TYPE, RoccatLightSelector))
#define IS_ROCCAT_LIGHT_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ROCCAT_LIGHT_SELECTOR_TYPE))

typedef struct _RoccatLightSelector RoccatLightSelector;

GType roccat_light_selector_get_type(void);
GtkWidget *roccat_light_selector_new_with_range(gdouble min, gdouble max, gdouble step);

void roccat_light_selector_set_range(RoccatLightSelector *selector, gdouble min, gdouble max);
void roccat_light_selector_set_increment(RoccatLightSelector *selector, gdouble step);

guint roccat_light_selector_get_brightness(RoccatLightSelector *selector);
void roccat_light_selector_set_brightness(RoccatLightSelector *selector, guint new_brightness);

guint roccat_light_selector_get_dimness(RoccatLightSelector *selector);
void roccat_light_selector_set_dimness(RoccatLightSelector *selector, guint new_dimness);

guint roccat_light_selector_get_timeout(RoccatLightSelector *selector);
void roccat_light_selector_set_timeout(RoccatLightSelector *selector, guint timeout);

G_END_DECLS

#endif
