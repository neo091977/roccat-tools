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

#include "nyth_side_buttons_frame.h"
#include "nyth_profile_buttons.h"
#include "nyth_button_combo_box.h"
#include "i18n.h"

#define NYTH_SIDE_BUTTONS_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), NYTH_SIDE_BUTTONS_FRAME_TYPE, NythSideButtonsFrameClass))
#define IS_NYTH_SIDE_BUTTONS_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), NYTH_SIDE_BUTTONS_FRAME_TYPE))
#define NYTH_SIDE_BUTTONS_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), NYTH_SIDE_BUTTONS_FRAME_TYPE, NythSideButtonsFramePrivate))

typedef struct _NythSideButtonsFrameClass NythSideButtonsFrameClass;
typedef struct _NythSideButtonsFramePrivate NythSideButtonsFramePrivate;
typedef struct _NythSideButtonsFramePrivateData NythSideButtonsFramePrivateData;

enum {
	NYTH_SIDE_BUTTONS_NUM = 12,
};

struct _NythSideButtonsFrame {
	GtkFrame parent;
	NythSideButtonsFramePrivate *priv;
};

struct _NythSideButtonsFrameClass {
	GtkFrameClass parent_class;
};

struct _NythSideButtonsFramePrivate {
	NythSideButtonsFramePrivateData *data;
	NythButtonComboBox *standard_buttons[NYTH_SIDE_BUTTONS_NUM];
	NythButtonComboBox *easyshift_buttons[NYTH_SIDE_BUTTONS_NUM];
};

struct _NythSideButtonsFramePrivateData {
	NythButtonComboBoxData standard_datas[NYTH_SIDE_BUTTONS_NUM];
	NythButtonComboBoxData easyshift_datas[NYTH_SIDE_BUTTONS_NUM];
};

G_DEFINE_TYPE(NythSideButtonsFrame, nyth_side_buttons_frame, GTK_TYPE_FRAME);

typedef struct {
	gchar const * const name;
	guint standard_mask;
	guint easyshift_mask;
} ButtonList;

