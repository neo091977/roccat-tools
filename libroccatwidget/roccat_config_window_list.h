#ifndef __ROCCAT_CONFIG_WINDOW_LIST_H__
#define __ROCCAT_CONFIG_WINDOW_LIST_H__

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

#include "roccat_config_window.h"
#include <gaminggear/gaminggear_profile_page.h>

G_BEGIN_DECLS

#define ROCCAT_CONFIG_WINDOW_LIST_TYPE (roccat_config_window_list_get_type())
#define ROCCAT_CONFIG_WINDOW_LIST_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ROCCAT_CONFIG_WINDOW_LIST_TYPE, RoccatConfigWindowListClass))
#define IS_ROCCAT_CONFIG_WINDOW_LIST_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ROCCAT_CONFIG_WINDOW_LIST_TYPE))
#define ROCCAT_CONFIG_WINDOW_LIST(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ROCCAT_CONFIG_WINDOW_LIST_TYPE, RoccatConfigWindowList))
#define IS_ROCCAT_CONFIG_WINDOW_LIST(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ROCCAT_CONFIG_WINDOW_LIST_TYPE))

typedef struct _RoccatConfigWindowListClass RoccatConfigWindowListClass;
typedef struct _RoccatConfigWindowList RoccatConfigWindowList;
typedef struct _RoccatConfigWindowListPrivate RoccatConfigWindowListPrivate;

struct _RoccatConfigWindowListClass {
	RoccatConfigWindowClass parent_class;
};

struct _RoccatConfigWindowList {
	RoccatConfigWindow window;
	RoccatConfigWindowListPrivate *priv;
};

GType roccat_config_window_list_get_type(void);

gboolean roccat_config_window_list_load(RoccatConfigWindowList *window, GError **error);
gboolean roccat_config_window_list_read(RoccatConfigWindowList *window, RoccatDevice *device, guint count, GError **error);

GaminggearProfilePage *roccat_config_window_list_get_profile_page(RoccatConfigWindowList *window);

gchar const *roccat_config_window_list_get_default_profile_name(RoccatConfigWindowList *window);
void roccat_config_window_list_set_default_profile_name(RoccatConfigWindowList *window, gchar const *name);

G_END_DECLS

#endif
