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
#include "isku_thumbster_frame.h"
#include "isku_key_combo_box.h"
#include "i18n-lib.h"

#define ISKU_THUMBSTER_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ISKU_THUMBSTER_FRAME_TYPE, IskuThumbsterFrameClass))
#define IS_ISKU_THUMBSTER_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ISKU_THUMBSTER_FRAME_TYPE))
#define ISKU_THUMBSTER_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ISKU_THUMBSTER_FRAME_TYPE, IskuThumbsterFramePrivate))

typedef struct _IskuThumbsterFrameClass IskuThumbsterFrameClass;
typedef struct _IskuThumbsterFramePrivate IskuThumbsterFramePrivate;

struct _IskuThumbsterFrame {
	GtkFrame parent;
	IskuThumbsterFramePrivate *priv;
};

struct _IskuThumbsterFrameClass {
	GtkFrameClass parent_class;
};

struct _IskuThumbsterFramePrivate {
	IskuKeyComboBox *buttons[ISKU_KEYS_THUMBSTER_NUM];
	guint button_rkp_index[ISKU_KEYS_THUMBSTER_NUM];
};

G_DEFINE_TYPE(IskuThumbsterFrame, isku_thumbster_frame, GTK_TYPE_FRAME);

void isku_thumbster_frame_set_key(IskuThumbsterFrame *thumbster_frame, guint key_index, IskuRkpMacroKeyInfo *key_info) {
	IskuThumbsterFramePrivate *priv = thumbster_frame->priv;
	guint i;
	for (i = 0; i < ISKU_KEYS_THUMBSTER_NUM; ++i)
		if (priv->button_rkp_index[i] == key_index)
			isku_key_combo_box_set_value_blocked(priv->buttons[i], key_info);
}

enum {
	PHYSICAL_BUTTONS_NUM = 3,
};

typedef struct {
	gchar const * const name;
	guint standard_index;
	guint standard_type;
	guint easyshift_index;
	guint easyshift_type;
} ButtonList;

static ButtonList button_list[PHYSICAL_BUTTONS_NUM] = {
	{"T1", ISKU_KEY_INDEX_T1, ISKU_KEY_TYPE_PROFILE_DOWN, ISKU_KEY_INDEX_SHIFT_T1, HID_UID_KB_CAPS_LOCK},
	{"T2", ISKU_KEY_INDEX_T2, ISKU_KEY_TYPE_OPEN_DRIVER, ISKU_KEY_INDEX_SHIFT_T2, ISKU_KEY_TYPE_MULTIMEDIA_OPEN_PLAYER},
	{"T3", ISKU_KEY_INDEX_T3, ISKU_KEY_TYPE_PROFILE_UP, ISKU_KEY_INDEX_SHIFT_T3, ISKU_KEY_TYPE_APP_EMAIL},
};

GtkWidget *isku_thumbster_frame_new(void) {
	IskuThumbsterFrame *frame;

	frame = ISKU_THUMBSTER_FRAME(g_object_new(ISKU_THUMBSTER_FRAME_TYPE,
			"label", _("Thumbster keys"),
			NULL));

	return GTK_WIDGET(frame);
}

static void isku_thumbster_frame_init(IskuThumbsterFrame *frame) {
	IskuThumbsterFramePrivate *priv = ISKU_THUMBSTER_FRAME_GET_PRIVATE(frame);
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

static void isku_thumbster_frame_class_init(IskuThumbsterFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(IskuThumbsterFramePrivate));
}

void isku_thumbster_frame_set_from_rkp(IskuThumbsterFrame *buttons_frame, IskuRkp *rkp) {
	IskuThumbsterFramePrivate *priv = buttons_frame->priv;
	guint i;
	IskuRkpMacroKeyInfo *key_info;

	for (i = 0; i < ISKU_KEYS_THUMBSTER_NUM; ++i) {
		key_info = isku_rkp_get_macro_key_info(rkp, priv->button_rkp_index[i]);
		isku_key_combo_box_set_value_blocked(priv->buttons[i], key_info);
	}
}

void isku_thumbster_frame_update_rkp(IskuThumbsterFrame *buttons_frame, IskuRkp *rkp) {
	IskuThumbsterFramePrivate *priv = buttons_frame->priv;
	guint i;
	IskuRkpMacroKeyInfo *key_info;

	for (i = 0; i < ISKU_KEYS_THUMBSTER_NUM; ++i) {
		key_info = isku_key_combo_box_get_value(priv->buttons[i]);
		isku_rkp_set_macro_key_info(rkp, priv->button_rkp_index[i], key_info);
		isku_rkp_macro_key_info_free(key_info);
	}
}
