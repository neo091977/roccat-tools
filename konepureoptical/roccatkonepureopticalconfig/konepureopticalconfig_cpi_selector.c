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

#include "konepureopticalconfig_cpi_selector.h"
#include "konepureoptical.h"
#include "konepureopticalconfig_single_cpi_selector.h"
#include "roccat_helper.h"
#include "i18n.h"

#define KONEPUREOPTICALCONFIG_CPI_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KONEPUREOPTICALCONFIG_CPI_SELECTOR_TYPE, KonepureopticalconfigCpiSelectorClass))
#define IS_KONEPUREOPTICALCONFIG_CPI_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KONEPUREOPTICALCONFIG_CPI_SELECTOR_TYPE))
#define KONEPUREOPTICALCONFIG_CPI_SELECTOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KONEPUREOPTICALCONFIG_CPI_SELECTOR_TYPE, KonepureopticalconfigCpiSelectorPrivate))

struct _KonepureopticalconfigCpiSelectorPrivate {
	guint count;
	GtkVBox *box;
	KonepureopticalconfigSingleCpiSelector **selectors;
};

enum {
	PROP_0,
	PROP_COUNT,
};

G_DEFINE_TYPE(KonepureopticalconfigCpiSelector, konepureopticalconfig_cpi_selector, GTK_TYPE_FRAME);

static GObject *konepureopticalconfig_cpi_selector_constructor(GType gtype, guint n_properties, GObjectConstructParam *properties) {
	KonepureopticalconfigCpiSelectorPrivate *priv;
	KonepureopticalconfigCpiSelector *selector;
	GObject *obj;
	gchar *label;
	guint i;
	GSList *group;

	obj = G_OBJECT_CLASS(konepureopticalconfig_cpi_selector_parent_class)->constructor(gtype, n_properties, properties);
	selector = KONEPUREOPTICALCONFIG_CPI_SELECTOR(obj);
	priv = selector->priv;

	priv->selectors = (KonepureopticalconfigSingleCpiSelector **)g_malloc0(priv->count * sizeof(KonepureopticalconfigSingleCpiSelector *));

	group = NULL;
	for (i = 0; i < priv->count; ++i) {
		label = g_strdup_printf("%u", i + 1);
		priv->selectors[i] = KONEPUREOPTICALCONFIG_SINGLE_CPI_SELECTOR(konepureopticalconfig_single_cpi_selector_new(label, group));
		g_free(label);
		group = konepureopticalconfig_single_cpi_selector_get_group(priv->selectors[i]);
		gtk_box_pack_start(GTK_BOX(priv->box), GTK_WIDGET(priv->selectors[i]), TRUE, TRUE, 0);
	}

	return obj;
}

GtkWidget *konepureopticalconfig_cpi_selector_new(void) {
	KonepureopticalconfigCpiSelector *selector;

	selector = g_object_new(KONEPUREOPTICALCONFIG_CPI_SELECTOR_TYPE,
			"count", KONEPLUS_PROFILE_SETTING_CPI_LEVELS_NUM,
			NULL);

	return GTK_WIDGET(selector);
}

static void konepureopticalconfig_cpi_selector_init(KonepureopticalconfigCpiSelector *selector) {
	KonepureopticalconfigCpiSelectorPrivate *priv = KONEPUREOPTICALCONFIG_CPI_SELECTOR_GET_PRIVATE(selector);

	selector->priv = priv;

	priv->box = GTK_VBOX(gtk_vbox_new(FALSE, 0));
	gtk_container_add(GTK_CONTAINER(selector), GTK_WIDGET(priv->box));

	gtk_frame_set_label(GTK_FRAME(selector), _("CPI switcher"));
}

static void konepureopticalconfig_cpi_selector_finalize(GObject *object) {
	KonepureopticalconfigCpiSelector *selector = KONEPUREOPTICALCONFIG_CPI_SELECTOR(object);
	g_free(selector->priv->selectors);
	G_OBJECT_CLASS(konepureopticalconfig_cpi_selector_parent_class)->finalize(object);
}

