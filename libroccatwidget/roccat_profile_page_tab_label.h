#ifndef __ROCCAT_PROFILE_PAGE_TAB_LABEL_H__
#define __ROCCAT_PROFILE_PAGE_TAB_LABEL_H__

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

#define ROCCAT_PROFILE_PAGE_TAB_LABEL_TYPE (roccat_profile_page_tab_label_get_type())
#define ROCCAT_PROFILE_PAGE_TAB_LABEL(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ROCCAT_PROFILE_PAGE_TAB_LABEL_TYPE, RoccatProfilePageTabLabel))
#define IS_ROCCAT_PROFILE_PAGE_TAB_LABEL(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ROCCAT_PROFILE_PAGE_TAB_LABEL_TYPE))

#define ROCCAT_PROFILE_PAGE_TAB_LABEL_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ROCCAT_PROFILE_PAGE_TAB_LABEL_TYPE, RoccatProfilePageTabLabelClass))
#define IS_ROCCAT_PROFILE_PAGE_TAB_LABEL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ROCCAT_PROFILE_PAGE_TAB_LABEL_TYPE))

typedef struct _RoccatProfilePageTabLabelClass RoccatProfilePageTabLabelClass;
typedef struct _RoccatProfilePageTabLabel RoccatProfilePageTabLabel;
typedef struct _RoccatProfilePageTabLabelPrivate RoccatProfilePageTabLabelPrivate;

struct _RoccatProfilePageTabLabel {
	GtkHBox parent;
	RoccatProfilePageTabLabelPrivate *priv;
};

struct _RoccatProfilePageTabLabelClass {
	GtkHBoxClass parent_class;
};

GType roccat_profile_page_tab_label_get_type(void);
GtkWidget *roccat_profile_page_tab_label_new(void);

void roccat_profile_page_tab_label_set_name(RoccatProfilePageTabLabel *tab_label, gchar const *name);
gchar *roccat_profile_page_tab_label_get_name(RoccatProfilePageTabLabel *tab_label);
void roccat_profile_page_tab_label_set_index(RoccatProfilePageTabLabel *tab_label, gint index);
gint roccat_profile_page_tab_label_get_index(RoccatProfilePageTabLabel *tab_label);

void roccat_profile_page_tab_label_set_group(RoccatProfilePageTabLabel *tab_label, GSList *group);
GSList *roccat_profile_page_tab_label_get_group(RoccatProfilePageTabLabel *tab_label);
void roccat_profile_page_tab_label_set_active(RoccatProfilePageTabLabel *tab_label);

gboolean roccat_profile_page_tab_label_get_closeable(RoccatProfilePageTabLabel *tab_label);
void roccat_profile_page_tab_label_set_closeable(RoccatProfilePageTabLabel *tab_label, gboolean closeable);

G_END_DECLS

#endif
