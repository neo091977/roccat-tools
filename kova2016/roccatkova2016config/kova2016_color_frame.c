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

#include "kova2016_color_frame.h"
#include "roccat_swarm_color_selection_button.h"
#include "roccat_helper.h"
#include "i18n.h"

#define KOVA2016_COLOR_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KOVA2016_COLOR_FRAME_TYPE, Kova2016ColorFrameClass))
#define IS_KOVA2016_COLOR_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KOVA2016_COLOR_FRAME_TYPE))
#define KOVA2016_COLOR_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KOVA2016_COLOR_FRAME_TYPE, Kova2016ColorFramePrivate))

typedef struct _Kova2016ColorFrameClass Kova2016ColorFrameClass;
typedef struct _Kova2016ColorFramePrivate Kova2016ColorFramePrivate;

struct _Kova2016ColorFrame {
	GtkFrame parent;
	Kova2016ColorFramePrivate *priv;
};

struct _Kova2016ColorFrameClass {
	GtkFrameClass parent_class;
};

struct _Kova2016ColorFramePrivate {
	GtkToggleButton *light_on;
	RoccatColorSelectionButton *colors[KOVA2016_LIGHTS_NUM];
};

G_DEFINE_TYPE(Kova2016ColorFrame, kova2016_color_frame, GTK_TYPE_FRAME);

static gchar const * const labels[KOVA2016_LIGHTS_NUM] = {
	N_("Pipe"),
	N_("Wheel"),
};

void kova2016_color_frame_set_from_profile_data(Kova2016ColorFrame *frame, Kova2016ProfileData const *profile_data) {
	Kova2016ColorFramePrivate *priv = frame->priv;
	Kova2016ProfileSettings const *profile_settings = &profile_data->hardware.profile_settings;
	guint i;
	gboolean custom;

	gtk_toggle_button_set_active(priv->light_on, profile_settings->lights_enabled & KOVA2016_PROFILE_SETTINGS_LIGHTS_ENABLED_MASK);

	for (i = 0; i < KOVA2016_LIGHTS_NUM; ++i) {
		roccat_color_selection_button_set_roccat_light(priv->colors[i], &profile_settings->lights[i]);
		custom = roccat_get_bit8(profile_settings->lights_enabled, i + KOVA2016_PROFILE_SETTINGS_LIGHTS_ENABLED_BIT_CUSTOM_COLOR_PIPE);
		roccat_color_selection_button_set_active_type(priv->colors[i], custom ? ROCCAT_COLOR_SELECTION_DIALOG_TYPE_CUSTOM : ROCCAT_COLOR_SELECTION_DIALOG_TYPE_PALETTE);
	}
}

void kova2016_color_frame_update_profile_data(Kova2016ColorFrame *frame, Kova2016ProfileData *profile_data) {
	Kova2016ColorFramePrivate *priv = frame->priv;
	Kova2016ProfileSettings *profile_settings = &profile_data->hardware.profile_settings;
	guint8 lights_enabled;
	RoccatLight light;
	guint i;

	lights_enabled = gtk_toggle_button_get_active(priv->light_on) ? KOVA2016_PROFILE_SETTINGS_LIGHTS_ENABLED_ON : KOVA2016_PROFILE_SETTINGS_LIGHTS_ENABLED_OFF;

	for (i = 0; i < KOVA2016_LIGHTS_NUM; ++i) {
		roccat_set_bit8(&lights_enabled, i + KOVA2016_PROFILE_SETTINGS_LIGHTS_ENABLED_BIT_CUSTOM_COLOR_PIPE,
				roccat_color_selection_button_get_active_type(priv->colors[i]) == ROCCAT_COLOR_SELECTION_DIALOG_TYPE_CUSTOM);

		roccat_color_selection_button_get_roccat_light(priv->colors[i], &light);

		if (!roccat_light_equal(&light, &profile_settings->lights[i])) {
			roccat_light_copy(&profile_settings->lights[i], &light);
			profile_data->hardware.modified_profile_settings = TRUE;
		}
	}

	if (lights_enabled != profile_settings->lights_enabled) {
		profile_settings->lights_enabled = lights_enabled;
		profile_data->hardware.modified_profile_settings = TRUE;
	}
}

GtkWidget *kova2016_color_frame_new(void) {
	return GTK_WIDGET(g_object_new(KOVA2016_COLOR_FRAME_TYPE, NULL));
}

static void kova2016_color_frame_init(Kova2016ColorFrame *frame) {
	Kova2016ColorFramePrivate *priv = KOVA2016_COLOR_FRAME_GET_PRIVATE(frame);
	GtkTable *table;
	GtkVBox *vbox;
	guint i;

	frame->priv = priv;

	vbox = GTK_VBOX(gtk_vbox_new(FALSE, 0));
	table = GTK_TABLE(gtk_table_new(KOVA2016_LIGHTS_NUM, 2, FALSE));

	priv->light_on = GTK_TOGGLE_BUTTON(gtk_check_button_new_with_label(_("Lights on")));

	for (i = 0; i < KOVA2016_LIGHTS_NUM; ++i) {
		gtk_table_attach(table, gtk_label_new(_N(labels[i])), 0, 1, 0 + i, 1 + i, GTK_EXPAND, GTK_EXPAND, 0, 0);

		priv->colors[i] = ROCCAT_COLOR_SELECTION_BUTTON(roccat_swarm_color_selection_button_new());
		gtk_table_attach(table, GTK_WIDGET(priv->colors[i]), 1, 2, 0 + i, 1 + i, GTK_EXPAND, GTK_EXPAND, 0, 0);
	}

	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(priv->light_on), TRUE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), gtk_hseparator_new(), TRUE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(table), TRUE, TRUE, 0);

	gtk_container_add(GTK_CONTAINER(frame), GTK_WIDGET(vbox));
	gtk_frame_set_label(GTK_FRAME(frame), _("Light"));
}

static void kova2016_color_frame_class_init(Kova2016ColorFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(Kova2016ColorFramePrivate));
}
