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

#include "savuconfig_light_frame.h"
#include "roccat_color_selection_button.h"
#include "gdk_roccat_helper.h"
#include "i18n.h"

#define SAVUCONFIG_LIGHT_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), SAVUCONFIG_LIGHT_FRAME_TYPE, SavuconfigLightFrameClass))
#define IS_SAVUCONFIG_LIGHT_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), SAVUCONFIG_LIGHT_FRAME_TYPE))
#define SAVUCONFIG_LIGHT_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), SAVUCONFIG_LIGHT_FRAME_TYPE, SavuconfigLightFramePrivate))

typedef struct _SavuconfigLightFrameClass SavuconfigLightFrameClass;
typedef struct _SavuconfigLightFramePrivate SavuconfigLightFramePrivate;

struct _SavuconfigLightFrame {
	GtkFrame parent;
	SavuconfigLightFramePrivate *priv;
};

struct _SavuconfigLightFrameClass {
	GtkFrameClass parent_class;
};

struct _SavuconfigLightFramePrivate {
	GtkToggleButton *breathing;
	GtkToggleButton *light;

	GtkToggleButton *palette_color_radio;
	GtkToggleButton *custom_color_radio;
	GtkToggleButton *color_flow_radio;

	RoccatColorSelectionButton *color;
};

G_DEFINE_TYPE(SavuconfigLightFrame, savuconfig_light_frame, GTK_TYPE_FRAME);

static GdkColor const screen_colors[SAVU_SINGLE_COLOR_NUM] = {
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

static struct { guint red, green, blue; } const hardware_colors[SAVU_SINGLE_COLOR_NUM] = {
	{0x05, 0x9c, 0xfd},
	{0x00, 0x71, 0xfd},
	{0x00, 0x00, 0xfd},
	{0x10, 0x00, 0xfd},
	{0x60, 0x1a, 0xfd},
	{0xbe, 0x1a, 0xfd},
	{0xfd, 0x00, 0x7f},
	{0xfd, 0x00, 0x00},
	{0xfd, 0x16, 0x00},
	{0xfd, 0x7f, 0x00},
	{0xfd, 0xfd, 0x00},
	{0x2a, 0xfd, 0x00},
	{0x00, 0xfd, 0x00},
	{0x1a, 0xa5, 0x2a},
	{0x16, 0xfd, 0x9c},
	{0x16, 0xfd, 0xfd},
};

static void light_info_to_color(guint red, guint green, guint blue, GdkColor *color) {
	color->pixel = 0;
	color->red = red * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR;
	color->green = green * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR;
	color->blue = blue * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR;
}

static void update(SavuconfigLightFrame *frame) {
	SavuconfigLightFramePrivate *priv = frame->priv;
	gboolean use_palette = gtk_toggle_button_get_active(priv->palette_color_radio);
	roccat_color_selection_button_set_view_type(priv->color, (use_palette) ? ROCCAT_COLOR_SELECTION_DIALOG_TYPE_PALETTE : ROCCAT_COLOR_SELECTION_DIALOG_TYPE_CUSTOM);
}

static void use_palette_cb(GtkToggleButton *toggle_button, gpointer user_data) {
	update(SAVUCONFIG_LIGHT_FRAME(user_data));
}

static void button_off_cb(GtkToggleButton *toggle_button, gpointer user_data) {
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(user_data), FALSE);
}

void savuconfig_light_frame_set_from_rmp(SavuconfigLightFrame *frame, SavuRmp *rmp) {
	SavuconfigLightFramePrivate *priv = frame->priv;
	GdkColor color;
	guint color_index;
	guint color_flow;

	gtk_toggle_button_set_active(priv->breathing, savu_rmp_get_breath_on(rmp));
	gtk_toggle_button_set_active(priv->light, savu_rmp_get_light_switch(rmp));

	light_info_to_color(
			savu_rmp_get_color_red(rmp),
			savu_rmp_get_color_green(rmp),
			savu_rmp_get_color_blue(rmp),
			&color);
	roccat_color_selection_button_set_custom_color(priv->color, &color);

	color_index = savu_rmp_get_color_database(rmp);
	if (color_index < SAVU_GENERAL_COLOR_INDEX_CUSTOM)
		roccat_color_selection_button_set_palette_index(priv->color, color_index);

	color_flow = savu_rmp_get_color_flow(rmp);
	if (color_flow)
		gtk_toggle_button_set_active(priv->color_flow_radio, TRUE);
	else if (color_index == SAVU_GENERAL_COLOR_INDEX_CUSTOM)
		gtk_toggle_button_set_active(priv->custom_color_radio, TRUE);
	else
		gtk_toggle_button_set_active(priv->palette_color_radio, TRUE);

	update(frame);
}

