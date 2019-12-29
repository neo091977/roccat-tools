#ifndef __ROCCAT_SOVACONFIG_WINDOW_H__
#define __ROCCAT_SOVACONFIG_WINDOW_H__

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

#define SOVACONFIG_WINDOW_TYPE (sovaconfig_window_get_type())
#define SOVACONFIG_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), SOVACONFIG_WINDOW_TYPE, SovaconfigWindow))
#define IS_SOVACONFIG_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), SOVACONFIG_WINDOW))

typedef struct _SovaconfigWindow SovaconfigWindow;

GType sovaconfig_window_get_type(void);
GtkWidget *sovaconfig_window_new(void);

G_END_DECLS

#endif
