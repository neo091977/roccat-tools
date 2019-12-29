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

#include "konepuremilitary_single_cpi_selector.h"
#include "konepuremilitary.h"
#include <gaminggear/gaminggear_xy_scales.h>

#define KONEPUREMILITARY_SINGLE_CPI_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KONEPUREMILITARY_SINGLE_CPI_SELECTOR_TYPE, KonepuremilitarySingleCpiSelectorClass))
#define IS_KONEPUREMILITARY_SINGLE_CPI_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KONEPUREMILITARY_SINGLE_CPI_SELECTOR_TYPE))
#define KONEPUREMILITARY_SINGLE_CPI_SELECTOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KONEPUREMILITARY_SINGLE_CPI_SELECTOR_TYPE, KonepuremilitarySingleCpiSelectorPrivate))

typedef struct _KonepuremilitarySingleCpiSelectorClass KonepuremilitarySingleCpiSelectorClass;

struct _KonepuremilitarySingleCpiSelectorClass {
	GtkHBoxClass parent_class;
};

struct _KonepuremilitarySingleCpiSelectorPrivate {
	GtkRadioButton *radio;
	GtkCheckButton *active;
	GaminggearXyScales *scale;
	gdouble min;
	gdouble max;
	gdouble step;
};

enum {
	PROP_0,
	PROP_MIN,
	PROP_MAX,
	PROP_STEP,
};

G_DEFINE_TYPE(KonepuremilitarySingleCpiSelector, konepuremilitary_single_cpi_selector, GTK_TYPE_HBOX);

GSList *konepuremilitary_single_cpi_selector_get_group(KonepuremilitarySingleCpiSelector *selector) {
	return gtk_radio_button_get_group(selector->priv->radio);
}

void konepuremilitary_single_cpi_selector_set_group(KonepuremilitarySingleCpiSelector *selector, GSList *group) {
	gtk_radio_button_set_group(selector->priv->radio, group);
}

guint konepuremilitary_single_cpi_selector_get_x_value(KonepuremilitarySingleCpiSelector *selector) {
	return (guint)gaminggear_xy_scales_get_x_value(selector->priv->scale);
}

void konepuremilitary_single_cpi_selector_set_x_value(KonepuremilitarySingleCpiSelector *selector, guint value) {
	gaminggear_xy_scales_set_x_value(selector->priv->scale, (gdouble)value);
}

guint konepuremilitary_single_cpi_selector_get_y_value(KonepuremilitarySingleCpiSelector *selector) {
	return (guint)gaminggear_xy_scales_get_y_value(selector->priv->scale);
}

void konepuremilitary_single_cpi_selector_set_y_value(KonepuremilitarySingleCpiSelector *selector, guint value) {
	gaminggear_xy_scales_set_y_value(selector->priv->scale, (gdouble)value);
}

gboolean konepuremilitary_single_cpi_selector_get_linked(KonepuremilitarySingleCpiSelector *selector) {
	return gaminggear_xy_scales_get_linked(selector->priv->scale);
}

void konepuremilitary_single_cpi_selector_set_linked(KonepuremilitarySingleCpiSelector *selector, gboolean value) {
	gaminggear_xy_scales_set_linked(selector->priv->scale, value);
}

gboolean konepuremilitary_single_cpi_selector_get_active(KonepuremilitarySingleCpiSelector *selector) {
	return gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(selector->priv->active));
}

void konepuremilitary_single_cpi_selector_set_active(KonepuremilitarySingleCpiSelector *selector, gboolean active) {
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(selector->priv->active), active);
}

gboolean konepuremilitary_single_cpi_selector_get_selected(KonepuremilitarySingleCpiSelector *selector) {
	return gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(selector->priv->radio));
}

void konepuremilitary_single_cpi_selector_set_selected(KonepuremilitarySingleCpiSelector *selector, gboolean active) {
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(selector->priv->radio), active);
}

GtkWidget *konepuremilitary_single_cpi_selector_new(GSList *group, gdouble min, gdouble max, gdouble step) {
	KonepuremilitarySingleCpiSelector *selector;

	selector = g_object_new(KONEPUREMILITARY_SINGLE_CPI_SELECTOR_TYPE,
			"homogeneous", FALSE,
			"spacing", 0,
			"min", min,
			"max", max,
			"step", step,
			NULL);

	konepuremilitary_single_cpi_selector_set_group(selector, group);

	return GTK_WIDGET(selector);
}

