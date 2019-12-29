#ifndef __ROCCAT_KEY_MASK_SELECTOR_H__
#define __ROCCAT_KEY_MASK_SELECTOR_H__

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

#define ROCCAT_KEY_MASK_SELECTOR_TYPE (roccat_key_mask_selector_get_type())
#define ROCCAT_KEY_MASK_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ROCCAT_KEY_MASK_SELECTOR_TYPE, RoccatKeyMaskSelector))
#define IS_ROCCAT_KEY_MASK_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ROCCAT_KEY_MASK_SELECTOR_TYPE))

typedef struct _RoccatKeyMaskSelector RoccatKeyMaskSelector;
typedef struct _RoccatKeyMaskSelectorClass RoccatKeyMaskSelectorClass;
typedef struct _RoccatKeyMaskSelectorPrivate RoccatKeyMaskSelectorPrivate;

struct _RoccatKeyMaskSelectorClass {
	GtkFrameClass parent_class;
};

struct _RoccatKeyMaskSelector {
	GtkFrame parent;
	RoccatKeyMaskSelectorPrivate *priv;
};

GType roccat_key_mask_selector_get_type(void);
GtkWidget *roccat_key_mask_selector_new(guint count, gchar const **values);

void roccat_key_mask_selector_set_active(RoccatKeyMaskSelector *selector, guint index, gboolean is_active);
gboolean roccat_key_mask_selector_get_active(RoccatKeyMaskSelector *selector, guint index);

G_END_DECLS

#endif
