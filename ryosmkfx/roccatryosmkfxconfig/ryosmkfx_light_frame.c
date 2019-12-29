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

#include "ryosmkfx_light_frame.h"
#include "roccat_light_selector.h"
#include "ryosmkfx_dimness_selector.h"
#include "i18n.h"

#define RYOSMKFX_LIGHT_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), RYOSMKFX_LIGHT_FRAME_TYPE, RyosmkfxLightFrameClass))
#define IS_RYOSMKFX_LIGHT_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), RYOSMKFX_LIGHT_FRAME_TYPE))
#define RYOSMKFX_LIGHT_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), RYOSMKFX_LIGHT_FRAME_TYPE, RyosmkfxLightFramePrivate))

typedef struct _RyosmkfxLightFrameClass RyosmkfxLightFrameClass;
typedef struct _RyosmkfxLightFramePrivate RyosmkfxLightFramePrivate;

struct _RyosmkfxLightFrame {
	GtkFrame parent;
	RyosmkfxLightFramePrivate *priv;
};

struct _RyosmkfxLightFrameClass {
	GtkFrameClass parent_class;
};

struct _RyosmkfxLightFramePrivate {
	RoccatLightSelector *light_selector;
	RyosmkfxDimnessSelector *dimness_selector;
};

G_DEFINE_TYPE(RyosmkfxLightFrame, ryosmkfx_light_frame, GTK_TYPE_FRAME);

GtkWidget *ryosmkfx_light_frame_new(void) {
	return GTK_WIDGET(g_object_new(RYOSMKFX_LIGHT_FRAME_TYPE, NULL));
}

static void ryosmkfx_light_frame_init(RyosmkfxLightFrame *light_frame) {
	RyosmkfxLightFramePrivate *priv = RYOSMKFX_LIGHT_FRAME_GET_PRIVATE(light_frame);
	light_frame->priv = priv;
	guint rows;

	priv->light_selector = ROCCAT_LIGHT_SELECTOR(roccat_light_selector_new_with_range(RYOS_LIGHT_BRIGHTNESS_MIN, RYOS_LIGHT_BRIGHTNESS_MAX, 1));
	priv->dimness_selector = RYOSMKFX_DIMNESS_SELECTOR(ryosmkfx_dimness_selector_new());

	gtk_container_add(GTK_CONTAINER(light_frame), GTK_WIDGET(priv->light_selector));

#if (GTK_CHECK_VERSION(2, 22, 0))
	gtk_table_get_size(GTK_TABLE(priv->light_selector), &rows, NULL);
#else
	g_object_get(G_OBJECT(priv->light_selector), "n-rows", &rows, NULL);
#endif

	gtk_table_attach(GTK_TABLE(priv->light_selector), gtk_label_new(_("Function")), 0, 1, rows, rows + 1, GTK_SHRINK, GTK_EXPAND, 0, 0);
	gtk_table_attach(GTK_TABLE(priv->light_selector), GTK_WIDGET(priv->dimness_selector), 1, 2, rows, rows + 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);

	gtk_frame_set_label(GTK_FRAME(light_frame), _("Light"));
}

static void ryosmkfx_light_frame_class_init(RyosmkfxLightFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(RyosmkfxLightFramePrivate));
}

void ryosmkfx_light_frame_set_brightness(RyosmkfxLightFrame *light_frame, guint new_brightness) {
	roccat_light_selector_set_brightness(light_frame->priv->light_selector, new_brightness);
}

void ryosmkfx_light_frame_set_from_profile_data(RyosmkfxLightFrame *light_frame, RyosmkfxProfileData const *profile_data) {
	RyosmkfxLightFramePrivate *priv = light_frame->priv;
	RyosmkfxLight const *light = &profile_data->hardware.light;

	roccat_light_selector_set_brightness(priv->light_selector, light->brightness);
	roccat_light_selector_set_dimness(priv->light_selector, light->dimness);
	roccat_light_selector_set_timeout(priv->light_selector, light->timeout);

	ryosmkfx_dimness_selector_set_from_profile_data(priv->dimness_selector, profile_data);
}

void ryosmkfx_light_frame_update_profile_data(RyosmkfxLightFrame *light_frame, RyosmkfxProfileData *profile_data) {
	RyosmkfxLightFramePrivate *priv = light_frame->priv;
	RyosmkfxLight *light = &profile_data->hardware.light;
	guint brightness;
	guint dimness;
	guint timeout;

	brightness = roccat_light_selector_get_brightness(priv->light_selector);
	dimness = roccat_light_selector_get_dimness(priv->light_selector);
	timeout = roccat_light_selector_get_timeout(priv->light_selector);

	if (light->brightness != brightness || light->dimness != dimness || light->timeout != timeout) {
		light->brightness = brightness;
		light->dimness = dimness;
		light->timeout = timeout;
		profile_data->hardware.modified_light = TRUE;
	}

	ryosmkfx_dimness_selector_update_profile_data(priv->dimness_selector, profile_data);
}
