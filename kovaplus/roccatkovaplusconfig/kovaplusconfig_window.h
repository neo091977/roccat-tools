#ifndef __ROCCAT_KOVAPLUSCONFIG_WINDOW_H__
#define __ROCCAT_KOVAPLUSCONFIG_WINDOW_H__

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

#define KOVAPLUSCONFIG_WINDOW_TYPE (kovaplusconfig_window_get_type())
#define KOVAPLUSCONFIG_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), KOVAPLUSCONFIG_WINDOW_TYPE, KovaplusconfigWindow))
#define IS_KOVAPLUSCONFIG_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), KOVAPLUSCONFIG_WINDOW))

typedef struct _KovaplusconfigWindow KovaplusconfigWindow;

GType kovaplusconfig_window_get_type(void);
GtkWidget *kovaplusconfig_window_new(void);

G_END_DECLS

#endif
