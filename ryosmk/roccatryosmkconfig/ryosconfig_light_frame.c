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

#include "ryosconfig_light_frame.h"
#include "roccat_light_selector.h"
#include "ryosconfig_pro_dimness_selector.h"
#include "roccat_multiwidget.h"
#include "i18n.h"

#define RYOSCONFIG_LIGHT_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), RYOSCONFIG_LIGHT_FRAME_TYPE, RyosconfigLightFrameClass))
#define IS_RYOSCONFIG_LIGHT_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), RYOSCONFIG_LIGHT_FRAME_TYPE))
#define RYOSCONFIG_LIGHT_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), RYOSCONFIG_LIGHT_FRAME_TYPE, RyosconfigLightFramePrivate))

typedef struct _RyosconfigLightFrameClass RyosconfigLightFrameClass;
typedef struct _RyosconfigLightFramePrivate RyosconfigLightFramePrivate;

struct _RyosconfigLightFrame {
	GtkFrame parent;
	RyosconfigLightFramePrivate *priv;
};

struct _RyosconfigLightFrameClass {
	GtkFrameClass parent_class;
};

struct _RyosconfigLightFramePrivate {
	RoccatMultiwidget *multiwidget;
	RoccatLightSelector *light_selector;
	RyosconfigProDimnessSelector *pro_dimness_selector;
	GtkLabel *nothing;
	GtkFileChooser *script;
};

G_DEFINE_TYPE(RyosconfigLightFrame, ryosconfig_light_frame, GTK_TYPE_FRAME);

static void add_file_filter_to_file_chooser(GtkFileChooser *chooser) {
	GtkFileFilter *filter;
	filter = gtk_file_filter_new();
	gtk_file_filter_set_name(filter, _("Lua script file"));
	gtk_file_filter_add_pattern(filter, "*.lua");
	gtk_file_filter_add_pattern(filter, "*.lc");
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(chooser), filter);

	filter = gtk_file_filter_new();
	gtk_file_filter_set_name(filter, _("All files"));
	gtk_file_filter_add_pattern(filter, "*");
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(chooser), filter);
}

GtkWidget *ryosconfig_light_frame_new(void) {
	return GTK_WIDGET(g_object_new(RYOSCONFIG_LIGHT_FRAME_TYPE, NULL));
}

static void ryosconfig_light_frame_init(RyosconfigLightFrame *light_frame) {
	RyosconfigLightFramePrivate *priv = RYOSCONFIG_LIGHT_FRAME_GET_PRIVATE(light_frame);
	light_frame->priv = priv;
	guint rows;

	priv->multiwidget = ROCCAT_MULTIWIDGET(roccat_multiwidget_new());
	priv->light_selector = ROCCAT_LIGHT_SELECTOR(roccat_light_selector_new_with_range(RYOS_LIGHT_BRIGHTNESS_MIN, RYOS_LIGHT_BRIGHTNESS_MAX, 1));
	priv->nothing = GTK_LABEL(gtk_label_new(_("This model does not support light settings")));
	priv->pro_dimness_selector = RYOSCONFIG_PRO_DIMNESS_SELECTOR(ryosconfig_pro_dimness_selector_new());
	priv->script = GTK_FILE_CHOOSER(gtk_file_chooser_button_new(_("Select script"), GTK_FILE_CHOOSER_ACTION_OPEN));

	gtk_label_set_line_wrap(GTK_LABEL(priv->nothing), TRUE);
	add_file_filter_to_file_chooser(priv->script);

	roccat_multiwidget_add(priv->multiwidget, GTK_WIDGET(priv->light_selector));
	roccat_multiwidget_add(priv->multiwidget, GTK_WIDGET(priv->nothing));

	gtk_container_add(GTK_CONTAINER(light_frame), GTK_WIDGET(priv->multiwidget));

#if (GTK_CHECK_VERSION(2, 22, 0))
	gtk_table_get_size(GTK_TABLE(priv->light_selector), &rows, NULL);
#else
	g_object_get(G_OBJECT(priv->light_selector), "n-rows", &rows, NULL);
#endif

	gtk_table_attach(GTK_TABLE(priv->light_selector), gtk_label_new(_("Function")), 0, 1, rows, rows + 1, GTK_SHRINK, GTK_EXPAND, 0, 0);
	gtk_table_attach(GTK_TABLE(priv->light_selector), GTK_WIDGET(priv->pro_dimness_selector), 1, 2, rows, rows + 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);
	gtk_table_attach(GTK_TABLE(priv->light_selector), gtk_label_new(_("Script FX")), 0, 1, rows + 1, rows + 2, GTK_SHRINK, GTK_EXPAND, 0, 0);
	gtk_table_attach(GTK_TABLE(priv->light_selector), GTK_WIDGET(priv->script), 1, 2, rows + 1, rows + 2, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);

	gtk_frame_set_label(GTK_FRAME(light_frame), _("Light"));
}

