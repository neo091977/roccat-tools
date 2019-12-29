#ifndef __ROCCAT_MULTIWIDGET_H__
#define __ROCCAT_MULTIWIDGET_H__

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

#define ROCCAT_MULTIWIDGET_TYPE (roccat_multiwidget_get_type())
#define ROCCAT_MULTIWIDGET(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ROCCAT_MULTIWIDGET_TYPE, RoccatMultiwidget))
#define IS_ROCCAT_MULTIWIDGET(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ROCCAT_MULTIWIDGET_TYPE))

typedef struct _RoccatMultiwidget RoccatMultiwidget;

GType roccat_multiwidget_get_type(void);
GtkWidget *roccat_multiwidget_new(void);

void roccat_multiwidget_add(RoccatMultiwidget *multiwidget, GtkWidget *widget);
void roccat_multiwidget_show(RoccatMultiwidget *multiwidget, GtkWidget *child);
void roccat_multiwidget_show_all(RoccatMultiwidget *multiwidget);
void roccat_multiwidget_show_none(RoccatMultiwidget *multiwidget);

G_END_DECLS

#endif
