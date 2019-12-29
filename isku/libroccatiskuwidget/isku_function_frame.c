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
#include "isku_function_frame.h"
#include "isku_key_combo_box.h"
#include "i18n-lib.h"

#define ISKU_FUNCTION_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ISKU_FUNCTION_FRAME_TYPE, IskuFunctionFrameClass))
#define IS_ISKU_FUNCTION_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ISKU_FUNCTION_FRAME_TYPE))
#define ISKU_FUNCTION_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ISKU_FUNCTION_FRAME_TYPE, IskuFunctionFramePrivate))

typedef struct _IskuFunctionFrameClass IskuFunctionFrameClass;
typedef struct _IskuFunctionFramePrivate IskuFunctionFramePrivate;

struct _IskuFunctionFrame {
	GtkFrame parent;
	IskuFunctionFramePrivate *priv;
};

struct _IskuFunctionFrameClass {
	GtkFrameClass parent_class;
};

struct _IskuFunctionFramePrivate {
	IskuKeyComboBox *buttons[ISKU_KEYS_FUNCTION_NUM];
	guint button_rkp_index[ISKU_KEYS_FUNCTION_NUM];
};

G_DEFINE_TYPE(IskuFunctionFrame, isku_function_frame, GTK_TYPE_FRAME);

void isku_function_frame_set_key(IskuFunctionFrame *function_frame, guint key_index, IskuRkpMacroKeyInfo *key_info) {
	IskuFunctionFramePrivate *priv = function_frame->priv;
	guint i;
	for (i = 0; i < ISKU_KEYS_FUNCTION_NUM; ++i)
		if (priv->button_rkp_index[i] == key_index)
			isku_key_combo_box_set_value_blocked(priv->buttons[i], key_info);
}

typedef struct {
	gchar const * const name;
	guint index;
	guint standard_type;
} ButtonList;

static ButtonList button_list[ISKU_KEYS_FUNCTION_NUM] = {
	{"F1", ISKU_KEY_INDEX_F1 + 0, HID_UID_KB_F1},
	{"F2", ISKU_KEY_INDEX_F1 + 1, HID_UID_KB_F2},
	{"F3", ISKU_KEY_INDEX_F1 + 2, HID_UID_KB_F3},
	{"F4", ISKU_KEY_INDEX_F1 + 3, HID_UID_KB_F4},
	{"F5", ISKU_KEY_INDEX_F1 + 4, HID_UID_KB_F5},
	{"F6", ISKU_KEY_INDEX_F1 + 5, HID_UID_KB_F6},
	{"F7", ISKU_KEY_INDEX_F1 + 6, HID_UID_KB_F7},
	{"F8", ISKU_KEY_INDEX_F1 + 7, HID_UID_KB_F8},
	{"F9", ISKU_KEY_INDEX_F1 + 8, HID_UID_KB_F9},
	{"F10", ISKU_KEY_INDEX_F1 + 9, HID_UID_KB_F10},
	{"F11", ISKU_KEY_INDEX_F1 + 10, HID_UID_KB_F11},
	{"F12", ISKU_KEY_INDEX_F1 + 11, HID_UID_KB_F12},
};

GtkWidget *isku_function_frame_new(void) {
	IskuFunctionFrame *frame;

	frame = ISKU_FUNCTION_FRAME(g_object_new(ISKU_FUNCTION_FRAME_TYPE,
			"label", _("Function keys"),
			NULL));

	return GTK_WIDGET(frame);
}

static void isku_function_frame_init(IskuFunctionFrame *frame) {
	IskuFunctionFramePrivate *priv = ISKU_FUNCTION_FRAME_GET_PRIVATE(frame);
	GtkWidget *table;
	guint i;

	frame->priv = priv;

	table = gtk_table_new(ISKU_KEYS_FUNCTION_NUM, 2, FALSE);
	gtk_container_add(GTK_CONTAINER(frame), table);

	for (i = 0; i < ISKU_KEYS_FUNCTION_NUM; ++i) {
		priv->button_rkp_index[i] = button_list[i].index;
		priv->buttons[i] = ISKU_KEY_COMBO_BOX(
				isku_key_combo_box_new(ROCCAT_KEY_COMBO_BOX_GROUP_MACRO, button_list[i].standard_type));
		gtk_table_attach(GTK_TABLE(table), gtk_label_new(button_list[i].name), 0, 1, i, i + 1, GTK_EXPAND, GTK_EXPAND, 0, 0);
		gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(priv->buttons[i]), 1, 2, i, i + 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);
	}
}

static void isku_function_frame_class_init(IskuFunctionFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(IskuFunctionFramePrivate));
}

void isku_function_frame_set_from_rkp(IskuFunctionFrame *buttons_frame, IskuRkp *rkp) {
	IskuFunctionFramePrivate *priv = buttons_frame->priv;
	guint i;
	IskuRkpMacroKeyInfo *key_info;

	for (i = 0; i < ISKU_KEYS_FUNCTION_NUM; ++i) {
		key_info = isku_rkp_get_macro_key_info(rkp, priv->button_rkp_index[i]);
		isku_key_combo_box_set_value_blocked(priv->buttons[i], key_info);
	}
}

void isku_function_frame_update_rkp(IskuFunctionFrame *buttons_frame, IskuRkp *rkp) {
	IskuFunctionFramePrivate *priv = buttons_frame->priv;
	guint i;
	IskuRkpMacroKeyInfo *key_info;

	for (i = 0; i < ISKU_KEYS_FUNCTION_NUM; ++i) {
		key_info = isku_key_combo_box_get_value(priv->buttons[i]);
		isku_rkp_set_macro_key_info(rkp, priv->button_rkp_index[i], key_info);
		isku_rkp_macro_key_info_free(key_info);
	}
}
