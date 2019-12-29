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

#include "koneconfig_buttons_frame.h"
#include "koneconfig_key_combo_box.h"
#include "i18n.h"

#define KONECONFIG_BUTTONS_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KONECONFIG_BUTTONS_FRAME_TYPE, KoneconfigButtonsFrameClass))
#define IS_KONECONFIG_BUTTONS_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KONECONFIG_BUTTONS_FRAME_TYPE))
#define KONECONFIG_BUTTONS_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KONECONFIG_BUTTONS_FRAME_TYPE, KoneconfigButtonsFramePrivate))

typedef struct _KoneconfigButtonsFrameClass KoneconfigButtonsFrameClass;
typedef struct _KoneconfigButtonsFramePrivate KoneconfigButtonsFramePrivate;

struct _KoneconfigButtonsFrame {
	GtkFrame parent;
	KoneconfigButtonsFramePrivate *priv;
};

struct _KoneconfigButtonsFrameClass {
	GtkFrameClass parent_class;
};

struct _KoneconfigButtonsFramePrivate {
	KoneconfigKeyComboBox *buttons[KONE_BUTTON_INFO_NUM];
	guint button_rmp_index[KONE_BUTTON_INFO_NUM];
};

G_DEFINE_TYPE(KoneconfigButtonsFrame, koneconfig_buttons_frame, GTK_TYPE_FRAME);

typedef struct {
	gchar const *name;
	KoneButtonInfoIndex index;
} ButtonList;

static ButtonList button_list[KONE_BUTTON_INFO_NUM] = {
	{N_("Top"), KONE_BUTTON_INFO_INDEX_TOP},
	{N_("Tilt left"), KONE_BUTTON_INFO_INDEX_WHEEL_TILT_LEFT},
	{N_("Tilt right"), KONE_BUTTON_INFO_INDEX_WHEEL_TILT_RIGHT},
	{N_("Forward"), KONE_BUTTON_INFO_INDEX_FORWARD},
	{N_("Backward"), KONE_BUTTON_INFO_INDEX_BACKWARD},
	{N_("Middle"), KONE_BUTTON_INFO_INDEX_MIDDLE},
	{N_("Plus"), KONE_BUTTON_INFO_INDEX_PLUS},
	{N_("Minus"), KONE_BUTTON_INFO_INDEX_MINUS}
};

GtkWidget *koneconfig_buttons_frame_new(void) {
	KoneconfigButtonsFrame *frame;

	frame = KONECONFIG_BUTTONS_FRAME(g_object_new(KONECONFIG_BUTTONS_FRAME_TYPE,
			"label", _("Buttons"),
			NULL));

	return GTK_WIDGET(frame);
}

static void koneconfig_buttons_frame_init(KoneconfigButtonsFrame *frame) {
	KoneconfigButtonsFramePrivate *priv = KONECONFIG_BUTTONS_FRAME_GET_PRIVATE(frame);
	GtkWidget *table;
	guint i;

	frame->priv = priv;

	table = gtk_table_new(KONE_BUTTON_INFO_NUM, 2, FALSE);
	gtk_container_add(GTK_CONTAINER(frame), table);

	for (i = 0; i < KONE_BUTTON_INFO_NUM; ++i) {
		priv->button_rmp_index[i] = button_list[i].index;
		priv->buttons[i] = KONECONFIG_KEY_COMBO_BOX(koneconfig_key_combo_box_new());
		gtk_table_attach(GTK_TABLE(table), gtk_label_new(_N(button_list[i].name)), 0, 1, i, i + 1, GTK_EXPAND, GTK_EXPAND, 0, 0);
		gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(priv->buttons[i]), 1, 2, i, i + 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);
	}
}

static void koneconfig_buttons_frame_class_init(KoneconfigButtonsFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(KoneconfigButtonsFramePrivate));
}

void koneconfig_buttons_frame_set_from_rmp(KoneconfigButtonsFrame *buttons_frame, KoneRMP *rmp) {
	KoneconfigButtonsFramePrivate *priv = buttons_frame->priv;
	guint i;
	KoneRMPButtonInfo *key_info;

	for (i = 0; i < KONE_BUTTON_INFO_NUM; ++i) {
		key_info = kone_rmp_get_rmp_button_info(rmp, priv->button_rmp_index[i]);
		koneconfig_key_combo_box_set_value_blocked(priv->buttons[i], key_info);
	}
}

void koneconfig_buttons_frame_update_rmp(KoneconfigButtonsFrame *buttons_frame, KoneRMP *rmp) {
	KoneconfigButtonsFramePrivate *priv = buttons_frame->priv;
	guint i;
	KoneRMPButtonInfo *key_info;

	for (i = 0; i < KONE_BUTTON_INFO_NUM; ++i) {
		key_info = koneconfig_key_combo_box_get_value(priv->buttons[i]);
		kone_rmp_set_rmp_button_info(rmp, priv->button_rmp_index[i], key_info);
	}
}