void savuconfig_light_frame_update_rmp(SavuconfigLightFrame *frame, SavuRmp *rmp) {
	SavuconfigLightFramePrivate *priv = frame->priv;
	GdkColor color;
	gboolean on;
	guint index;

	if (gtk_toggle_button_get_active(priv->custom_color_radio)) {
		roccat_color_selection_button_get_custom_color(priv->color, &color);
		savu_rmp_set_color_red(rmp, color.red / GDK_ROCCAT_BYTE_TO_COLOR_FACTOR);
		savu_rmp_set_color_green(rmp, color.green / GDK_ROCCAT_BYTE_TO_COLOR_FACTOR);
		savu_rmp_set_color_blue(rmp, color.blue / GDK_ROCCAT_BYTE_TO_COLOR_FACTOR);
		savu_rmp_set_color_database(rmp, SAVU_GENERAL_COLOR_INDEX_CUSTOM);
		savu_rmp_set_breath_on(rmp, gtk_toggle_button_get_active(priv->breathing) ? 1 : 0);
		savu_rmp_set_color_flow(rmp, 0);
	} else if (gtk_toggle_button_get_active(priv->palette_color_radio)) {
		index = roccat_color_selection_button_get_palette_index(priv->color);
		savu_rmp_set_color_red(rmp, hardware_colors[index].red);
		savu_rmp_set_color_green(rmp, hardware_colors[index].green);
		savu_rmp_set_color_blue(rmp, hardware_colors[index].blue);
		savu_rmp_set_color_database(rmp, index);
		savu_rmp_set_breath_on(rmp, gtk_toggle_button_get_active(priv->breathing) ? 1 : 0);
		savu_rmp_set_color_flow(rmp, 0);
	} else if (gtk_toggle_button_get_active(priv->color_flow_radio)) {
		savu_rmp_set_breath_on(rmp, 0);
		savu_rmp_set_color_flow(rmp, 1);
	}

	on = gtk_toggle_button_get_active(priv->light);
	savu_rmp_set_light_switch(rmp, on ? 1 : 0);
}

GtkWidget *savuconfig_light_frame_new(void) {
	SavuconfigLightFrame *frame;

	frame = SAVUCONFIG_LIGHT_FRAME(g_object_new(SAVUCONFIG_LIGHT_FRAME_TYPE,
			NULL));

	return GTK_WIDGET(frame);
}


static void savuconfig_light_frame_init(SavuconfigLightFrame *frame) {
	SavuconfigLightFramePrivate *priv = SAVUCONFIG_LIGHT_FRAME_GET_PRIVATE(frame);
	GtkWidget *vbox;

	frame->priv = priv;

	vbox = gtk_vbox_new(FALSE, 0);
	priv->light = GTK_TOGGLE_BUTTON(gtk_check_button_new_with_label(_("Fully lighted")));
	priv->breathing = GTK_TOGGLE_BUTTON(gtk_check_button_new_with_label(_("Breathing")));
	priv->palette_color_radio = GTK_TOGGLE_BUTTON(gtk_radio_button_new_with_label(NULL, _("Palette color")));
	priv->custom_color_radio = GTK_TOGGLE_BUTTON(gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(priv->palette_color_radio), _("Custom color")));
	priv->color_flow_radio = GTK_TOGGLE_BUTTON(gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(priv->custom_color_radio), _("Color flow")));
	priv->color = ROCCAT_COLOR_SELECTION_BUTTON(roccat_color_selection_button_new());
	roccat_color_selection_button_set_palette_dimensions(priv->color, SAVU_SINGLE_COLOR_NUM, 0);
	roccat_color_selection_button_set_palette_colors(priv->color, screen_colors, SAVU_SINGLE_COLOR_NUM);

	g_signal_connect(G_OBJECT(priv->palette_color_radio), "toggled", G_CALLBACK(use_palette_cb), frame);
	g_signal_connect(G_OBJECT(priv->color_flow_radio), "toggled", G_CALLBACK(button_off_cb), priv->breathing);

	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(priv->light), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(priv->breathing), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), gtk_hseparator_new(), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(priv->palette_color_radio), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(priv->custom_color_radio), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(priv->color_flow_radio), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), gtk_hseparator_new(), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(priv->color), TRUE, TRUE, 0);

	gtk_container_add(GTK_CONTAINER(frame), vbox);

	gtk_frame_set_label(GTK_FRAME(frame), _("Light"));
}

static void savuconfig_light_frame_class_init(SavuconfigLightFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(SavuconfigLightFramePrivate));
}
