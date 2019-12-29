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

#include "tyon_profile_buttons.h"
#include "tyon_buttons_frame.h"
#include "tyon_key_combo_box.h"
#include "i18n.h"

#define TYON_BUTTONS_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), TYON_BUTTONS_FRAME_TYPE, TyonButtonsFrameClass))
#define IS_TYON_BUTTONS_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), TYON_BUTTONS_FRAME_TYPE))
#define TYON_BUTTONS_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), TYON_BUTTONS_FRAME_TYPE, TyonButtonsFramePrivate))

typedef struct _TyonButtonsFrameClass TyonButtonsFrameClass;
typedef struct _TyonButtonsFramePrivate TyonButtonsFramePrivate;

struct _TyonButtonsFrame {
	GtkFrame parent;
	TyonButtonsFramePrivate *priv;
};

struct _TyonButtonsFrameClass {
	GtkFrameClass parent_class;
};

struct _TyonButtonsFramePrivate {
	TyonKeyComboBox *buttons[TYON_PROFILE_BUTTON_NUM];
	guint button_rmp_index[TYON_PROFILE_BUTTON_NUM];
};

G_DEFINE_TYPE(TyonButtonsFrame, tyon_buttons_frame, GTK_TYPE_FRAME);

typedef struct {
	gchar const * const name;
	TyonButtonIndex standard_index;
	guint standard_mask;
	TyonButtonIndex easyshift_index;
	guint easyshift_mask;
} ButtonList;

