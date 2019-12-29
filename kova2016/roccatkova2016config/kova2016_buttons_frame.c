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

#include "kova2016_buttons_frame.h"
#include "kova2016_profile_buttons.h"
#include "kova2016_button_combo_box.h"
#include "i18n.h"

#define KOVA2016_BUTTONS_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KOVA2016_BUTTONS_FRAME_TYPE, Kova2016ButtonsFrameClass))
#define IS_KOVA2016_BUTTONS_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KOVA2016_BUTTONS_FRAME_TYPE))
#define KOVA2016_BUTTONS_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KOVA2016_BUTTONS_FRAME_TYPE, Kova2016ButtonsFramePrivate))

typedef struct _Kova2016ButtonsFrameClass Kova2016ButtonsFrameClass;
typedef struct _Kova2016ButtonsFramePrivate Kova2016ButtonsFramePrivate;
typedef struct _Kova2016ButtonsFramePrivateData Kova2016ButtonsFramePrivateData;

struct _Kova2016ButtonsFrame {
	GtkFrame parent;
	Kova2016ButtonsFramePrivate *priv;
};

struct _Kova2016ButtonsFrameClass {
	GtkFrameClass parent_class;
};

struct _Kova2016ButtonsFramePrivate {
	Kova2016ButtonsFramePrivateData *data;
	Kova2016ButtonComboBox *standard_buttons[KOVA2016_PHYSICAL_BUTTON_NUM];
	Kova2016ButtonComboBox *easyshift_buttons[KOVA2016_PHYSICAL_BUTTON_NUM];
};

struct _Kova2016ButtonsFramePrivateData {
	Kova2016ButtonComboBoxData standard_datas[KOVA2016_PHYSICAL_BUTTON_NUM];
	Kova2016ButtonComboBoxData easyshift_datas[KOVA2016_PHYSICAL_BUTTON_NUM];
};

G_DEFINE_TYPE(Kova2016ButtonsFrame, kova2016_buttons_frame, GTK_TYPE_FRAME);

typedef struct {
	gchar const * const name;
	guint standard_mask;
	guint easyshift_mask;
} ButtonList;

