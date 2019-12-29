#ifndef __ROCCAT_GAMEFILE_SELECTOR_H__
#define __ROCCAT_GAMEFILE_SELECTOR_H__

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

#define ROCCAT_GAMEFILE_SELECTOR_TYPE (roccat_gamefile_selector_get_type())
#define ROCCAT_GAMEFILE_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ROCCAT_GAMEFILE_SELECTOR_TYPE, RoccatGamefileSelector))
#define IS_ROCCAT_GAMEFILE_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ROCCAT_GAMEFILE_SELECTOR_TYPE))

typedef struct _RoccatGamefileSelector RoccatGamefileSelector;
typedef struct _RoccatGamefileSelectorPrivate RoccatGamefileSelectorPrivate;

struct _RoccatGamefileSelector {
	GtkFrame frame;
	RoccatGamefileSelectorPrivate *priv;
};

GType roccat_gamefile_selector_get_type(void);
GtkWidget *roccat_gamefile_selector_new(guint count);

gchar const *roccat_gamefile_selector_get_text(RoccatGamefileSelector *selector, guint index);
void roccat_gamefile_selector_set_text(RoccatGamefileSelector *selector, guint index, gchar const *new_value);

G_END_DECLS

#endif
