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

#include "roccat_cpi_selector.h"
#include "roccat_single_cpi_selector.h"
#include "roccat_helper.h"
#include "i18n-lib.h"

#define ROCCAT_CPI_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ROCCAT_CPI_SELECTOR_TYPE, RoccatCpiSelectorClass))
#define IS_ROCCAT_CPI_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ROCCAT_CPI_SELECTOR_TYPE))
#define ROCCAT_CPI_SELECTOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ROCCAT_CPI_SELECTOR_TYPE, RoccatCpiSelectorPrivate))

struct _RoccatCpiSelectorPrivate {
	guint count;
	GtkVBox *box;
	RoccatSingleCpiSelector **selectors;
	gdouble min, max, step;
};

enum {
	PROP_0,
	PROP_COUNT,
	PROP_MIN,
	PROP_MAX,
	PROP_STEP,
};

G_DEFINE_TYPE(RoccatCpiSelector, roccat_cpi_selector, GTK_TYPE_FRAME);

gdouble roccat_cpi_selector_get_value(RoccatCpiSelector *selector, guint index) {
	RoccatCpiSelectorPrivate *priv = selector->priv;
	index = roccat_sanitize_index_max(priv->count, index);
	return roccat_single_cpi_selector_get_value(priv->selectors[index]);
}

void roccat_cpi_selector_set_value(RoccatCpiSelector *selector, guint index, gdouble value) {
	RoccatCpiSelectorPrivate *priv = selector->priv;
	index = roccat_sanitize_index_max(priv->count, index);
	roccat_single_cpi_selector_set_value(priv->selectors[index], value);
}

guint roccat_cpi_selector_get_all_active(RoccatCpiSelector *selector) {
	RoccatCpiSelectorPrivate *priv = selector->priv;
	guint i;
	guint bit;
	guint mask;

	bit = 1;
	mask = 0;
	for (i = 0; i < priv->count; ++i) {
		if (roccat_single_cpi_selector_get_active(priv->selectors[i]))
			mask |= bit;
		bit <<= 1;
	}

	return mask;
}

void roccat_cpi_selector_set_all_active(RoccatCpiSelector *selector, guint mask) {
	RoccatCpiSelectorPrivate *priv = selector->priv;
	guint i;
	guint bit;

	bit = 1;
	for (i = 0; i < priv->count; ++i) {
		roccat_single_cpi_selector_set_active(priv->selectors[i], bit & mask);
		bit <<= 1;
	}
}

guint roccat_cpi_selector_get_selected(RoccatCpiSelector *selector) {
	RoccatCpiSelectorPrivate *priv = selector->priv;
	guint i;

	for (i = 0; i < priv->count; ++i)
		if (roccat_single_cpi_selector_get_selected(priv->selectors[i]))
			return i;

	return 0;
}

void roccat_cpi_selector_set_selected(RoccatCpiSelector *selector, guint index) {
	RoccatCpiSelectorPrivate *priv = selector->priv;
	index = roccat_sanitize_index_max(priv->count, index);
	roccat_single_cpi_selector_set_selected(priv->selectors[index], TRUE);
}

static GObject *roccat_cpi_selector_constructor(GType gtype, guint n_properties, GObjectConstructParam *properties) {
	RoccatCpiSelectorPrivate *priv;
	RoccatCpiSelector *selector;
	GObject *obj;
	gchar *label;
	guint i;
	GSList *group;

	obj = G_OBJECT_CLASS(roccat_cpi_selector_parent_class)->constructor(gtype, n_properties, properties);
	selector = ROCCAT_CPI_SELECTOR(obj);
	priv = selector->priv;

	priv->selectors = (RoccatSingleCpiSelector **)g_malloc0(priv->count * sizeof(RoccatSingleCpiSelector *));

	group = NULL;
	for (i = 0; i < priv->count; ++i) {
		label = g_strdup_printf("%u", i + 1);
		priv->selectors[i] = ROCCAT_SINGLE_CPI_SELECTOR(roccat_single_cpi_selector_new(label, group, priv->min, priv->max, priv->step));
		g_free(label);
		group = roccat_single_cpi_selector_get_group(priv->selectors[i]);
		gtk_box_pack_start(GTK_BOX(priv->box), GTK_WIDGET(priv->selectors[i]), TRUE, TRUE, 0);
	}

	return obj;
}

GtkWidget *roccat_cpi_selector_new(guint count, gdouble min, gdouble max, gdouble step) {
	RoccatCpiSelector *selector;

	selector = g_object_new(ROCCAT_CPI_SELECTOR_TYPE,
			"count", count,
			"min", min,
			"max", max,
			"step", step,
			NULL);

	return GTK_WIDGET(selector);
}

static void roccat_cpi_selector_init(RoccatCpiSelector *selector) {
	RoccatCpiSelectorPrivate *priv = ROCCAT_CPI_SELECTOR_GET_PRIVATE(selector);

	selector->priv = priv;

	priv->box = GTK_VBOX(gtk_vbox_new(FALSE, 0));
	gtk_container_add(GTK_CONTAINER(selector), GTK_WIDGET(priv->box));

	gtk_frame_set_label(GTK_FRAME(selector), _("CPI switcher"));
}

static void roccat_cpi_selector_finalize(GObject *object) {
	RoccatCpiSelector *selector = ROCCAT_CPI_SELECTOR(object);
	g_free(selector->priv->selectors);
	G_OBJECT_CLASS(roccat_cpi_selector_parent_class)->finalize(object);
}

static void roccat_cpi_selector_set_property(GObject *object, guint prop_id, GValue const *value, GParamSpec *pspec) {
	RoccatCpiSelectorPrivate *priv = ROCCAT_CPI_SELECTOR(object)->priv;
	switch(prop_id) {
	case PROP_COUNT:
		priv->count = g_value_get_uint(value);
		break;
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

static void roccat_cpi_selector_class_init(RoccatCpiSelectorClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->constructor = roccat_cpi_selector_constructor;
	gobject_class->finalize = roccat_cpi_selector_finalize;
	gobject_class->set_property = roccat_cpi_selector_set_property;

	g_type_class_add_private(klass, sizeof(RoccatCpiSelectorPrivate));

	g_object_class_install_property(gobject_class, PROP_COUNT,
			g_param_spec_uint("count",
					"count",
					"Count",
					0, G_MAXUINT, 1,
					G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));

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