static ButtonList button_list[KOVA2016_PHYSICAL_BUTTON_NUM] = { // FIXME
	{N_("Left"), KOVA2016_BUTTON_COMBO_BOX_GROUP_EXTENDED | ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT,
		KOVA2016_BUTTON_COMBO_BOX_GROUP_EXTENDED | ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{N_("Right"), KOVA2016_BUTTON_COMBO_BOX_GROUP_EXTENDED | ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT,
		KOVA2016_BUTTON_COMBO_BOX_GROUP_EXTENDED | ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{N_("Middle"), KOVA2016_BUTTON_COMBO_BOX_GROUP_EXTENDED | ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT,
		KOVA2016_BUTTON_COMBO_BOX_GROUP_EXTENDED | ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{N_("Left Smart Cast"), 0,
		ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{N_("Left forward"), 0,
		ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{N_("Left backward"), 0,
		ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{N_("Right Smart Cast"), 0,
		ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{N_("Right forward"), 0,
		ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{N_("Right backward"), 0,
		ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{N_("Top"), 0,
		ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{N_("Wheel up"), 0,
		ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{N_("Wheel down"), 0,
		ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
};

GtkWidget *kova2016_buttons_frame_new(void) {
	return GTK_WIDGET(g_object_new(KOVA2016_BUTTONS_FRAME_TYPE,
			"label", _("Buttons"),
			NULL));
}

static void kova2016_buttons_frame_init(Kova2016ButtonsFrame *frame) {
	Kova2016ButtonsFramePrivate *priv = KOVA2016_BUTTONS_FRAME_GET_PRIVATE(frame);
	GtkWidget *standard_frame;
	GtkWidget *standard_table;
	GtkWidget *easyshift_frame;
	GtkWidget *easyshift_table;
	GtkWidget *hbox;
	guint i;

	frame->priv = priv;

	priv->data = (Kova2016ButtonsFramePrivateData *)g_malloc(sizeof(Kova2016ButtonsFramePrivateData));

	hbox = gtk_hbox_new(FALSE, 0);
	standard_frame = gtk_frame_new(_("Standard"));
	easyshift_frame = gtk_frame_new(_("Easyshift"));
	standard_table = gtk_table_new(KOVA2016_PHYSICAL_BUTTON_NUM, 1, FALSE);
	easyshift_table = gtk_table_new(KOVA2016_PHYSICAL_BUTTON_NUM, 2, FALSE);

	gtk_container_add(GTK_CONTAINER(frame), hbox);
	gtk_container_add(GTK_CONTAINER(standard_frame), standard_table);
	gtk_container_add(GTK_CONTAINER(easyshift_frame), easyshift_table);

	gtk_box_pack_start(GTK_BOX(hbox), standard_frame, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), easyshift_frame, TRUE, TRUE, 0);

	for (i = 0; i < KOVA2016_PHYSICAL_BUTTON_NUM; ++i) {
		priv->standard_buttons[i] = KOVA2016_BUTTON_COMBO_BOX(kova2016_button_combo_box_new(button_list[i].standard_mask));
		kova2016_button_combo_box_set_data_pointer(priv->standard_buttons[i], &priv->data->standard_datas[i]);
		gtk_table_attach(GTK_TABLE(standard_table), gtk_label_new(_N(button_list[i].name)), 0, 1, i, i + 1, GTK_EXPAND, GTK_EXPAND, 0, 0);
		gtk_table_attach(GTK_TABLE(standard_table), GTK_WIDGET(priv->standard_buttons[i]), 1, 2, i, i + 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);

		priv->easyshift_buttons[i] = KOVA2016_BUTTON_COMBO_BOX(kova2016_button_combo_box_new(button_list[i].easyshift_mask));
		kova2016_button_combo_box_set_data_pointer(priv->easyshift_buttons[i], &priv->data->easyshift_datas[i]);
		gtk_table_attach(GTK_TABLE(easyshift_table), GTK_WIDGET(priv->easyshift_buttons[i]), 0, 1, i, i + 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);
	}
}

static void finalize(GObject *object) {
	Kova2016ButtonsFramePrivate *priv = KOVA2016_BUTTONS_FRAME(object)->priv;
	g_free(priv->data);
	G_OBJECT_CLASS(kova2016_buttons_frame_parent_class)->finalize(object);
}

static void kova2016_buttons_frame_class_init(Kova2016ButtonsFrameClass *klass) {
	GObjectClass *gobject_class = (GObjectClass*)klass;

	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(Kova2016ButtonsFramePrivate));
}

static void set_from_profile_data(Kova2016ButtonComboBoxData *data, Kova2016ProfileData const *profile_data, guint index) {
	roccat_button_copy(&data->button, &profile_data->hardware.profile_buttons.buttons[index]);
	kova2016_macro_copy(&data->macro, &profile_data->hardware.macros[index]);
	roccat_timer_copy(&data->timer, &profile_data->eventhandler.timers[index]);
	g_strlcpy(data->opener, profile_data->eventhandler.openers[index], ROCCAT_SWARM_RMP_OPENER_LENGTH);
	data->talk_target = profile_data->eventhandler.talk_targets[index];
}

void kova2016_buttons_frame_set_from_profile_data(Kova2016ButtonsFrame *buttons_frame, Kova2016ProfileData const *profile_data) {
	Kova2016ButtonsFramePrivate *priv = buttons_frame->priv;
	guint i;

	for (i = 0; i < KOVA2016_PHYSICAL_BUTTON_NUM; ++i) {
		set_from_profile_data(&priv->data->standard_datas[i], profile_data, i + KOVA2016_BUTTON_INDEX_LEFT);
		kova2016_button_combo_box_update(priv->standard_buttons[i]);
		set_from_profile_data(&priv->data->easyshift_datas[i], profile_data, i + KOVA2016_BUTTON_INDEX_SHIFT_LEFT);
		kova2016_button_combo_box_update(priv->easyshift_buttons[i]);
	}
}

static void update_profile_data(Kova2016ButtonComboBoxData *data, Kova2016ProfileData *profile_data, guint index) {
	if (!roccat_button_equal(&profile_data->hardware.profile_buttons.buttons[index], &data->button)) {
		roccat_button_copy(&profile_data->hardware.profile_buttons.buttons[index], &data->button);
		profile_data->hardware.modified_profile_buttons = TRUE;
	}
	kova2016_profile_data_hardware_set_macro(&profile_data->hardware, index, &data->macro);
	kova2016_profile_data_eventhandler_set_timer(&profile_data->eventhandler, index, &data->timer);
	kova2016_profile_data_eventhandler_set_opener(&profile_data->eventhandler, index, data->opener);
	kova2016_profile_data_eventhandler_set_talk_target(&profile_data->eventhandler, index, data->talk_target);
}

void kova2016_buttons_frame_update_profile_data(Kova2016ButtonsFrame *buttons_frame, Kova2016ProfileData *profile_data) {
	Kova2016ButtonsFramePrivate *priv = buttons_frame->priv;
	guint i;

	for (i = 0; i < KOVA2016_PHYSICAL_BUTTON_NUM; ++i) {
		update_profile_data(&priv->data->standard_datas[i], profile_data, i + KOVA2016_BUTTON_INDEX_LEFT);
		update_profile_data(&priv->data->easyshift_datas[i], profile_data, i + KOVA2016_BUTTON_INDEX_SHIFT_LEFT);
	}
}

static void swap_buttons_single_layer(Kova2016ButtonComboBox *buttons[], Kova2016ButtonComboBoxData *datas, guint one, guint other) {
	kova2016_button_combo_box_data_swap(&datas[one], &datas[other]);
	kova2016_button_combo_box_set_data_pointer(buttons[one], &datas[one]);
	kova2016_button_combo_box_set_data_pointer(buttons[other], &datas[other]);
}

static void swap_buttons_both_layers(Kova2016ButtonsFrame *buttons_frame, guint one, guint other) {
	Kova2016ButtonsFramePrivate *priv = buttons_frame->priv;
	swap_buttons_single_layer(priv->standard_buttons, priv->data->standard_datas, one, other);
	swap_buttons_single_layer(priv->easyshift_buttons, priv->data->easyshift_datas, one, other);
}

/* As the device doesn't swap anything itself, it's not enough to just swap the data pointers for display.
 * Windows driver v1.41 does not swap smart cast buttons.
 */
void kova2016_buttons_frame_change_handedness(Kova2016ButtonsFrame *buttons_frame) {
	swap_buttons_both_layers(buttons_frame, KOVA2016_BUTTON_INDEX_LEFT, KOVA2016_BUTTON_INDEX_RIGHT);
	swap_buttons_both_layers(buttons_frame, KOVA2016_BUTTON_INDEX_SMART_CAST_LEFT, KOVA2016_BUTTON_INDEX_SMART_CAST_RIGHT);
	swap_buttons_both_layers(buttons_frame, KOVA2016_BUTTON_INDEX_FORWARD_LEFT, KOVA2016_BUTTON_INDEX_FORWARD_RIGHT);
	swap_buttons_both_layers(buttons_frame, KOVA2016_BUTTON_INDEX_BACKWARD_LEFT, KOVA2016_BUTTON_INDEX_BACKWARD_RIGHT);
}
