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
#include "konepuremilitary_anglesnap_frame.h"
#include "i18n-lib.h"
#include <gaminggear/gaminggear_hscale.h>

#define KONEPUREMILITARY_ANGLESNAP_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KONEPUREMILITARY_ANGLESNAP_FRAME_TYPE, KonepuremilitaryAnglesnapFrameClass))
#define IS_KONEPUREMILITARY_ANGLESNAP_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KONEPUREMILITARY_ANGLESNAP_FRAME_TYPE))
#define KONEPUREMILITARY_ANGLESNAP_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KONEPUREMILITARY_ANGLESNAP_FRAME_TYPE, KonepuremilitaryAnglesnapFramePrivate))

typedef struct _KonepuremilitaryAnglesnapFrameClass KonepuremilitaryAnglesnapFrameClass;
typedef struct _KonepuremilitaryAnglesnapFramePrivate KonepuremilitaryAnglesnapFramePrivate;

struct _KonepuremilitaryAnglesnapFrame {
	GtkFrame parent;
	KonepuremilitaryAnglesnapFramePrivate *priv;
};

struct _KonepuremilitaryAnglesnapFrameClass {
	GtkFrameClass parent_class;
};

struct _KonepuremilitaryAnglesnapFramePrivate {
	GaminggearHScale *scale;
	GtkCheckButton *flag;
};

G_DEFINE_TYPE(KonepuremilitaryAnglesnapFrame, konepuremilitary_anglesnap_frame, GTK_TYPE_FRAME);

GtkWidget *konepuremilitary_anglesnap_frame_new(void) {
	KonepuremilitaryAnglesnapFrame *konepuremilitary_anglesnap_frame;

	konepuremilitary_anglesnap_frame = KONEPUREMILITARY_ANGLESNAP_FRAME(g_object_new(KONEPUREMILITARY_ANGLESNAP_FRAME_TYPE,
			NULL));

	return GTK_WIDGET(konepuremilitary_anglesnap_frame);
}

static GaminggearHScale *scale_new(void) {
	GaminggearHScale *scale;
	gint i;

	scale = GAMINGGEAR_HSCALE(gaminggear_hscale_new_with_range(KONEPUREMILITARY_ANGLESNAP_MIN, KONEPUREMILITARY_ANGLESNAP_MAX, 1));

	for (i = KONEPUREMILITARY_ANGLESNAP_MIN; i <= KONEPUREMILITARY_ANGLESNAP_MAX; ++i)
		gaminggear_hscale_add_mark(scale, i, GTK_POS_BOTTOM, NULL);
	return scale;
}

static void konepuremilitary_anglesnap_frame_init(KonepuremilitaryAnglesnapFrame *frame) {
	KonepuremilitaryAnglesnapFramePrivate *priv = KONEPUREMILITARY_ANGLESNAP_FRAME_GET_PRIVATE(frame);
	GtkWidget *hbox;

	frame->priv = priv;

	hbox = gtk_hbox_new(FALSE, 0);
	priv->scale = scale_new();
	priv->flag = GTK_CHECK_BUTTON(gtk_check_button_new_with_label(_("Active")));

	gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(priv->flag), FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(priv->scale), TRUE, TRUE, 0);
	gtk_container_add(GTK_CONTAINER(frame), hbox);

	gtk_frame_set_label(GTK_FRAME(frame), _("Angle snapping"));
}

static void konepuremilitary_anglesnap_frame_class_init(KonepuremilitaryAnglesnapFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(KonepuremilitaryAnglesnapFramePrivate));
}

void konepuremilitary_anglesnap_frame_set_from_rmp(KonepuremilitaryAnglesnapFrame *frame, KoneplusRmp *rmp) {
	KonepuremilitaryAnglesnapFramePrivate *priv = frame->priv;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->flag),
			konepuremilitary_rmp_get_anglesnap_flag(rmp) == KONEPUREMILITARY_RMP_ANGLESNAP_FLAG_ON);
	gaminggear_hscale_set_value(priv->scale, konepuremilitary_rmp_get_anglesnap_value(rmp));
}

void konepuremilitary_anglesnap_frame_update_rmp(KonepuremilitaryAnglesnapFrame *frame, KoneplusRmp *rmp) {
	KonepuremilitaryAnglesnapFramePrivate *priv = frame->priv;
	konepuremilitary_rmp_set_anglesnap_flag(rmp, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(priv->flag)) ?
			KONEPUREMILITARY_RMP_ANGLESNAP_FLAG_ON : KONEPUREMILITARY_RMP_ANGLESNAP_FLAG_OFF);
	konepuremilitary_rmp_set_anglesnap_value(rmp, gaminggear_hscale_get_value(priv->scale));
}
