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

#include "kiro_color_frame.h"
#include "roccat_swarm_color_selection_button.h"
#include "roccat_helper.h"
#include "i18n.h"

#define KIRO_COLOR_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KIRO_COLOR_FRAME_TYPE, KiroColorFrameClass))
#define IS_KIRO_COLOR_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KIRO_COLOR_FRAME_TYPE))
#define KIRO_COLOR_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KIRO_COLOR_FRAME_TYPE, KiroColorFramePrivate))

typedef struct _KiroColorFrameClass KiroColorFrameClass;
typedef struct _KiroColorFramePrivate KiroColorFramePrivate;

struct _KiroColorFrame {
	GtkFrame parent;
	KiroColorFramePrivate *priv;
};

struct _KiroColorFrameClass {
	GtkFrameClass parent_class;
};

struct _KiroColorFramePrivate {
	GtkToggleButton *light_on;
	RoccatColorSelectionButton *color;
};

G_DEFINE_TYPE(KiroColorFrame, kiro_color_frame, GTK_TYPE_FRAME);

void kiro_color_frame_set_from_profile_data(KiroColorFrame *frame, KiroProfileData const *profile_data) {
	KiroColorFramePrivate *priv = frame->priv;
	KiroProfile const *profile = &profile_data->hardware.profile;
	gboolean custom;

	gtk_toggle_button_set_active(priv->light_on, profile->lights_enabled & KIRO_PROFILE_LIGHTS_ENABLED_MASK);

	roccat_color_selection_button_set_roccat_light(priv->color, &profile->light);
	custom = roccat_get_bit8(profile->lights_enabled, KIRO_PROFILE_LIGHTS_ENABLED_BIT_CUSTOM_COLOR);
	roccat_color_selection_button_set_active_type(priv->color, custom ? ROCCAT_COLOR_SELECTION_DIALOG_TYPE_CUSTOM : ROCCAT_COLOR_SELECTION_DIALOG_TYPE_PALETTE);
}

void kiro_color_frame_update_profile_data(KiroColorFrame *frame, KiroProfileData *profile_data) {
	KiroColorFramePrivate *priv = frame->priv;
	KiroProfile *profile = &profile_data->hardware.profile;
	guint8 lights_enabled;
	RoccatLight light;

	lights_enabled = gtk_toggle_button_get_active(priv->light_on) ? KIRO_PROFILE_LIGHTS_ENABLED_ON : KIRO_PROFILE_LIGHTS_ENABLED_OFF;

	roccat_set_bit8(&lights_enabled, KIRO_PROFILE_LIGHTS_ENABLED_BIT_CUSTOM_COLOR,
			roccat_color_selection_button_get_active_type(priv->color) == ROCCAT_COLOR_SELECTION_DIALOG_TYPE_CUSTOM);

	roccat_color_selection_button_get_roccat_light(priv->color, &light);

	if (!roccat_light_equal(&light, &profile->light)) {
		roccat_light_copy(&profile->light, &light);
		profile_data->hardware.modified_profile = TRUE;
	}

	if (lights_enabled != profile->lights_enabled) {
		profile->lights_enabled = lights_enabled;
		profile_data->hardware.modified_profile = TRUE;
	}
}

GtkWidget *kiro_color_frame_new(void) {
	return GTK_WIDGET(g_object_new(KIRO_COLOR_FRAME_TYPE, NULL));
}

static void kiro_color_frame_init(KiroColorFrame *frame) {
	KiroColorFramePrivate *priv = KIRO_COLOR_FRAME_GET_PRIVATE(frame);
	GtkVBox *vbox;

	frame->priv = priv;

	vbox = GTK_VBOX(gtk_vbox_new(FALSE, 0));

	priv->light_on = GTK_TOGGLE_BUTTON(gtk_check_button_new_with_label(_("Lights on")));

	priv->color = ROCCAT_COLOR_SELECTION_BUTTON(roccat_swarm_color_selection_button_new());

	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(priv->light_on), TRUE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), gtk_hseparator_new(), TRUE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(priv->color), TRUE, TRUE, 0);

	gtk_container_add(GTK_CONTAINER(frame), GTK_WIDGET(vbox));
	gtk_frame_set_label(GTK_FRAME(frame), _("Light"));
}

static void kiro_color_frame_class_init(KiroColorFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(KiroColorFramePrivate));
}
