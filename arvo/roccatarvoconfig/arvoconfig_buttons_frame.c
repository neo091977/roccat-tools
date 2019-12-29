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

#include "arvoconfig_buttons_frame.h"
#include "arvoconfig_key_combo_box.h"
#include "i18n.h"

#define ARVOCONFIG_BUTTONS_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ARVOCONFIG_BUTTONS_FRAME_TYPE, ArvoconfigButtonsFrameClass))
#define IS_ARVOCONFIG_BUTTONS_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ARVOCONFIG_BUTTONS_FRAME_TYPE))
#define ARVOCONFIG_BUTTONS_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ARVOCONFIG_BUTTONS_FRAME_TYPE, ArvoconfigButtonsFramePrivate))

typedef struct _ArvoconfigButtonsFrameClass ArvoconfigButtonsFrameClass;
typedef struct _ArvoconfigButtonsFramePrivate ArvoconfigButtonsFramePrivate;

struct _ArvoconfigButtonsFrame {
	GtkFrame parent;
	ArvoconfigButtonsFramePrivate *priv;
};

struct _ArvoconfigButtonsFrameClass {
	GtkFrameClass parent_class;
};

struct _ArvoconfigButtonsFramePrivate {
	ArvoconfigKeyComboBox *buttons[ARVO_BUTTON_NUM];
};

G_DEFINE_TYPE(ArvoconfigButtonsFrame, arvoconfig_buttons_frame, GTK_TYPE_FRAME);

GtkWidget *arvoconfig_buttons_frame_new(void) {
	ArvoconfigButtonsFrame *frame;

	frame = ARVOCONFIG_BUTTONS_FRAME(g_object_new(ARVOCONFIG_BUTTONS_FRAME_TYPE,
			"label", _("Keys"),
			NULL));

	return GTK_WIDGET(frame);
}

static gchar *create_button_name(guint index) {
	return g_strdup_printf("%i", index + 1);
}

static void arvoconfig_buttons_frame_init(ArvoconfigButtonsFrame *frame) {
	ArvoconfigButtonsFramePrivate *priv = ARVOCONFIG_BUTTONS_FRAME_GET_PRIVATE(frame);
	GtkWidget *table;
	gchar *button_name;
	guint i;

	frame->priv = priv;

	table = gtk_table_new(ARVO_BUTTON_NUM, 2, FALSE);
	gtk_container_add(GTK_CONTAINER(frame), table);

	for (i = 0; i < ARVO_BUTTON_NUM; ++i) {
		priv->buttons[i] = ARVOCONFIG_KEY_COMBO_BOX(arvoconfig_key_combo_box_new());
		button_name = create_button_name(i);
		gtk_table_attach(GTK_TABLE(table), gtk_label_new(button_name), 0, 1, i, i + 1, GTK_EXPAND, GTK_EXPAND, 0, 0);
		gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(priv->buttons[i]), 1, 2, i, i + 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);
		g_free(button_name);
	}
}

static void arvoconfig_buttons_frame_class_init(ArvoconfigButtonsFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(ArvoconfigButtonsFramePrivate));
}

void arvoconfig_buttons_frame_set_from_rkp(ArvoconfigButtonsFrame *buttons_frame, ArvoRkp *rkp) {
	ArvoconfigButtonsFramePrivate *priv = buttons_frame->priv;
	guint i;
	ArvoRkpButtonInfo *key_info;

	for (i = 0; i < ARVO_BUTTON_NUM; ++i) {
		key_info = arvo_rkp_get_button_info(rkp, i);
		arvoconfig_key_combo_box_set_value_blocked(priv->buttons[i], key_info);
	}
}

void arvoconfig_buttons_frame_update_rkp(ArvoconfigButtonsFrame *buttons_frame, ArvoRkp *rkp) {
	ArvoconfigButtonsFramePrivate *priv = buttons_frame->priv;
	guint i;
	ArvoRkpButtonInfo *key_info;

	for (i = 0; i < ARVO_BUTTON_NUM; ++i) {
		key_info = arvoconfig_key_combo_box_get_value(priv->buttons[i]);
		arvo_rkp_set_button_info(rkp, i, key_info);
	}
}