static void click_radio_cb(GtkToggleButton *button, gpointer user_data) {
	KonepuremilitarySingleCpiSelector *selector = KONEPUREMILITARY_SINGLE_CPI_SELECTOR(user_data);
	KonepuremilitarySingleCpiSelectorPrivate *priv = selector->priv;

	/* make sure selected is activated */
	if (gtk_toggle_button_get_active(button) && !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(priv->active)))
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->active), TRUE);
}

static void toggle_active_cb(GtkToggleButton *button, gpointer user_data) {
	KonepuremilitarySingleCpiSelector *selector = KONEPUREMILITARY_SINGLE_CPI_SELECTOR(user_data);
	KonepuremilitarySingleCpiSelectorPrivate *priv = selector->priv;
	gboolean new_state = gtk_toggle_button_get_active(button);

	/* selected can't be deactivated */
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(priv->radio)) && !new_state)
		gtk_toggle_button_set_active(button, TRUE);
}

static GObject *konepuremilitary_single_cpi_selector_constructor(GType gtype, guint n_properties, GObjectConstructParam *properties) {
	KonepuremilitarySingleCpiSelectorPrivate *priv;
	KonepuremilitarySingleCpiSelector *selector;
	GObject *obj;

	obj = G_OBJECT_CLASS(konepuremilitary_single_cpi_selector_parent_class)->constructor(gtype, n_properties, properties);
	selector = KONEPUREMILITARY_SINGLE_CPI_SELECTOR(obj);
	priv = selector->priv;

	priv->radio = GTK_RADIO_BUTTON(gtk_radio_button_new(NULL));
	gtk_box_pack_start(GTK_BOX(selector), GTK_WIDGET(priv->radio), FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(priv->radio), "toggled", G_CALLBACK(click_radio_cb), selector);

	priv->active = GTK_CHECK_BUTTON(gtk_check_button_new());
	gtk_box_pack_start(GTK_BOX(selector), GTK_WIDGET(priv->active), FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(priv->active), "toggled", G_CALLBACK(toggle_active_cb), selector);

	priv->scale = GAMINGGEAR_XY_SCALES(gaminggear_xy_scales_new(priv->min, priv->max, priv->step));
	gtk_box_pack_start(GTK_BOX(selector), GTK_WIDGET(priv->scale), TRUE, TRUE, 0);

	gtk_widget_show_all(GTK_WIDGET(selector));

	return obj;
}

static void konepuremilitary_single_cpi_selector_init(KonepuremilitarySingleCpiSelector *selector) {
	selector->priv = KONEPUREMILITARY_SINGLE_CPI_SELECTOR_GET_PRIVATE(selector);
}

static void konepuremilitary_single_cpi_selector_set_property(GObject *object, guint prop_id, GValue const *value, GParamSpec *pspec) {
	KonepuremilitarySingleCpiSelectorPrivate *priv = KONEPUREMILITARY_SINGLE_CPI_SELECTOR(object)->priv;
	switch(prop_id) {
	case PROP_MIN:
		priv->min = g_value_get_double(value);
		break;
	case PROP_MAX:
		priv->max = g_value_get_double(value);
		break;
	case PROP_STEP:
		priv->step = g_value_get_double(value);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
}

static void konepuremilitary_single_cpi_selector_class_init(KonepuremilitarySingleCpiSelectorClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = (GObjectClass *)klass;

	gobject_class->constructor = konepuremilitary_single_cpi_selector_constructor;
	gobject_class->set_property = konepuremilitary_single_cpi_selector_set_property;

	g_type_class_add_private(klass, sizeof(KonepuremilitarySingleCpiSelectorPrivate));

	g_object_class_install_property(gobject_class, PROP_MIN,
			g_param_spec_double("min",
					"minimum value",
					"The minimum value",
					-G_MAXDOUBLE, G_MAXDOUBLE, 0.0,
					G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));

	g_object_class_install_property(gobject_class, PROP_MAX,
			g_param_spec_double("max",
					"maximum value",
					"The maximum value",
					-G_MAXDOUBLE, G_MAXDOUBLE, 0.0,
					G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));

	g_object_class_install_property(gobject_class, PROP_STEP,
			g_param_spec_double("step",
					"step",
					"Step",
					G_MINDOUBLE, G_MAXDOUBLE, 1.0,
					G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
}
