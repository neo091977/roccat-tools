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

#include "skeltr_color_frame.h"
#include "roccat_swarm_color_selection_button.h"
#include "skeltr_light.h"
#include "gtk_roccat_helper.h"
#include "i18n.h"

#define SKELTR_COLOR_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), SKELTR_COLOR_FRAME_TYPE, SkeltrColorFrameClass))
#define IS_SKELTR_COLOR_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), SKELTR_COLOR_FRAME_TYPE))
#define SKELTR_COLOR_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), SKELTR_COLOR_FRAME_TYPE, SkeltrColorFramePrivate))

typedef struct _SkeltrColorFrameClass SkeltrColorFrameClass;
typedef struct _SkeltrColorFramePrivate SkeltrColorFramePrivate;

struct _SkeltrColorFrame {
	GtkFrame parent;
	SkeltrColorFramePrivate *priv;
};

struct _SkeltrColorFrameClass {
	GtkFrameClass parent_class;
};

struct _SkeltrColorFramePrivate {
	RoccatColorSelectionButton *colors[SKELTR_LIGHTS_NUM];
	GSList *radios_color_flow;
};

static struct {
	guint index, left, top;
} const lights[SKELTR_LIGHTS_NUM] = {
		{SKELTR_LIGHT_COLOR_INDEX_KEYS, 1, 1},
		{SKELTR_LIGHT_COLOR_INDEX_TOP_LEFT, 0, 0},
		{SKELTR_LIGHT_COLOR_INDEX_BOTTOM_LEFT, 0, 2},
		{SKELTR_LIGHT_COLOR_INDEX_TOP_RIGHT, 2, 0},
		{SKELTR_LIGHT_COLOR_INDEX_BOTTOM_RIGHT, 2, 2},
};

G_DEFINE_TYPE(SkeltrColorFrame, skeltr_color_frame, GTK_TYPE_FRAME);

static gchar const * const value_key = "value";

static void pack_radio_button(gpointer button, gpointer parent) {
	gtk_box_pack_start(parent, button, TRUE, TRUE, 0);
}

static GtkRadioButton *find_radio(GSList *list, guint wanted) {
	GSList *child;
	guint value;

	for (child = list; child; child = g_slist_next(child)) {
		value = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(child->data), value_key));
		if (value == wanted)
			return child->data;
	}
	return NULL;
}

void skeltr_color_frame_set_from_profile_data(SkeltrColorFrame *frame, SkeltrProfileData const *profile_data) {
	SkeltrColorFramePrivate *priv = frame->priv;
	SkeltrLight const *light = skeltr_profile_data_get_light(profile_data);
	guint i;
	GtkRadioButton *radio;

	radio = find_radio(priv->radios_color_flow, light->color_flow);
	if (radio)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio), TRUE);

	for (i = 0; i < SKELTR_LIGHTS_NUM; ++i) {
		roccat_color_selection_button_set_roccat_light(priv->colors[i], &light->colors[i]);
		roccat_color_selection_button_set_active_type(priv->colors[i], (light->colors[i].index == ROCCAT_SWARM_COLORS_NUM) ? ROCCAT_COLOR_SELECTION_DIALOG_TYPE_CUSTOM : ROCCAT_COLOR_SELECTION_DIALOG_TYPE_PALETTE);
	}
}

void skeltr_color_frame_update_profile_data(SkeltrColorFrame *frame, SkeltrProfileData *profile_data) {
	SkeltrColorFramePrivate *priv = frame->priv;
	SkeltrLight skeltr_light;
	guint i;
	GtkWidget *active;

	skeltr_light_copy(&skeltr_light, skeltr_profile_data_get_light(profile_data));

	active = gtk_roccat_radio_button_group_get_active(priv->radios_color_flow);
	skeltr_light.color_flow = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(active), value_key));

	for (i = 0; i < SKELTR_LIGHTS_NUM; ++i) {
		roccat_color_selection_button_get_roccat_light(priv->colors[i], &skeltr_light.colors[i]);
		if (roccat_color_selection_button_get_active_type(priv->colors[i]) == ROCCAT_COLOR_SELECTION_DIALOG_TYPE_CUSTOM)
			skeltr_light.colors[i].index = ROCCAT_SWARM_COLORS_NUM;
	}

	skeltr_profile_data_set_light(profile_data, &skeltr_light);
}

static void add_radios_color_flow(GtkBox *parent, SkeltrColorFrame *frame) {
	SkeltrColorFramePrivate *priv = frame->priv;
	GtkWidget *radio;

	radio = gtk_radio_button_new_with_label(NULL, _("Flow direction right"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(SKELTR_LIGHT_COLOR_FLOW_RIGHT));

	radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio), _("Flow direction left"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(SKELTR_LIGHT_COLOR_FLOW_LEFT));

	radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio), _("Flow direction down"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(SKELTR_LIGHT_COLOR_FLOW_DOWN));

	radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio), _("Flow direction up"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(SKELTR_LIGHT_COLOR_FLOW_UP));

	radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio), _("All lights simultaneously"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(SKELTR_LIGHT_COLOR_FLOW_ALL_LIGHTS_SIMULTANEOUSLY));

	radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio), _("No color flow"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(SKELTR_LIGHT_COLOR_FLOW_OFF));

	priv->radios_color_flow = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio));
	g_slist_foreach(priv->radios_color_flow, pack_radio_button, parent);
}

GtkWidget *skeltr_color_frame_new(void) {
	return GTK_WIDGET(g_object_new(SKELTR_COLOR_FRAME_TYPE, NULL));
}

static void skeltr_color_frame_init(SkeltrColorFrame *frame) {
	SkeltrColorFramePrivate *priv = SKELTR_COLOR_FRAME_GET_PRIVATE(frame);
	GtkTable *table;
	guint i;
	GtkBox *vbox;

	frame->priv = priv;

	vbox = GTK_BOX(gtk_vbox_new(FALSE, 0));
	table = GTK_TABLE(gtk_table_new(4, 3, TRUE));

	for (i = 0; i < SKELTR_LIGHTS_NUM; ++i) {
		priv->colors[lights[i].index] = ROCCAT_COLOR_SELECTION_BUTTON(roccat_swarm_color_selection_button_new());
		gtk_table_attach(table, GTK_WIDGET(priv->colors[lights[i].index]), lights[i].left, lights[i].left + 1, lights[i].top, lights[i].top + 2, GTK_EXPAND, GTK_EXPAND, 0, 0);
	}

	add_radios_color_flow(vbox, frame);
	gtk_box_pack_start(vbox, gtk_hseparator_new(), TRUE, TRUE, 0);
	gtk_box_pack_start(vbox, GTK_WIDGET(table), TRUE, TRUE, 0);

	gtk_container_add(GTK_CONTAINER(frame), GTK_WIDGET(vbox));
	gtk_frame_set_label(GTK_FRAME(frame), _("Light"));
}

static void skeltr_color_frame_class_init(SkeltrColorFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(SkeltrColorFramePrivate));
}
