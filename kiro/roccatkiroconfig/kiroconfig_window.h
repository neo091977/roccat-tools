#ifndef __ROCCAT_KIROCONFIG_WINDOW_H__
#define __ROCCAT_KIROCONFIG_WINDOW_H__

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

#define KIROCONFIG_WINDOW_TYPE (kiroconfig_window_get_type())
#define KIROCONFIG_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), KIROCONFIG_WINDOW_TYPE, KiroconfigWindow))
#define IS_KIROCONFIG_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), KIROCONFIG_WINDOW))

typedef struct _KiroconfigWindow KiroconfigWindow;

GType kiroconfig_window_get_type(void);
GtkWidget *kiroconfig_window_new(void);

G_END_DECLS

#endif
