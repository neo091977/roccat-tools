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

#include <gaminggear/hid_uid.h>
#include "isku_capslock_frame.h"
#include "isku_key_combo_box.h"
#include "i18n-lib.h"

#define ISKU_CAPSLOCK_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ISKU_CAPSLOCK_FRAME_TYPE, IskuCapslockFrameClass))
#define IS_ISKU_CAPSLOCK_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ISKU_CAPSLOCK_FRAME_TYPE))
#define ISKU_CAPSLOCK_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ISKU_CAPSLOCK_FRAME_TYPE, IskuCapslockFramePrivate))

typedef struct _IskuCapslockFrameClass IskuCapslockFrameClass;
typedef struct _IskuCapslockFramePrivate IskuCapslockFramePrivate;

struct _IskuCapslockFrame {
	GtkFrame parent;
	IskuCapslockFramePrivate *priv;
};

struct _IskuCapslockFrameClass {
	GtkFrameClass parent_class;
};

struct _IskuCapslockFramePrivate {
	IskuKeyComboBox *button;
};

G_DEFINE_TYPE(IskuCapslockFrame, isku_capslock_frame, GTK_TYPE_FRAME);

void isku_capslock_frame_set_key(IskuCapslockFrame *capslock_frame, guint key_index, IskuRkpMacroKeyInfo *key_info) {
	if (key_index == ISKU_KEY_INDEX_CAPSLOCK)
		isku_key_combo_box_set_value_blocked(capslock_frame->priv->button, key_info);
}

GtkWidget *isku_capslock_frame_new(void) {
	IskuCapslockFrame *frame;

	frame = ISKU_CAPSLOCK_FRAME(g_object_new(ISKU_CAPSLOCK_FRAME_TYPE,
			"label", _("Caps lock key"),
			NULL));

	return GTK_WIDGET(frame);
}

static void isku_capslock_frame_init(IskuCapslockFrame *frame) {
	IskuCapslockFramePrivate *priv = ISKU_CAPSLOCK_FRAME_GET_PRIVATE(frame);
	GtkWidget *table;

	frame->priv = priv;

	table = gtk_table_new(1, 2, FALSE);
	gtk_container_add(GTK_CONTAINER(frame), table);

	priv->button = ISKU_KEY_COMBO_BOX(isku_key_combo_box_new(
			ROCCAT_KEY_COMBO_BOX_GROUP_MACRO | ROCCAT_KEY_COMBO_BOX_GROUP_SHORTCUT,
			HID_UID_KB_CAPS_LOCK));
	gtk_table_attach(GTK_TABLE(table), gtk_label_new(_("Caps lock")), 0, 1, 0, 1, GTK_EXPAND, GTK_EXPAND, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(priv->button), 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);
}

static void isku_capslock_frame_class_init(IskuCapslockFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(IskuCapslockFramePrivate));
}

void isku_capslock_frame_set_from_rkp(IskuCapslockFrame *buttons_frame, IskuRkp *rkp) {
	IskuCapslockFramePrivate *priv = buttons_frame->priv;
	IskuRkpMacroKeyInfo *key_info;

	key_info = isku_rkp_get_macro_key_info(rkp, ISKU_KEY_INDEX_CAPSLOCK);
	isku_key_combo_box_set_value_blocked(priv->button, key_info);
}

void isku_capslock_frame_update_rkp(IskuCapslockFrame *buttons_frame, IskuRkp *rkp) {
	IskuCapslockFramePrivate *priv = buttons_frame->priv;
	IskuRkpMacroKeyInfo *key_info;

	key_info = isku_key_combo_box_get_value(priv->button);
	isku_rkp_set_macro_key_info(rkp, ISKU_KEY_INDEX_CAPSLOCK, key_info);
	isku_rkp_macro_key_info_free(key_info);
}
