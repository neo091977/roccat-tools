#ifndef __ROCCAT_CONFIG_WINDOW_H__
#define __ROCCAT_CONFIG_WINDOW_H__

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

#include "roccat_device_scanner_interface.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define ROCCAT_CONFIG_WINDOW_TYPE (roccat_config_window_get_type())
#define ROCCAT_CONFIG_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ROCCAT_CONFIG_WINDOW_TYPE, RoccatConfigWindowClass))
#define IS_ROCCAT_CONFIG_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ROCCAT_CONFIG_WINDOW_TYPE))
#define ROCCAT_CONFIG_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ROCCAT_CONFIG_WINDOW_TYPE, RoccatConfigWindow))
#define IS_ROCCAT_CONFIG_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ROCCAT_CONFIG_WINDOW_TYPE))

typedef struct _RoccatConfigWindowClass RoccatConfigWindowClass;
typedef struct _RoccatConfigWindow RoccatConfigWindow;
typedef struct _RoccatConfigWindowPrivate RoccatConfigWindowPrivate;

struct _RoccatConfigWindowClass {
	GtkWindowClass parent_class;
};

struct _RoccatConfigWindow {
	GtkWindow window;
	RoccatConfigWindowPrivate *priv;
};

GType roccat_config_window_get_type(void);
GtkWidget *roccat_config_window_new(gchar const *device_name);

void roccat_config_window_add(RoccatConfigWindow *config_window, GtkWidget *content);

void roccat_config_window_menu_insert(RoccatConfigWindow *config_window, GtkMenuItem *menu_item);
void roccat_config_window_edit_menu_append(RoccatConfigWindow *config_window, GtkMenuItem *menu_item);
void roccat_config_window_device_menu_append(RoccatConfigWindow *config_window, GtkMenuItem *menu_item);

void roccat_config_window_set_device_scanner(RoccatConfigWindow *config_window, RoccatDeviceScannerInterface *device_scanner_interface);
void roccat_config_window_device_scanner_suspend(RoccatConfigWindow *config_window);
void roccat_config_window_device_scanner_continue(RoccatConfigWindow *config_window);

RoccatDevice *roccat_config_window_get_device(RoccatConfigWindow *config_window);
gboolean roccat_config_window_warn_if_no_device(RoccatConfigWindow *config_window);

G_END_DECLS

#endif
