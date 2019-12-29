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

#include "kovaplusconfig_light_frame.h"
#include "roccat_color_selection_button.h"
#include "gtk_roccat_helper.h"
#include "gdk_roccat_helper.h"
#include <gaminggear/gaminggear_hscale.h>
#include "i18n.h"

#define KOVAPLUSCONFIG_LIGHT_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KOVAPLUSCONFIG_LIGHT_FRAME_TYPE, KovaplusconfigLightFrameClass))
#define IS_KOVAPLUSCONFIG_LIGHT_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KOVAPLUSCONFIG_LIGHT_FRAME_TYPE))
#define KOVAPLUSCONFIG_LIGHT_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KOVAPLUSCONFIG_LIGHT_FRAME_TYPE, KovaplusconfigLightFramePrivate))

typedef struct _KovaplusconfigLightFrameClass KovaplusconfigLightFrameClass;
typedef struct _KovaplusconfigLightFramePrivate KovaplusconfigLightFramePrivate;

struct _KovaplusconfigLightFrame {
	GtkFrame parent;
	KovaplusconfigLightFramePrivate *priv;
};

struct _KovaplusconfigLightFrameClass {
	GtkFrameClass parent_class;
};

struct _KovaplusconfigLightFramePrivate {
	GSList *flags;
	GSList *effects;
	GaminggearHScale *scale;
	RoccatColorSelectionButton *color;
};

G_DEFINE_TYPE(KovaplusconfigLightFrame, kovaplusconfig_light_frame, GTK_TYPE_FRAME);

static gchar const * const value_key = "value";

enum {
	LIGHT_TYPE_OFF = 0,
	LIGHT_TYPE_ON = 1,
	LIGHT_TYPE_BREATHING = 2,
};

static GdkColor const colors[KOVAPLUS_COLOR_NUM] = {
	{0,   4 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 147 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 255 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0,   1 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR,  23 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 254 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0, 140 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR,   1 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 254 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0, 231 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR,  24 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 218 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0, 255 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR,   0 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR,  14 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0, 253 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 101 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR,   0 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
	{0,   0 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR, 255 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR,   7 * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR},
};

static void pack_radio_button(gpointer button, gpointer parent) {
	gtk_box_pack_start(parent, button, TRUE, TRUE, 0);
}

static gint find_value(gconstpointer a, gconstpointer b) {
	guint a_value = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(a), value_key));
	guint b_value = GPOINTER_TO_UINT(b);

	return (gint)a_value - (gint)b_value;
}

static guint get_light_type(KovaplusRmp *rmp) {
	guint light_type_index;
	if (kovaplus_rmp_get_light_switch(rmp) == KOVAPLUS_PROFILE_SETTINGS_LIGHT_SWITCH_ON) {
		if (kovaplus_rmp_get_light_type(rmp) == KOVAPLUS_PROFILE_SETTINGS_LIGHT_TYPE_FULL_LIGHTENED)
			light_type_index = LIGHT_TYPE_ON;
		else
			light_type_index = LIGHT_TYPE_BREATHING;
	} else
		light_type_index = LIGHT_TYPE_OFF;
	return light_type_index;
}

void kovaplusconfig_light_frame_set_from_rmp(KovaplusconfigLightFrame *frame, KovaplusRmp *rmp) {
	KovaplusconfigLightFramePrivate *priv = frame->priv;
	GSList *element;
	guint light_type;

	element = g_slist_find_custom(priv->flags, GUINT_TO_POINTER(kovaplus_rmp_get_color_flag(rmp)), find_value);
	if (element)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(element->data), TRUE);

	light_type = get_light_type(rmp);
	element = g_slist_find_custom(priv->effects, GUINT_TO_POINTER(light_type), find_value);
	if (element)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(element->data), TRUE);

	gaminggear_hscale_set_value(priv->scale, kovaplus_rmp_get_color_change_mode(rmp));
	roccat_color_selection_button_set_palette_index(priv->color, kovaplus_rmp_get_color_options(rmp));
}

static void set_light_type(KovaplusRmp *rmp, guint value) {
	switch (value) {
	case LIGHT_TYPE_OFF:
		kovaplus_rmp_set_light_switch(rmp, KOVAPLUS_PROFILE_SETTINGS_LIGHT_SWITCH_OFF);
		break;
	case LIGHT_TYPE_ON:
		kovaplus_rmp_set_light_switch(rmp, KOVAPLUS_PROFILE_SETTINGS_LIGHT_SWITCH_ON);
		kovaplus_rmp_set_light_type(rmp, KOVAPLUS_PROFILE_SETTINGS_LIGHT_TYPE_FULL_LIGHTENED);
		break;
	case LIGHT_TYPE_BREATHING:
		kovaplus_rmp_set_light_switch(rmp, KOVAPLUS_PROFILE_SETTINGS_LIGHT_SWITCH_ON);
		kovaplus_rmp_set_light_type(rmp, KOVAPLUS_PROFILE_SETTINGS_LIGHT_TYPE_PULSATING);
		break;
	}
}

