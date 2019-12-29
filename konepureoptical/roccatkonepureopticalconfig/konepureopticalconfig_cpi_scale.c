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

#include "konepureopticalconfig_cpi_scale.h"
#include "konepureoptical.h"

#define KONEPUREOPTICALCONFIG_CPI_SCALE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KONEPUREOPTICALCONFIG_CPI_SCALE_TYPE, KonepureopticalconfigCpiScaleClass))
#define IS_KONEPUREOPTICALCONFIG_CPI_SCALE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KONEPUREOPTICALCONFIG_CPI_SCALE_TYPE))
#define KONEPUREOPTICALCONFIG_CPI_SCALE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KONEPUREOPTICALCONFIG_CPI_SCALE_TYPE, KonepureopticalconfigCpiScalePrivate))

typedef struct _KonepureopticalconfigCpiScaleClass KonepureopticalconfigCpiScaleClass;

struct _KonepureopticalconfigCpiScaleClass {
	GtkHBoxClass parent_class;
	void(*value_changed)(KonepureopticalconfigCpiScale *scale);
};

struct _KonepureopticalconfigCpiScalePrivate {
	GtkHScale *scale;
	GtkSpinButton *spin;
	gulong scale_handler;
	gulong spin_handler;
	guint actual_cpi;
};

enum {
	PROP_0,
	PROP_VALUE,
};

G_DEFINE_TYPE(KonepureopticalconfigCpiScale, konepureopticalconfig_cpi_scale, GTK_TYPE_HBOX);

static guint cpis[KONEPUREOPTICAL_CPI_VALUES_NUM] = { 400, 800, 1200, 1600, 2000, 3200, 4000 };

static guint cpi_to_index(guint cpi, gboolean up) {
	gint i;

	if (up) {
		for (i = 0; i < KONEPUREOPTICAL_CPI_VALUES_NUM; ++i)
			if (cpis[i] >= cpi)
				return i;
	} else {
		for (i = KONEPUREOPTICAL_CPI_VALUES_NUM - 1; i >= 0; --i)
			if (cpis[i] <= cpi)
				return i;
	}

	/* always return sane value */
	return 3;
}

static guint rmp_value_to_index(guint rmp_value) {
	if (rmp_value <= KONEPUREOPTICAL_CPI_VALUES_NUM)
		return rmp_value - 1;

	/* always return sane value */
	return 3;
}

static guint index_to_rmp_value(guint index) {
	return index + 1;
}

static guint spin_button_get_cpi(GtkSpinButton *spin_button) {
	return (guint)(gtk_spin_button_get_value(spin_button) + 0.5);
}

static guint range_get_index(GtkRange *range) {
	return (guint)(gtk_range_get_value(range) + 0.5);
}

guint konepureopticalconfig_cpi_scale_get_value(KonepureopticalconfigCpiScale *hscale) {
	return index_to_rmp_value(range_get_index(GTK_RANGE(hscale->priv->scale)));
}

static void set_value_blocked(KonepureopticalconfigCpiScale *hscale, guint index) {
	KonepureopticalconfigCpiScalePrivate *priv = hscale->priv;
	guint cpi;

	cpi = cpis[index];

	g_signal_handler_block(G_OBJECT(priv->scale), priv->scale_handler);
	gtk_range_set_value(GTK_RANGE(priv->scale), index);
	g_signal_handler_unblock(G_OBJECT(priv->scale), priv->scale_handler);
	g_signal_handler_block(G_OBJECT(priv->spin), priv->spin_handler);
	gtk_spin_button_set_value(priv->spin, cpi);
	g_signal_handler_unblock(G_OBJECT(priv->spin), priv->spin_handler);
	g_signal_emit_by_name((gpointer)hscale, "value-changed");

	priv->actual_cpi = cpi;
}

void konepureopticalconfig_cpi_scale_set_value_blocked(KonepureopticalconfigCpiScale *hscale, guint rmp_value) {
	set_value_blocked(hscale, rmp_value_to_index(rmp_value));
}

GtkWidget *konepureopticalconfig_cpi_scale_new(void) {
	KonepureopticalconfigCpiScale *hscale;

	hscale = g_object_new(KONEPUREOPTICALCONFIG_CPI_SCALE_TYPE, NULL);

	return GTK_WIDGET(hscale);
}

