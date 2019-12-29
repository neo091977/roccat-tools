#ifndef __ROCCAT_PROFILE_PAGE_H__
#define __ROCCAT_PROFILE_PAGE_H__

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

#define ROCCAT_PROFILE_PAGE_TYPE (roccat_profile_page_get_type())
#define ROCCAT_PROFILE_PAGE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ROCCAT_PROFILE_PAGE_TYPE, RoccatProfilePage))
#define IS_ROCCAT_PROFILE_PAGE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ROCCAT_PROFILE_PAGE_TYPE))

#define ROCCAT_PROFILE_PAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ROCCAT_PROFILE_PAGE_TYPE, RoccatProfilePageClass))
#define IS_ROCCAT_PROFILE_PAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ROCCAT_PROFILE_PAGE_TYPE))

typedef struct _RoccatProfilePageClass RoccatProfilePageClass;
typedef struct _RoccatProfilePage RoccatProfilePage;
typedef struct _RoccatProfilePagePrivate RoccatProfilePagePrivate;

struct _RoccatProfilePage {
	GtkVBox parent;
	RoccatProfilePagePrivate *priv;
};

struct _RoccatProfilePageClass {
	GtkVBoxClass parent_class;
};

GType roccat_profile_page_get_type(void);

void roccat_profile_page_set_name(RoccatProfilePage *profile_page, gchar const *name);
gchar *roccat_profile_page_get_name(RoccatProfilePage *profile_page);

G_END_DECLS

#endif
