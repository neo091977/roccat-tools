#ifndef __ROCCAT_ISKUFXCONFIG_WINDOW_H__
#define __ROCCAT_ISKUFXCONFIG_WINDOW_H__

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

#define ISKUFXCONFIG_WINDOW_TYPE (iskufxconfig_window_get_type())
#define ISKUFXCONFIG_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ISKUFXCONFIG_WINDOW_TYPE, IskufxconfigWindow))
#define IS_ISKUFXCONFIG_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ISKUFXCONFIG_WINDOW))

typedef struct _IskufxconfigWindow IskufxconfigWindow;

GType iskufxconfig_window_get_type(void);
GtkWidget *iskufxconfig_window_new(void);

G_END_DECLS

#endif
