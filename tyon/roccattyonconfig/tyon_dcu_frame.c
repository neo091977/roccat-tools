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

#include "tyon_dcu_frame.h"
#include "tyon_control_unit.h"
#include "gtk_roccat_helper.h"
#include "i18n.h"

#define TYON_DCU_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), TYON_DCU_FRAME_TYPE, TyonDcuFrameClass))
#define IS_TYON_DCU_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), TYON_DCU_FRAME_TYPE))
#define TYON_DCU_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), TYON_DCU_FRAME_TYPE, TyonDcuFramePrivate))

typedef struct _TyonDcuFrameClass TyonDcuFrameClass;
typedef struct _TyonDcuFramePrivate TyonDcuFramePrivate;

struct _TyonDcuFrame {
	GtkFrame parent;
	TyonDcuFramePrivate *priv;
};

struct _TyonDcuFrameClass {
	GtkFrameClass parent_class;
	void (*changed)(TyonDcuFrame *frame, guint value);
};

struct _TyonDcuFramePrivate {
	GSList *radios;
};

G_DEFINE_TYPE(TyonDcuFrame, tyon_dcu_frame, GTK_TYPE_FRAME);

enum {
	CHANGED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };
static gchar const * const state_key = "State";

static void radio_toggled_cb(GtkToggleButton *radio, gpointer user_data) {
	TyonDcuFrame *frame = (TyonDcuFrame *)user_data;

	if (gtk_toggle_button_get_active(radio))
		g_signal_emit((gpointer)frame, signals[CHANGED], 0);
}

void tyon_dcu_frame_set_value_blocked(TyonDcuFrame *frame, guint new_value) {
	GSList *child;
	guint value;

	for (child = frame->priv->radios; child; child = g_slist_next(child)) {
		value = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(child->data), state_key));
		if (value == new_value) {
			g_signal_handlers_block_by_func(G_OBJECT(child->data), radio_toggled_cb, frame);
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(child->data), TRUE);
			g_signal_handlers_unblock_by_func(G_OBJECT(child->data), radio_toggled_cb, frame);
			break;
		}

	}
}

guint tyon_dcu_frame_get_value(TyonDcuFrame *frame) {
	GtkWidget *active = gtk_roccat_radio_button_group_get_active(frame->priv->radios);
	if (active)
		return GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(active), state_key));
	else
		return 0;
}

GtkWidget *tyon_dcu_frame_new(void) {
	TyonDcuFrame *tyon_dcu_frame;

	tyon_dcu_frame = TYON_DCU_FRAME(g_object_new(TYON_DCU_FRAME_TYPE,
			NULL));

	return GTK_WIDGET(tyon_dcu_frame);
}

static void pack_radio_button(gpointer button, gpointer parent) {
	gtk_box_pack_start(parent, button, TRUE, TRUE, 0);
}

static void tyon_dcu_frame_init(TyonDcuFrame *frame) {
	TyonDcuFramePrivate *priv = TYON_DCU_FRAME_GET_PRIVATE(frame);
	GtkWidget *box;
	GtkWidget *radio;
	int i;

	frame->priv = priv;

	box = gtk_vbox_new(FALSE, 0);

	gtk_container_add(GTK_CONTAINER(frame), box);

	struct {
		TyonControlUnitDcu state;
		gchar const *title;
	} radios[4] = {
		{TYON_DISTANCE_CONTROL_UNIT_OFF, N_("Off")},
		{TYON_DISTANCE_CONTROL_UNIT_EXTRA_LOW, N_("Extra low")},
		{TYON_DISTANCE_CONTROL_UNIT_LOW, N_("Low")},
		{TYON_DISTANCE_CONTROL_UNIT_NORMAL, N_("Normal")},
	};

	priv->radios = NULL;

	for (i = 3; i >= 0; --i) {
		radio = gtk_radio_button_new_with_label(priv->radios, _N(radios[i].title));
		g_object_set_data(G_OBJECT(radio), state_key, GUINT_TO_POINTER(radios[i].state));
		priv->radios = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio));
		g_signal_connect(G_OBJECT(radio), "toggled", G_CALLBACK(radio_toggled_cb), frame);
	}

	g_slist_foreach(priv->radios, pack_radio_button, box);

	gtk_frame_set_label(GTK_FRAME(frame), _("Distance control unit"));

	gtk_widget_show_all(box);
}

static void tyon_dcu_frame_class_init(TyonDcuFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(TyonDcuFramePrivate));

	signals[CHANGED] = g_signal_new("changed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			G_STRUCT_OFFSET(TyonDcuFrameClass, changed),
			NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}
