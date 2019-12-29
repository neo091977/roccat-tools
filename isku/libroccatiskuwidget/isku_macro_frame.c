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

#include "isku_macro_frame.h"
#include "isku_key_combo_box.h"
#include "i18n-lib.h"

#define ISKU_MACRO_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ISKU_MACRO_FRAME_TYPE, IskuMacroFrameClass))
#define IS_ISKU_MACRO_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ISKU_MACRO_FRAME_TYPE))
#define ISKU_MACRO_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ISKU_MACRO_FRAME_TYPE, IskuMacroFramePrivate))

typedef struct _IskuMacroFrameClass IskuMacroFrameClass;
typedef struct _IskuMacroFramePrivate IskuMacroFramePrivate;

struct _IskuMacroFrame {
	GtkFrame parent;
	IskuMacroFramePrivate *priv;
};

struct _IskuMacroFrameClass {
	GtkFrameClass parent_class;
};

struct _IskuMacroFramePrivate {
	IskuKeyComboBox *buttons[ISKU_KEYS_MACRO_NUM];
	guint button_rkp_index[ISKU_KEYS_MACRO_NUM];
};

G_DEFINE_TYPE(IskuMacroFrame, isku_macro_frame, GTK_TYPE_FRAME);

void isku_macro_frame_set_key(IskuMacroFrame *macro_frame, guint key_index, IskuRkpMacroKeyInfo *key_info) {
	IskuMacroFramePrivate *priv = macro_frame->priv;
	guint i;
	for (i = 0; i < ISKU_KEYS_MACRO_NUM; ++i)
		if (priv->button_rkp_index[i] == key_index)
			isku_key_combo_box_set_value_blocked(priv->buttons[i], key_info);
}

enum {
	PHYSICAL_BUTTONS_NUM = 5,
};

typedef struct {
	gchar const * const name;
	guint standard_index;
	guint standard_type;
	guint easyshift_index;
	guint easyshift_type;
} ButtonList;

static ButtonList button_list[PHYSICAL_BUTTONS_NUM] = {
	{"M1", ISKU_KEY_INDEX_M1, ISKU_KEY_TYPE_STANDARD_SHIFT_1, ISKU_KEY_INDEX_SHIFT_M1, ISKU_KEY_TYPE_STANDARD_CTRL_1},
	{"M2", ISKU_KEY_INDEX_M2, ISKU_KEY_TYPE_STANDARD_SHIFT_2, ISKU_KEY_INDEX_SHIFT_M2, ISKU_KEY_TYPE_STANDARD_CTRL_2},
	{"M3", ISKU_KEY_INDEX_M3, ISKU_KEY_TYPE_STANDARD_SHIFT_3, ISKU_KEY_INDEX_SHIFT_M3, ISKU_KEY_TYPE_STANDARD_CTRL_3},
	{"M4", ISKU_KEY_INDEX_M4, ISKU_KEY_TYPE_STANDARD_SHIFT_4, ISKU_KEY_INDEX_SHIFT_M4, ISKU_KEY_TYPE_STANDARD_CTRL_4},
	{"M5", ISKU_KEY_INDEX_M5, ISKU_KEY_TYPE_STANDARD_SHIFT_5, ISKU_KEY_INDEX_SHIFT_M5, ISKU_KEY_TYPE_STANDARD_CTRL_5},
};

GtkWidget *isku_macro_frame_new(void) {
	IskuMacroFrame *frame;

	frame = ISKU_MACRO_FRAME(g_object_new(ISKU_MACRO_FRAME_TYPE,
			"label", _("Macro keys"),
			NULL));

	return GTK_WIDGET(frame);
}

static void isku_macro_frame_init(IskuMacroFrame *frame) {
	IskuMacroFramePrivate *priv = ISKU_MACRO_FRAME_GET_PRIVATE(frame);
	GtkWidget *standard_frame;
	GtkWidget *standard_table;
	GtkWidget *easyshift_frame;
	GtkWidget *easyshift_table;
	GtkWidget *hbox;
	guint i;

	frame->priv = priv;

	hbox = gtk_hbox_new(FALSE, 0);
	standard_frame = gtk_frame_new(_("Standard"));
	easyshift_frame = gtk_frame_new(_("Easyshift"));
	standard_table = gtk_table_new(PHYSICAL_BUTTONS_NUM, 2, FALSE);
	easyshift_table = gtk_table_new(PHYSICAL_BUTTONS_NUM, 1, FALSE);

	gtk_container_add(GTK_CONTAINER(frame), hbox);
	gtk_container_add(GTK_CONTAINER(standard_frame), standard_table);
	gtk_container_add(GTK_CONTAINER(easyshift_frame), easyshift_table);

	gtk_box_pack_start(GTK_BOX(hbox), standard_frame, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), easyshift_frame, TRUE, TRUE, 0);

	for (i = 0; i < PHYSICAL_BUTTONS_NUM; ++i) {
		priv->button_rkp_index[i] = button_list[i].standard_index;
		priv->buttons[i] = ISKU_KEY_COMBO_BOX(
				isku_key_combo_box_new(0, button_list[i].standard_type));
		gtk_table_attach(GTK_TABLE(standard_table), gtk_label_new(button_list[i].name), 0, 1, i, i + 1, GTK_EXPAND, GTK_EXPAND, 0, 0);
		gtk_table_attach(GTK_TABLE(standard_table), GTK_WIDGET(priv->buttons[i]), 1, 2, i, i + 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);

		priv->button_rkp_index[i + PHYSICAL_BUTTONS_NUM] = button_list[i].easyshift_index;
		priv->buttons[i + PHYSICAL_BUTTONS_NUM] = ISKU_KEY_COMBO_BOX(
				isku_key_combo_box_new(0, button_list[i].easyshift_type));
		gtk_table_attach(GTK_TABLE(easyshift_table), GTK_WIDGET(priv->buttons[i + PHYSICAL_BUTTONS_NUM]), 0, 1, i, i + 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);
	}
}

static void isku_macro_frame_class_init(IskuMacroFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(IskuMacroFramePrivate));
}

void isku_macro_frame_set_from_rkp(IskuMacroFrame *buttons_frame, IskuRkp *rkp) {
	IskuMacroFramePrivate *priv = buttons_frame->priv;
	guint i;
	IskuRkpMacroKeyInfo *key_info;

	for (i = 0; i < ISKU_KEYS_MACRO_NUM; ++i) {
		key_info = isku_rkp_get_macro_key_info(rkp, priv->button_rkp_index[i]);
		isku_key_combo_box_set_value_blocked(priv->buttons[i], key_info);
	}
}

void isku_macro_frame_update_rkp(IskuMacroFrame *buttons_frame, IskuRkp *rkp) {
	IskuMacroFramePrivate *priv = buttons_frame->priv;
	guint i;
	IskuRkpMacroKeyInfo *key_info;

	for (i = 0; i < ISKU_KEYS_MACRO_NUM; ++i) {
		key_info = isku_key_combo_box_get_value(priv->buttons[i]);
		isku_rkp_set_macro_key_info(rkp, priv->button_rkp_index[i], key_info);
		isku_rkp_macro_key_info_free(key_info);
	}
}
