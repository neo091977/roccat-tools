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

#include "roccat_gamefile_selector.h"
#include "roccat_file_entry.h"
#include "i18n-lib.h"

#define ROCCAT_GAMEFILE_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ROCCAT_GAMEFILE_SELECTOR_TYPE, RoccatGamefileSelectorClass))
#define IS_ROCCAT_GAMEFILE_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ROCCAT_GAMEFILE_SELECTOR_TYPE))
#define ROCCAT_GAMEFILE_SELECTOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ROCCAT_GAMEFILE_SELECTOR_TYPE, RoccatGamefileSelectorPrivate))

typedef struct _RoccatGamefileSelectorClass RoccatGamefileSelectorClass;

struct _RoccatGamefileSelectorClass {
	GtkFrameClass parent_class;
};

struct _RoccatGamefileSelectorPrivate {
	guint count;
	GtkVBox *box;
	RoccatFileEntry **entries;
};

enum {
	PROP_0,
	PROP_COUNT,
};

G_DEFINE_TYPE(RoccatGamefileSelector, roccat_gamefile_selector, GTK_TYPE_FRAME);

gchar const *roccat_gamefile_selector_get_text(RoccatGamefileSelector *selector, guint index) {
	return roccat_file_entry_get_text(selector->priv->entries[index]);
}

void roccat_gamefile_selector_set_text(RoccatGamefileSelector *selector, guint index, gchar const *new_value) {
	roccat_file_entry_set_text(selector->priv->entries[index], new_value);
}

static GObject *roccat_gamefile_selector_constructor(GType gtype, guint n_properties, GObjectConstructParam *properties) {
	RoccatGamefileSelectorPrivate *priv;
	RoccatGamefileSelector *selector;
	GObject *obj;
	guint i;

	obj = G_OBJECT_CLASS(roccat_gamefile_selector_parent_class)->constructor(gtype, n_properties, properties);
	selector = ROCCAT_GAMEFILE_SELECTOR(obj);
	priv = selector->priv;

	priv->entries = (RoccatFileEntry **)g_malloc0(priv->count * sizeof(RoccatFileEntry *));

	for (i = 0; i < priv->count; ++i) {
		priv->entries[i] = ROCCAT_FILE_ENTRY(roccat_file_entry_new());
		gtk_box_pack_start(GTK_BOX(priv->box), GTK_WIDGET(priv->entries[i]), TRUE, TRUE, 0);
	}

	return obj;
}

GtkWidget *roccat_gamefile_selector_new(guint n) {
	RoccatGamefileSelector *selector;
	gchar const *label;

	label = (n == 1) ? _("Gamefile") : _("Gamefiles");

	selector = g_object_new(ROCCAT_GAMEFILE_SELECTOR_TYPE,
			"label", label,
			"count", n,
			NULL);

	return GTK_WIDGET(selector);
}

static void roccat_gamefile_selector_init(RoccatGamefileSelector *selector) {
	RoccatGamefileSelectorPrivate *priv = ROCCAT_GAMEFILE_SELECTOR_GET_PRIVATE(selector);

	selector->priv = priv;

	priv->box = GTK_VBOX(gtk_vbox_new(FALSE, 0));
	gtk_container_add(GTK_CONTAINER(selector), GTK_WIDGET(priv->box));
}

static void roccat_gamefile_selector_finalize(GObject *object) {
	RoccatGamefileSelectorPrivate *priv = ROCCAT_GAMEFILE_SELECTOR(object)->priv;
	g_free(priv->entries);
	G_OBJECT_CLASS(roccat_gamefile_selector_parent_class)->finalize(object);
}

static void roccat_gamefile_selector_set_property(GObject *object, guint prop_id, GValue const *value, GParamSpec *pspec) {
	RoccatGamefileSelectorPrivate *priv = ROCCAT_GAMEFILE_SELECTOR(object)->priv;

	switch(prop_id) {
	case PROP_COUNT:
		priv->count = g_value_get_uint(value);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
	}
}

static void roccat_gamefile_selector_class_init(RoccatGamefileSelectorClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->constructor = roccat_gamefile_selector_constructor;
	gobject_class->finalize = roccat_gamefile_selector_finalize;
	gobject_class->set_property = roccat_gamefile_selector_set_property;

	g_type_class_add_private(klass, sizeof(RoccatGamefileSelectorPrivate));

	g_object_class_install_property(gobject_class, PROP_COUNT,
			g_param_spec_uint("count", "count", "Sets count",
					0,
					G_MAXUINT,
					1,
					G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
}
