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

#include "kiro_buttons_frame.h"
#include "kiro_profile.h"
#include "kiro_button_combo_box.h"
#include "i18n.h"

#define KIRO_BUTTONS_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KIRO_BUTTONS_FRAME_TYPE, KiroButtonsFrameClass))
#define IS_KIRO_BUTTONS_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KIRO_BUTTONS_FRAME_TYPE))
#define KIRO_BUTTONS_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KIRO_BUTTONS_FRAME_TYPE, KiroButtonsFramePrivate))

typedef struct _KiroButtonsFrameClass KiroButtonsFrameClass;
typedef struct _KiroButtonsFramePrivate KiroButtonsFramePrivate;
typedef struct _KiroButtonsFramePrivateData KiroButtonsFramePrivateData;

struct _KiroButtonsFrame {
	GtkFrame parent;
	KiroButtonsFramePrivate *priv;
};

struct _KiroButtonsFrameClass {
	GtkFrameClass parent_class;
};

struct _KiroButtonsFramePrivate {
	KiroButtonsFramePrivateData *data;
	KiroButtonComboBox *buttons[KIRO_BUTTON_NUM];
};

struct _KiroButtonsFramePrivateData {
	KiroButtonComboBoxData datas[KIRO_BUTTON_NUM];
};

G_DEFINE_TYPE(KiroButtonsFrame, kiro_buttons_frame, GTK_TYPE_FRAME);

typedef struct {
	gchar const * const name;
	guint mask;
} ButtonList;

static ButtonList button_list[KIRO_BUTTON_NUM] = {
	{N_("Left"), 0},
	{N_("Right"), 0},
	{N_("Middle"), 0},
	{N_("Left forward"), 0},
	{N_("Left backward"), 0},
	{N_("Right forward"), 0},
	{N_("Right backward"), 0},
	{N_("Top"), 0},
	{N_("Wheel up"), 0},
	{N_("Wheel down"), 0},
};

GtkWidget *kiro_buttons_frame_new(void) {
	return GTK_WIDGET(g_object_new(KIRO_BUTTONS_FRAME_TYPE,
			"label", _("Buttons"),
			NULL));
}

static void kiro_buttons_frame_init(KiroButtonsFrame *frame) {
	KiroButtonsFramePrivate *priv = KIRO_BUTTONS_FRAME_GET_PRIVATE(frame);
	GtkWidget *standard_table;
	guint i;

	frame->priv = priv;

	priv->data = (KiroButtonsFramePrivateData *)g_malloc(sizeof(KiroButtonsFramePrivateData));

	standard_table = gtk_table_new(KIRO_BUTTON_NUM, 1, FALSE);

	gtk_container_add(GTK_CONTAINER(frame), standard_table);

	for (i = 0; i < KIRO_BUTTON_NUM; ++i) {
		priv->buttons[i] = KIRO_BUTTON_COMBO_BOX(kiro_button_combo_box_new(button_list[i].mask));
		kiro_button_combo_box_set_data_pointer(priv->buttons[i], &priv->data->datas[i]);
		gtk_table_attach(GTK_TABLE(standard_table), gtk_label_new(_N(button_list[i].name)), 0, 1, i, i + 1, GTK_EXPAND, GTK_EXPAND, 0, 0);
		gtk_table_attach(GTK_TABLE(standard_table), GTK_WIDGET(priv->buttons[i]), 1, 2, i, i + 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);
	}
}

static void finalize(GObject *object) {
	KiroButtonsFramePrivate *priv = KIRO_BUTTONS_FRAME(object)->priv;
	g_free(priv->data);
	G_OBJECT_CLASS(kiro_buttons_frame_parent_class)->finalize(object);
}

static void kiro_buttons_frame_class_init(KiroButtonsFrameClass *klass) {
	GObjectClass *gobject_class = (GObjectClass*)klass;

	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(KiroButtonsFramePrivate));
}

static void set_from_profile_data(KiroButtonComboBoxData *data, KiroProfileData const *profile_data, guint index) {
	roccat_button_copy(&data->button, &profile_data->hardware.profile.buttons[index]);
	g_strlcpy(data->opener, profile_data->eventhandler.openers[index], ROCCAT_SWARM_RMP_OPENER_LENGTH);
}

void kiro_buttons_frame_set_from_profile_data(KiroButtonsFrame *buttons_frame, KiroProfileData const *profile_data) {
	KiroButtonsFramePrivate *priv = buttons_frame->priv;
	guint i;

	for (i = 0; i < KIRO_BUTTON_NUM; ++i) {
		set_from_profile_data(&priv->data->datas[i], profile_data, i + KIRO_BUTTON_INDEX_LEFT);
		kiro_button_combo_box_update(priv->buttons[i]);
	}
}

static void update_profile_data(KiroButtonComboBoxData *data, KiroProfileData *profile_data, guint index) {
	if (!roccat_button_equal(&profile_data->hardware.profile.buttons[index], &data->button)) {
		roccat_button_copy(&profile_data->hardware.profile.buttons[index], &data->button);
		profile_data->hardware.modified_profile = TRUE;
	}
	kiro_profile_data_eventhandler_set_opener(&profile_data->eventhandler, index, data->opener);
}

void kiro_buttons_frame_update_profile_data(KiroButtonsFrame *buttons_frame, KiroProfileData *profile_data) {
	KiroButtonsFramePrivate *priv = buttons_frame->priv;
	guint i;

	for (i = 0; i < KIRO_BUTTON_NUM; ++i) {
		update_profile_data(&priv->data->datas[i], profile_data, i + KIRO_BUTTON_INDEX_LEFT);
	}
}

static void swap_buttons(KiroButtonComboBox *buttons[], KiroButtonComboBoxData *datas, guint one, guint other) {
	kiro_button_combo_box_data_swap(&datas[one], &datas[other]);
	kiro_button_combo_box_set_data_pointer(buttons[one], &datas[one]);
	kiro_button_combo_box_set_data_pointer(buttons[other], &datas[other]);
}

void kiro_buttons_frame_change_handedness(KiroButtonsFrame *buttons_frame) {
	KiroButtonsFramePrivate *priv = buttons_frame->priv;
	swap_buttons(priv->buttons, priv->data->datas, KIRO_BUTTON_INDEX_LEFT, KIRO_BUTTON_INDEX_RIGHT);
	swap_buttons(priv->buttons, priv->data->datas, KIRO_BUTTON_INDEX_FORWARD_LEFT, KIRO_BUTTON_INDEX_FORWARD_RIGHT);
	swap_buttons(priv->buttons, priv->data->datas, KIRO_BUTTON_INDEX_BACKWARD_LEFT, KIRO_BUTTON_INDEX_BACKWARD_RIGHT);
}