static ButtonList button_list[NYTH_SIDE_BUTTONS_NUM] = {
	{"1", 0, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{"2", 0, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{"3", 0, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{"4", 0, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{"5", 0, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{"6", 0, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{"7", 0, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{"8", 0, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{"9", 0, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{"10", 0, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{"11", 0, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
	{"12", 0, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT},
};

GtkWidget *nyth_side_buttons_frame_new(void) {
	return GTK_WIDGET(g_object_new(NYTH_SIDE_BUTTONS_FRAME_TYPE,
			"label", _("Side buttons"),
			NULL));
}

static void nyth_side_buttons_frame_init(NythSideButtonsFrame *frame) {
	NythSideButtonsFramePrivate *priv = NYTH_SIDE_BUTTONS_FRAME_GET_PRIVATE(frame);
	GtkWidget *standard_frame;
	GtkWidget *standard_table;
	GtkWidget *easyshift_frame;
	GtkWidget *easyshift_table;
	GtkWidget *hbox;
	guint i;

	frame->priv = priv;

	priv->data = (NythSideButtonsFramePrivateData *)g_malloc(sizeof(NythSideButtonsFramePrivateData));

	hbox = gtk_hbox_new(FALSE, 0);
	standard_frame = gtk_frame_new(_("Standard"));
	easyshift_frame = gtk_frame_new(_("Easyshift"));
	standard_table = gtk_table_new(NYTH_SIDE_BUTTONS_NUM, 1, FALSE);
	easyshift_table = gtk_table_new(NYTH_SIDE_BUTTONS_NUM, 2, FALSE);

	gtk_container_add(GTK_CONTAINER(frame), hbox);
	gtk_container_add(GTK_CONTAINER(standard_frame), standard_table);
	gtk_container_add(GTK_CONTAINER(easyshift_frame), easyshift_table);

	gtk_box_pack_start(GTK_BOX(hbox), standard_frame, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), easyshift_frame, TRUE, TRUE, 0);

	for (i = 0; i < NYTH_SIDE_BUTTONS_NUM; ++i) {
		priv->standard_buttons[i] = NYTH_BUTTON_COMBO_BOX(nyth_button_combo_box_new(button_list[i].standard_mask));
		nyth_button_combo_box_set_data_pointer(priv->standard_buttons[i], &priv->data->standard_datas[i]);
		gtk_table_attach(GTK_TABLE(standard_table), gtk_label_new(_N(button_list[i].name)), 0, 1, i, i + 1, GTK_EXPAND, GTK_EXPAND, 0, 0);
		gtk_table_attach(GTK_TABLE(standard_table), GTK_WIDGET(priv->standard_buttons[i]), 1, 2, i, i + 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);

		priv->easyshift_buttons[i] = NYTH_BUTTON_COMBO_BOX(nyth_button_combo_box_new(button_list[i].easyshift_mask));
		nyth_button_combo_box_set_data_pointer(priv->easyshift_buttons[i], &priv->data->easyshift_datas[i]);
		gtk_table_attach(GTK_TABLE(easyshift_table), GTK_WIDGET(priv->easyshift_buttons[i]), 0, 1, i, i + 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);
	}
}

static void finalize(GObject *object) {
	NythSideButtonsFramePrivate *priv = NYTH_SIDE_BUTTONS_FRAME(object)->priv;
	g_free(priv->data);
	G_OBJECT_CLASS(nyth_side_buttons_frame_parent_class)->finalize(object);
}

static void nyth_side_buttons_frame_class_init(NythSideButtonsFrameClass *klass) {
	GObjectClass *gobject_class = (GObjectClass*)klass;

	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(NythSideButtonsFramePrivate));
}

static void set_from_profile_data(NythButtonComboBoxData *data, NythProfileData const *profile_data, guint index) {
	roccat_button_copy(&data->button, &profile_data->hardware.profile_buttons.buttons[index]);
	nyth_macro_copy(&data->macro, &profile_data->hardware.macros[index]);
	roccat_timer_copy(&data->timer, &profile_data->eventhandler.timers[index]);
	g_strlcpy(data->opener, profile_data->eventhandler.openers[index], ROCCAT_SWARM_RMP_OPENER_LENGTH);
	data->talk_target = profile_data->eventhandler.talk_targets[index];
}

void nyth_side_buttons_frame_set_from_profile_data(NythSideButtonsFrame *buttons_frame, NythProfileData const *profile_data) {
	NythSideButtonsFramePrivate *priv = buttons_frame->priv;
	guint i;

	for (i = 0; i < NYTH_SIDE_BUTTONS_NUM; ++i) {
		set_from_profile_data(&priv->data->standard_datas[i], profile_data, i + NYTH_BUTTON_INDEX_1);
		nyth_button_combo_box_update(priv->standard_buttons[i]);
		set_from_profile_data(&priv->data->easyshift_datas[i], profile_data, i + NYTH_BUTTON_INDEX_SHIFT_1);
		nyth_button_combo_box_update(priv->easyshift_buttons[i]);
	}
}

static void update_profile_data(NythButtonComboBoxData *data, NythProfileData *profile_data, guint index) {
	if (!roccat_button_equal(&profile_data->hardware.profile_buttons.buttons[index], &data->button)) {
		roccat_button_copy(&profile_data->hardware.profile_buttons.buttons[index], &data->button);
		profile_data->hardware.modified_profile_buttons = TRUE;
	}
	nyth_profile_data_hardware_set_macro(&profile_data->hardware, index, &data->macro);
	nyth_profile_data_eventhandler_set_timer(&profile_data->eventhandler, index, &data->timer);
	nyth_profile_data_eventhandler_set_opener(&profile_data->eventhandler, index, data->opener);
	nyth_profile_data_eventhandler_set_talk_target(&profile_data->eventhandler, index, data->talk_target);
}

void nyth_side_buttons_frame_update_profile_data(NythSideButtonsFrame *buttons_frame, NythProfileData *profile_data) {
	NythSideButtonsFramePrivate *priv = buttons_frame->priv;
	guint i;

	for (i = 0; i < NYTH_SIDE_BUTTONS_NUM; ++i) {
		update_profile_data(&priv->data->standard_datas[i], profile_data, i + NYTH_BUTTON_INDEX_1);
		update_profile_data(&priv->data->easyshift_datas[i], profile_data, i + NYTH_BUTTON_INDEX_SHIFT_1);
	}
}
