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

#include "iskufxconfig_color_frame.h"
#include "iskufx.h"
#include "roccat_color_selection_button.h"
#include "gdk_roccat_helper.h"
#include "i18n.h"

#define ISKUFXCONFIG_COLOR_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ISKUFXCONFIG_COLOR_FRAME_TYPE, IskufxconfigColorFrameClass))
#define IS_ISKUFXCONFIG_COLOR_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ISKUFXCONFIG_COLOR_FRAME_TYPE))
#define ISKUFXCONFIG_COLOR_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ISKUFXCONFIG_COLOR_FRAME_TYPE, IskufxconfigColorFramePrivate))

typedef struct _IskufxconfigColorFrameClass IskufxconfigColorFrameClass;
typedef struct _IskufxconfigColorFramePrivate IskufxconfigColorFramePrivate;

struct _IskufxconfigColorFrame {
	GtkFrame parent;
	IskufxconfigColorFramePrivate *priv;
};

struct _IskufxconfigColorFrameClass {
	GtkFrameClass parent_class;
};

struct _IskufxconfigColorFramePrivate {
	GtkToggleButton *breathing;
	GtkToggleButton *light;

	GtkToggleButton *single_color_radio;
	GtkToggleButton *custom_color_radio;
	GtkToggleButton *color_flow_radio;

	RoccatColorSelectionButton *color;
};

G_DEFINE_TYPE(IskufxconfigColorFrame, iskufxconfig_color_frame, GTK_TYPE_FRAME);

