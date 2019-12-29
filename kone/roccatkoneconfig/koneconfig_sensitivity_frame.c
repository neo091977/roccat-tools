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

#include "koneconfig_sensitivity_frame.h"
#include "i18n.h"
#include <gaminggear/gaminggear_xy_scales.h>

#define KONECONFIG_SENSITIVITY_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KONECONFIG_SENSITIVITY_FRAME_TYPE, KoneconfigSensitivityFrameClass))
#define IS_KONECONFIG_SENSITIVITY_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KONECONFIG_SENSITIVITY_FRAME_TYPE))
#define KONECONFIG_SENSITIVITY_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KONECONFIG_SENSITIVITY_FRAME_TYPE, KoneconfigSensitivityFramePrivate))

typedef struct _KoneconfigSensitivityFrameClass KoneconfigSensitivityFrameClass;
typedef struct _KoneconfigSensitivityFramePrivate KoneconfigSensitivityFramePrivate;

struct _KoneconfigSensitivityFrame {
	GtkFrame parent;
	KoneconfigSensitivityFramePrivate *priv;
};

struct _KoneconfigSensitivityFrameClass {
	GtkFrameClass parent_class;
};

struct _KoneconfigSensitivityFramePrivate {
	GaminggearXyScales *scale;
};

G_DEFINE_TYPE(KoneconfigSensitivityFrame, koneconfig_sensitivity_frame, GTK_TYPE_FRAME);

static guint from_scale(gdouble scale) {
	return (guint)(100.0 * scale);
}

static gdouble to_scale(guint bin) {
	return (gdouble)bin / 100.0;
}

void koneconfig_sensitivity_frame_set_from_rmp(KoneconfigSensitivityFrame *frame, KoneRMP *rmp) {
	guint advanced = kone_rmp_get_advanced_sensitivity(rmp);
	gaminggear_xy_scales_set_linked(frame->priv->scale, advanced == KONE_ADVANCED_SENSITIVITY_OFF);
	if (advanced == KONE_ADVANCED_SENSITIVITY_ON) {
		gaminggear_xy_scales_set_x_value(frame->priv->scale, to_scale(kone_rmp_get_x_sensitivity(rmp)));
		gaminggear_xy_scales_set_y_value(frame->priv->scale, to_scale(kone_rmp_get_y_sensitivity(rmp)));
	} else {
		gaminggear_xy_scales_set_x_value(frame->priv->scale, to_scale(kone_rmp_get_main_sensitivity(rmp)));
		gaminggear_xy_scales_set_y_value(frame->priv->scale, to_scale(kone_rmp_get_main_sensitivity(rmp)));
	}
}

void koneconfig_sensitivity_frame_update_rmp(KoneconfigSensitivityFrame *frame, KoneRMP *rmp) {
	kone_rmp_set_main_sensitivity(rmp, from_scale(gaminggear_xy_scales_get_x_value(frame->priv->scale)));
	kone_rmp_set_x_sensitivity(rmp, from_scale(gaminggear_xy_scales_get_x_value(frame->priv->scale)));
	kone_rmp_set_y_sensitivity(rmp, from_scale(gaminggear_xy_scales_get_y_value(frame->priv->scale)));
	kone_rmp_set_advanced_sensitivity(rmp, gaminggear_xy_scales_get_linked(frame->priv->scale) ? KONE_ADVANCED_SENSITIVITY_OFF : KONE_ADVANCED_SENSITIVITY_ON);
}

GtkWidget *koneconfig_sensitivity_frame_new(void) {
	return GTK_WIDGET(g_object_new(KONECONFIG_SENSITIVITY_FRAME_TYPE, NULL));
}

static void koneconfig_sensitivity_frame_init(KoneconfigSensitivityFrame *frame) {
	frame->priv = KONECONFIG_SENSITIVITY_FRAME_GET_PRIVATE(frame);
	frame->priv->scale = GAMINGGEAR_XY_SCALES(gaminggear_xy_scales_new(
			(gdouble)to_scale(KONE_MAIN_SENSITIVITY_MIN),
			(gdouble)to_scale(KONE_MAIN_SENSITIVITY_MAX),
			(gdouble)1.0));
	gaminggear_xy_scales_set_digits(GAMINGGEAR_XY_SCALES(frame->priv->scale), 2);
	gtk_container_add(GTK_CONTAINER(frame), GTK_WIDGET(frame->priv->scale));
	gtk_frame_set_label(GTK_FRAME(frame), _("Sensitivity"));
}

static void koneconfig_sensitivity_frame_class_init(KoneconfigSensitivityFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(KoneconfigSensitivityFramePrivate));
}
