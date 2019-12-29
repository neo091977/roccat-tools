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

#include "roccat_key_mask_selector.h"
#include "i18n-lib.h"

#define ROCCAT_KEY_MASK_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ROCCAT_KEY_MASK_SELECTOR_TYPE, RoccatKeyMaskSelectorClass))
#define IS_ROCCAT_KEY_MASK_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ROCCAT_KEY_MASK_SELECTOR_TYPE))
#define ROCCAT_KEY_MASK_SELECTOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ROCCAT_KEY_MASK_SELECTOR_TYPE, RoccatKeyMaskSelectorPrivate))


struct _RoccatKeyMaskSelectorPrivate {
	guint count;
	GtkToggleButton **buttons;
	gchar const **values;
};

enum {
	PROP_0,
	PROP_COUNT,
	PROP_VALUES,
};

G_DEFINE_TYPE(RoccatKeyMaskSelector, roccat_key_mask_selector, GTK_TYPE_FRAME);

void roccat_key_mask_selector_set_active(RoccatKeyMaskSelector *selector, guint index, gboolean is_active) {
	gtk_toggle_button_set_active(selector->priv->buttons[index], is_active);
}

gboolean roccat_key_mask_selector_get_active(RoccatKeyMaskSelector *selector, guint index) {
	return gtk_toggle_button_get_active(selector->priv->buttons[index]);
}

static GObject *roccat_key_mask_selector_constructor(GType gtype, guint n_properties, GObjectConstructParam *properties) {
	RoccatKeyMaskSelectorPrivate *priv;
	RoccatKeyMaskSelector *selector;
	GObject *obj;
	GtkWidget *box;
	guint i;

	obj = G_OBJECT_CLASS(roccat_key_mask_selector_parent_class)->constructor(gtype, n_properties, properties);
	selector = ROCCAT_KEY_MASK_SELECTOR(obj);
	priv = selector->priv;

	box = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(selector), box);

	priv->buttons = (GtkToggleButton **)g_malloc0(priv->count * sizeof(GtkToggleButton *));
	for (i = 0; i < priv->count; ++i) {
		priv->buttons[i] = GTK_TOGGLE_BUTTON(gtk_check_button_new_with_label(_N(priv->values[i])));
		gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(priv->buttons[i]), TRUE, TRUE, 0);
	}

	gtk_frame_set_label(GTK_FRAME(selector), _("Enabled keys"));

	return obj;
}

GtkWidget *roccat_key_mask_selector_new(guint count, gchar const **values) {
	RoccatKeyMaskSelector *roccat_key_mask_selector;

	roccat_key_mask_selector = ROCCAT_KEY_MASK_SELECTOR(g_object_new(ROCCAT_KEY_MASK_SELECTOR_TYPE,
			"count", count,
			"values", values,
			NULL));

	return GTK_WIDGET(roccat_key_mask_selector);
}

static void roccat_key_mask_selector_init(RoccatKeyMaskSelector *roccat_key_mask_selector) {
	RoccatKeyMaskSelectorPrivate *priv = ROCCAT_KEY_MASK_SELECTOR_GET_PRIVATE(roccat_key_mask_selector);
	roccat_key_mask_selector->priv = priv;
}

static void roccat_key_mask_selector_finalize(GObject *object) {
	RoccatKeyMaskSelectorPrivate *priv = ROCCAT_KEY_MASK_SELECTOR(object)->priv;
	g_free(priv->buttons);
	G_OBJECT_CLASS(roccat_key_mask_selector_parent_class)->finalize(object);
}

static void roccat_key_mask_selector_set_property(GObject *object, guint prop_id, GValue const *value, GParamSpec *pspec) {
	RoccatKeyMaskSelectorPrivate *priv = ROCCAT_KEY_MASK_SELECTOR(object)->priv;
	switch(prop_id) {
	case PROP_COUNT:
		priv->count = g_value_get_uint(value);
		break;
	case PROP_VALUES:
		priv->values = g_value_get_pointer(value);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
}

static void roccat_key_mask_selector_class_init(RoccatKeyMaskSelectorClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = (GObjectClass *)klass;

	gobject_class->constructor = roccat_key_mask_selector_constructor;
	gobject_class->finalize = roccat_key_mask_selector_finalize;
	gobject_class->set_property = roccat_key_mask_selector_set_property;

	g_type_class_add_private(klass, sizeof(RoccatKeyMaskSelectorPrivate));

	g_object_class_install_property(gobject_class, PROP_COUNT,
			g_param_spec_uint("count", "count", "Count",
					0,
					G_MAXUINT,
					1,
					G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));

	g_object_class_install_property(gobject_class, PROP_VALUES,
			g_param_spec_pointer("values",
					"values",
					"Values",
					G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
}