static GdkColor const screen_colors[ISKUFX_RKP_LIGHT_INFO_COLORS_NUM] = {
	{0,  50 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 165 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 254 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0,   0 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 113 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 254 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0,  37 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR,   0 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 253 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0,  92 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR,  24 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 231 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0, 129 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR,  24 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 230 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0, 196 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR,  24 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 230 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0, 255 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR,   5 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 128 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0, 255 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR,  18 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR,   0 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0, 253 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 123 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR,   1 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0, 254 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 242 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR,   0 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0, 197 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 245 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR,   9 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0,  40 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 254 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR,   0 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0,  24 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 167 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR,  43 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0,  19 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 235 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 150 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0,  13 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 237 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 227 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0,   1 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 197 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 255 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
};

static void light_info_to_color(IskufxRkpLightInfo const *light_info, GdkColor *color) {
	color->pixel = 0;
	color->red = light_info->red * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR;
	color->green = light_info->green * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR;
	color->blue = light_info->blue * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR;
}

static void update(IskufxconfigColorFrame *frame) {
	IskufxconfigColorFramePrivate *priv = frame->priv;
	gboolean use_palette = gtk_toggle_button_get_active(priv->single_color_radio);
	roccat_color_selection_button_set_view_type(priv->color, (use_palette) ? ROCCAT_COLOR_SELECTION_DIALOG_TYPE_PALETTE : ROCCAT_COLOR_SELECTION_DIALOG_TYPE_CUSTOM);
}

static void use_palette_cb(GtkToggleButton *toggle_button, gpointer user_data) {
	update(ISKUFXCONFIG_COLOR_FRAME(user_data));
}

void iskufxconfig_color_frame_set_from_rkp(IskufxconfigColorFrame *frame, IskuRkp *rkp) {
	IskufxconfigColorFramePrivate *priv = frame->priv;
	IskufxRkpLightInfo *light_info;
	GdkColor color;
	guint type;

	gtk_toggle_button_set_active(priv->breathing, iskufx_rkp_get_light_breath_switch(rkp));
	gtk_toggle_button_set_active(priv->light, iskufx_rkp_get_light_switch(rkp));

	type = iskufx_rkp_get_light_color_type(rkp);
	switch (type) {
	case ISKUFX_LIGHT_COLOR_TYPE_SINGLE:
		gtk_toggle_button_set_active(priv->single_color_radio, TRUE);
		break;
	case ISKUFX_LIGHT_COLOR_TYPE_CUSTOM:
		gtk_toggle_button_set_active(priv->custom_color_radio, TRUE);
		break;
	case ISKUFX_LIGHT_COLOR_TYPE_FLOW:
		gtk_toggle_button_set_active(priv->color_flow_radio, TRUE);
		break;
	}

	light_info = iskufx_rkp_get_rkp_light_info(rkp);

	if (light_info->index)
		roccat_color_selection_button_set_palette_index(priv->color, light_info->index);

	light_info_to_color(light_info, &color);
	roccat_color_selection_button_set_custom_color(priv->color, &color);

	g_free(light_info);

	update(frame);
}

void iskufxconfig_color_frame_update_rkp(IskufxconfigColorFrame *frame, IskuRkp *rkp) {
	IskufxconfigColorFramePrivate *priv = frame->priv;
	IskufxRkpLightInfo light_info;
	IskufxRkpLightInfo const *standard_light_info;
	GdkColor color;
	gboolean on;
	guint index;

	on = gtk_toggle_button_get_active(priv->light);
	iskufx_rkp_set_light_switch(rkp, on ? ISKUFX_LIGHT_STATE_ON : ISKUFX_LIGHT_STATE_OFF);

	on = gtk_toggle_button_get_active(priv->breathing);
	iskufx_rkp_set_light_breath_switch(rkp, on ? ISKUFX_LIGHT_BREATHING_ON : ISKUFX_LIGHT_BREATHING_OFF);

	if (gtk_toggle_button_get_active(priv->single_color_radio)) {
		index = roccat_color_selection_button_get_palette_index(priv->color);
		standard_light_info = iskufx_rkp_light_info_get_standard(index);
		light_info = *standard_light_info;
		iskufx_rkp_set_rkp_light_info(rkp, &light_info);
		iskufx_rkp_set_light_color_type(rkp, ISKUFX_LIGHT_COLOR_TYPE_SINGLE);
	} else if (gtk_toggle_button_get_active(priv->custom_color_radio)) {
		roccat_color_selection_button_get_custom_color(priv->color, &color);
		light_info.index = 0;
		light_info.state = ISKUFX_RKP_LIGHT_INFO_STATE_ON;
		light_info.red = color.red / GDK_ROCCAT_BYTE_TO_COLOR_FACTOR;
		light_info.green = color.green / GDK_ROCCAT_BYTE_TO_COLOR_FACTOR;
		light_info.blue = color.blue / GDK_ROCCAT_BYTE_TO_COLOR_FACTOR;
		iskufx_rkp_set_rkp_light_info(rkp, &light_info);
		iskufx_rkp_set_light_color_type(rkp, ISKUFX_LIGHT_COLOR_TYPE_CUSTOM);
	} else if (gtk_toggle_button_get_active(priv->color_flow_radio)) {
		iskufx_rkp_set_light_color_type(rkp, ISKUFX_LIGHT_COLOR_TYPE_FLOW);
	}
}

GtkWidget *iskufxconfig_color_frame_new(void) {
	IskufxconfigColorFrame *frame;

	frame = ISKUFXCONFIG_COLOR_FRAME(g_object_new(ISKUFXCONFIG_COLOR_FRAME_TYPE,
			NULL));

	return GTK_WIDGET(frame);
}

static void iskufxconfig_color_frame_init(IskufxconfigColorFrame *frame) {
	IskufxconfigColorFramePrivate *priv = ISKUFXCONFIG_COLOR_FRAME_GET_PRIVATE(frame);
	GtkWidget *vbox;

	frame->priv = priv;

	vbox = gtk_vbox_new(FALSE, 0);
	priv->light = GTK_TOGGLE_BUTTON(gtk_check_button_new_with_label(_("Fully lighted")));
	priv->breathing = GTK_TOGGLE_BUTTON(gtk_check_button_new_with_label(_("Breathing")));
	priv->single_color_radio = GTK_TOGGLE_BUTTON(gtk_radio_button_new_with_label(NULL, _("Single color")));
	priv->custom_color_radio = GTK_TOGGLE_BUTTON(gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(priv->single_color_radio), _("Custom color")));
	priv->color_flow_radio = GTK_TOGGLE_BUTTON(gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(priv->custom_color_radio), _("Color flow")));
	priv->color = ROCCAT_COLOR_SELECTION_BUTTON(roccat_color_selection_button_new());
	roccat_color_selection_button_set_palette_dimensions(priv->color, ISKUFX_RKP_LIGHT_INFO_COLORS_NUM, 0);
	roccat_color_selection_button_set_palette_colors(priv->color, screen_colors, ISKUFX_RKP_LIGHT_INFO_COLORS_NUM);

	g_signal_connect(G_OBJECT(priv->single_color_radio), "toggled", G_CALLBACK(use_palette_cb), frame);

	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(priv->light), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(priv->breathing), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), gtk_hseparator_new(), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(priv->single_color_radio), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(priv->custom_color_radio), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(priv->color_flow_radio), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), gtk_hseparator_new(), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(priv->color), TRUE, TRUE, 0);

	gtk_container_add(GTK_CONTAINER(frame), vbox);

	gtk_frame_set_label(GTK_FRAME(frame), _("Color"));
}

static void iskufxconfig_color_frame_class_init(IskufxconfigColorFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(IskufxconfigColorFramePrivate));
}
