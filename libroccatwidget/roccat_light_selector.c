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

#include "roccat_light_selector.h"
#include "i18n-lib.h"
#include <gaminggear/gaminggear_hscale.h>

#define ROCCAT_LIGHT_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ROCCAT_LIGHT_SELECTOR_TYPE, RoccatLightSelectorClass))
#define IS_ROCCAT_LIGHT_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ROCCAT_LIGHT_SELECTOR_TYPE))
#define ROCCAT_LIGHT_SELECTOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ROCCAT_LIGHT_SELECTOR_TYPE, RoccatLightSelectorPrivate))

typedef struct _RoccatLightSelectorClass RoccatLightSelectorClass;
typedef struct _RoccatLightSelectorPrivate RoccatLightSelectorPrivate;

struct _RoccatLightSelector {
	GtkTable parent;
	RoccatLightSelectorPrivate *priv;
};

struct _RoccatLightSelectorClass {
	GtkTableClass parent_class;
};

struct _RoccatLightSelectorPrivate {
	GaminggearHScale *brightness;
	GaminggearHScale *dimness;
	GtkSpinButton *timeout;
};

G_DEFINE_TYPE(RoccatLightSelector, roccat_light_selector, GTK_TYPE_TABLE);

void roccat_light_selector_set_brightness(RoccatLightSelector *selector, guint new_brightness) {
	gaminggear_hscale_set_value(selector->priv->brightness, new_brightness);
}

guint roccat_light_selector_get_brightness(RoccatLightSelector *selector) {
	return gaminggear_hscale_get_value(selector->priv->brightness);
}

void roccat_light_selector_set_dimness(RoccatLightSelector *selector, guint new_dimness) {
	gaminggear_hscale_set_value(selector->priv->dimness, new_dimness);
}

guint roccat_light_selector_get_dimness(RoccatLightSelector *selector) {
	return gaminggear_hscale_get_value(selector->priv->dimness);
}

void roccat_light_selector_set_timeout(RoccatLightSelector *selector, guint timeout) {
	gtk_spin_button_set_value(selector->priv->timeout, timeout);
}

guint roccat_light_selector_get_timeout(RoccatLightSelector *selector) {
	return gtk_spin_button_get_value(selector->priv->timeout);
}

GtkWidget *roccat_light_selector_new_with_range(gdouble min, gdouble max, gdouble step) {
	RoccatLightSelector *roccat_light_selector;

	roccat_light_selector = ROCCAT_LIGHT_SELECTOR(g_object_new(ROCCAT_LIGHT_SELECTOR_TYPE,
			NULL));

	roccat_light_selector_set_range(roccat_light_selector, min, max);
	roccat_light_selector_set_increment(roccat_light_selector, step);

	return GTK_WIDGET(roccat_light_selector);
}

static void roccat_light_selector_init(RoccatLightSelector *selector) {
	RoccatLightSelectorPrivate *priv = ROCCAT_LIGHT_SELECTOR_GET_PRIVATE(selector);
	selector->priv = priv;

	gtk_table_set_homogeneous(GTK_TABLE(selector), FALSE);
	gtk_table_resize(GTK_TABLE(selector), 3, 2);

	priv->brightness = GAMINGGEAR_HSCALE(gaminggear_hscale_new_with_range(0.0, 1.0, 1.0));
	priv->dimness = GAMINGGEAR_HSCALE(gaminggear_hscale_new_with_range(0.0, 1.0, 1.0));
	priv->timeout = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(1, G_MAXUINT8, 1));

	gtk_table_attach(GTK_TABLE(selector), gtk_label_new(_("Brightness")), 0, 1, 0, 1, GTK_SHRINK, GTK_EXPAND, 0, 0);
	gtk_table_attach(GTK_TABLE(selector), gtk_label_new(_("Dimness")), 0, 1, 1, 2, GTK_SHRINK, GTK_EXPAND, 0, 0);
	gtk_table_attach(GTK_TABLE(selector), gtk_label_new(_("Timeout")), 0, 1, 2, 3, GTK_SHRINK, GTK_EXPAND, 0, 0);
	gtk_table_attach(GTK_TABLE(selector), GTK_WIDGET(priv->brightness), 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);
	gtk_table_attach(GTK_TABLE(selector), GTK_WIDGET(priv->dimness), 1, 2, 1, 2, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);
	gtk_table_attach(GTK_TABLE(selector), GTK_WIDGET(priv->timeout), 1, 2, 2, 3, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);
}

static void roccat_light_selector_class_init(RoccatLightSelectorClass *klass) {
	g_type_class_add_private(klass, sizeof(RoccatLightSelectorPrivate));
}

void roccat_light_selector_set_range(RoccatLightSelector *selector, gdouble min, gdouble max) {
	gaminggear_hscale_set_range(selector->priv->brightness, min, max);
	gaminggear_hscale_set_range(selector->priv->dimness, min, max);
}

void roccat_light_selector_set_increment(RoccatLightSelector *selector, gdouble step) {
	gaminggear_hscale_set_increment(selector->priv->brightness, step);
	gaminggear_hscale_set_increment(selector->priv->dimness, step);
}