void kovaplusconfig_light_frame_update_rmp(KovaplusconfigLightFrame *frame, KovaplusRmp *rmp) {
	KovaplusconfigLightFramePrivate *priv = frame->priv;
	GtkWidget *radio;
	guint value;

	kovaplus_rmp_set_color_change_mode(rmp, gaminggear_hscale_get_value(priv->scale));

	radio = gtk_roccat_radio_button_group_get_active(priv->flags);
	value = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(radio), value_key));
	kovaplus_rmp_set_color_flag(rmp, value);

	radio = gtk_roccat_radio_button_group_get_active(priv->effects);
	value = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(radio), value_key));
	set_light_type(rmp, value);

	kovaplus_rmp_set_color_options(rmp, roccat_color_selection_button_get_palette_index(priv->color));
}

GtkWidget *kovaplusconfig_light_frame_new(void) {
	KovaplusconfigLightFrame *frame;

	frame = KOVAPLUSCONFIG_LIGHT_FRAME(g_object_new(KOVAPLUSCONFIG_LIGHT_FRAME_TYPE,
			NULL));

	return GTK_WIDGET(frame);
}

static GSList *flag_radios(void) {
	GtkWidget *radio;

	radio = gtk_radio_button_new_with_label_from_widget(NULL, _("Multicolor cycling"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(KOVAPLUS_RMP_COLOR_FLAG_MULTICOLOR_CYCLING));

	radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio), _("Single color"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(KOVAPLUS_RMP_COLOR_FLAG_SINGLE_COLOR));

	return gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio));
}

static GSList *effect_radios(void) {
	GtkWidget *radio;

	radio = gtk_radio_button_new_with_label_from_widget(NULL, _("Breathing"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(LIGHT_TYPE_BREATHING));

	radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio), _("Fully lighted"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(LIGHT_TYPE_ON));

	radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio), _("All lights off"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(LIGHT_TYPE_OFF));

	return gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio));
}

static GaminggearHScale *scale_new(void) {
	GaminggearHScale *scale;
	guint i;

	scale = GAMINGGEAR_HSCALE(gaminggear_hscale_new_with_range(KOVAPLUS_RMP_COLOR_CHANGE_MODE_5_SEC, KOVAPLUS_RMP_COLOR_CHANGE_MODE_15_SEC, 1.0));
	gaminggear_hscale_set_draw_spin(scale, FALSE);
	for (i = KOVAPLUS_RMP_COLOR_CHANGE_MODE_5_SEC; i <= KOVAPLUS_RMP_COLOR_CHANGE_MODE_15_SEC; ++i)
		gaminggear_hscale_add_mark(scale, i, GTK_POS_BOTTOM, NULL);

	return scale;
}

static void kovaplusconfig_light_frame_init(KovaplusconfigLightFrame *frame) {
	KovaplusconfigLightFramePrivate *priv = KOVAPLUSCONFIG_LIGHT_FRAME_GET_PRIVATE(frame);
	GtkBox *vbox;
	GtkBox *hbox;

	frame->priv = priv;

	hbox = GTK_BOX(gtk_hbox_new(FALSE, 0));
	vbox = GTK_BOX(gtk_vbox_new(FALSE, 0));
	priv->flags = flag_radios();
	priv->effects = effect_radios();
	priv->color = ROCCAT_COLOR_SELECTION_BUTTON(roccat_color_selection_button_new());
	roccat_color_selection_button_set_view_type(priv->color, ROCCAT_COLOR_SELECTION_DIALOG_TYPE_PALETTE);
	roccat_color_selection_button_set_palette_dimensions(priv->color, KOVAPLUS_COLOR_NUM, KOVAPLUS_COLOR_NUM);
	roccat_color_selection_button_set_palette_colors(priv->color, colors, KOVAPLUS_COLOR_NUM);
	priv->scale = scale_new();

	gtk_box_pack_start(hbox, gtk_label_new(_("Effect speed")), FALSE, FALSE, 0);
	gtk_box_pack_start(hbox, GTK_WIDGET(priv->scale), TRUE, TRUE, 0);

	g_slist_foreach(priv->effects, pack_radio_button, vbox);
	gtk_box_pack_start(vbox, gtk_hseparator_new(), TRUE, TRUE, 0);
	g_slist_foreach(priv->flags, pack_radio_button, vbox);
	gtk_box_pack_start(vbox, gtk_hseparator_new(), TRUE, TRUE, 0);
	gtk_box_pack_start(vbox, GTK_WIDGET(hbox), TRUE, TRUE, 0);
	gtk_box_pack_start(vbox, gtk_hseparator_new(), TRUE, TRUE, 0);
	gtk_box_pack_start(vbox, GTK_WIDGET(priv->color), TRUE, TRUE, 0);

	gtk_container_add(GTK_CONTAINER(frame), GTK_WIDGET(vbox));

	gtk_frame_set_label(GTK_FRAME(frame), _("Light"));
}

static void kovaplusconfig_light_frame_class_init(KovaplusconfigLightFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(KovaplusconfigLightFramePrivate));
}
