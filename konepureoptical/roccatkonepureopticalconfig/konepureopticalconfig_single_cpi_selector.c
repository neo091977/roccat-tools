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

#include "konepureopticalconfig_single_cpi_selector.h"
#include "konepureopticalconfig_cpi_scale.h"

#define KONEPUREOPTICALCONFIG_SINGLE_CPI_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KONEPUREOPTICALCONFIG_SINGLE_CPI_SELECTOR_TYPE, KonepureopticalconfigSingleCpiSelectorClass))
#define IS_KONEPUREOPTICALCONFIG_SINGLE_CPI_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KONEPUREOPTICALCONFIG_SINGLE_CPI_SELECTOR_TYPE))
#define KONEPUREOPTICALCONFIG_SINGLE_CPI_SELECTOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KONEPUREOPTICALCONFIG_SINGLE_CPI_SELECTOR_TYPE, KonepureopticalconfigSingleCpiSelectorPrivate))

typedef struct _KonepureopticalconfigSingleCpiSelectorClass KonepureopticalconfigSingleCpiSelectorClass;

struct _KonepureopticalconfigSingleCpiSelectorClass {
	GtkFrameClass parent_class;
};

struct _KonepureopticalconfigSingleCpiSelectorPrivate {
	GtkRadioButton *radio;
	GtkCheckButton *active;
	KonepureopticalconfigCpiScale *scale;
};

G_DEFINE_TYPE(KonepureopticalconfigSingleCpiSelector, konepureopticalconfig_single_cpi_selector, GTK_TYPE_FRAME);

GSList *konepureopticalconfig_single_cpi_selector_get_group(KonepureopticalconfigSingleCpiSelector *selector) {
	return gtk_radio_button_get_group(selector->priv->radio);
}

void konepureopticalconfig_single_cpi_selector_set_group(KonepureopticalconfigSingleCpiSelector *selector, GSList *group) {
	gtk_radio_button_set_group(selector->priv->radio, group);
}

guint konepureopticalconfig_single_cpi_selector_get_value(KonepureopticalconfigSingleCpiSelector *selector) {
	return konepureopticalconfig_cpi_scale_get_value(selector->priv->scale);
}

void konepureopticalconfig_single_cpi_selector_set_value_blocked(KonepureopticalconfigSingleCpiSelector *selector, guint value) {
	konepureopticalconfig_cpi_scale_set_value_blocked(selector->priv->scale, value);
}

gboolean konepureopticalconfig_single_cpi_selector_get_active(KonepureopticalconfigSingleCpiSelector *selector) {
	return gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(selector->priv->active));
}

void konepureopticalconfig_single_cpi_selector_set_active(KonepureopticalconfigSingleCpiSelector *selector, gboolean active) {
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(selector->priv->active), active);
}

gboolean konepureopticalconfig_single_cpi_selector_get_selected(KonepureopticalconfigSingleCpiSelector *selector) {
	return gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(selector->priv->radio));
}

void konepureopticalconfig_single_cpi_selector_set_selected(KonepureopticalconfigSingleCpiSelector *selector, gboolean active) {
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(selector->priv->radio), active);
}

GtkWidget *konepureopticalconfig_single_cpi_selector_new(gchar const *label, GSList *group) {
	KonepureopticalconfigSingleCpiSelector *selector;

	selector = g_object_new(KONEPUREOPTICALCONFIG_SINGLE_CPI_SELECTOR_TYPE,
			"label", label,
			NULL);

	konepureopticalconfig_single_cpi_selector_set_group(selector, group);

	return GTK_WIDGET(selector);
}

static void click_radio_cb(GtkToggleButton *button, gpointer user_data) {
	KonepureopticalconfigSingleCpiSelector *selector = (KonepureopticalconfigSingleCpiSelector *)user_data;
	KonepureopticalconfigSingleCpiSelectorPrivate *priv = selector->priv;

	/* make sure selected is activated */
	if (gtk_toggle_button_get_active(button) && !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(priv->active)))
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->active), TRUE);
}

static void toggle_active_cb(GtkToggleButton *button, gpointer user_data) {
	KonepureopticalconfigSingleCpiSelector *selector = (KonepureopticalconfigSingleCpiSelector *)user_data;
	KonepureopticalconfigSingleCpiSelectorPrivate *priv = selector->priv;
	gboolean new_state = gtk_toggle_button_get_active(button);

	/* selected can't be deactivated */
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(priv->radio)) && !new_state)
		gtk_toggle_button_set_active(button, TRUE);
}

static void konepureopticalconfig_single_cpi_selector_init(KonepureopticalconfigSingleCpiSelector *selector) {
	KonepureopticalconfigSingleCpiSelectorPrivate *priv = KONEPUREOPTICALCONFIG_SINGLE_CPI_SELECTOR_GET_PRIVATE(selector);
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

	priv->scale = KONEPUREOPTICALCONFIG_CPI_SCALE(konepureopticalconfig_cpi_scale_new());
	gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(priv->scale), TRUE, TRUE, 0);
}

static void konepureopticalconfig_single_cpi_selector_class_init(KonepureopticalconfigSingleCpiSelectorClass *klass) {
	g_type_class_add_private(klass, sizeof(KonepureopticalconfigSingleCpiSelectorPrivate));
}
