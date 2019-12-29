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

#include "roccat_color_palette_selector.h"
#include "roccat_color_button.h"
#include "gtk_roccat_helper.h"
#include "g_roccat_helper.h"
#include <math.h>

#define ROCCAT_COLOR_PALETTE_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ROCCAT_COLOR_PALETTE_SELECTOR_TYPE, RoccatColorPaletteSelectorClass))
#define IS_ROCCAT_COLOR_PALETTE_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ROCCAT_COLOR_PALETTE_SELECTOR_TYPE))
#define ROCCAT_COLOR_PALETTE_SELECTOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ROCCAT_COLOR_PALETTE_SELECTOR_TYPE, RoccatColorPaletteSelectorPrivate))

typedef struct _RoccatColorPaletteSelectorClass RoccatColorPaletteSelectorClass;

struct _RoccatColorPaletteSelectorClass {
	GtkTableClass parent_class;
};

struct _RoccatColorPaletteSelectorPrivate {
	gint index;
	size_t length;
	GtkWidget **buttons;
};

G_DEFINE_TYPE(RoccatColorPaletteSelector, roccat_color_palette_selector, GTK_TYPE_TABLE);

enum {
	COLOR_CHANGED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

static gchar const * const index_key = "Index";

static size_t calculate_table_width(size_t list_length) {
	return (size_t)ceil(sqrt((double)list_length));
}

static void object_set_index(GObject *object, gint value) {
	g_object_set_data(object, index_key, GINT_TO_POINTER(value));
}

static gint object_get_index(GObject *object) {
	return GPOINTER_TO_INT(g_object_get_data(object, index_key));
}

static void button_clicked_cb(GtkButton *button, gpointer user_data) {
	RoccatColorPaletteSelector *selector = ROCCAT_COLOR_PALETTE_SELECTOR(user_data);

	selector->priv->index = object_get_index(G_OBJECT(button));
	g_signal_emit((gpointer)selector, signals[COLOR_CHANGED], 0);
}

gboolean roccat_color_palette_selector_get_color_for_index(RoccatColorPaletteSelector *color_palette_selector, gint index, GdkColor *color) {
	RoccatColorPaletteSelectorPrivate *priv = color_palette_selector->priv;

	if (index < 0 || index >= (gint)priv->length)
		return FALSE;

	roccat_color_button_get_color(ROCCAT_COLOR_BUTTON(priv->buttons[index]), color);
	return TRUE;
}

gboolean roccat_color_palette_selector_get_color(RoccatColorPaletteSelector *color_palette_selector, GdkColor *color) {
	return roccat_color_palette_selector_get_color_for_index(color_palette_selector, color_palette_selector->priv->index, color);
}

gint roccat_color_palette_selector_get_index(RoccatColorPaletteSelector *color_palette_selector) {
	return color_palette_selector->priv->index;
}

static void clear_colors(RoccatColorPaletteSelector *color_palette_selector) {
	RoccatColorPaletteSelectorPrivate *priv = color_palette_selector->priv;
	guint i;

	gtk_roccat_table_clear(GTK_TABLE(color_palette_selector));

	for (i = 0; i < priv->length; ++i)
		g_object_unref(G_OBJECT(priv->buttons[i]));

	g_clear_pointer(&priv->buttons, g_free);
	priv->length = 0;
	priv->index = -1;
}

void roccat_color_palette_selector_set_dimensions(RoccatColorPaletteSelector *color_palette_selector, size_t length, guint width) {
	RoccatColorPaletteSelectorPrivate *priv = color_palette_selector->priv;
	size_t i, row, col;
	size_t cols;

	clear_colors(color_palette_selector);

	if (length == 0)
		return;

	cols = (width == 0) ? calculate_table_width(length) : width;

	priv->buttons = (GtkWidget **)g_malloc(sizeof(GtkWidget *) * length);
	priv->length = length;

	for (i = 0; i < length; ++i) {
		priv->buttons[i] = roccat_color_button_new();
		g_object_ref_sink(G_OBJECT(priv->buttons[i]));
		object_set_index(G_OBJECT(priv->buttons[i]), i);
		g_signal_connect(G_OBJECT(priv->buttons[i]), "clicked", G_CALLBACK(button_clicked_cb), color_palette_selector);

		row = i / cols;
		col = i % cols;
		gtk_table_attach(GTK_TABLE(color_palette_selector), priv->buttons[i], col, col + 1, row, row + 1, GTK_EXPAND, GTK_EXPAND, 0, 0);
	}

	gtk_widget_show_all(GTK_WIDGET(color_palette_selector));
}

void roccat_color_palette_selector_set_index(RoccatColorPaletteSelector *color_palette_selector, gint index) {
	color_palette_selector->priv->index = index;
}

void roccat_color_palette_selector_set_colors(RoccatColorPaletteSelector *color_palette_selector, GdkColor const *colors, size_t length) {
	RoccatColorPaletteSelectorPrivate *priv = color_palette_selector->priv;
	guint i;

	g_assert(length == priv->length);

	for (i = 0; i < length; ++i)
		roccat_color_button_set_color(ROCCAT_COLOR_BUTTON(priv->buttons[i]), &colors[i]);
}

GtkWidget *roccat_color_palette_selector_new(void) {
	return GTK_WIDGET(g_object_new(ROCCAT_COLOR_PALETTE_SELECTOR_TYPE, NULL));
}

GtkWidget *roccat_color_palette_selector_new_with_colors(GdkColor const *colors, size_t length, guint width) {
	RoccatColorPaletteSelector *color_palette_selector = ROCCAT_COLOR_PALETTE_SELECTOR(roccat_color_palette_selector_new());
	roccat_color_palette_selector_set_dimensions(color_palette_selector, length, width);
	roccat_color_palette_selector_set_colors(color_palette_selector, colors, length);
	return GTK_WIDGET(color_palette_selector);
}

static void roccat_color_palette_selector_init(RoccatColorPaletteSelector *color_palette_selector) {
	RoccatColorPaletteSelectorPrivate *priv = ROCCAT_COLOR_PALETTE_SELECTOR_GET_PRIVATE(color_palette_selector);
	color_palette_selector->priv = priv;
	priv->index = -1;
	priv->buttons = NULL;
	priv->length = 0;
}

static void roccat_color_palette_selector_finalize(GObject *object) {
	clear_colors(ROCCAT_COLOR_PALETTE_SELECTOR(object));

	G_OBJECT_CLASS(roccat_color_palette_selector_parent_class)->finalize(object);
}

static void roccat_color_palette_selector_class_init(RoccatColorPaletteSelectorClass *klass) {
	GObjectClass *gobject_class = (GObjectClass *)klass;

	gobject_class->finalize = roccat_color_palette_selector_finalize;

	g_type_class_add_private(klass, sizeof(RoccatColorPaletteSelectorPrivate));

	signals[COLOR_CHANGED] = g_signal_new("color-changed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}
