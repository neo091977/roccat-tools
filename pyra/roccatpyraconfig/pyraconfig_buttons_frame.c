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

#include "pyraconfig_buttons_frame.h"
#include "pyraconfig_key_combo_box.h"
#include "i18n.h"

#define PYRACONFIG_BUTTONS_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), PYRACONFIG_BUTTONS_FRAME_TYPE, PyraconfigButtonsFrameClass))
#define IS_PYRACONFIG_BUTTONS_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), PYRACONFIG_BUTTONS_FRAME_TYPE))
#define PYRACONFIG_BUTTONS_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), PYRACONFIG_BUTTONS_FRAME_TYPE, PyraconfigButtonsFramePrivate))

typedef struct _PyraconfigButtonsFrameClass PyraconfigButtonsFrameClass;
typedef struct _PyraconfigButtonsFramePrivate PyraconfigButtonsFramePrivate;

struct _PyraconfigButtonsFrame {
	GtkFrame parent;
	PyraconfigButtonsFramePrivate *priv;
};

struct _PyraconfigButtonsFrameClass {
	GtkFrameClass parent_class;
};

struct _PyraconfigButtonsFramePrivate {
	PyraconfigKeyComboBox *buttons[PYRA_BUTTONS_NUM];
	guint button_rmp_index[PYRA_BUTTONS_NUM];
};

G_DEFINE_TYPE(PyraconfigButtonsFrame, pyraconfig_buttons_frame, GTK_TYPE_FRAME);

enum {
	PHYSICAL_BUTTONS_NUM = 7,
};

typedef struct {
	gchar const * const name;
	PyraRMPMacroKeyIndex standard_index;
	PyraRMPMacroKeyIndex easyshift_index;
} ButtonList;

static ButtonList button_list[PHYSICAL_BUTTONS_NUM] = {
	{N_("Left"), PYRA_RMP_MACRO_KEY_INDEX_LEFT, PYRA_RMP_MACRO_KEY_INDEX_SHIFT_LEFT},
	{N_("Right"), PYRA_RMP_MACRO_KEY_INDEX_RIGHT, PYRA_RMP_MACRO_KEY_INDEX_SHIFT_RIGHT},
	{N_("Middle"), PYRA_RMP_MACRO_KEY_INDEX_MIDDLE, PYRA_RMP_MACRO_KEY_INDEX_SHIFT_MIDDLE},
	{N_("Left side"), PYRA_RMP_MACRO_KEY_INDEX_LEFT_SIDE, PYRA_RMP_MACRO_KEY_INDEX_SHIFT_LEFT_SIDE},
	{N_("Right side"), PYRA_RMP_MACRO_KEY_INDEX_RIGHT_SIDE, PYRA_RMP_MACRO_KEY_INDEX_SHIFT_RIGHT_SIDE},
	{N_("Wheel up"), PYRA_RMP_MACRO_KEY_INDEX_WHEEL_UP, PYRA_RMP_MACRO_KEY_INDEX_SHIFT_WHEEL_UP},
	{N_("Wheel down"), PYRA_RMP_MACRO_KEY_INDEX_WHEEL_DOWN, PYRA_RMP_MACRO_KEY_INDEX_SHIFT_WHEEL_DOWN},
};

GtkWidget *pyraconfig_buttons_frame_new(void) {
	PyraconfigButtonsFrame *frame;

	frame = PYRACONFIG_BUTTONS_FRAME(g_object_new(PYRACONFIG_BUTTONS_FRAME_TYPE,
			"label", _("Buttons"),
			NULL));

	return GTK_WIDGET(frame);
}

static void pyraconfig_buttons_frame_init(PyraconfigButtonsFrame *frame) {
	PyraconfigButtonsFramePrivate *priv = PYRACONFIG_BUTTONS_FRAME_GET_PRIVATE(frame);
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
		priv->buttons[i] = PYRACONFIG_KEY_COMBO_BOX(pyraconfig_key_combo_box_new());
		gtk_table_attach(GTK_TABLE(standard_table), gtk_label_new(_N(button_list[i].name)), 0, 1, i, i + 1, GTK_EXPAND, GTK_EXPAND, 0, 0);
		gtk_table_attach(GTK_TABLE(standard_table), GTK_WIDGET(priv->buttons[i]), 1, 2, i, i + 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);

		priv->button_rmp_index[i + PHYSICAL_BUTTONS_NUM] = button_list[i].easyshift_index;
		priv->buttons[i + PHYSICAL_BUTTONS_NUM] = PYRACONFIG_KEY_COMBO_BOX(pyraconfig_key_combo_box_new());
		gtk_table_attach(GTK_TABLE(easyshift_table), GTK_WIDGET(priv->buttons[i + PHYSICAL_BUTTONS_NUM]), 0, 1, i, i + 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);
	}
}

static void pyraconfig_buttons_frame_class_init(PyraconfigButtonsFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(PyraconfigButtonsFramePrivate));
}

void pyraconfig_buttons_frame_set_from_rmp(PyraconfigButtonsFrame *buttons_frame, PyraRMP *rmp) {
	PyraconfigButtonsFramePrivate *priv = buttons_frame->priv;
	guint i;
	PyraRMPMacroKeyInfo *key_info;

	for (i = 0; i < PYRA_BUTTONS_NUM; ++i) {
		key_info = pyra_rmp_get_macro_key_info(rmp, priv->button_rmp_index[i]);
		pyraconfig_key_combo_box_set_value_blocked(priv->buttons[i], key_info);
	}
}

void pyraconfig_buttons_frame_update_rmp(PyraconfigButtonsFrame *buttons_frame, PyraRMP *rmp) {
	PyraconfigButtonsFramePrivate *priv = buttons_frame->priv;
	guint i;
	PyraRMPMacroKeyInfo *key_info;

	for (i = 0; i < PYRA_BUTTONS_NUM; ++i) {
		key_info = pyraconfig_key_combo_box_get_value(priv->buttons[i]);
		pyra_rmp_set_macro_key_info(rmp, priv->button_rmp_index[i], key_info);
	}
}
