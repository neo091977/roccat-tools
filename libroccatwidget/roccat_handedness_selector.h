#ifndef __ROCCAT_HANDEDNESS_SELECTOR_H__
#define __ROCCAT_HANDEDNESS_SELECTOR_H__

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

#define ROCCAT_HANDEDNESS_SELECTOR_TYPE (roccat_handedness_selector_get_type())
#define ROCCAT_HANDEDNESS_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ROCCAT_HANDEDNESS_SELECTOR_TYPE, RoccatHandednessSelector))
#define IS_ROCCAT_HANDEDNESS_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ROCCAT_HANDEDNESS_SELECTOR_TYPE))

typedef struct _RoccatHandednessSelectorClass RoccatHandednessSelectorClass;
typedef struct _RoccatHandednessSelector RoccatHandednessSelector;
typedef struct _RoccatHandednessSelectorPrivate RoccatHandednessSelectorPrivate;

struct _RoccatHandednessSelectorClass {
	GtkFrameClass parent_class;
};

struct _RoccatHandednessSelector {
	GtkFrame frame;
	RoccatHandednessSelectorPrivate *priv;
};

typedef enum {
	ROCCAT_HANDEDNESS_SELECTOR_NONE,
	ROCCAT_HANDEDNESS_SELECTOR_LEFT,
	ROCCAT_HANDEDNESS_SELECTOR_RIGHT,
} RoccatHandednessSelectorValues;

GType roccat_handedness_selector_get_type(void);
GtkWidget *roccat_handedness_selector_new(void);

guint roccat_handedness_selector_get_value(RoccatHandednessSelector *selector);
void roccat_handedness_selector_set_value(RoccatHandednessSelector *selector, guint new_value);
void roccat_handedness_selector_set_value_blocked(RoccatHandednessSelector *selector, guint new_value);

G_END_DECLS

#endif
