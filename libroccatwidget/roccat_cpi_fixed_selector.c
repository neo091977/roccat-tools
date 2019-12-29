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

#include "roccat_cpi_fixed_selector.h"
#include "roccat_helper.h"
#include "i18n-lib.h"

#define ROCCAT_CPI_FIXED_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ROCCAT_CPI_FIXED_SELECTOR_TYPE, RoccatCpiFixedSelectorClass))
#define IS_ROCCAT_CPI_FIXED_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ROCCAT_CPI_FIXED_SELECTOR_TYPE))
#define ROCCAT_CPI_FIXED_SELECTOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ROCCAT_CPI_FIXED_SELECTOR_TYPE, RoccatCpiFixedSelectorPrivate))

struct _RoccatCpiFixedSelectorPrivate {
	gboolean with_buttons;
	guint count;
	GtkTable *table;
	GtkRadioButton **radios;
	GtkCheckButton **buttons;
	guint const * values;
};

enum {
	PROP_0,
	PROP_COUNT,
	PROP_VALUES,
	PROP_WITH_BUTTONS,
};

static gchar const * const index_key = "index";

G_DEFINE_TYPE(RoccatCpiFixedSelector, roccat_cpi_fixed_selector, GTK_TYPE_FRAME);

gboolean roccat_cpi_fixed_selector_get_active(RoccatCpiFixedSelector *selector, guint index) {
	RoccatCpiFixedSelectorPrivate *priv = selector->priv;
	if (priv->with_buttons) {
		index = roccat_sanitize_index_max(priv->count, index);
		return gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(priv->buttons[index]));
	} else
		return TRUE;
}

void roccat_cpi_fixed_selector_set_active(RoccatCpiFixedSelector *selector, guint index, gboolean state) {
	RoccatCpiFixedSelectorPrivate *priv = selector->priv;
	if (priv->with_buttons) {
		index = roccat_sanitize_index_max(priv->count, index);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->buttons[index]), state);
	}
}

guint roccat_cpi_fixed_selector_get_all_active(RoccatCpiFixedSelector *selector) {
	RoccatCpiFixedSelectorPrivate *priv = selector->priv;
	guint i;
	guint bit;
	guint mask;

	bit = 1;
	mask = 0;
	for (i = 0; i < priv->count; ++i) {
		if (roccat_cpi_fixed_selector_get_active(selector, i))
			mask |= bit;
		bit <<= 1;
	}

	return mask;
}

void roccat_cpi_fixed_selector_set_all_active(RoccatCpiFixedSelector *selector, guint mask) {
	RoccatCpiFixedSelectorPrivate *priv = selector->priv;
	guint i;
	guint bit;

	bit = 1;
	for (i = 0; i < priv->count; ++i) {
		roccat_cpi_fixed_selector_set_active(selector, i, bit & mask);
		bit <<= 1;
	}
}

guint roccat_cpi_fixed_selector_get_selected(RoccatCpiFixedSelector *selector) {
	RoccatCpiFixedSelectorPrivate *priv = selector->priv;
	guint i;

	for (i = 0; i < priv->count; ++i)
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(priv->radios[i])))
			return i;

	return 0;
}

void roccat_cpi_fixed_selector_set_selected(RoccatCpiFixedSelector *selector, guint index) {
	RoccatCpiFixedSelectorPrivate *priv = selector->priv;
	index = roccat_sanitize_index_max(priv->count, index);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->radios[index]), TRUE);
}

static void click_radio_cb(GtkToggleButton *button, gpointer user_data) {
	RoccatCpiFixedSelector *selector = (RoccatCpiFixedSelector *)user_data;
	RoccatCpiFixedSelectorPrivate *priv = selector->priv;
	guint index = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(button), index_key));

	/* make sure selected is activated */
	if (priv->with_buttons && gtk_toggle_button_get_active(button) &&
			!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(priv->buttons[index])))
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->buttons[index]), TRUE);
}

static void toggle_active_cb(GtkToggleButton *button, gpointer user_data) {
	RoccatCpiFixedSelector *selector = (RoccatCpiFixedSelector *)user_data;
	RoccatCpiFixedSelectorPrivate *priv = selector->priv;
	guint index = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(button), index_key));
	gboolean new_state = gtk_toggle_button_get_active(button);

	/* selected can't be deactivated */
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(priv->radios[index])) && !new_state)
		gtk_toggle_button_set_active(button, TRUE);
}

