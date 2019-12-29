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

#include "roccat_single_cpi_selector.h"
#include <gaminggear/gaminggear_hscale.h>

#define ROCCAT_SINGLE_CPI_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ROCCAT_SINGLE_CPI_SELECTOR_TYPE, RoccatSingleCpiSelectorClass))
#define IS_ROCCAT_SINGLE_CPI_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ROCCAT_SINGLE_CPI_SELECTOR_TYPE))
#define ROCCAT_SINGLE_CPI_SELECTOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ROCCAT_SINGLE_CPI_SELECTOR_TYPE, RoccatSingleCpiSelectorPrivate))

typedef struct _RoccatSingleCpiSelectorClass RoccatSingleCpiSelectorClass;

struct _RoccatSingleCpiSelectorClass {
	GtkFrameClass parent_class;
};

struct _RoccatSingleCpiSelectorPrivate {
	GtkRadioButton *radio;
	GtkCheckButton *active;
	GaminggearHScale *scale;
};

G_DEFINE_TYPE(RoccatSingleCpiSelector, roccat_single_cpi_selector, GTK_TYPE_FRAME);

GSList *roccat_single_cpi_selector_get_group(RoccatSingleCpiSelector *selector) {
	return gtk_radio_button_get_group(selector->priv->radio);
}

void roccat_single_cpi_selector_set_group(RoccatSingleCpiSelector *selector, GSList *group) {
	gtk_radio_button_set_group(selector->priv->radio, group);
}

void roccat_single_cpi_selector_set_range(RoccatSingleCpiSelector *selector, gdouble min, gdouble max) {
	RoccatSingleCpiSelectorPrivate *priv = selector->priv;
	gdouble i;
	gaminggear_hscale_set_range(priv->scale, min, max);
	gaminggear_hscale_clear_marks(priv->scale);
	for (i = max; i >= min; i -= 1000.0)
		gaminggear_hscale_add_mark(priv->scale, i, GTK_POS_BOTTOM, NULL);
}

void roccat_single_cpi_selector_set_increment(RoccatSingleCpiSelector *selector, gdouble step) {
	gaminggear_hscale_set_increment(selector->priv->scale, step);
}

gdouble roccat_single_cpi_selector_get_value(RoccatSingleCpiSelector *selector) {
	return gaminggear_hscale_get_value(selector->priv->scale);
}

void roccat_single_cpi_selector_set_value(RoccatSingleCpiSelector *selector, gdouble value) {
	gaminggear_hscale_set_value(selector->priv->scale, value);
}

gboolean roccat_single_cpi_selector_get_active(RoccatSingleCpiSelector *selector) {
	return gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(selector->priv->active));
}

void roccat_single_cpi_selector_set_active(RoccatSingleCpiSelector *selector, gboolean active) {
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(selector->priv->active), active);
}

gboolean roccat_single_cpi_selector_get_selected(RoccatSingleCpiSelector *selector) {
	return gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(selector->priv->radio));
}

void roccat_single_cpi_selector_set_selected(RoccatSingleCpiSelector *selector, gboolean active) {
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(selector->priv->radio), active);
}

GtkWidget *roccat_single_cpi_selector_new(gchar const *label, GSList *group, gdouble min, gdouble max, gdouble step) {
	RoccatSingleCpiSelector *selector;

	selector = g_object_new(ROCCAT_SINGLE_CPI_SELECTOR_TYPE,
			"label", label,
			NULL);

	roccat_single_cpi_selector_set_group(selector, group);
	roccat_single_cpi_selector_set_range(selector, min, max);
	roccat_single_cpi_selector_set_increment(selector, step);

	return GTK_WIDGET(selector);
}

static void click_radio_cb(GtkToggleButton *button, gpointer user_data) {
	RoccatSingleCpiSelector *selector = (RoccatSingleCpiSelector *)user_data;
	RoccatSingleCpiSelectorPrivate *priv = selector->priv;

	/* make sure selected is activated */
	if (gtk_toggle_button_get_active(button) && !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(priv->active)))
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->active), TRUE);
}

static void toggle_active_cb(GtkToggleButton *button, gpointer user_data) {
	RoccatSingleCpiSelector *selector = (RoccatSingleCpiSelector *)user_data;
	RoccatSingleCpiSelectorPrivate *priv = selector->priv;
	gboolean new_state = gtk_toggle_button_get_active(button);

	/* selected can't be deactivated */
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(priv->radio)) && !new_state)
		gtk_toggle_button_set_active(button, TRUE);
}

static void roccat_single_cpi_selector_init(RoccatSingleCpiSelector *selector) {
	RoccatSingleCpiSelectorPrivate *priv = ROCCAT_SINGLE_CPI_SELECTOR_GET_PRIVATE(selector);
	GtkHBox *hbox;

	selector->priv = priv;

	hbox = GTK_HBOX(gtk_hbox_new(FALSE, 0));
	gtk_container_add(GTK_CONTAINER(selector), GTK_WIDGET(hbox));

	priv->radio = GTK_RADIO_BUTTON(gtk_radio_button_new(NULL));
	gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(priv->radio), FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(priv->radio), "toggled", G_CALLBACK(click_radio_cb), selector);

	priv->active = GTK_CHECK_BUTTON(gtk_check_button_new());
	gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(priv->active), FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(priv->active), "toggled", G_CALLBACK(toggle_active_cb), selector);

	priv->scale = GAMINGGEAR_HSCALE(gaminggear_hscale_new_with_range(0, 1, 1));
	gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(priv->scale), TRUE, TRUE, 0);
}

static void roccat_single_cpi_selector_class_init(RoccatSingleCpiSelectorClass *klass) {
	g_type_class_add_private(klass, sizeof(RoccatSingleCpiSelectorPrivate));
}
