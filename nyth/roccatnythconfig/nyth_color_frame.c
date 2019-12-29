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

#include "nyth_color_frame.h"
#include "roccat_swarm_color_selection_button.h"
#include "roccat_helper.h"
#include "i18n.h"

#define NYTH_COLOR_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), NYTH_COLOR_FRAME_TYPE, NythColorFrameClass))
#define IS_NYTH_COLOR_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), NYTH_COLOR_FRAME_TYPE))
#define NYTH_COLOR_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), NYTH_COLOR_FRAME_TYPE, NythColorFramePrivate))

typedef struct _NythColorFrameClass NythColorFrameClass;
typedef struct _NythColorFramePrivate NythColorFramePrivate;

struct _NythColorFrame {
	GtkFrame parent;
	NythColorFramePrivate *priv;
};

struct _NythColorFrameClass {
	GtkFrameClass parent_class;
};

struct _NythColorFramePrivate {
	GtkToggleButton *light_on;
	RoccatColorSelectionButton *colors[NYTH_LIGHTS_NUM];
};

G_DEFINE_TYPE(NythColorFrame, nyth_color_frame, GTK_TYPE_FRAME);

static gchar const * const labels[NYTH_LIGHTS_NUM] = {
	N_("Logo"),
	N_("Bottom"),
};

void nyth_color_frame_set_from_profile_data(NythColorFrame *frame, NythProfileData const *profile_data) {
	NythColorFramePrivate *priv = frame->priv;
	NythProfileSettings const *profile_settings = &profile_data->hardware.profile_settings;
	guint i;
	gboolean custom;

	gtk_toggle_button_set_active(priv->light_on, profile_settings->lights_enabled & NYTH_PROFILE_SETTINGS_LIGHTS_ENABLED_MASK);

	for (i = 0; i < NYTH_LIGHTS_NUM; ++i) {
		roccat_color_selection_button_set_roccat_light(priv->colors[i], &profile_settings->lights[i]);
		custom = roccat_get_bit8(profile_settings->lights_enabled, i + NYTH_PROFILE_SETTINGS_LIGHTS_ENABLED_BIT_CUSTOM_COLOR_LOGO);
		roccat_color_selection_button_set_active_type(priv->colors[i], custom ? ROCCAT_COLOR_SELECTION_DIALOG_TYPE_CUSTOM : ROCCAT_COLOR_SELECTION_DIALOG_TYPE_PALETTE);
	}
}

void nyth_color_frame_update_profile_data(NythColorFrame *frame, NythProfileData *profile_data) {
	NythColorFramePrivate *priv = frame->priv;
	NythProfileSettings *profile_settings = &profile_data->hardware.profile_settings;
	guint8 lights_enabled;
	RoccatLight light;
	guint i;

	lights_enabled = gtk_toggle_button_get_active(priv->light_on) ? NYTH_PROFILE_SETTINGS_LIGHTS_ENABLED_ON : NYTH_PROFILE_SETTINGS_LIGHTS_ENABLED_OFF;

	for (i = 0; i < NYTH_LIGHTS_NUM; ++i) {
		roccat_set_bit8(&lights_enabled, i + NYTH_PROFILE_SETTINGS_LIGHTS_ENABLED_BIT_CUSTOM_COLOR_LOGO,
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

GtkWidget *nyth_color_frame_new(void) {
	return GTK_WIDGET(g_object_new(NYTH_COLOR_FRAME_TYPE, NULL));
}

static void nyth_color_frame_init(NythColorFrame *frame) {
	NythColorFramePrivate *priv = NYTH_COLOR_FRAME_GET_PRIVATE(frame);
	GtkTable *table;
	GtkVBox *vbox;
	guint i;

	frame->priv = priv;

	vbox = GTK_VBOX(gtk_vbox_new(FALSE, 0));
	table = GTK_TABLE(gtk_table_new(NYTH_LIGHTS_NUM, 2, FALSE));

	priv->light_on = GTK_TOGGLE_BUTTON(gtk_check_button_new_with_label(_("Lights on")));

	for (i = 0; i < NYTH_LIGHTS_NUM; ++i) {
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

static void nyth_color_frame_class_init(NythColorFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(NythColorFramePrivate));
}
