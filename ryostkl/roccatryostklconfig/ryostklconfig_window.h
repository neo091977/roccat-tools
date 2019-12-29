#ifndef __ROCCAT_RYOSTKLCONFIG_WINDOW_H__
#define __ROCCAT_RYOSTKLCONFIG_WINDOW_H__

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

#include "roccat_config_window_pages.h"

G_BEGIN_DECLS

#define RYOSTKLCONFIG_WINDOW_TYPE (ryostklconfig_window_get_type())
#define RYOSTKLCONFIG_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), RYOSTKLCONFIG_WINDOW_TYPE, RyostklconfigWindow))
#define IS_RYOSTKLCONFIG_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), RYOSTKLCONFIG_WINDOW))

typedef struct _RyostklconfigWindow RyostklconfigWindow;

GType ryostklconfig_window_get_type(void);
GtkWidget *ryostklconfig_window_new(void);

G_END_DECLS

#endif
