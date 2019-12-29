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
#include <gaminggear/gdk_key_translations.h>
#include "isku_easyzone_frame.h"
#include "isku_key_combo_box.h"
#include "i18n-lib.h"

#define ISKU_EASYZONE_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ISKU_EASYZONE_FRAME_TYPE, IskuEasyzoneFrameClass))
#define IS_ISKU_EASYZONE_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ISKU_EASYZONE_FRAME_TYPE))
#define ISKU_EASYZONE_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ISKU_EASYZONE_FRAME_TYPE, IskuEasyzoneFramePrivate))

typedef struct _IskuEasyzoneFrameClass IskuEasyzoneFrameClass;
typedef struct _IskuEasyzoneFramePrivate IskuEasyzoneFramePrivate;

struct _IskuEasyzoneFrame {
	GtkFrame parent;
	IskuEasyzoneFramePrivate *priv;
};

struct _IskuEasyzoneFrameClass {
	GtkFrameClass parent_class;
};

struct _IskuEasyzoneFramePrivate {
	IskuKeyComboBox *buttons[ISKU_KEYS_EASYZONE_NUM];
	guint button_rkp_index[ISKU_KEYS_EASYZONE_NUM];
};

G_DEFINE_TYPE(IskuEasyzoneFrame, isku_easyzone_frame, GTK_TYPE_FRAME);

void isku_easyzone_frame_set_key(IskuEasyzoneFrame *easyzone_frame, guint key_index, IskuRkpMacroKeyInfo *key_info) {
	IskuEasyzoneFramePrivate *priv = easyzone_frame->priv;
	guint i;
	for (i = 0; i < ISKU_KEYS_EASYZONE_NUM; ++i)
		if (priv->button_rkp_index[i] == key_index)
			isku_key_combo_box_set_value_blocked(priv->buttons[i], key_info);
}

typedef struct {
	guint index;
	guint hid_usage_id;
} ButtonList;

static ButtonList button_list[ISKU_KEYS_EASYZONE_NUM] = {
	{ISKU_KEY_INDEX_1 + 0, HID_UID_KB_1},
	{ISKU_KEY_INDEX_1 + 1, HID_UID_KB_2},
	{ISKU_KEY_INDEX_1 + 2, HID_UID_KB_3},
	{ISKU_KEY_INDEX_1 + 3, HID_UID_KB_4},
	{ISKU_KEY_INDEX_1 + 4, HID_UID_KB_5},
	{ISKU_KEY_INDEX_1 + 5, HID_UID_KB_Q},
	{ISKU_KEY_INDEX_1 + 6, HID_UID_KB_W},
	{ISKU_KEY_INDEX_1 + 7, HID_UID_KB_E},
	{ISKU_KEY_INDEX_1 + 8, HID_UID_KB_R},
	{ISKU_KEY_INDEX_1 + 9, HID_UID_KB_T},
	{ISKU_KEY_INDEX_1 + 10, HID_UID_KB_A},
	{ISKU_KEY_INDEX_1 + 11, HID_UID_KB_S},
	{ISKU_KEY_INDEX_1 + 12, HID_UID_KB_D},
	{ISKU_KEY_INDEX_1 + 13, HID_UID_KB_F},
	{ISKU_KEY_INDEX_1 + 14, HID_UID_KB_G},
	{ISKU_KEY_INDEX_1 + 15, HID_UID_KB_Z},
	{ISKU_KEY_INDEX_1 + 16, HID_UID_KB_X},
	{ISKU_KEY_INDEX_1 + 17, HID_UID_KB_C},
	{ISKU_KEY_INDEX_1 + 18, HID_UID_KB_V},
	{ISKU_KEY_INDEX_1 + 19, HID_UID_KB_B},
};

GtkWidget *isku_easyzone_frame_new(void) {
	IskuEasyzoneFrame *frame;

	frame = ISKU_EASYZONE_FRAME(g_object_new(ISKU_EASYZONE_FRAME_TYPE,
			"label", _("Easyzone keys"),
			NULL));

	return GTK_WIDGET(frame);
}

static GtkWidget *create_frame(IskuEasyzoneFrame *frame, guint offset, guint count) {
	IskuEasyzoneFramePrivate *priv = ISKU_EASYZONE_FRAME_GET_PRIVATE(frame);
	GtkWidget *subframe;
	GtkWidget *table;
	guint i;
	gchar *name;

	subframe = gtk_frame_new(NULL);
	table = gtk_table_new(count, 2, FALSE);

	for (i = 0; i < count; ++i) {

		priv->button_rkp_index[offset + i] = button_list[offset + i].index;
		priv->buttons[offset + i] = ISKU_KEY_COMBO_BOX(
				isku_key_combo_box_new(0, button_list[offset + i].hid_usage_id));

		name = gaminggear_hid_to_name(button_list[offset + i].hid_usage_id);
		gtk_table_attach(GTK_TABLE(table), gtk_label_new(name), 0, 1, i, i + 1, GTK_EXPAND, GTK_EXPAND, 0, 0);
		g_free(name);

		gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(priv->buttons[offset + i]), 1, 2, i, i + 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);
	}

	gtk_container_add(GTK_CONTAINER(subframe), table);

	return subframe;
}

static void isku_easyzone_frame_init(IskuEasyzoneFrame *frame) {
	IskuEasyzoneFramePrivate *priv = ISKU_EASYZONE_FRAME_GET_PRIVATE(frame);
	GtkWidget *table;
	guint easyzone_num_quarter;

	frame->priv = priv;

	table = gtk_table_new(2, 2, FALSE);

	easyzone_num_quarter = ISKU_KEYS_EASYZONE_NUM >> 2;

	gtk_table_attach(GTK_TABLE(table), create_frame(frame, easyzone_num_quarter * 0, easyzone_num_quarter), 0, 1, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), create_frame(frame, easyzone_num_quarter * 1, easyzone_num_quarter), 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), create_frame(frame, easyzone_num_quarter * 2, easyzone_num_quarter), 0, 1, 1, 2, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), create_frame(frame, easyzone_num_quarter * 3, easyzone_num_quarter), 1, 2, 1, 2, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

	gtk_container_add(GTK_CONTAINER(frame), table);
}

static void isku_easyzone_frame_class_init(IskuEasyzoneFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(IskuEasyzoneFramePrivate));
}

void isku_easyzone_frame_set_from_rkp(IskuEasyzoneFrame *buttons_frame, IskuRkp *rkp) {
	IskuEasyzoneFramePrivate *priv = buttons_frame->priv;
	guint i;
	IskuRkpMacroKeyInfo *key_info;

	for (i = 0; i < ISKU_KEYS_EASYZONE_NUM; ++i) {
		key_info = isku_rkp_get_macro_key_info(rkp, priv->button_rkp_index[i]);
		isku_key_combo_box_set_value_blocked(priv->buttons[i], key_info);
	}
}

void isku_easyzone_frame_update_rkp(IskuEasyzoneFrame *buttons_frame, IskuRkp *rkp) {
	IskuEasyzoneFramePrivate *priv = buttons_frame->priv;
	guint i;
	IskuRkpMacroKeyInfo *key_info;

	for (i = 0; i < ISKU_KEYS_EASYZONE_NUM; ++i) {
		key_info = isku_key_combo_box_get_value(priv->buttons[i]);
		isku_rkp_set_macro_key_info(rkp, priv->button_rkp_index[i], key_info);
		isku_rkp_macro_key_info_free(key_info);
	}
}
