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

#include "roccat_sensitivity_frame.h"
#include "i18n-lib.h"
#include <gaminggear/gaminggear_xy_scales.h>

#define ROCCAT_SENSITIVITY_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ROCCAT_SENSITIVITY_FRAME_TYPE, RoccatSensitivityFrameClass))
#define IS_ROCCAT_SENSITIVITY_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ROCCAT_SENSITIVITY_FRAME_TYPE))
#define ROCCAT_SENSITIVITY_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ROCCAT_SENSITIVITY_FRAME_TYPE, RoccatSensitivityFramePrivate))

typedef struct _RoccatSensitivityFrameClass RoccatSensitivityFrameClass;
typedef struct _RoccatSensitivityFramePrivate RoccatSensitivityFramePrivate;

struct _RoccatSensitivityFrame {
	GtkFrame parent;
	RoccatSensitivityFramePrivate *priv;
};

struct _RoccatSensitivityFrameClass {
	GtkFrameClass parent_class;
};


struct _RoccatSensitivityFramePrivate {
	GaminggearXyScales *scale;
};

G_DEFINE_TYPE(RoccatSensitivityFrame, roccat_sensitivity_frame, GTK_TYPE_FRAME);

GtkWidget *roccat_sensitivity_frame_new(void) {
	return GTK_WIDGET(g_object_new(ROCCAT_SENSITIVITY_FRAME_TYPE, NULL));
}

static void roccat_sensitivity_frame_init(RoccatSensitivityFrame *frame) {
	frame->priv = ROCCAT_SENSITIVITY_FRAME_GET_PRIVATE(frame);
	frame->priv->scale = GAMINGGEAR_XY_SCALES(gaminggear_xy_scales_new(
			(gdouble)roccat_sensitivity_from_bin_to_value(ROCCAT_SENSITIVITY_MIN),
			(gdouble)roccat_sensitivity_from_bin_to_value(ROCCAT_SENSITIVITY_MAX),
			(gdouble)1.0));
	gtk_container_add(GTK_CONTAINER(frame), GTK_WIDGET(frame->priv->scale));
	gtk_frame_set_label(GTK_FRAME(frame), _("Sensitivity"));
}

static void roccat_sensitivity_frame_class_init(RoccatSensitivityFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(RoccatSensitivityFramePrivate));
}

RoccatSensitivity roccat_sensitivity_frame_get_x_value(RoccatSensitivityFrame *frame) {
	return roccat_sensitivity_from_value_to_bin(gaminggear_xy_scales_get_x_value(frame->priv->scale));
}

void roccat_sensitivity_frame_set_x_value(RoccatSensitivityFrame *frame, RoccatSensitivity value) {
	gaminggear_xy_scales_set_x_value(frame->priv->scale, roccat_sensitivity_from_bin_to_value(value));
}

RoccatSensitivity roccat_sensitivity_frame_get_y_value(RoccatSensitivityFrame *frame) {
	return roccat_sensitivity_from_value_to_bin(gaminggear_xy_scales_get_y_value(frame->priv->scale));
}

void roccat_sensitivity_frame_set_y_value(RoccatSensitivityFrame *frame, RoccatSensitivity value) {
	gaminggear_xy_scales_set_y_value(frame->priv->scale, roccat_sensitivity_from_bin_to_value(value));
}

guint roccat_sensitivity_frame_get_advanced(RoccatSensitivityFrame *frame) {
	return gaminggear_xy_scales_get_linked(frame->priv->scale) ? ROCCAT_SENSITIVITY_ADVANCED_OFF : ROCCAT_SENSITIVITY_ADVANCED_ON;
}

void roccat_sensitivity_frame_set_advanced(RoccatSensitivityFrame *frame, guint value) {
	gaminggear_xy_scales_set_linked(frame->priv->scale, value == ROCCAT_SENSITIVITY_ADVANCED_OFF);
}