static GObject *roccat_cpi_fixed_selector_constructor(GType gtype, guint n_properties, GObjectConstructParam *properties) {
	RoccatCpiFixedSelectorPrivate *priv;
	RoccatCpiFixedSelector *selector;
	GObject *obj;
	guint i;
	guint buttonrow;
	guint labelrow;
	guint radiorow;
	gchar *string;
	GSList *group;

	obj = G_OBJECT_CLASS(roccat_cpi_fixed_selector_parent_class)->constructor(gtype, n_properties, properties);
	selector = ROCCAT_CPI_FIXED_SELECTOR(obj);
	priv = selector->priv;

	priv->radios = (GtkRadioButton **)g_malloc0(priv->count * sizeof(GtkRadioButton *));

	if (priv->with_buttons) {
		priv->buttons = (GtkCheckButton **)g_malloc0(priv->count * sizeof(GtkCheckButton *));
		buttonrow = 0;
		labelrow = 1;
		radiorow = 2;
	} else {
		labelrow = 1;
		radiorow = 0;
	}

	group = NULL;
	for (i = 0; i < priv->count; ++i) {
		if (priv->with_buttons) {
			priv->buttons[i] = GTK_CHECK_BUTTON(gtk_check_button_new());
			g_object_set_data(G_OBJECT(priv->buttons[i]), index_key, GUINT_TO_POINTER(i));
			g_signal_connect(G_OBJECT(priv->buttons[i]), "toggled", G_CALLBACK(toggle_active_cb), selector);
			gtk_table_attach(priv->table, GTK_WIDGET(priv->buttons[i]), i, i + 1, buttonrow,  buttonrow + 1, GTK_EXPAND, GTK_EXPAND, 0, 0);
		}

		string = g_strdup_printf("%u", priv->values[i]);
		gtk_table_attach(priv->table, gtk_label_new(string), i, i + 1, labelrow, labelrow + 1, GTK_EXPAND, GTK_EXPAND, 5, 0);
		g_free(string);

		priv->radios[i] = GTK_RADIO_BUTTON(gtk_radio_button_new(group));
		g_object_set_data(G_OBJECT(priv->radios[i]), index_key, GUINT_TO_POINTER(i));
		g_signal_connect(G_OBJECT(priv->radios[i]), "toggled", G_CALLBACK(click_radio_cb), selector);
		group = gtk_radio_button_get_group(priv->radios[i]);
		gtk_table_attach(priv->table, GTK_WIDGET(priv->radios[i]), i, i + 1, radiorow, radiorow + 1, GTK_EXPAND, GTK_EXPAND, 0, 0);
	}

	return obj;
}

GtkWidget *roccat_cpi_fixed_selector_new(guint count, guint const * const values, gboolean with_buttons) {
	RoccatCpiFixedSelector *selector;

	selector = g_object_new(ROCCAT_CPI_FIXED_SELECTOR_TYPE,
			"count", count,
			"values", values,
			"with-buttons", with_buttons,
			NULL);

	return GTK_WIDGET(selector);
}

static void roccat_cpi_fixed_selector_init(RoccatCpiFixedSelector *selector) {
	RoccatCpiFixedSelectorPrivate *priv = ROCCAT_CPI_FIXED_SELECTOR_GET_PRIVATE(selector);

	selector->priv = priv;

	priv->table = GTK_TABLE(gtk_table_new(1, 1, TRUE));
	gtk_container_add(GTK_CONTAINER(selector), GTK_WIDGET(priv->table));

	gtk_frame_set_label(GTK_FRAME(selector), _("CPI switcher"));
}

static void roccat_cpi_fixed_selector_finalize(GObject *object) {
	RoccatCpiFixedSelectorPrivate *priv = ROCCAT_CPI_FIXED_SELECTOR(object)->priv;
	if (priv->with_buttons)
		g_free(priv->buttons);
	g_free(priv->radios);
	G_OBJECT_CLASS(roccat_cpi_fixed_selector_parent_class)->finalize(object);
}

static void roccat_cpi_fixed_selector_set_property(GObject *object, guint prop_id, GValue const *value, GParamSpec *pspec) {
	RoccatCpiFixedSelectorPrivate *priv = ROCCAT_CPI_FIXED_SELECTOR(object)->priv;
	switch(prop_id) {
	case PROP_COUNT:
		priv->count = g_value_get_uint(value);
		break;
	case PROP_VALUES:
		priv->values = g_value_get_pointer(value);
		break;
	case PROP_WITH_BUTTONS:
		priv->with_buttons = g_value_get_boolean(value);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
}
static void roccat_cpi_fixed_selector_class_init(RoccatCpiFixedSelectorClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = (GObjectClass *)klass;

	gobject_class->constructor = roccat_cpi_fixed_selector_constructor;
	gobject_class->finalize = roccat_cpi_fixed_selector_finalize;
	gobject_class->set_property = roccat_cpi_fixed_selector_set_property;

	g_type_class_add_private(klass, sizeof(RoccatCpiFixedSelectorPrivate));

	g_object_class_install_property(gobject_class, PROP_COUNT,
			g_param_spec_uint("count", "count", "Count",
					0,
					G_MAXUINT,
					1,
					G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));

	g_object_class_install_property(gobject_class, PROP_VALUES,
			g_param_spec_pointer("values",
					"values",
					"Values",
					G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));

	g_object_class_install_property(gobject_class, PROP_WITH_BUTTONS,
			g_param_spec_boolean("with-buttons",
					"with buttons",
					"With buttons",
					TRUE,
					G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
}
