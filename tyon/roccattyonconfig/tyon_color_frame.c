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

#include "tyon_color_frame.h"
#include "roccat_color_selection_button.h"
#include "gdk_roccat_helper.h"
#include "i18n.h"

#define TYON_COLOR_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), TYON_COLOR_FRAME_TYPE, TyonColorFrameClass))
#define IS_TYON_COLOR_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), TYON_COLOR_FRAME_TYPE))
#define TYON_COLOR_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), TYON_COLOR_FRAME_TYPE, TyonColorFramePrivate))

typedef struct _TyonColorFrameClass TyonColorFrameClass;
typedef struct _TyonColorFramePrivate TyonColorFramePrivate;

struct _TyonColorFrame {
	GtkFrame parent;
	TyonColorFramePrivate *priv;
};

struct _TyonColorFrameClass {
	GtkFrameClass parent_class;
};

struct _TyonColorFramePrivate {
	GtkToggleButton *use_palette;
	GtkToggleButton *use_custom;
	GtkToggleButton *buttons[TYON_LIGHTS_NUM];
	GtkToggleButton *use_color_for_all;
	RoccatColorSelectionButton *colors[TYON_LIGHTS_NUM];
};

G_DEFINE_TYPE(TyonColorFrame, tyon_color_frame, GTK_TYPE_FRAME);

static gchar const * const labels[TYON_LIGHTS_NUM] = {
	N_("Wheel"),
	N_("Bottom"),
};

