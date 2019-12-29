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

#include "kovaplusconfig_buttons_frame.h"
#include "kovaplusconfig_key_combo_box.h"
#include "i18n.h"

#define KOVAPLUSCONFIG_BUTTONS_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KOVAPLUSCONFIG_BUTTONS_FRAME_TYPE, KovaplusconfigButtonsFrameClass))
#define IS_KOVAPLUSCONFIG_BUTTONS_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KOVAPLUSCONFIG_BUTTONS_FRAME_TYPE))
#define KOVAPLUSCONFIG_BUTTONS_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KOVAPLUSCONFIG_BUTTONS_FRAME_TYPE, KovaplusconfigButtonsFramePrivate))

typedef struct _KovaplusconfigButtonsFrameClass KovaplusconfigButtonsFrameClass;
typedef struct _KovaplusconfigButtonsFramePrivate KovaplusconfigButtonsFramePrivate;

struct _KovaplusconfigButtonsFrame {
	GtkFrame parent;
	KovaplusconfigButtonsFramePrivate *priv;
};

struct _KovaplusconfigButtonsFrameClass {
	GtkFrameClass parent_class;
};

struct _KovaplusconfigButtonsFramePrivate {
	KovaplusconfigKeyComboBox *buttons[KOVAPLUS_BUTTON_NUM];
	guint button_rmp_index[KOVAPLUS_BUTTON_NUM];
};

G_DEFINE_TYPE(KovaplusconfigButtonsFrame, kovaplusconfig_buttons_frame, GTK_TYPE_FRAME);

enum {
	PHYSICAL_BUTTONS_NUM = 9,
};

typedef struct {
	gchar const * const name;
	KovaplusProfileButtonsButtonIndex standard_index;
	guint standard_mask;
	KovaplusProfileButtonsButtonIndex easyshift_index;
	guint easyshift_mask;
} ButtonList;

static ButtonList button_list[PHYSICAL_BUTTONS_NUM] = {
	{N_("Left"), KOVAPLUS_PROFILE_BUTTONS_BUTTON_INDEX_LEFT, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT,
		KOVAPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_LEFT, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{N_("Right"), KOVAPLUS_PROFILE_BUTTONS_BUTTON_INDEX_RIGHT, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT,
		KOVAPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_RIGHT, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{N_("Middle"), KOVAPLUS_PROFILE_BUTTONS_BUTTON_INDEX_MIDDLE, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT,
		KOVAPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_MIDDLE, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{N_("Left forward"), KOVAPLUS_PROFILE_BUTTONS_BUTTON_INDEX_FORWARD_LEFT, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT,
		KOVAPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_FORWARD_LEFT, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{N_("Left backward"), KOVAPLUS_PROFILE_BUTTONS_BUTTON_INDEX_BACKWARD_LEFT, 0,
		KOVAPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_BACKWARD_LEFT, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{N_("Right forward"), KOVAPLUS_PROFILE_BUTTONS_BUTTON_INDEX_FORWARD_RIGHT, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT,
		KOVAPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_FORWARD_RIGHT, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{N_("Right backward"), KOVAPLUS_PROFILE_BUTTONS_BUTTON_INDEX_BACKWARD_RIGHT, 0,
		KOVAPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_BACKWARD_RIGHT, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{N_("Wheel up"), KOVAPLUS_PROFILE_BUTTONS_BUTTON_INDEX_WHEEL_UP, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT,
		KOVAPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_WHEEL_UP, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{N_("Wheel down"), KOVAPLUS_PROFILE_BUTTONS_BUTTON_INDEX_WHEEL_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT,
		KOVAPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_WHEEL_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
};

GtkWidget *kovaplusconfig_buttons_frame_new(void) {
	KovaplusconfigButtonsFrame *frame;

	frame = KOVAPLUSCONFIG_BUTTONS_FRAME(g_object_new(KOVAPLUSCONFIG_BUTTONS_FRAME_TYPE,
			"label", _("Buttons"),
			NULL));

	return GTK_WIDGET(frame);
}

static void kovaplusconfig_buttons_frame_init(KovaplusconfigButtonsFrame *frame) {
	KovaplusconfigButtonsFramePrivate *priv = KOVAPLUSCONFIG_BUTTONS_FRAME_GET_PRIVATE(frame);
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
	standard_table = gtk_table_new(PHYSICAL_BUTTONS_NUM, 3, FALSE);
	easyshift_table = gtk_table_new(PHYSICAL_BUTTONS_NUM, 2, FALSE);

	gtk_container_add(GTK_CONTAINER(frame), hbox);
	gtk_container_add(GTK_CONTAINER(standard_frame), standard_table);
	gtk_container_add(GTK_CONTAINER(easyshift_frame), easyshift_table);

	gtk_box_pack_start(GTK_BOX(hbox), standard_frame, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), easyshift_frame, TRUE, TRUE, 0);

	for (i = 0; i < PHYSICAL_BUTTONS_NUM; ++i) {
		priv->button_rmp_index[i] = button_list[i].standard_index;
		priv->buttons[i] = KOVAPLUSCONFIG_KEY_COMBO_BOX(kovaplusconfig_key_combo_box_new(button_list[i].standard_mask));
		gtk_table_attach(GTK_TABLE(standard_table), gtk_label_new(_N(button_list[i].name)), 0, 1, i, i + 1, GTK_EXPAND, GTK_EXPAND, 0, 0);
		gtk_table_attach(GTK_TABLE(standard_table), GTK_WIDGET(priv->buttons[i]), 1, 2, i, i + 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);

		priv->button_rmp_index[i + PHYSICAL_BUTTONS_NUM] = button_list[i].easyshift_index;
		priv->buttons[i + PHYSICAL_BUTTONS_NUM] = KOVAPLUSCONFIG_KEY_COMBO_BOX(kovaplusconfig_key_combo_box_new(button_list[i].easyshift_mask));
		gtk_table_attach(GTK_TABLE(easyshift_table), GTK_WIDGET(priv->buttons[i + PHYSICAL_BUTTONS_NUM]), 0, 1, i, i + 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);
	}
}

static void kovaplusconfig_buttons_frame_class_init(KovaplusconfigButtonsFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(KovaplusconfigButtonsFramePrivate));
}

void kovaplusconfig_buttons_frame_set_from_rmp(KovaplusconfigButtonsFrame *buttons_frame, KovaplusRmp *rmp) {
	KovaplusconfigButtonsFramePrivate *priv = buttons_frame->priv;
	guint i;
	KovaplusRmpMacroKeyInfo *key_info;

	for (i = 0; i < KOVAPLUS_BUTTON_NUM; ++i) {
		key_info = kovaplus_rmp_get_macro_key_info(rmp, priv->button_rmp_index[i]);
		kovaplusconfig_key_combo_box_set_value_blocked(priv->buttons[i], key_info);
	}
}

void kovaplusconfig_buttons_frame_update_rmp(KovaplusconfigButtonsFrame *buttons_frame, KovaplusRmp *rmp) {
	KovaplusconfigButtonsFramePrivate *priv = buttons_frame->priv;
	guint i;
	KovaplusRmpMacroKeyInfo *key_info;

	for (i = 0; i < KOVAPLUS_BUTTON_NUM; ++i) {
		key_info = kovaplusconfig_key_combo_box_get_value(priv->buttons[i]);
		kovaplus_rmp_set_macro_key_info(rmp, priv->button_rmp_index[i], key_info);
	}
}