static void konepureopticalconfig_cpi_selector_set_property(GObject *object, guint prop_id, GValue const *value, GParamSpec *pspec) {
	KonepureopticalconfigCpiSelectorPrivate *priv = KONEPUREOPTICALCONFIG_CPI_SELECTOR(object)->priv;
	switch(prop_id) {
	case PROP_COUNT:
		priv->count = g_value_get_uint(value);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
}

static void konepureopticalconfig_cpi_selector_class_init(KonepureopticalconfigCpiSelectorClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->constructor = konepureopticalconfig_cpi_selector_constructor;
	gobject_class->finalize = konepureopticalconfig_cpi_selector_finalize;
	gobject_class->set_property = konepureopticalconfig_cpi_selector_set_property;

	g_type_class_add_private(klass, sizeof(KonepureopticalconfigCpiSelectorPrivate));

	g_object_class_install_property(gobject_class, PROP_COUNT,
			g_param_spec_uint("count",
					"count",
					"Count",
					0, G_MAXUINT, 1,
					G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
}

static guint konepureopticalconfig_cpi_selector_get_value(KonepureopticalconfigCpiSelector *selector, guint index) {
	KonepureopticalconfigCpiSelectorPrivate *priv = selector->priv;
	index = roccat_sanitize_index_max(priv->count, index);
	return konepureopticalconfig_single_cpi_selector_get_value(priv->selectors[index]);
}

static void konepureopticalconfig_cpi_selector_set_value_blocked(KonepureopticalconfigCpiSelector *selector, guint index, guint value) {
	KonepureopticalconfigCpiSelectorPrivate *priv = selector->priv;
	index = roccat_sanitize_index_max(priv->count, index);
	konepureopticalconfig_single_cpi_selector_set_value_blocked(priv->selectors[index], value);
}

static guint konepureopticalconfig_cpi_selector_get_all_active(KonepureopticalconfigCpiSelector *selector) {
	KonepureopticalconfigCpiSelectorPrivate *priv = selector->priv;
	guint i;
	guint bit;
	guint mask;

	bit = 1;
	mask = 0;
	for (i = 0; i < priv->count; ++i) {
		if (konepureopticalconfig_single_cpi_selector_get_active(priv->selectors[i]))
			mask |= bit;
		bit <<= 1;
	}

	return mask;
}

static void konepureopticalconfig_cpi_selector_set_all_active(KonepureopticalconfigCpiSelector *selector, guint mask) {
	KonepureopticalconfigCpiSelectorPrivate *priv = selector->priv;
	guint i;
	guint bit;

	bit = 1;
	for (i = 0; i < priv->count; ++i) {
		konepureopticalconfig_single_cpi_selector_set_active(priv->selectors[i], bit & mask);
		bit <<= 1;
	}
}

static guint konepureopticalconfig_cpi_selector_get_selected(KonepureopticalconfigCpiSelector *selector) {
	KonepureopticalconfigCpiSelectorPrivate *priv = selector->priv;
	guint i;

	for (i = 0; i < priv->count; ++i)
		if (konepureopticalconfig_single_cpi_selector_get_selected(priv->selectors[i]))
			return i;

	return 0;
}

static void konepureopticalconfig_cpi_selector_set_selected(KonepureopticalconfigCpiSelector *selector, guint index) {
	KonepureopticalconfigCpiSelectorPrivate *priv = selector->priv;
	index = roccat_sanitize_index_max(priv->count, index);
	konepureopticalconfig_single_cpi_selector_set_selected(priv->selectors[index], TRUE);
}

void konepureopticalconfig_cpi_selector_set_from_rmp(KonepureopticalconfigCpiSelector *selector, KoneplusRmp *rmp) {
	guint i;

	konepureopticalconfig_cpi_selector_set_selected(selector, koneplus_rmp_get_cpi_x(rmp));
	konepureopticalconfig_cpi_selector_set_all_active(selector, koneplus_rmp_get_cpi_all(rmp));
	for (i = 0; i < KONEPLUS_PROFILE_SETTING_CPI_LEVELS_NUM; ++i)
		konepureopticalconfig_cpi_selector_set_value_blocked(selector, i, koneplus_rmp_get_cpi_level_x(rmp, i));
}

void konepureopticalconfig_cpi_selector_update_rmp(KonepureopticalconfigCpiSelector *selector, KoneplusRmp *rmp) {
	guint i;

	koneplus_rmp_set_cpi_x(rmp, konepureopticalconfig_cpi_selector_get_selected(selector));
	koneplus_rmp_set_cpi_y(rmp, konepureopticalconfig_cpi_selector_get_selected(selector));
	koneplus_rmp_set_cpi_all(rmp, konepureopticalconfig_cpi_selector_get_all_active(selector));
	for (i = 0; i < KONEPLUS_PROFILE_SETTING_CPI_LEVELS_NUM; ++i) {
		koneplus_rmp_set_cpi_level_x(rmp, i, konepureopticalconfig_cpi_selector_get_value(selector, i));
		koneplus_rmp_set_cpi_level_y(rmp, i, konepureopticalconfig_cpi_selector_get_value(selector, i));
	}
}
