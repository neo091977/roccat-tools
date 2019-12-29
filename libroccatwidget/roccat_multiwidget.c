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

#include "roccat_multiwidget.h"
#include "g_roccat_helper.h"

#define ROCCAT_MULTIWIDGET_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ROCCAT_MULTIWIDGET_TYPE, RoccatMultiwidgetClass))
#define IS_ROCCAT_MULTIWIDGET_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ROCCAT_MULTIWIDGET_TYPE))
#define ROCCAT_MULTIWIDGET_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ROCCAT_MULTIWIDGET_TYPE, RoccatMultiwidgetPrivate))

typedef struct _RoccatMultiwidgetClass RoccatMultiwidgetClass;
typedef struct _RoccatMultiwidgetPrivate RoccatMultiwidgetPrivate;

struct _RoccatMultiwidget {
	GtkVBox parent;
	RoccatMultiwidgetPrivate *priv;
};

struct _RoccatMultiwidgetClass {
	GtkVBoxClass parent_class;
};

struct _RoccatMultiwidgetPrivate {
	GSList *widgets;
};

G_DEFINE_TYPE(RoccatMultiwidget, roccat_multiwidget, GTK_TYPE_VBOX);

static void remove_cb(GtkWidget *widget, gpointer user_data) {
	gtk_container_remove(GTK_CONTAINER(user_data), widget);
}

static void add_cb(gpointer data, gpointer user_data) {
	GtkWidget *widget = GTK_WIDGET(data);
	gtk_box_pack_start(GTK_BOX(user_data), widget, TRUE, TRUE, 0);
	gtk_widget_show_all(widget);
}

void roccat_multiwidget_show_none(RoccatMultiwidget *multiwidget) {
	gtk_container_foreach(GTK_CONTAINER(multiwidget), remove_cb, multiwidget);
}

void roccat_multiwidget_show_all(RoccatMultiwidget *multiwidget) {
	gtk_container_foreach(GTK_CONTAINER(multiwidget), remove_cb, multiwidget);
	g_slist_foreach(multiwidget->priv->widgets, add_cb, multiwidget);
}

void roccat_multiwidget_show(RoccatMultiwidget *multiwidget, GtkWidget *widget) {
	gtk_container_foreach(GTK_CONTAINER(multiwidget), remove_cb, multiwidget);
	gtk_container_add(GTK_CONTAINER(multiwidget), widget);
	gtk_widget_show_all(widget);
}

void roccat_multiwidget_add(RoccatMultiwidget *multiwidget, GtkWidget *widget) {
	RoccatMultiwidgetPrivate *priv = multiwidget->priv;
	g_object_ref(G_OBJECT(widget));
	priv->widgets = g_slist_append(priv->widgets, widget);
}

GtkWidget *roccat_multiwidget_new(void) {
	RoccatMultiwidget *multiwidget;

	multiwidget = ROCCAT_MULTIWIDGET(g_object_new(ROCCAT_MULTIWIDGET_TYPE, NULL));

	return GTK_WIDGET(multiwidget);
}

static void roccat_multiwidget_init(RoccatMultiwidget *multiwidget) {
	RoccatMultiwidgetPrivate *priv = ROCCAT_MULTIWIDGET_GET_PRIVATE(multiwidget);
	multiwidget->priv = priv;
	priv->widgets = NULL;
}

static void finalize(GObject *object) {
	RoccatMultiwidgetPrivate *priv = ROCCAT_MULTIWIDGET(object)->priv;
	g_slist_free_full(priv->widgets, g_object_unref);
	G_OBJECT_CLASS(roccat_multiwidget_parent_class)->finalize(object);
}

static void roccat_multiwidget_class_init(RoccatMultiwidgetClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(RoccatMultiwidgetPrivate));
}
