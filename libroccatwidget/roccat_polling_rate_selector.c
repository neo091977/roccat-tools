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

#include "roccat_polling_rate_selector.h"
#include "gtk_roccat_helper.h"
#include "i18n-lib.h"

// TODO add toggle handler
// TODO make labels clickable

#define ROCCAT_POLLING_RATE_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ROCCAT_POLLING_RATE_SELECTOR_TYPE, RoccatPollingRateSelectorClass))
#define IS_ROCCAT_POLLING_RATE_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ROCCAT_POLLING_RATE_SELECTOR_TYPE))
#define ROCCAT_POLLING_RATE_SELECTOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ROCCAT_POLLING_RATE_SELECTOR_TYPE, RoccatPollingRateSelectorPrivate))

struct _RoccatPollingRateSelectorPrivate {
	GSList *radios;
	guint mask;
};

enum {
	PROP_0,
	PROP_VALUE,
	PROP_MASK,
};

enum {
	ROCCAT_POLLING_RATE_SELECTOR_NUM = 4,
};

static gchar const * const value_key = "Value";

G_DEFINE_TYPE(RoccatPollingRateSelector, roccat_polling_rate_selector, GTK_TYPE_TABLE);

guint roccat_polling_rate_selector_get_mask(RoccatPollingRateSelector *selector) {
	return selector->priv->mask;
}

static void roccat_polling_rate_selector_set_mask(RoccatPollingRateSelector *selector, guint value) {
	selector->priv->mask = value;
}

guint roccat_polling_rate_selector_get_value(RoccatPollingRateSelector *selector) {
	GtkWidget *active;

	active = gtk_roccat_radio_button_group_get_active(selector->priv->radios);
	if (active)
		return GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(active), value_key));
	else
		return ROCCAT_POLLING_RATE_SELECTOR_NONE;
}

void roccat_polling_rate_selector_set_value(RoccatPollingRateSelector *selector, guint new_value) {
	GSList *element;
	guint value;

	for (element = selector->priv->radios; element; element = g_slist_next(element)) {
		value = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(element->data), value_key));
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(element->data), value == new_value);
	}
}

static GObject *roccat_polling_rate_selector_constructor(GType gtype, guint n_properties, GObjectConstructParam *properties) {
	RoccatPollingRateSelectorPrivate *priv;
	GObject *obj;
	GtkTable *table;
	GtkWidget *radio;
	guint i;
	guint column_number;

	struct {
		guint value;
		gchar *title;
	} radios[ROCCAT_POLLING_RATE_SELECTOR_NUM] = {
			{ROCCAT_POLLING_RATE_SELECTOR_125, "125"},
			{ROCCAT_POLLING_RATE_SELECTOR_250, "250"},
			{ROCCAT_POLLING_RATE_SELECTOR_500, "500"},
			{ROCCAT_POLLING_RATE_SELECTOR_1000, "1000"},
	};

	obj = G_OBJECT_CLASS(roccat_polling_rate_selector_parent_class)->constructor(gtype, n_properties, properties);
	table = GTK_TABLE(obj);
	priv = ROCCAT_POLLING_RATE_SELECTOR(obj)->priv;

	priv->radios = NULL;
	column_number = 0;
	for (i = 0; i < ROCCAT_POLLING_RATE_SELECTOR_NUM; ++i) {
		if (radios[i].value & priv->mask) {
			radio = gtk_radio_button_new(priv->radios);
			g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(radios[i].value));
			priv->radios = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio));

			gtk_table_attach(table, radio, column_number, column_number + 1, 0, 1, GTK_EXPAND, GTK_EXPAND, 0, 0);
			gtk_table_attach(table, gtk_label_new(radios[i].title), column_number, column_number + 1, 1, 2, GTK_EXPAND, GTK_EXPAND, 0, 0);

			++column_number;
		}
	}
	return obj;
}

GtkWidget *roccat_polling_rate_selector_new(guint mask) {
	RoccatPollingRateSelector *selector;

	selector = g_object_new(ROCCAT_POLLING_RATE_SELECTOR_TYPE,
			"mask", mask,
			NULL);

	return GTK_WIDGET(selector);
}

static void roccat_polling_rate_selector_set_property(GObject *object, guint prop_id, GValue const *value, GParamSpec *pspec) {
	RoccatPollingRateSelector *selector = ROCCAT_POLLING_RATE_SELECTOR(object);

	switch(prop_id) {
	case PROP_VALUE:
		roccat_polling_rate_selector_set_value(selector, g_value_get_uint(value));
		break;
	case PROP_MASK:
		roccat_polling_rate_selector_set_mask(selector, g_value_get_uint(value));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
	}
}

static void roccat_polling_rate_selector_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec) {
	RoccatPollingRateSelector *selector = ROCCAT_POLLING_RATE_SELECTOR(object);

	switch(prop_id) {
	case PROP_VALUE:
		g_value_set_uint(value, roccat_polling_rate_selector_get_value(selector));
		break;
	case PROP_MASK:
		g_value_set_uint(value, roccat_polling_rate_selector_get_mask(selector));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
	}
}

static void roccat_polling_rate_selector_init(RoccatPollingRateSelector *selector) {
	selector->priv = ROCCAT_POLLING_RATE_SELECTOR_GET_PRIVATE(selector);
}

static void roccat_polling_rate_selector_class_init(RoccatPollingRateSelectorClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = (GObjectClass *)klass;

	gobject_class->constructor = roccat_polling_rate_selector_constructor;
	gobject_class->set_property = roccat_polling_rate_selector_set_property;
	gobject_class->get_property = roccat_polling_rate_selector_get_property;

	g_type_class_add_private(klass, sizeof(RoccatPollingRateSelectorPrivate));

	g_object_class_install_property(gobject_class, PROP_VALUE,
			g_param_spec_uint("value", "value", "Reads or sets value",
					ROCCAT_POLLING_RATE_SELECTOR_NONE,
					ROCCAT_POLLING_RATE_SELECTOR_1000,
					ROCCAT_POLLING_RATE_SELECTOR_NONE,
					G_PARAM_READWRITE));

	g_object_class_install_property(gobject_class, PROP_MASK,
			g_param_spec_uint("mask", "mask", "Reads or sets mask",
					ROCCAT_POLLING_RATE_SELECTOR_NONE,
					ROCCAT_POLLING_RATE_SELECTOR_ALL,
					ROCCAT_POLLING_RATE_SELECTOR_ALL,
					G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
}
