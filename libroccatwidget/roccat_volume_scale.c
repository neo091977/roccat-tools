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

#include "roccat_volume_scale.h"

#define ROCCAT_VOLUME_SCALE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ROCCAT_VOLUME_SCALE_TYPE, RoccatVolumeScaleClass))
#define IS_ROCCAT_VOLUME_SCALE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ROCCAT_VOLUME_SCALE_TYPE))
#define ROCCAT_VOLUME_SCALE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ROCCAT_VOLUME_SCALE_TYPE, RoccatVolumeScalePrivate))

typedef struct _RoccatVolumeScaleClass RoccatVolumeScaleClass;
typedef struct _RoccatVolumeScalePrivate RoccatVolumeScalePrivate;

struct _RoccatVolumeScale {
	GtkHScale parent;
};

struct _RoccatVolumeScaleClass {
	GtkHScaleClass parent_class;
};

G_DEFINE_TYPE(RoccatVolumeScale, roccat_volume_scale, GTK_TYPE_HSCALE);

gdouble roccat_volume_scale_get_value(RoccatVolumeScale *volume_scale) {
	return gtk_range_get_value(GTK_RANGE(volume_scale));
}

void roccat_volume_scale_set_value(RoccatVolumeScale *volume_scale, gdouble value) {
	gtk_range_set_value(GTK_RANGE(volume_scale), value);
}

GtkWidget *roccat_volume_scale_new(void) {
	return GTK_WIDGET(g_object_new(ROCCAT_VOLUME_SCALE_TYPE, NULL));
}

static gchar *format_value_cb(GtkScale *scale, gdouble value, gpointer user_data) {
	return g_strdup_printf("%+0.0f dB", value);
}

static void roccat_volume_scale_init(RoccatVolumeScale *volume_scale) {
}

static GObject *constructor(GType gtype, guint n_properties, GObjectConstructParam *properties) {
	GObject *obj;

	obj = G_OBJECT_CLASS(roccat_volume_scale_parent_class)->constructor(gtype, n_properties, properties);

	gtk_range_set_range(GTK_RANGE(obj), -100.0, 10.0);
	gtk_range_set_increments(GTK_RANGE(obj), 1.0, 10.0);

	gtk_scale_set_draw_value(GTK_SCALE(obj), TRUE);
	gtk_scale_set_value_pos(GTK_SCALE(obj), GTK_POS_RIGHT);
	g_signal_connect(G_OBJECT(obj), "format-value", G_CALLBACK(format_value_cb), NULL);

	return obj;
}

static void roccat_volume_scale_class_init(RoccatVolumeScaleClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->constructor = constructor;
}