static void konepureopticalconfig_cpi_scale_set_property(GObject *object, guint prop_id, GValue const *value, GParamSpec *pspec) {
	KonepureopticalconfigCpiScale *hscale = KONEPUREOPTICALCONFIG_CPI_SCALE(object);

	switch(prop_id) {
	case PROP_VALUE:
		konepureopticalconfig_cpi_scale_set_value_blocked(hscale, g_value_get_double(value));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
	}
}

static void konepureopticalconfig_cpi_scale_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec) {
	KonepureopticalconfigCpiScale *hscale = KONEPUREOPTICALCONFIG_CPI_SCALE(object);

	switch(prop_id) {
	case PROP_VALUE:
		g_value_set_double(value, konepureopticalconfig_cpi_scale_get_value(hscale));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
	}
}

static void konepureopticalconfig_cpi_scale_scale_value_changed_cb(GtkRange *range, gpointer user_data) {
	set_value_blocked(KONEPUREOPTICALCONFIG_CPI_SCALE(user_data), range_get_index(range));
}

static void konepureopticalconfig_cpi_scale_spin_value_changed_cb(GtkSpinButton *spinbutton, gpointer user_data) {
	KonepureopticalconfigCpiScale *hscale = KONEPUREOPTICALCONFIG_CPI_SCALE(user_data);
	KonepureopticalconfigCpiScalePrivate *priv = hscale->priv;
	guint cpi;
	guint index;

	cpi = spin_button_get_cpi(spinbutton);
	index = cpi_to_index(cpi, cpi > priv->actual_cpi);

	/* always set spin button with corrected value */
	set_value_blocked(hscale, index);
}

/*
 * g_object_bin_property() would be a better way to link both together but it's
 * only available since gobject-2.26
 */
static void konepureopticalconfig_cpi_scale_init(KonepureopticalconfigCpiScale *hscale) {
	KonepureopticalconfigCpiScalePrivate *priv = KONEPUREOPTICALCONFIG_CPI_SCALE_GET_PRIVATE(hscale);
	guint i;

	hscale->priv = priv;

	/* Small GTK problem if steps and mark distances are equal.
	 * Partial solution: making step at least a tenth fraction of mark distance.
	 * Setting "digits" to 0 has no effect if "draw_value" is FALSE.
	 * Maybe setting "round-digits" to 0 (available since 2.24) could help.
	 */
	priv->scale = GTK_HSCALE(gtk_hscale_new_with_range(0.0, (gdouble)KONEPUREOPTICAL_CPI_VALUES_NUM - 1.0, 0.1));
	gtk_scale_set_draw_value(GTK_SCALE(priv->scale), FALSE);
	gtk_box_pack_start(GTK_BOX(hscale), GTK_WIDGET(priv->scale), TRUE, TRUE, 0);
	priv->scale_handler = g_signal_connect(G_OBJECT(priv->scale), "value-changed", G_CALLBACK(konepureopticalconfig_cpi_scale_scale_value_changed_cb), hscale);

	for (i = 0; i < KONEPUREOPTICAL_CPI_VALUES_NUM; ++i)
		gtk_scale_add_mark(GTK_SCALE(priv->scale), (gdouble)i, GTK_POS_BOTTOM, NULL);

	priv->spin = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(cpis[0], cpis[KONEPUREOPTICAL_CPI_VALUES_NUM - 1], 400));
	gtk_box_pack_start(GTK_BOX(hscale), GTK_WIDGET(priv->spin), FALSE, FALSE, 0);
	priv->spin_handler = g_signal_connect(G_OBJECT(priv->spin), "value-changed", G_CALLBACK(konepureopticalconfig_cpi_scale_spin_value_changed_cb), hscale);
}

static void konepureopticalconfig_cpi_scale_class_init(KonepureopticalconfigCpiScaleClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = (GObjectClass*)klass;

	gobject_class->set_property = konepureopticalconfig_cpi_scale_set_property;
	gobject_class->get_property = konepureopticalconfig_cpi_scale_get_property;

	g_type_class_add_private(klass, sizeof(KonepureopticalconfigCpiScalePrivate));

	g_signal_new("value-changed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			G_STRUCT_OFFSET(KonepureopticalconfigCpiScaleClass, value_changed),
			NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

	g_object_class_install_property(gobject_class, PROP_VALUE,
			g_param_spec_double("value",
					"value",
					"Reads or sets value",
					-G_MAXDOUBLE, G_MAXDOUBLE, 0.0,
					G_PARAM_READWRITE));
}
