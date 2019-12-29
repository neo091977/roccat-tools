#ifndef __ROCCAT_ACTUAL_PROFILE_SELECTOR_H__
#define __ROCCAT_ACTUAL_PROFILE_SELECTOR_H__

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

#define ROCCAT_ACTUAL_PROFILE_SELECTOR_TYPE (roccat_actual_profile_selector_get_type())
#define ROCCAT_ACTUAL_PROFILE_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ROCCAT_ACTUAL_PROFILE_SELECTOR_TYPE, RoccatActualProfileSelector))
#define IS_ROCCAT_ACTUAL_PROFILE_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ROCCAT_ACTUAL_PROFILE_SELECTOR_TYPE))

typedef struct _RoccatActualProfileSelector RoccatActualProfileSelector;
typedef struct _RoccatActualProfileSelectorPrivate RoccatActualProfileSelectorPrivate;

struct _RoccatActualProfileSelector {
	GtkFrame frame;
	RoccatActualProfileSelectorPrivate *priv;
};

GType roccat_actual_profile_selector_get_type(void);
GtkWidget *roccat_actual_profile_selector_new(guint max);

guint roccat_actual_profile_selector_get_value(RoccatActualProfileSelector *selector);
void roccat_actual_profile_selector_set_value(RoccatActualProfileSelector *selector, guint profile_number);

G_END_DECLS

#endif
