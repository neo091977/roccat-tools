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

#include "konepuremilitary.h"
#include "konepuremilitary_sensor_alignment_frame.h"
#include "i18n-lib.h"
#include <gaminggear/gaminggear_hscale.h>

#define KONEPUREMILITARY_SENSOR_ALIGNMENT_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KONEPUREMILITARY_SENSOR_ALIGNMENT_FRAME_TYPE, KonepuremilitarySensorAlignmentFrameClass))
#define IS_KONEPUREMILITARY_SENSOR_ALIGNMENT_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KONEPUREMILITARY_SENSOR_ALIGNMENT_FRAME_TYPE))
#define KONEPUREMILITARY_SENSOR_ALIGNMENT_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KONEPUREMILITARY_SENSOR_ALIGNMENT_FRAME_TYPE, KonepuremilitarySensorAlignmentFramePrivate))

typedef struct _KonepuremilitarySensorAlignmentFrameClass KonepuremilitarySensorAlignmentFrameClass;
typedef struct _KonepuremilitarySensorAlignmentFramePrivate KonepuremilitarySensorAlignmentFramePrivate;

struct _KonepuremilitarySensorAlignmentFrame {
	GtkFrame parent;
	KonepuremilitarySensorAlignmentFramePrivate *priv;
};

struct _KonepuremilitarySensorAlignmentFrameClass {
	GtkFrameClass parent_class;
};

struct _KonepuremilitarySensorAlignmentFramePrivate {
	GaminggearHScale *scale;
	GtkCheckButton *flag;
};

G_DEFINE_TYPE(KonepuremilitarySensorAlignmentFrame, konepuremilitary_sensor_alignment_frame, GTK_TYPE_FRAME);

GtkWidget *konepuremilitary_sensor_alignment_frame_new(void) {
	KonepuremilitarySensorAlignmentFrame *konepuremilitary_sensor_alignment_frame;

	konepuremilitary_sensor_alignment_frame = KONEPUREMILITARY_SENSOR_ALIGNMENT_FRAME(g_object_new(KONEPUREMILITARY_SENSOR_ALIGNMENT_FRAME_TYPE,
			NULL));

	return GTK_WIDGET(konepuremilitary_sensor_alignment_frame);
}

static GaminggearHScale *scale_new(void) {
	GaminggearHScale *scale;
	gint i;

	scale = GAMINGGEAR_HSCALE(gaminggear_hscale_new_with_range(
			-KONEPUREMILITARY_SENSOR_ALIGNMENT_RANGE, KONEPUREMILITARY_SENSOR_ALIGNMENT_RANGE, 1));

	for (i = -KONEPUREMILITARY_SENSOR_ALIGNMENT_RANGE; i <= KONEPUREMILITARY_SENSOR_ALIGNMENT_RANGE; i += 10)
		gaminggear_hscale_add_mark(scale, i, GTK_POS_BOTTOM, NULL);
	return scale;
}

static void konepuremilitary_sensor_alignment_frame_init(KonepuremilitarySensorAlignmentFrame *frame) {
	KonepuremilitarySensorAlignmentFramePrivate *priv = KONEPUREMILITARY_SENSOR_ALIGNMENT_FRAME_GET_PRIVATE(frame);
	GtkWidget *hbox;

	frame->priv = priv;

	hbox = gtk_hbox_new(FALSE, 0);
	priv->scale = scale_new();
	priv->flag = GTK_CHECK_BUTTON(gtk_check_button_new_with_label(_("Active")));

	gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(priv->flag), FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(priv->scale), TRUE, TRUE, 0);
	gtk_container_add(GTK_CONTAINER(frame), hbox);

	gtk_frame_set_label(GTK_FRAME(frame), _("Sensor alignment"));
}

static void konepuremilitary_sensor_alignment_frame_class_init(KonepuremilitarySensorAlignmentFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(KonepuremilitarySensorAlignmentFramePrivate));
}

void konepuremilitary_sensor_alignment_frame_set_from_rmp(KonepuremilitarySensorAlignmentFrame *frame, KoneplusRmp *rmp) {
	KonepuremilitarySensorAlignmentFramePrivate *priv = frame->priv;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->flag),
			konepuremilitary_rmp_get_sensoralign_flag(rmp) == KONEPUREMILITARY_RMP_ANGLESNAP_FLAG_ON);
	gaminggear_hscale_set_value(priv->scale,
			(gdouble)konepuremilitary_rmp_get_sensoralign_value(rmp) - KONEPUREMILITARY_SENSOR_ALIGNMENT_RANGE);
}

void konepuremilitary_sensor_alignment_frame_update_rmp(KonepuremilitarySensorAlignmentFrame *frame, KoneplusRmp *rmp) {
	KonepuremilitarySensorAlignmentFramePrivate *priv = frame->priv;
	konepuremilitary_rmp_set_sensoralign_flag(rmp, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(priv->flag)) ?
			KONEPUREMILITARY_RMP_ANGLESNAP_FLAG_ON : KONEPUREMILITARY_RMP_ANGLESNAP_FLAG_OFF);
	konepuremilitary_rmp_set_sensoralign_value(rmp,
			gaminggear_hscale_get_value(priv->scale) + KONEPUREMILITARY_SENSOR_ALIGNMENT_RANGE);
}
