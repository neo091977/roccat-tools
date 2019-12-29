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

#include "pyraconfig_light_frame.h"
#include "gtk_roccat_helper.h"
#include "i18n.h"

#define PYRACONFIG_LIGHT_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), PYRACONFIG_LIGHT_FRAME_TYPE, PyraconfigLightFrameClass))
#define IS_PYRACONFIG_LIGHT_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), PYRACONFIG_LIGHT_FRAME_TYPE))
#define PYRACONFIG_LIGHT_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), PYRACONFIG_LIGHT_FRAME_TYPE, PyraconfigLightFramePrivate))

typedef struct _PyraconfigLightFrameClass PyraconfigLightFrameClass;
typedef struct _PyraconfigLightFramePrivate PyraconfigLightFramePrivate;

struct _PyraconfigLightFrame {
	GtkFrame parent;
	PyraconfigLightFramePrivate *priv;
};

struct _PyraconfigLightFrameClass {
	GtkFrameClass parent_class;
};

struct _PyraconfigLightFramePrivate {
	GSList *radios;
	GtkToggleButton *state;
};

G_DEFINE_TYPE(PyraconfigLightFrame, pyraconfig_light_frame, GTK_TYPE_FRAME);

static gchar const * const pyra_value_key = "value";

static void pack_radio_button(gpointer button, gpointer parent) {
	gtk_box_pack_start(parent, button, TRUE, TRUE, 0);
}

static gint find_light_type(gconstpointer a, gconstpointer b) {
	guint a_value = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(a), pyra_value_key));
	guint b_value = GPOINTER_TO_UINT(b);

	return (gint)a_value - (gint)b_value;
}

void pyraconfig_light_frame_set_from_rmp(PyraconfigLightFrame *frame, PyraRMP *rmp) {
	PyraconfigLightFramePrivate *priv = frame->priv;
	GSList *element;
	guint value;

	value = pyra_rmp_get_light_type(rmp);
	element = g_slist_find_custom(priv->radios, GUINT_TO_POINTER(value), find_light_type);
	if (element)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(element->data), TRUE);
	else
		g_warning(_("Got unknown light type %i"), value);

	value = pyra_rmp_get_light_switch(rmp);
	gtk_toggle_button_set_active(priv->state, (value == PYRA_LIGHTSWITCH_ON) ? TRUE : FALSE);
}

void pyraconfig_light_frame_update_rmp(PyraconfigLightFrame *frame, PyraRMP *rmp) {
	PyraconfigLightFramePrivate *priv = frame->priv;
	guint light_type;
	gboolean state;
	GtkWidget *active;

	active = gtk_roccat_radio_button_group_get_active(priv->radios);
	light_type = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(active), pyra_value_key));
	pyra_rmp_set_light_type(rmp, light_type);

	state = gtk_toggle_button_get_active(priv->state) ? PYRA_LIGHTSWITCH_ON: PYRA_LIGHTSWITCH_OFF;
	pyra_rmp_set_light_switch(rmp, state);
}

GtkWidget *pyraconfig_light_frame_new(void) {
	PyraconfigLightFrame *frame;

	frame = PYRACONFIG_LIGHT_FRAME(g_object_new(PYRACONFIG_LIGHT_FRAME_TYPE,
			NULL));

	return GTK_WIDGET(frame);
}

static GSList *light_type_radios(void) {
	GtkWidget *radio;

	radio = gtk_radio_button_new_with_label(NULL, _("Pulsating"));
	g_object_set_data(G_OBJECT(radio), pyra_value_key, GUINT_TO_POINTER(PYRA_LIGHT_TYPE_PULSATING));

	radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio), _("Fully lighted"));
	g_object_set_data(G_OBJECT(radio), pyra_value_key, GUINT_TO_POINTER(PYRA_LIGHT_TYPE_FULL_LIGHTENED));

	return gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio));
}

static void pyraconfig_light_frame_init(PyraconfigLightFrame *frame) {
	PyraconfigLightFramePrivate *priv = PYRACONFIG_LIGHT_FRAME_GET_PRIVATE(frame);
	GtkWidget *box;

	frame->priv = priv;

	box = gtk_vbox_new(FALSE, 0);
	priv->state = GTK_TOGGLE_BUTTON(gtk_check_button_new_with_label(_("On")));
	priv->radios = light_type_radios();

	g_slist_foreach(priv->radios, pack_radio_button, box);
	gtk_box_pack_start(GTK_BOX(box), gtk_hseparator_new(), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(priv->state), TRUE, TRUE, 0);
	gtk_container_add(GTK_CONTAINER(frame), box);

	gtk_frame_set_label(GTK_FRAME(frame), _("Light"));
}

static void pyraconfig_light_frame_class_init(PyraconfigLightFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(PyraconfigLightFramePrivate));
}
