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

#include "kiro_sensitivity_frame.h"
#include <gaminggear/gaminggear_hscale.h>
#include "roccat.h"
#include "i18n.h"

#define KIRO_SENSITIVITY_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KIRO_SENSITIVITY_FRAME_TYPE, KiroSensitivityFrameClass))
#define IS_KIRO_SENSITIVITY_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KIRO_SENSITIVITY_FRAME_TYPE))
#define KIRO_SENSITIVITY_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KIRO_SENSITIVITY_FRAME_TYPE, KiroSensitivityFramePrivate))

typedef struct _KiroSensitivityFrameClass KiroSensitivityFrameClass;
typedef struct _KiroSensitivityFramePrivate KiroSensitivityFramePrivate;

struct _KiroSensitivityFrame {
	GtkFrame parent;
	KiroSensitivityFramePrivate *priv;
};

struct _KiroSensitivityFrameClass {
	GtkFrameClass parent_class;
};

struct _KiroSensitivityFramePrivate {
	GaminggearHScale *sensitivity;
};

G_DEFINE_TYPE(KiroSensitivityFrame, kiro_sensitivity_frame, GTK_TYPE_FRAME);

void kiro_sensitivity_frame_set_value(KiroSensitivityFrame *frame, guint new_value) {
	gaminggear_hscale_set_value(frame->priv->sensitivity, roccat_sensitivity_from_bin_to_value(new_value));
}

guint kiro_sensitivity_frame_get_value(KiroSensitivityFrame *frame) {
	return roccat_sensitivity_from_value_to_bin(gaminggear_hscale_get_value(frame->priv->sensitivity));
}

GtkWidget *kiro_sensitivity_frame_new(void) {
	KiroSensitivityFrame *kiro_sensitivity_frame;

	kiro_sensitivity_frame = KIRO_SENSITIVITY_FRAME(g_object_new(KIRO_SENSITIVITY_FRAME_TYPE,
			NULL));

	return GTK_WIDGET(kiro_sensitivity_frame);
}

static void kiro_sensitivity_frame_init(KiroSensitivityFrame *frame) {
	KiroSensitivityFramePrivate *priv = KIRO_SENSITIVITY_FRAME_GET_PRIVATE(frame);
	gint i;

	frame->priv = priv;

	priv->sensitivity = GAMINGGEAR_HSCALE(gaminggear_hscale_new_with_range(
			roccat_sensitivity_from_bin_to_value(ROCCAT_SENSITIVITY_MIN),
			roccat_sensitivity_from_bin_to_value(ROCCAT_SENSITIVITY_MAX),
			1.0));

	gtk_container_add(GTK_CONTAINER(frame), GTK_WIDGET(priv->sensitivity));

	for (i = ROCCAT_SENSITIVITY_MIN; i < ROCCAT_SENSITIVITY_MAX; ++i)
		gaminggear_hscale_add_mark(priv->sensitivity, roccat_sensitivity_from_bin_to_value(i), GTK_POS_BOTTOM, NULL);

	gtk_frame_set_label(GTK_FRAME(frame), _("Sensitivity"));

	gtk_widget_show_all(GTK_WIDGET(priv->sensitivity));
}

static void kiro_sensitivity_frame_class_init(KiroSensitivityFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(KiroSensitivityFramePrivate));
}
