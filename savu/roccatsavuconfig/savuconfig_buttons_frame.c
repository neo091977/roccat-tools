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

#include "savuconfig_buttons_frame.h"
#include "savuconfig_key_combo_box.h"
#include "i18n.h"

#define SAVUCONFIG_BUTTONS_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), SAVUCONFIG_BUTTONS_FRAME_TYPE, SavuconfigButtonsFrameClass))
#define IS_SAVUCONFIG_BUTTONS_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), SAVUCONFIG_BUTTONS_FRAME_TYPE))
#define SAVUCONFIG_BUTTONS_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), SAVUCONFIG_BUTTONS_FRAME_TYPE, SavuconfigButtonsFramePrivate))

typedef struct _SavuconfigButtonsFrameClass SavuconfigButtonsFrameClass;
typedef struct _SavuconfigButtonsFramePrivate SavuconfigButtonsFramePrivate;

struct _SavuconfigButtonsFrame {
	GtkFrame parent;
	SavuconfigButtonsFramePrivate *priv;
};

struct _SavuconfigButtonsFrameClass {
	GtkFrameClass parent_class;
};

struct _SavuconfigButtonsFramePrivate {
	SavuconfigKeyComboBox *buttons[SAVU_BUTTON_NUM];
	guint button_rmp_index[SAVU_BUTTON_NUM];
};

G_DEFINE_TYPE(SavuconfigButtonsFrame, savuconfig_buttons_frame, GTK_TYPE_FRAME);

typedef struct {
	gchar const * const name;
	SavuButtonIndex standard_index;
	guint standard_mask;
	SavuButtonIndex easyshift_index;
	guint easyshift_mask;
} ButtonList;

static ButtonList button_list[SAVU_PHYSICAL_BUTTON_NUM] = {
	{N_("Left"), SAVU_BUTTON_INDEX_LEFT, 0, SAVU_BUTTON_INDEX_SHIFT_LEFT, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{N_("Right"), SAVU_BUTTON_INDEX_RIGHT, 0, SAVU_BUTTON_INDEX_SHIFT_RIGHT, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{N_("Middle"), SAVU_BUTTON_INDEX_MIDDLE, 0, SAVU_BUTTON_INDEX_SHIFT_MIDDLE, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{N_("Forward"), SAVU_BUTTON_INDEX_FORWARD, 0, SAVU_BUTTON_INDEX_SHIFT_FORWARD, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{N_("Backward"), SAVU_BUTTON_INDEX_BACKWARD, 0, SAVU_BUTTON_INDEX_SHIFT_BACKWARD, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{N_("Wheel up"), SAVU_BUTTON_INDEX_WHEEL_UP, 0, SAVU_BUTTON_INDEX_SHIFT_WHEEL_UP, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{N_("Wheel down"), SAVU_BUTTON_INDEX_WHEEL_DOWN, 0, SAVU_BUTTON_INDEX_SHIFT_WHEEL_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
};

GtkWidget *savuconfig_buttons_frame_new(void) {
	SavuconfigButtonsFrame *frame;

	frame = SAVUCONFIG_BUTTONS_FRAME(g_object_new(SAVUCONFIG_BUTTONS_FRAME_TYPE,
			"label", _("Buttons"),
			NULL));

	return GTK_WIDGET(frame);
}

static void savuconfig_buttons_frame_init(SavuconfigButtonsFrame *frame) {
	SavuconfigButtonsFramePrivate *priv = SAVUCONFIG_BUTTONS_FRAME_GET_PRIVATE(frame);
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
	standard_table = gtk_table_new(SAVU_PHYSICAL_BUTTON_NUM, 2, FALSE);
	easyshift_table = gtk_table_new(SAVU_PHYSICAL_BUTTON_NUM, 1, FALSE);

	gtk_container_add(GTK_CONTAINER(frame), hbox);
	gtk_container_add(GTK_CONTAINER(standard_frame), standard_table);
	gtk_container_add(GTK_CONTAINER(easyshift_frame), easyshift_table);

	gtk_box_pack_start(GTK_BOX(hbox), standard_frame, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), easyshift_frame, TRUE, TRUE, 0);

	for (i = 0; i < SAVU_PHYSICAL_BUTTON_NUM; ++i) {
		priv->button_rmp_index[i] = button_list[i].standard_index;
		priv->buttons[i] = SAVUCONFIG_KEY_COMBO_BOX(savuconfig_key_combo_box_new(button_list[i].standard_mask));
		gtk_table_attach(GTK_TABLE(standard_table), gtk_label_new(_N(button_list[i].name)), 0, 1, i, i + 1, GTK_EXPAND, GTK_EXPAND, 0, 0);
		gtk_table_attach(GTK_TABLE(standard_table), GTK_WIDGET(priv->buttons[i]), 1, 2, i, i + 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);

		priv->button_rmp_index[i + SAVU_PHYSICAL_BUTTON_NUM] = button_list[i].easyshift_index;
		priv->buttons[i + SAVU_PHYSICAL_BUTTON_NUM] = SAVUCONFIG_KEY_COMBO_BOX(savuconfig_key_combo_box_new(button_list[i].easyshift_mask));
		gtk_table_attach(GTK_TABLE(easyshift_table), GTK_WIDGET(priv->buttons[i + SAVU_PHYSICAL_BUTTON_NUM]), 0, 1, i, i + 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);
	}
}

static void savuconfig_buttons_frame_class_init(SavuconfigButtonsFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(SavuconfigButtonsFramePrivate));
}

void savuconfig_buttons_frame_set_from_rmp(SavuconfigButtonsFrame *buttons_frame, SavuRmp *rmp) {
	SavuconfigButtonsFramePrivate *priv = buttons_frame->priv;
	guint i;

	for (i = 0; i < SAVU_BUTTON_NUM; ++i)
		savuconfig_key_combo_box_set_from_rmp(priv->buttons[i], rmp, priv->button_rmp_index[i]);
}

void savuconfig_buttons_frame_update_rmp(SavuconfigButtonsFrame *buttons_frame, SavuRmp *rmp) {
	SavuconfigButtonsFramePrivate *priv = buttons_frame->priv;
	guint i;

	for (i = 0; i < SAVU_BUTTON_NUM; ++i)
		savuconfig_key_combo_box_update_rmp(priv->buttons[i], rmp, priv->button_rmp_index[i]);
}
