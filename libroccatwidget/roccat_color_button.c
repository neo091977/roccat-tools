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

#include "roccat_color_button.h"

#define ROCCAT_COLOR_BUTTON_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ROCCAT_COLOR_BUTTON_TYPE, RoccatColorButtonClass))
#define IS_ROCCAT_COLOR_BUTTON_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ROCCAT_COLOR_BUTTON_TYPE))
#define ROCCAT_COLOR_BUTTON_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ROCCAT_COLOR_BUTTON_TYPE, RoccatColorButtonPrivate))

struct _RoccatColorButtonPrivate {
	GtkWidget *draw_area;
	GdkColor color;
};

static GdkColor default_color;

G_DEFINE_TYPE(RoccatColorButton, roccat_color_button, GTK_TYPE_BUTTON);

void roccat_color_button_set_color(RoccatColorButton *color_button, GdkColor const *color) {
	RoccatColorButtonPrivate *priv = color_button->priv;
	priv->color = *color;
	gtk_widget_queue_draw(priv->draw_area);
}

void roccat_color_button_set_default_color(RoccatColorButton *color_button) {
	roccat_color_button_set_color(color_button, &default_color);
}

void roccat_color_button_get_color(RoccatColorButton *color_button, GdkColor *color) {
	RoccatColorButtonPrivate *priv = color_button->priv;
	*color = priv->color;
}

GtkWidget *roccat_color_button_new(void) {
	return GTK_WIDGET(g_object_new(ROCCAT_COLOR_BUTTON_TYPE, NULL));
}

GtkWidget *roccat_color_button_new_with_color(GdkColor const *color) {
	RoccatColorButton *color_button = ROCCAT_COLOR_BUTTON(roccat_color_button_new());
	roccat_color_button_set_color(color_button, color);
	return GTK_WIDGET(color_button);
}

static gboolean expose_cb(GtkWidget *widget, GdkEventExpose *event, gpointer user_data) {
	RoccatColorButtonPrivate *priv = ROCCAT_COLOR_BUTTON(user_data)->priv;
	GtkAllocation allocation;
	cairo_t *cr;

	cr = gdk_cairo_create(event->window);

	gtk_widget_get_allocation(widget, &allocation);
	gdk_cairo_rectangle(cr, &allocation);
	cairo_clip(cr);
	gdk_cairo_set_source_color(cr, &priv->color);
	cairo_paint(cr);
	cairo_destroy(cr);

	return FALSE;
}

static void roccat_color_button_init(RoccatColorButton *color_button) {
	RoccatColorButtonPrivate *priv = ROCCAT_COLOR_BUTTON_GET_PRIVATE(color_button);
	color_button->priv = priv;

	priv->draw_area = gtk_alignment_new(0.5, 0.5, 0.0, 0.0);
	gtk_widget_set_size_request(GTK_WIDGET(priv->draw_area), 25, 25);
	g_signal_connect(G_OBJECT(priv->draw_area), "expose-event", G_CALLBACK(expose_cb), color_button);
	gtk_container_add(GTK_CONTAINER(color_button), priv->draw_area);
	gtk_widget_show(priv->draw_area);

	roccat_color_button_set_default_color(color_button);
}

static void roccat_color_button_class_init(RoccatColorButtonClass *klass) {
	g_type_class_add_private(klass, sizeof(RoccatColorButtonPrivate));

	gdk_color_parse("black", &default_color);
}
