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

#include "konepure_buttons_frame.h"
#include "koneplus_key_combo_box.h"
#include "i18n-lib.h"

#define KONEPURE_BUTTONS_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KONEPURE_BUTTONS_FRAME_TYPE, KonepureButtonsFrameClass))
#define IS_KONEPURE_BUTTONS_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KONEPURE_BUTTONS_FRAME_TYPE))
#define KONEPURE_BUTTONS_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KONEPURE_BUTTONS_FRAME_TYPE, KonepureButtonsFramePrivate))

typedef struct _KonepureButtonsFrameClass KonepureButtonsFrameClass;
typedef struct _KonepureButtonsFramePrivate KonepureButtonsFramePrivate;

struct _KonepureButtonsFrame {
	GtkFrame parent;
	KonepureButtonsFramePrivate *priv;
};

struct _KonepureButtonsFrameClass {
	GtkFrameClass parent_class;
};

struct _KonepureButtonsFramePrivate {
	KoneplusKeyComboBox *buttons[KONEPURE_PROFILE_BUTTON_NUM];
	guint button_rmp_index[KONEPURE_PROFILE_BUTTON_NUM];
};

G_DEFINE_TYPE(KonepureButtonsFrame, konepure_buttons_frame, GTK_TYPE_FRAME);

enum {
	PHYSICAL_BUTTONS_NUM = 9,
};

typedef struct {
	gchar const * const name;
	KoneplusProfileButtonsButtonIndex standard_index;
	guint standard_mask;
	KoneplusProfileButtonsButtonIndex easyshift_index;
	guint easyshift_mask;
} ButtonList;

static ButtonList button_list[PHYSICAL_BUTTONS_NUM] = {
	{N_("Left"), KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_LEFT, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT,
		KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_LEFT, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{N_("Right"), KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_RIGHT, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT,
		KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_RIGHT, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{N_("Middle"), KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_MIDDLE, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT,
		KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_MIDDLE, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{N_("Forward"), KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_FORWARD, 0,
		KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_FORWARD, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{N_("Backward"), KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_BACKWARD, 0,
		KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_BACKWARD, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{N_("Wheel up"), KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_WHEEL_UP, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT,
		KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_WHEEL_UP, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{N_("Wheel down"), KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_WHEEL_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT,
		KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_WHEEL_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{N_("Plus"), KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_PLUS, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT,
		KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_PLUS, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{N_("Minus"), KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_MINUS, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT,
		KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_MINUS, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
};

GtkWidget *konepure_buttons_frame_new(void) {
	KonepureButtonsFrame *frame;

	frame = KONEPURE_BUTTONS_FRAME(g_object_new(KONEPURE_BUTTONS_FRAME_TYPE,
			"label", _("Buttons"),
			NULL));

	return GTK_WIDGET(frame);
}

static void konepure_buttons_frame_init(KonepureButtonsFrame *frame) {
	KonepureButtonsFramePrivate *priv = KONEPURE_BUTTONS_FRAME_GET_PRIVATE(frame);
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
		priv->button_rmp_index[i] = button_list[i].standard_index;
		priv->buttons[i] = KONEPLUS_KEY_COMBO_BOX(koneplus_key_combo_box_new(button_list[i].standard_mask));
		gtk_table_attach(GTK_TABLE(standard_table), gtk_label_new(_N(button_list[i].name)), 0, 1, i, i + 1, GTK_EXPAND, GTK_EXPAND, 0, 0);
		gtk_table_attach(GTK_TABLE(standard_table), GTK_WIDGET(priv->buttons[i]), 1, 2, i, i + 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);

		priv->button_rmp_index[i + PHYSICAL_BUTTONS_NUM] = button_list[i].easyshift_index;
		priv->buttons[i + PHYSICAL_BUTTONS_NUM] = KONEPLUS_KEY_COMBO_BOX(koneplus_key_combo_box_new(button_list[i].easyshift_mask));
		gtk_table_attach(GTK_TABLE(easyshift_table), GTK_WIDGET(priv->buttons[i + PHYSICAL_BUTTONS_NUM]), 0, 1, i, i + 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);
	}
}

static void konepure_buttons_frame_class_init(KonepureButtonsFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(KonepureButtonsFramePrivate));
}

void konepure_buttons_frame_set_from_rmp(KonepureButtonsFrame *buttons_frame, KoneplusRmp *rmp) {
	KonepureButtonsFramePrivate *priv = buttons_frame->priv;
	guint i;
	KoneplusRmpMacroKeyInfo *key_info;

	for (i = 0; i < KONEPURE_PROFILE_BUTTON_NUM; ++i) {
		key_info = koneplus_rmp_get_macro_key_info(rmp, priv->button_rmp_index[i]);
		koneplus_key_combo_box_set_value_blocked(priv->buttons[i], key_info);
	}
}

void konepure_buttons_frame_update_rmp(KonepureButtonsFrame *buttons_frame, KoneplusRmp *rmp) {
	KonepureButtonsFramePrivate *priv = buttons_frame->priv;
	guint i;
	KoneplusRmpMacroKeyInfo *key_info;

	for (i = 0; i < KONEPURE_PROFILE_BUTTON_NUM; ++i) {
		key_info = koneplus_key_combo_box_get_value(priv->buttons[i]);
		koneplus_rmp_set_macro_key_info(rmp, priv->button_rmp_index[i], key_info);
	}
}
