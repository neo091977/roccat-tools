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

#include "tyon_2d_positional.h"

#define TYON_2D_POSITIONAL_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), TYON_2D_POSITIONAL_TYPE, Tyon2DPositionalClass))
#define IS_TYON_2D_POSITIONAL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), TYON_2D_POSITIONAL_TYPE))
#define TYON_2D_POSITIONAL_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), TYON_2D_POSITIONAL_TYPE, Tyon2DPositionalPrivate))

typedef struct _Tyon2DPositionalClass Tyon2DPositionalClass;
typedef struct _Tyon2DPositionalPrivate Tyon2DPositionalPrivate;

struct _Tyon2DPositional {
	GtkDrawingArea parent;
	Tyon2DPositionalPrivate *priv;
};

struct _Tyon2DPositionalClass {
	GtkDrawingAreaClass parent_class;
};

struct _Tyon2DPositionalPrivate {
	double x, y;
};

static const gint widget_size = 1 << 7;
static const gint pixel_min_size = 1 << 3;

G_DEFINE_TYPE(Tyon2DPositional, tyon_2d_positional, GTK_TYPE_DRAWING_AREA);

static void tyon_2d_positional_paint(Tyon2DPositional *positional) {
	Tyon2DPositionalPrivate *priv = positional->priv;
	GdkWindow *window;
	GtkWidget *widget;
	GtkAllocation allocation;
	cairo_t *cr;
	guint x, y;
	guint pixel_size;

	widget = GTK_WIDGET(positional);
	gtk_widget_get_allocation(widget, &allocation);

	window = gtk_widget_get_window(widget);

	if (!GDK_IS_DRAWABLE(window))
		return;

	cr = gdk_cairo_create(window);

	cairo_set_line_width(cr, 1.0);
	cairo_set_line_cap(cr, CAIRO_LINE_CAP_SQUARE);

	pixel_size = MAX(MIN(allocation.width, allocation.height) >> 5, pixel_min_size);

	// do background
	gdk_cairo_set_source_color(cr, &gtk_widget_get_style(widget)->bg[GTK_STATE_NORMAL]);

	// select entire view
	cairo_rectangle(cr, 0, 0, allocation.width, allocation.height);

	// clear view
	cairo_fill_preserve(cr);

	// do foreground
	gdk_cairo_set_source_color(cr, &gtk_widget_get_style(widget)->fg[GTK_STATE_NORMAL]);

	// draw border
	cairo_stroke(cr);

	x = ((priv->x + 1.0) * allocation.width - pixel_size) / 2;
	y = ((priv->y + 1.0) * allocation.height - pixel_size) / 2;

	// draw position indicator
	cairo_rectangle(cr, x, y, pixel_size, pixel_size);
	cairo_fill(cr);

	cairo_destroy(cr);
}

GtkWidget *tyon_2d_positional_new(void) {
	return GTK_WIDGET(g_object_new(TYON_2D_POSITIONAL_TYPE, NULL));
}

static gint tyon_2d_positional_expose(GtkWidget *widget, GdkEventExpose *event) {
	g_return_val_if_fail(widget != NULL || event != NULL, FALSE);
	g_return_val_if_fail(IS_TYON_2D_POSITIONAL(widget), FALSE);
	tyon_2d_positional_paint(TYON_2D_POSITIONAL(widget));
	return TRUE;
}

static void tyon_2d_positional_size_request(GtkWidget *widget, GtkRequisition *requisition) {
	g_return_if_fail(widget != NULL || requisition != NULL);
	g_return_if_fail(IS_TYON_2D_POSITIONAL(widget));
	requisition->height = requisition->width;
}

static void tyon_2d_positional_init(Tyon2DPositional *positional) {
	positional->priv = TYON_2D_POSITIONAL_GET_PRIVATE(positional);
	gtk_widget_set_size_request(GTK_WIDGET(positional), widget_size, widget_size);
}

static void tyon_2d_positional_class_init(Tyon2DPositionalClass *klass) {
	GtkWidgetClass *widget_class;

	widget_class = (GtkWidgetClass*)klass;
	widget_class->expose_event = tyon_2d_positional_expose;
	widget_class->size_request = tyon_2d_positional_size_request;

	g_type_class_add_private(klass, sizeof(Tyon2DPositionalPrivate));
}

void tyon_2d_positional_set(Tyon2DPositional *positional, double x, double y) {
	Tyon2DPositionalPrivate *priv = positional->priv;
	priv->x = x;
	priv->y = y;
	tyon_2d_positional_paint(positional);
}
