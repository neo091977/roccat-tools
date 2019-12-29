#ifndef __ROCCAT_KEY_COMBO_BOX_H__
#define __ROCCAT_KEY_COMBO_BOX_H__

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

#define ROCCAT_KEY_COMBO_BOX_TYPE (roccat_key_combo_box_get_type())
#define ROCCAT_KEY_COMBO_BOX(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ROCCAT_KEY_COMBO_BOX_TYPE, RoccatKeyComboBox))
#define IS_ROCCAT_KEY_COMBO_BOX(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ROCCAT_KEY_COMBO_BOX_TYPE))
#define ROCCAT_KEY_COMBO_BOX_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ROCCAT_KEY_COMBO_BOX_TYPE, RoccatKeyComboBoxClass))
#define IS_ROCCAT_KEY_COMBO_BOX_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ROCCAT_KEY_COMBO_BOX_TYPE))
#define ROCCAT_KEY_COMBO_BOX_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), ROCCAT_KEY_COMBO_BOX_TYPE, RoccatKeyComboBoxClass))

typedef struct _RoccatKeyComboBox RoccatKeyComboBox;
typedef struct _RoccatKeyComboBoxClass RoccatKeyComboBoxClass;
typedef struct _RoccatKeyComboBoxPrivate RoccatKeyComboBoxPrivate;

struct _RoccatKeyComboBox {
#if (GTK_CHECK_VERSION(2,24,0))
	GtkComboBox parent;
#else
	GtkComboBoxEntry parent;
#endif
	RoccatKeyComboBoxPrivate *priv;
};

struct _RoccatKeyComboBoxClass {
#if (GTK_CHECK_VERSION(2,24,0))
	GtkComboBoxClass parent_class;
#else
	GtkComboBoxEntryClass parent_class;
#endif
};

/* device specific types are 8bit */
typedef enum {
	ROCCAT_KEY_COMBO_BOX_TYPE_NONE = 0xf000, /* used for toplevel elements */
} RoccatKeyComboBoxType;

typedef enum {
	ROCCAT_KEY_COMBO_BOX_GROUP_NONE = 1 << 0,
	ROCCAT_KEY_COMBO_BOX_GROUP_MACRO = 1 << 1,
	ROCCAT_KEY_COMBO_BOX_GROUP_SHORTCUT = 1 << 2,
	ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT = 1 << 3,
	ROCCAT_KEY_COMBO_BOX_GROUP_ANALOG = 1 << 4,
	ROCCAT_KEY_COMBO_BOX_GROUP_LAST = 1 << 8,
} RoccatKeyComboBoxGroup;

GType roccat_key_combo_box_get_type(void);

void roccat_key_combo_box_set_model(RoccatKeyComboBox *key_combo_box, GtkTreeModel *model);
void roccat_key_combo_box_set_text(RoccatKeyComboBox *key_combo_box, gchar const *text);
gint roccat_key_combo_box_get_active_type(RoccatKeyComboBox *key_combo_box);

void roccat_key_combo_box_set_exclude_mask(RoccatKeyComboBox *combo_box, guint exclude_mask);
guint roccat_key_combo_box_get_exclude_mask(RoccatKeyComboBox *combo_box);

GtkTreeStore *roccat_key_combo_box_store_new(void);
void roccat_key_combo_box_store_append(GtkTreeStore *store, GtkTreeIter *iter, GtkTreeIter *parent,
		gchar const *title, gint type, gint group);

gchar *roccat_key_combo_box_model_get_title_for_type(GtkTreeModel *model, gint type);
gchar *roccat_key_combo_box_model_get_title_for_type_explicit(GtkTreeModel *model, gint type);

G_END_DECLS

#endif