static ButtonList button_list[TYON_PHYSICAL_BUTTON_NUM] = {
	{N_("Left"), TYON_BUTTON_INDEX_LEFT, ROCCAT_KEY_COMBO_BOX_GROUP_ANALOG,
		TYON_BUTTON_INDEX_SHIFT_LEFT, ROCCAT_KEY_COMBO_BOX_GROUP_ANALOG | ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{N_("Right"), TYON_BUTTON_INDEX_RIGHT, ROCCAT_KEY_COMBO_BOX_GROUP_ANALOG,
		TYON_BUTTON_INDEX_SHIFT_RIGHT, ROCCAT_KEY_COMBO_BOX_GROUP_ANALOG | ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{N_("Middle"), TYON_BUTTON_INDEX_MIDDLE, ROCCAT_KEY_COMBO_BOX_GROUP_ANALOG,
		TYON_BUTTON_INDEX_SHIFT_MIDDLE, ROCCAT_KEY_COMBO_BOX_GROUP_ANALOG | ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{N_("Thumb backward"), TYON_BUTTON_INDEX_THUMB_BACK, ROCCAT_KEY_COMBO_BOX_GROUP_ANALOG,
		TYON_BUTTON_INDEX_SHIFT_THUMB_BACK, ROCCAT_KEY_COMBO_BOX_GROUP_ANALOG | ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{N_("Thumb forward"), TYON_BUTTON_INDEX_THUMB_FORWARD, ROCCAT_KEY_COMBO_BOX_GROUP_ANALOG,
		TYON_BUTTON_INDEX_SHIFT_THUMB_FORWARD, ROCCAT_KEY_COMBO_BOX_GROUP_ANALOG | ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{N_("Thumb pedal"), TYON_BUTTON_INDEX_THUMB_PEDAL, ROCCAT_KEY_COMBO_BOX_GROUP_ANALOG,
		TYON_BUTTON_INDEX_SHIFT_THUMB_PEDAL, ROCCAT_KEY_COMBO_BOX_GROUP_ANALOG | ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{N_("X-Celerator up"), TYON_BUTTON_INDEX_THUMB_PADDLE_UP, TYON_KEY_COMBO_BOX_GROUP_NOT_ANALOG,
		TYON_BUTTON_INDEX_SHIFT_THUMB_PADDLE_UP, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT | TYON_KEY_COMBO_BOX_GROUP_NOT_ANALOG},
	{N_("X-Celerator down"), TYON_BUTTON_INDEX_THUMB_PADDLE_DOWN, TYON_KEY_COMBO_BOX_GROUP_NOT_ANALOG,
		TYON_BUTTON_INDEX_SHIFT_THUMB_PADDLE_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT | TYON_KEY_COMBO_BOX_GROUP_NOT_ANALOG},
	{N_("Left backward"), TYON_BUTTON_INDEX_LEFT_BACK, ROCCAT_KEY_COMBO_BOX_GROUP_ANALOG,
		TYON_BUTTON_INDEX_SHIFT_LEFT_BACK, ROCCAT_KEY_COMBO_BOX_GROUP_ANALOG | ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{N_("Left forward"), TYON_BUTTON_INDEX_LEFT_FORWARD, ROCCAT_KEY_COMBO_BOX_GROUP_ANALOG,
		TYON_BUTTON_INDEX_SHIFT_LEFT_FORWARD, ROCCAT_KEY_COMBO_BOX_GROUP_ANALOG | ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{N_("Right backward"), TYON_BUTTON_INDEX_RIGHT_BACK, ROCCAT_KEY_COMBO_BOX_GROUP_ANALOG,
		TYON_BUTTON_INDEX_SHIFT_RIGHT_BACK, ROCCAT_KEY_COMBO_BOX_GROUP_ANALOG | ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{N_("Right forward"), TYON_BUTTON_INDEX_RIGHT_FORWARD, ROCCAT_KEY_COMBO_BOX_GROUP_ANALOG,
		TYON_BUTTON_INDEX_SHIFT_RIGHT_FORWARD, ROCCAT_KEY_COMBO_BOX_GROUP_ANALOG | ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{N_("Fin right"), TYON_BUTTON_INDEX_FIN_RIGHT, ROCCAT_KEY_COMBO_BOX_GROUP_ANALOG,
		TYON_BUTTON_INDEX_SHIFT_FIN_RIGHT, ROCCAT_KEY_COMBO_BOX_GROUP_ANALOG | ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{N_("Fin left"), TYON_BUTTON_INDEX_FIN_LEFT, ROCCAT_KEY_COMBO_BOX_GROUP_ANALOG,
		TYON_BUTTON_INDEX_SHIFT_FIN_LEFT, ROCCAT_KEY_COMBO_BOX_GROUP_ANALOG | ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{N_("Wheel up"), TYON_BUTTON_INDEX_WHEEL_UP, ROCCAT_KEY_COMBO_BOX_GROUP_ANALOG,
		TYON_BUTTON_INDEX_SHIFT_WHEEL_UP, ROCCAT_KEY_COMBO_BOX_GROUP_ANALOG | ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{N_("Wheel down"), TYON_BUTTON_INDEX_WHEEL_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_ANALOG,
		TYON_BUTTON_INDEX_SHIFT_WHEEL_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_ANALOG | ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
};

GtkWidget *tyon_buttons_frame_new(void) {
	TyonButtonsFrame *frame;

	frame = TYON_BUTTONS_FRAME(g_object_new(TYON_BUTTONS_FRAME_TYPE,
			"label", _("Buttons"),
			NULL));

	return GTK_WIDGET(frame);
}

static void tyon_buttons_frame_init(TyonButtonsFrame *frame) {
	TyonButtonsFramePrivate *priv = TYON_BUTTONS_FRAME_GET_PRIVATE(frame);
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
	standard_table = gtk_table_new(TYON_PHYSICAL_BUTTON_NUM, 1, FALSE);
	easyshift_table = gtk_table_new(TYON_PHYSICAL_BUTTON_NUM, 2, FALSE);

	gtk_container_add(GTK_CONTAINER(frame), hbox);
	gtk_container_add(GTK_CONTAINER(standard_frame), standard_table);
	gtk_container_add(GTK_CONTAINER(easyshift_frame), easyshift_table);

	gtk_box_pack_start(GTK_BOX(hbox), standard_frame, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), easyshift_frame, TRUE, TRUE, 0);

	for (i = 0; i < TYON_PHYSICAL_BUTTON_NUM; ++i) {
		priv->button_rmp_index[i] = button_list[i].standard_index;
		priv->buttons[i] = TYON_KEY_COMBO_BOX(tyon_key_combo_box_new(button_list[i].standard_mask));
		gtk_table_attach(GTK_TABLE(standard_table), gtk_label_new(_N(button_list[i].name)), 0, 1, i, i + 1, GTK_EXPAND, GTK_EXPAND, 0, 0);
		gtk_table_attach(GTK_TABLE(standard_table), GTK_WIDGET(priv->buttons[i]), 1, 2, i, i + 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);

		priv->button_rmp_index[i + TYON_PHYSICAL_BUTTON_NUM] = button_list[i].easyshift_index;
		priv->buttons[i + TYON_PHYSICAL_BUTTON_NUM] = TYON_KEY_COMBO_BOX(tyon_key_combo_box_new(button_list[i].easyshift_mask));
		gtk_table_attach(GTK_TABLE(easyshift_table), GTK_WIDGET(priv->buttons[i + TYON_PHYSICAL_BUTTON_NUM]), 0, 1, i, i + 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);
	}
}

static void tyon_buttons_frame_class_init(TyonButtonsFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(TyonButtonsFramePrivate));
}

void tyon_buttons_frame_set_from_rmp(TyonButtonsFrame *buttons_frame, TyonRmp *rmp) {
	TyonButtonsFramePrivate *priv = buttons_frame->priv;
	guint i;
	TyonRmpMacroKeyInfo *key_info;

	for (i = 0; i < TYON_PROFILE_BUTTON_NUM; ++i) {
		key_info = tyon_rmp_get_macro_key_info(rmp, priv->button_rmp_index[i]);
		tyon_key_combo_box_set_value_blocked(priv->buttons[i], key_info);
	}
}

void tyon_buttons_frame_update_rmp(TyonButtonsFrame *buttons_frame, TyonRmp *rmp) {
	TyonButtonsFramePrivate *priv = buttons_frame->priv;
	guint i;
	TyonRmpMacroKeyInfo *key_info;

	for (i = 0; i < TYON_PROFILE_BUTTON_NUM; ++i) {
		key_info = tyon_key_combo_box_get_value(priv->buttons[i]);
		tyon_rmp_set_macro_key_info(rmp, priv->button_rmp_index[i], key_info);
	}
}
