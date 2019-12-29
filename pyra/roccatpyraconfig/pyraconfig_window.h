#ifndef __ROCCAT_PYRACONFIG_WINDOW_H__
#define __ROCCAT_PYRACONFIG_WINDOW_H__

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

#define PYRACONFIG_WINDOW_TYPE (pyraconfig_window_get_type())
#define PYRACONFIG_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), PYRACONFIG_WINDOW_TYPE, PyraconfigWindow))
#define IS_PYRACONFIG_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), PYRACONFIG_WINDOW))

typedef struct _PyraconfigWindow PyraconfigWindow;

GType pyraconfig_window_get_type(void);
GtkWidget *pyraconfig_window_new(void);

G_END_DECLS

#endif
