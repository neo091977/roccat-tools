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

#include "pyraconfig_sensitivity_frame.h"
#include "i18n.h"
#include <gaminggear/gaminggear_xy_scales.h>

#define PYRACONFIG_SENSITIVITY_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), PYRACONFIG_SENSITIVITY_FRAME_TYPE, PyraconfigSensitivityFrameClass))
#define IS_PYRACONFIG_SENSITIVITY_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), PYRACONFIG_SENSITIVITY_FRAME_TYPE))
#define PYRACONFIG_SENSITIVITY_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), PYRACONFIG_SENSITIVITY_FRAME_TYPE, PyraconfigSensitivityFramePrivate))

typedef struct _PyraconfigSensitivityFrameClass PyraconfigSensitivityFrameClass;
typedef struct _PyraconfigSensitivityFramePrivate PyraconfigSensitivityFramePrivate;

struct _PyraconfigSensitivityFrame {
	GtkFrame parent;
	PyraconfigSensitivityFramePrivate *priv;
};

struct _PyraconfigSensitivityFrameClass {
	GtkFrameClass parent_class;
};

struct _PyraconfigSensitivityFramePrivate {
	GaminggearXyScales *scale;
};

G_DEFINE_TYPE(PyraconfigSensitivityFrame, pyraconfig_sensitivity_frame, GTK_TYPE_FRAME);

void pyraconfig_sensitivity_frame_set_from_rmp(PyraconfigSensitivityFrame *frame, PyraRMP *rmp) {
	gaminggear_xy_scales_set_x_value(frame->priv->scale, pyra_rmp_get_sensitivity_x(rmp));
	gaminggear_xy_scales_set_y_value(frame->priv->scale, pyra_rmp_get_sensitivity_y(rmp));
}

void pyraconfig_sensitivity_frame_update_rmp(PyraconfigSensitivityFrame *frame, PyraRMP *rmp) {
	pyra_rmp_set_sensitivity_x(rmp, gaminggear_xy_scales_get_x_value(frame->priv->scale));
	pyra_rmp_set_sensitivity_y(rmp, gaminggear_xy_scales_get_y_value(frame->priv->scale));
}

GtkWidget *pyraconfig_sensitivity_frame_new(void) {
	return GTK_WIDGET(g_object_new(PYRACONFIG_SENSITIVITY_FRAME_TYPE, NULL));
}

static void pyraconfig_sensitivity_frame_init(PyraconfigSensitivityFrame *frame) {
	frame->priv = PYRACONFIG_SENSITIVITY_FRAME_GET_PRIVATE(frame);
	frame->priv->scale = GAMINGGEAR_XY_SCALES(gaminggear_xy_scales_new(
			(gdouble)PYRA_SENSITIVITY_MIN,
			(gdouble)PYRA_SENSITIVITY_MAX,
			(gdouble)1.0));
	gtk_container_add(GTK_CONTAINER(frame), GTK_WIDGET(frame->priv->scale));
	gtk_frame_set_label(GTK_FRAME(frame), _("Sensitivity"));
}

static void pyraconfig_sensitivity_frame_class_init(PyraconfigSensitivityFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(PyraconfigSensitivityFramePrivate));
}