/* These are values for screen, atm identical to device values */
static GdkColor const screen_colors[TYON_RMP_LIGHT_INFO_COLORS_NUM] = {
	{0, 0x05 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0x90 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0xfe * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0, 0x00 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0x71 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0xff * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0, 0x00 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0x00 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0xff * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0, 0x5c * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0x18 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0xe6 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0, 0x81 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0x18 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0xe6 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0, 0xc5 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0x18 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0xe6 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0, 0xf8 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0x04 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0x7c * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0, 0xff * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0x00 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0x00 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0, 0xf7 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0x79 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0x00 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0, 0xe7 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0xdc * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0x00 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0, 0xc2 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0xf2 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0x08 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0, 0x00 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0xff * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0x00 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0, 0x18 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0xa6 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0x2a * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0, 0x13 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0xec * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0x96 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0, 0x0d * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0xe2 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0xd9 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0, 0x00 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0xbe * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 0xf4 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
};

static void tyon_rmp_light_info_to_color(TyonRmpLightInfo const *light_info, GdkColor *color) {
	color->pixel = 0;
	color->red = light_info->red * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR;
	color->green = light_info->green * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR;
	color->blue = light_info->blue * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR;
}

static void update(TyonColorFrame *frame) {
	TyonColorFramePrivate *priv = frame->priv;
	gboolean use_palette = gtk_toggle_button_get_active(priv->use_palette);
	guint i;

	for (i = 0; i < TYON_LIGHTS_NUM; ++i)
		roccat_color_selection_button_set_view_type(priv->colors[i], (use_palette) ? ROCCAT_COLOR_SELECTION_DIALOG_TYPE_PALETTE : ROCCAT_COLOR_SELECTION_DIALOG_TYPE_CUSTOM);
}

void tyon_color_frame_set_from_rmp(TyonColorFrame *frame, TyonRmp *rmp) {
	TyonColorFramePrivate *priv = frame->priv;
	TyonRmpLightInfo *light_info;
	GdkColor color;
	guint i;

	if (tyon_rmp_get_light_chose_type(rmp) == TYON_RMP_LIGHT_CHOSE_TYPE_PALETTE)
		gtk_toggle_button_set_active(priv->use_palette, TRUE);
	else
		gtk_toggle_button_set_active(priv->use_custom, TRUE);

	gtk_toggle_button_set_active(priv->use_color_for_all, tyon_rmp_get_use_color_for_all(rmp));

	for (i = 0; i < TYON_LIGHTS_NUM; ++i) {
		light_info = tyon_rmp_get_custom_light_info(rmp, i);
		tyon_rmp_light_info_to_color(light_info, &color);
		roccat_color_selection_button_set_custom_color(priv->colors[i], &color);
		gtk_toggle_button_set_active(priv->buttons[i], (light_info->state == TYON_RMP_LIGHT_INFO_STATE_ON) ? TRUE : FALSE);
		g_free(light_info);

		light_info = tyon_rmp_get_rmp_light_info(rmp, i);
		roccat_color_selection_button_set_palette_index(priv->colors[i], light_info->index);
		g_free(light_info);
	}

	update(frame);
}

void tyon_color_frame_update_rmp(TyonColorFrame *frame, TyonRmp *rmp) {
	TyonColorFramePrivate *priv = frame->priv;
	TyonRmpLightInfo light_info;
	GdkColor color;
	guint i;

	tyon_rmp_set_light_chose_type(rmp, gtk_toggle_button_get_active(priv->use_palette) ? TYON_RMP_LIGHT_CHOSE_TYPE_PALETTE : TYON_RMP_LIGHT_CHOSE_TYPE_CUSTOM);
	tyon_rmp_set_use_color_for_all(rmp, gtk_toggle_button_get_active(priv->use_color_for_all));

	for (i = 0; i < TYON_LIGHTS_NUM; ++i) {
		roccat_color_selection_button_get_custom_color(priv->colors[i], &color);
		light_info.index = 0; // FIXME check
		light_info.state = gtk_toggle_button_get_active(priv->buttons[i]);
		light_info.red = color.red / GDK_ROCCAT_BYTE_TO_COLOR_FACTOR;
		light_info.green = color.green / GDK_ROCCAT_BYTE_TO_COLOR_FACTOR;
		light_info.blue = color.blue / GDK_ROCCAT_BYTE_TO_COLOR_FACTOR;
		light_info.null = 0;
		tyon_rmp_set_custom_light_info(rmp, i, &light_info);

		light_info = *tyon_rmp_light_info_get_standard(roccat_color_selection_button_get_palette_index(priv->colors[i]));
		tyon_rmp_set_rmp_light_info(rmp, i, &light_info);
	}
}

GtkWidget *tyon_color_frame_new(void) {
	return GTK_WIDGET(g_object_new(TYON_COLOR_FRAME_TYPE, NULL));
}

static void use_palette_cb(GtkToggleButton *toggle_button, gpointer user_data) {
	update(TYON_COLOR_FRAME(user_data));
}

static void use_color_for_all_cb(GtkToggleButton *toggle_button, gpointer user_data) {
	//TyonColorFrame *frame = TYON_COLOR_FRAME(user_data);
	// FIXME do what?
}

static void color_cb(RoccatColorSelectionButton *palette_button, gpointer user_data) {
	TyonColorFrame *frame = TYON_COLOR_FRAME(user_data);
	TyonColorFramePrivate *priv = frame->priv;
	gint index;
	GdkColor color;
	guint i;

	if (gtk_toggle_button_get_active(priv->use_color_for_all)) {
		// TODO gets called again when changing other light
		if (gtk_toggle_button_get_active(priv->use_palette)) {
			index = roccat_color_selection_button_get_palette_index(palette_button);
			for (i = 0; i < TYON_LIGHTS_NUM; ++i)
				roccat_color_selection_button_set_palette_index(priv->colors[i], index);
		} else {
			roccat_color_selection_button_get_custom_color(palette_button, &color);
			for (i = 0; i < TYON_LIGHTS_NUM; ++i)
				roccat_color_selection_button_set_custom_color(priv->colors[i], &color);
		}
	}
}

static void tyon_color_frame_init(TyonColorFrame *frame) {
	TyonColorFramePrivate *priv = TYON_COLOR_FRAME_GET_PRIVATE(frame);
	GtkTable *table;
	GtkVBox *vbox;
	guint i;

	frame->priv = priv;

	vbox = GTK_VBOX(gtk_vbox_new(FALSE, 0));
	table = GTK_TABLE(gtk_table_new(TYON_LIGHTS_NUM, 2, FALSE));

	priv->use_palette = GTK_TOGGLE_BUTTON(gtk_radio_button_new_with_label(NULL, _("Palette")));
	g_signal_connect(G_OBJECT(priv->use_palette), "toggled", G_CALLBACK(use_palette_cb), frame);
	priv->use_custom = GTK_TOGGLE_BUTTON(gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(priv->use_palette), _("Custom")));

	for (i = 0; i < TYON_LIGHTS_NUM; ++i) {
		priv->colors[i] = ROCCAT_COLOR_SELECTION_BUTTON(roccat_color_selection_button_new());
		roccat_color_selection_button_set_palette_dimensions(priv->colors[i], TYON_RMP_LIGHT_INFO_COLORS_NUM, 0);
		roccat_color_selection_button_set_palette_colors(priv->colors[i], screen_colors, TYON_RMP_LIGHT_INFO_COLORS_NUM);
		gtk_table_attach(table, GTK_WIDGET(priv->colors[i]), 0, 1, 0 + i, 1 + i, GTK_EXPAND, GTK_EXPAND, 0, 0);
		g_signal_connect(G_OBJECT(priv->colors[i]), "value-changed", G_CALLBACK(color_cb), frame);

		priv->buttons[i] = GTK_TOGGLE_BUTTON(gtk_check_button_new_with_label(_N(labels[i])));
		gtk_table_attach(table, GTK_WIDGET(priv->buttons[i]), 1, 2, 0 + i, 1 + i, GTK_EXPAND, GTK_EXPAND, 0, 0);
	}

	priv->use_color_for_all = GTK_TOGGLE_BUTTON(gtk_check_button_new_with_label(_("Use color for all")));
	g_signal_connect(G_OBJECT(priv->use_color_for_all), "toggled", G_CALLBACK(use_color_for_all_cb), frame);

	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(priv->use_palette), TRUE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(priv->use_custom), TRUE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), gtk_hseparator_new(), TRUE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(table), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(priv->use_color_for_all), TRUE, FALSE, 0);

	gtk_container_add(GTK_CONTAINER(frame), GTK_WIDGET(vbox));
	gtk_frame_set_label(GTK_FRAME(frame), _("Light"));
}

static void tyon_color_frame_class_init(TyonColorFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(TyonColorFramePrivate));
}
