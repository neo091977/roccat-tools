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

#include <gaminggear/gdk_key_translations.h>
#include <gaminggear/hid_uid.h>
#include "roccat_key_button.h"
#include "gdk_roccat_helper.h"

#define ROCCAT_KEY_BUTTON_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ROCCAT_KEY_BUTTON_TYPE, RoccatKeyButtonClass))
#define IS_ROCCAT_KEY_BUTTON_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ROCCAT_KEY_BUTTON_TYPE))
#define ROCCAT_KEY_BUTTON_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ROCCAT_KEY_BUTTON_TYPE, RoccatKeyButtonPrivate))

static double const roccat_key_button_border_factor = 0.2;

typedef struct _RoccatKeyButtonClass RoccatKeyButtonClass;
typedef struct _RoccatKeyButtonPrivate RoccatKeyButtonPrivate;

struct _RoccatKeyButton {
	GtkRadioButton parent;
	RoccatKeyButtonPrivate *priv;
};

struct _RoccatKeyButtonClass {
	GtkRadioButtonClass parent_class;
};

struct _RoccatKeyButtonPrivate {
	GtkWidget *draw_area;
	roccat_key_button_name_converter converter;
	GtkLabel *label;
	gboolean light_on;
	GdkColor color;
	double intensity;
	double active_red;
	double active_green;
	double active_blue;
};

G_DEFINE_TYPE(RoccatKeyButton, roccat_key_button, GTK_TYPE_RADIO_BUTTON);

static void calculate_active_color(RoccatKeyButton *key_button) {
	RoccatKeyButtonPrivate *priv = key_button->priv;

	priv->active_red = (double)priv->color.red * priv->intensity / (double)G_MAXUINT16;
	priv->active_green = (double)priv->color.green * priv->intensity / (double)G_MAXUINT16;
	priv->active_blue = (double)priv->color.blue * priv->intensity / (double)G_MAXUINT16;

	gtk_widget_queue_draw(priv->draw_area);
}

static gboolean expose_cb(GtkWidget *widget, GdkEventExpose *event, gpointer user_data) {
	RoccatKeyButtonPrivate *priv = ROCCAT_KEY_BUTTON(user_data)->priv;
	GtkAllocation allocation;
	cairo_t *cr;
	double border_width;

	if (!priv->light_on)
		return FALSE;

	gtk_widget_get_allocation(widget, &allocation);

	cr = gdk_cairo_create(event->window);

	border_width = MIN(allocation.width, allocation.height) * roccat_key_button_border_factor;

	cairo_move_to(cr, allocation.x,                                   allocation.y);
	cairo_line_to(cr, allocation.x + allocation.width,                allocation.y);
	cairo_line_to(cr, allocation.x + allocation.width,                allocation.y + allocation.height);
	cairo_line_to(cr, allocation.x,                                   allocation.y + allocation.height);
	cairo_line_to(cr, allocation.x,                                   allocation.y);
	cairo_line_to(cr, allocation.x + border_width,                    allocation.y + border_width);
	cairo_line_to(cr, allocation.x + border_width,                    allocation.y + allocation.height - border_width);
	cairo_line_to(cr, allocation.x + allocation.width - border_width, allocation.y + allocation.height - border_width);
	cairo_line_to(cr, allocation.x + allocation.width - border_width, allocation.y + border_width);
	cairo_line_to(cr, allocation.x + border_width,                    allocation.y + border_width);
	cairo_close_path(cr);

	cairo_clip(cr);
	cairo_set_source_rgb(cr, priv->active_red, priv->active_green, priv->active_blue);
	cairo_paint(cr);
	cairo_destroy(cr);

	return FALSE;
}

static void button_size_request_cb(GtkWidget *button, GtkRequisition *requisition, gpointer user_data) {
	requisition->width = requisition->height;
}

void roccat_key_button_set_intensity(RoccatKeyButton *key_button, gfloat intensity) {
	key_button->priv->intensity = intensity;
	calculate_active_color(key_button);
}

void roccat_key_button_set_color(RoccatKeyButton *key_button, GdkColor const *color) {
	key_button->priv->color = *color;
	calculate_active_color(key_button);
}

void roccat_key_button_set_light(RoccatKeyButton *key_button, gboolean state) {
	key_button->priv->light_on = state;
	gtk_widget_queue_draw(key_button->priv->draw_area);
}

void roccat_key_button_set_label(RoccatKeyButton *key_button, guint hid_usage_id) {
	RoccatKeyButtonPrivate *priv = key_button->priv;
	gchar *label_text;

	label_text = NULL;
	if (priv->converter)
		label_text = priv->converter(hid_usage_id);
	if (!label_text)
		label_text = gaminggear_hid_to_button_name(hid_usage_id);

	gtk_label_set_text(priv->label, label_text);
	g_free(label_text);
}

GtkWidget *roccat_key_button_new(RoccatKeyButton *group, guint hid_usage_id, roccat_key_button_name_converter converter) {
	RoccatKeyButton *key_button;

	key_button = ROCCAT_KEY_BUTTON(g_object_new(ROCCAT_KEY_BUTTON_TYPE,
			"group", group,
			"draw_indicator", FALSE,
			NULL));

	key_button->priv->converter = converter;
	g_signal_connect(G_OBJECT(key_button), "size-request", G_CALLBACK(button_size_request_cb), NULL);
	roccat_key_button_set_label(key_button, hid_usage_id);

	return GTK_WIDGET(key_button);
}

static void roccat_key_button_init(RoccatKeyButton *key_button) {
	RoccatKeyButtonPrivate *priv = ROCCAT_KEY_BUTTON_GET_PRIVATE(key_button);
	key_button->priv = priv;

	gdk_color_parse("black", &priv->color);
	priv->intensity = 1.0;
	priv->light_on = FALSE;

	priv->draw_area = gtk_alignment_new(0.5, 0.5, 0.0, 0.0);
	gtk_widget_set_size_request(GTK_WIDGET(priv->draw_area), 25, 25);
	g_signal_connect(G_OBJECT(priv->draw_area), "expose-event", G_CALLBACK(expose_cb), key_button);
	gtk_widget_show(priv->draw_area);

	priv->label = GTK_LABEL(gtk_label_new(NULL));
	gtk_label_set_justify(priv->label, GTK_JUSTIFY_CENTER);

	gtk_container_add(GTK_CONTAINER(priv->draw_area), GTK_WIDGET(priv->label));
	gtk_container_add(GTK_CONTAINER(key_button), priv->draw_area);
}

static void roccat_key_button_class_init(RoccatKeyButtonClass *klass) {
	g_type_class_add_private(klass, sizeof(RoccatKeyButtonPrivate));
}
