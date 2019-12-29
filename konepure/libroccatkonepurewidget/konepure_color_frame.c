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

#include "konepure_color_frame.h"
#include "roccat_color_selection_button.h"
#include "gdk_roccat_helper.h"
#include "i18n-lib.h"

#define KONEPURE_COLOR_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KONEPURE_COLOR_FRAME_TYPE, KonepureColorFrameClass))
#define IS_KONEPURE_COLOR_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KONEPURE_COLOR_FRAME_TYPE))
#define KONEPURE_COLOR_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KONEPURE_COLOR_FRAME_TYPE, KonepureColorFramePrivate))

typedef struct _KonepureColorFrameClass KonepureColorFrameClass;
typedef struct _KonepureColorFramePrivate KonepureColorFramePrivate;

struct _KonepureColorFrame {
	GtkFrame parent;
	KonepureColorFramePrivate *priv;
};

struct _KonepureColorFrameClass {
	GtkFrameClass parent_class;
};

struct _KonepureColorFramePrivate {
	GtkToggleButton *use_palette;
	GtkToggleButton *use_custom;
	RoccatColorSelectionButton *color;
	GtkToggleButton *state;
};

/* these are the colors for on screen display, which differ from the device colors */
static GdkColor const colors[KONEPURE_RMP_LIGHT_INFO_COLORS_NUM] = {
	{0, 0x06 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0x90 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0xfd * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0, 0x00 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0x73 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0xff * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0, 0x24 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0x00 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0xff * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0, 0x5c * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0x18 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0xe9 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0, 0x80 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0x17 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0xe5 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0, 0xc6 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0x18 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0xeb * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0, 0xff * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0x05 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0x81 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0, 0xfb * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0x15 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0x00 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0, 0xfc * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0x7c * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0x00 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0, 0xfe * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0xf2 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0x00 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0, 0xce * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0xfb * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0x18 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0, 0x27 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0xfe * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0x03 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0, 0x1c * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0xa7 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0x30 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0, 0x15 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0xea * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0x96 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0, 0x0e * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0xeb * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0xe2 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0, 0x00 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0xc5 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0xfa * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
};

G_DEFINE_TYPE(KonepureColorFrame, konepure_color_frame, GTK_TYPE_FRAME);

static void koneplus_custom_rmp_light_info_to_color(KoneplusRmpLightInfo const *light_info, GdkColor *color) {
	color->pixel = 0;
	color->red = light_info->red * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR;
	color->green = light_info->green * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR;
	color->blue = light_info->blue * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR;
}

static void koneplus_color_to_custom_rmp_light_info(GdkColor const *color, KoneplusRmpLightInfo *light_info) {
	light_info->index = 0;
	light_info->state = 0;
	light_info->red = color->red / GDK_ROCCAT_BYTE_TO_COLOR_FACTOR;
	light_info->green = color->green / GDK_ROCCAT_BYTE_TO_COLOR_FACTOR;
	light_info->blue = color->blue / GDK_ROCCAT_BYTE_TO_COLOR_FACTOR;
}

static void update(KonepureColorFrame *frame) {
	KonepureColorFramePrivate *priv = frame->priv;
	gboolean use_palette = gtk_toggle_button_get_active(priv->use_palette);
	roccat_color_selection_button_set_view_type(priv->color, (use_palette) ? ROCCAT_COLOR_SELECTION_DIALOG_TYPE_PALETTE : ROCCAT_COLOR_SELECTION_DIALOG_TYPE_CUSTOM);
}

void konepure_color_frame_set_from_rmp(KonepureColorFrame *frame, KoneplusRmp *rmp) {
	KonepureColorFramePrivate *priv = frame->priv;
	KoneplusRmpLightInfo *light_info;
	GdkColor color;

	if (konextd_rmp_get_light_chose_type(rmp) == KONEXTD_RMP_LIGHT_CHOSE_TYPE_TABLE)
		gtk_toggle_button_set_active(priv->use_palette, TRUE);
	else
		gtk_toggle_button_set_active(priv->use_custom, TRUE);

	light_info = koneplus_rmp_get_rmp_light_info(rmp, 0);
	roccat_color_selection_button_set_palette_index(priv->color, light_info->index);
	/* custom light info has state always 0, so palette button state is always taken */
	gtk_toggle_button_set_active(priv->state, (light_info->state == KONEPLUS_RMP_LIGHT_INFO_STATE_ON));
	g_free(light_info);

	light_info = konextd_rmp_get_custom_light_info(rmp, 0);
	koneplus_custom_rmp_light_info_to_color(light_info, &color);
	roccat_color_selection_button_set_custom_color(priv->color, &color);
	g_free(light_info);

	update(frame);
}

void konepure_color_frame_update_rmp(KonepureColorFrame *frame, KoneplusRmp *rmp) {
	KonepureColorFramePrivate *priv = frame->priv;
	KoneplusRmpLightInfo light_info;
	KoneplusRmpLightInfo const *standard_light_info;
	guint color_index, state;
	GdkColor color;

	konextd_rmp_set_light_chose_type(rmp, gtk_toggle_button_get_active(priv->use_palette) ? KONEXTD_RMP_LIGHT_CHOSE_TYPE_TABLE : KONEXTD_RMP_LIGHT_CHOSE_TYPE_CUSTOM);

	color_index = roccat_color_selection_button_get_palette_index(priv->color);
	state = gtk_toggle_button_get_active(priv->state) ? KONEPLUS_RMP_LIGHT_INFO_STATE_ON : KONEPLUS_RMP_LIGHT_INFO_STATE_OFF;

	standard_light_info = konepure_rmp_light_info_get_standard(color_index);
	light_info = *standard_light_info;
	light_info.state = state;
	koneplus_rmp_set_rmp_light_info(rmp, 0, &light_info);

	roccat_color_selection_button_get_custom_color(priv->color, &color);
	koneplus_color_to_custom_rmp_light_info(&color, &light_info);
	konextd_rmp_set_custom_light_info(rmp, 0, &light_info);
}

static void use_palette_cb(GtkToggleButton *toggle_button, gpointer user_data) {
	update(KONEPURE_COLOR_FRAME(user_data));
}

GtkWidget *konepure_color_frame_new(void) {
	return GTK_WIDGET(g_object_new(KONEPURE_COLOR_FRAME_TYPE, NULL));
}

static void konepure_color_frame_init(KonepureColorFrame *frame) {
	KonepureColorFramePrivate *priv = KONEPURE_COLOR_FRAME_GET_PRIVATE(frame);
	GtkWidget *hbox;
	GtkWidget *vbox;

	frame->priv = priv;

	hbox = gtk_hbox_new(FALSE, 0);
	/* vbox is used to always keep vertical size of button */
	vbox = gtk_vbox_new(FALSE, 0);

	priv->use_palette = GTK_TOGGLE_BUTTON(gtk_radio_button_new_with_label(NULL, _("Palette")));
	priv->use_custom = GTK_TOGGLE_BUTTON(gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(priv->use_palette), _("Custom")));
	priv->color = ROCCAT_COLOR_SELECTION_BUTTON(roccat_color_selection_button_new());
	roccat_color_selection_button_set_palette_dimensions(priv->color, KONEPURE_RMP_LIGHT_INFO_COLORS_NUM, 0);
	roccat_color_selection_button_set_palette_colors(priv->color, colors, KONEPURE_RMP_LIGHT_INFO_COLORS_NUM);
	priv->state = GTK_TOGGLE_BUTTON(gtk_check_button_new());

	g_signal_connect(G_OBJECT(priv->use_palette), "toggled", G_CALLBACK(use_palette_cb), frame);

	gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(priv->color), TRUE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(priv->state), TRUE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(priv->use_palette), TRUE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(priv->use_custom), TRUE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), gtk_hseparator_new(), TRUE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, FALSE, 0);

	gtk_container_add(GTK_CONTAINER(frame), vbox);
	gtk_frame_set_label(GTK_FRAME(frame), _("Color"));
}

static void konepure_color_frame_class_init(KonepureColorFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(KonepureColorFramePrivate));
}