static void ryosconfig_light_frame_class_init(RyosconfigLightFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(RyosconfigLightFramePrivate));
}

void ryosconfig_light_frame_set_device_type(RyosconfigLightFrame *light_frame, RoccatDevice const *device) {
	RyosconfigLightFramePrivate *priv = light_frame->priv;
	switch (gaminggear_device_get_product_id(GAMINGGEAR_DEVICE(device))) {
	case USB_DEVICE_ID_ROCCAT_RYOS_MK:
		roccat_multiwidget_show(priv->multiwidget, GTK_WIDGET(priv->nothing));
		break;
	case USB_DEVICE_ID_ROCCAT_RYOS_MK_GLOW:
	case USB_DEVICE_ID_ROCCAT_RYOS_MK_PRO:
		roccat_multiwidget_show(priv->multiwidget, GTK_WIDGET(priv->light_selector));
		break;
	default:
		break;
	}
	ryosconfig_pro_dimness_selector_set_device_type(priv->pro_dimness_selector, device);
}

void ryosconfig_light_frame_set_brightness(RyosconfigLightFrame *light_frame, guint new_brightness) {
	roccat_light_selector_set_brightness(light_frame->priv->light_selector, new_brightness);
}

void ryosconfig_light_frame_set_from_profile_data(RyosconfigLightFrame *light_frame, RyosProfileData const *profile_data, RoccatKeyFile *config) {
	RyosconfigLightFramePrivate *priv = light_frame->priv;
	roccat_light_selector_set_brightness(priv->light_selector, profile_data->hardware.light.brightness);
	roccat_light_selector_set_dimness(priv->light_selector, profile_data->hardware.light.dimness);
	roccat_light_selector_set_timeout(priv->light_selector, profile_data->hardware.light.timeout);
	ryosconfig_pro_dimness_selector_set_from_profile_data(priv->pro_dimness_selector, profile_data);
	gchar *path;

	if (strlen(profile_data->eventhandler.effect_script) > 0)
		gtk_file_chooser_set_filename(priv->script, profile_data->eventhandler.effect_script);
	else if (config) {
		path = ryos_configuration_get_script_path(config);
		gtk_file_chooser_set_current_folder(priv->script, path);
		g_free(path);
	}
}

void ryosconfig_light_frame_update_profile_data(RyosconfigLightFrame *light_frame, RyosProfileData *profile_data, RoccatKeyFile *config) {
	RyosconfigLightFramePrivate *priv = light_frame->priv;
	guint8 brightness = roccat_light_selector_get_brightness(priv->light_selector);
	guint8 dimness = roccat_light_selector_get_dimness(priv->light_selector);
	guint8 timeout = roccat_light_selector_get_timeout(priv->light_selector);
	gchar *filename;
	gchar *path;

	if (profile_data->hardware.light.brightness != brightness) {
		profile_data->hardware.light.brightness = brightness;
		profile_data->hardware.modified_light = TRUE;
	}

	if (profile_data->hardware.light.dimness != dimness) {
		profile_data->hardware.light.dimness = dimness;
		profile_data->hardware.modified_light = TRUE;
	}

	if (profile_data->hardware.light.timeout != timeout) {
		profile_data->hardware.light.timeout = timeout;
		profile_data->hardware.modified_light = TRUE;
	}

	ryosconfig_pro_dimness_selector_update_profile_data(priv->pro_dimness_selector, profile_data);

	filename = gtk_file_chooser_get_filename(priv->script);
	if (filename) {
		if (config) {
			path = g_path_get_dirname(filename);
			ryos_configuration_set_script_path(config, path);
			g_free(path);
		}
		ryos_profile_data_eventhandler_set_effect_script_name(&profile_data->eventhandler, filename);
		g_free(filename);
	}
}
