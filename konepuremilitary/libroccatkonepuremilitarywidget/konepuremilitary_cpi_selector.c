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

#include "konepuremilitary_cpi_selector.h"
#include "konepuremilitary_single_cpi_selector.h"
#include "i18n-lib.h"

#define KONEPUREMILITARY_CPI_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KONEPUREMILITARY_CPI_SELECTOR_TYPE, KonepuremilitaryCpiSelectorClass))
#define IS_KONEPUREMILITARY_CPI_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KONEPUREMILITARY_CPI_SELECTOR_TYPE))
#define KONEPUREMILITARY_CPI_SELECTOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KONEPUREMILITARY_CPI_SELECTOR_TYPE, KonepuremilitaryCpiSelectorPrivate))

struct _KonepuremilitaryCpiSelectorPrivate {
	GtkVBox *box;
	KonepuremilitarySingleCpiSelector **selectors;
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

G_DEFINE_TYPE(KonepuremilitaryCpiSelector, konepuremilitary_cpi_selector, GTK_TYPE_FRAME);

static GObject *konepuremilitary_cpi_selector_constructor(GType gtype, guint n_properties, GObjectConstructParam *properties) {
	KonepuremilitaryCpiSelectorPrivate *priv;
	KonepuremilitaryCpiSelector *selector;
	GObject *obj;
	gchar *label;
	guint i;
	GSList *group;
	GtkWidget *frame;

	obj = G_OBJECT_CLASS(konepuremilitary_cpi_selector_parent_class)->constructor(gtype, n_properties, properties);
	selector = KONEPUREMILITARY_CPI_SELECTOR(obj);
	priv = selector->priv;

	priv->box = GTK_VBOX(gtk_vbox_new(FALSE, 0));
	gtk_container_add(GTK_CONTAINER(selector), GTK_WIDGET(priv->box));

	priv->selectors = (KonepuremilitarySingleCpiSelector **)g_malloc0(KONEPUREMILITARY_CPI_LEVELS_NUM * sizeof(KonepuremilitarySingleCpiSelector *));

	group = NULL;
	for (i = 0; i < KONEPUREMILITARY_CPI_LEVELS_NUM; ++i) {
		label = g_strdup_printf("%u", i + 1);
		frame = gtk_frame_new(label);
		g_free(label);
		priv->selectors[i] = KONEPUREMILITARY_SINGLE_CPI_SELECTOR(konepuremilitary_single_cpi_selector_new(
				group, priv->min, priv->max, priv->step));
		group = konepuremilitary_single_cpi_selector_get_group(priv->selectors[i]);
		gtk_container_add(GTK_CONTAINER(frame), GTK_WIDGET(priv->selectors[i]));
		gtk_box_pack_start(GTK_BOX(priv->box), frame, TRUE, TRUE, 0);
	}

	gtk_frame_set_label(GTK_FRAME(selector), _("CPI switcher"));

	return obj;
}

GtkWidget *konepuremilitary_cpi_selector_new(gdouble min, gdouble max, gdouble step) {
	return GTK_WIDGET(g_object_new(KONEPUREMILITARY_CPI_SELECTOR_TYPE,
			"min", min,
			"max", max,
			"step", step,
			NULL));
}

static void konepuremilitary_cpi_selector_init(KonepuremilitaryCpiSelector *selector) {
	selector->priv = KONEPUREMILITARY_CPI_SELECTOR_GET_PRIVATE(selector);
}

static void konepuremilitary_cpi_selector_finalize(GObject *object) {
	KonepuremilitaryCpiSelector *selector = KONEPUREMILITARY_CPI_SELECTOR(object);
	g_free(selector->priv->selectors);
	G_OBJECT_CLASS(konepuremilitary_cpi_selector_parent_class)->finalize(object);
}

static void konepuremilitary_cpi_selector_set_property(GObject *object, guint prop_id, GValue const *value, GParamSpec *pspec) {
	KonepuremilitaryCpiSelectorPrivate *priv = KONEPUREMILITARY_CPI_SELECTOR(object)->priv;
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

static void konepuremilitary_cpi_selector_class_init(KonepuremilitaryCpiSelectorClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->constructor = konepuremilitary_cpi_selector_constructor;
	gobject_class->finalize = konepuremilitary_cpi_selector_finalize;
	gobject_class->set_property = konepuremilitary_cpi_selector_set_property;

	g_type_class_add_private(klass, sizeof(KonepuremilitaryCpiSelectorPrivate));

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

static void konepuremilitary_cpi_selector_set_all_active(KonepuremilitaryCpiSelector *selector, guint mask) {
	KonepuremilitaryCpiSelectorPrivate *priv = selector->priv;
	guint i;
	guint bit;

	bit = 1;
	for (i = 0; i < KONEPUREMILITARY_CPI_LEVELS_NUM; ++i) {
		konepuremilitary_single_cpi_selector_set_active(priv->selectors[i], bit & mask);
		bit <<= 1;
	}
}

void konepuremilitary_cpi_selector_set_from_rmp(KonepuremilitaryCpiSelector *selector, KoneplusRmp *rmp) {
	KonepuremilitaryCpiSelectorPrivate *priv = selector->priv;
	guint i;

	konepuremilitary_single_cpi_selector_set_selected(priv->selectors[koneplus_rmp_get_cpi_x(rmp)], TRUE);
	konepuremilitary_cpi_selector_set_all_active(selector, koneplus_rmp_get_cpi_all(rmp));
	for (i = 0; i < KONEPLUS_PROFILE_SETTING_CPI_LEVELS_NUM; ++i) {
		konepuremilitary_single_cpi_selector_set_linked(priv->selectors[i], konepuremilitary_rmp_get_xy_cpi_flag(rmp, i));
		konepuremilitary_single_cpi_selector_set_x_value(priv->selectors[i], konepuremilitary_cpi_rpm_to_real(koneplus_rmp_get_cpi_level_x(rmp, i)));
		konepuremilitary_single_cpi_selector_set_y_value(priv->selectors[i], konepuremilitary_cpi_rpm_to_real(koneplus_rmp_get_cpi_level_y(rmp, i)));
	}
}

static guint konepuremilitary_cpi_selector_get_all_active(KonepuremilitaryCpiSelector *selector) {
	KonepuremilitaryCpiSelectorPrivate *priv = selector->priv;
	guint i;
	guint bit;
	guint mask;

	bit = 1;
	mask = 0;
	for (i = 0; i < KONEPUREMILITARY_CPI_LEVELS_NUM; ++i) {
		if (konepuremilitary_single_cpi_selector_get_active(priv->selectors[i]))
			mask |= bit;
		bit <<= 1;
	}

	return mask;
}

static guint konepuremilitary_cpi_selector_get_selected(KonepuremilitaryCpiSelector *selector) {
	KonepuremilitaryCpiSelectorPrivate *priv = selector->priv;
	guint i;

	for (i = 0; i < KONEPUREMILITARY_CPI_LEVELS_NUM; ++i)
		if (konepuremilitary_single_cpi_selector_get_selected(priv->selectors[i]))
			return i;

	return 0;
}

void konepuremilitary_cpi_selector_update_rmp(KonepuremilitaryCpiSelector *selector, KoneplusRmp *rmp) {
	KonepuremilitaryCpiSelectorPrivate *priv = selector->priv;
	guint i;

	koneplus_rmp_set_cpi_x(rmp, konepuremilitary_cpi_selector_get_selected(selector));
	koneplus_rmp_set_cpi_y(rmp, konepuremilitary_cpi_selector_get_selected(selector));
	koneplus_rmp_set_cpi_all(rmp, konepuremilitary_cpi_selector_get_all_active(selector));
	for (i = 0; i < KONEPLUS_PROFILE_SETTING_CPI_LEVELS_NUM; ++i) {
		konepuremilitary_rmp_set_xy_cpi_flag(rmp, i, konepuremilitary_single_cpi_selector_get_linked(priv->selectors[i]));
		koneplus_rmp_set_cpi_level_x(rmp, i, konepuremilitary_cpi_real_to_rpm(konepuremilitary_single_cpi_selector_get_x_value(priv->selectors[i])));
		koneplus_rmp_set_cpi_level_y(rmp, i, konepuremilitary_cpi_real_to_rpm(konepuremilitary_single_cpi_selector_get_y_value(priv->selectors[i])));
	}
}
