#ifndef __ROCCAT_CONFIG_WINDOW_PAGES_H__
#define __ROCCAT_CONFIG_WINDOW_PAGES_H__

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
#include "roccat_profile_page.h"

G_BEGIN_DECLS

#define ROCCAT_CONFIG_WINDOW_PAGES_TYPE (roccat_config_window_pages_get_type())
#define ROCCAT_CONFIG_WINDOW_PAGES_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ROCCAT_CONFIG_WINDOW_PAGES_TYPE, RoccatConfigWindowPagesClass))
#define IS_ROCCAT_CONFIG_WINDOW_PAGES_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ROCCAT_CONFIG_WINDOW_PAGES_TYPE))
#define ROCCAT_CONFIG_WINDOW_PAGES(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ROCCAT_CONFIG_WINDOW_PAGES_TYPE, RoccatConfigWindowPages))
#define IS_ROCCAT_CONFIG_WINDOW_PAGES(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ROCCAT_CONFIG_WINDOW_PAGES_TYPE))

typedef struct _RoccatConfigWindowPagesClass RoccatConfigWindowPagesClass;
typedef struct _RoccatConfigWindowPages RoccatConfigWindowPages;
typedef struct _RoccatConfigWindowPagesPrivate RoccatConfigWindowPagesPrivate;

struct _RoccatConfigWindowPagesClass {
	RoccatConfigWindowClass parent_class;
};

struct _RoccatConfigWindowPages {
	RoccatConfigWindow window;
	RoccatConfigWindowPagesPrivate *priv;
};

GType roccat_config_window_pages_get_type(void);
GtkWidget *roccat_config_window_pages_new(gchar const *device_name);

void roccat_config_window_pages_append_page(RoccatConfigWindowPages *config_window, RoccatProfilePage *profile_page);
void roccat_config_window_pages_remove_pages(RoccatConfigWindowPages *config_window);
gint roccat_config_window_pages_get_page_index(RoccatConfigWindowPages *config_window, RoccatProfilePage *profile_page);
RoccatProfilePage *roccat_config_window_pages_get_page(RoccatConfigWindowPages *config_window, guint index);
guint roccat_config_window_pages_get_num_pages(RoccatConfigWindowPages *config_window);

gboolean roccat_config_window_pages_get_page_moved(RoccatConfigWindowPages *config_window, RoccatProfilePage *profile_page);
void roccat_config_window_pages_set_page_unmoved(RoccatConfigWindowPages *config_window, RoccatProfilePage *profile_page);

void roccat_config_window_pages_set_active_page_blocked(RoccatConfigWindowPages *config_window, guint index);
gint roccat_config_window_pages_get_active_page(RoccatConfigWindowPages *config_window);

G_END_DECLS

#endif
