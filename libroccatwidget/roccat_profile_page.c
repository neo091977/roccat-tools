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

#include "roccat_profile_page.h"
#include <gaminggear/gaminggear_text_dialog.h>
#include "g_roccat_helper.h"
#include "gtk_roccat_helper.h"
#include "i18n-lib.h"

#define ROCCAT_PROFILE_PAGE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ROCCAT_PROFILE_PAGE_TYPE, RoccatProfilePagePrivate))

struct _RoccatProfilePagePrivate {
	gboolean renameable;
	gchar *profile_name;
};

G_DEFINE_TYPE(RoccatProfilePage, roccat_profile_page, GTK_TYPE_VBOX);

enum {
	PROP_0,
	PROP_RENAMEABLE,
};

enum {
	LOAD_FROM_FILE,
	SAVE_TO_FILE,
	RESET,
	RENAMED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

void roccat_profile_page_set_name(RoccatProfilePage *profile_page, gchar const *name) {
	RoccatProfilePagePrivate *priv = profile_page->priv;
	if (g_strcmp0(priv->profile_name, name)) {
		g_free(priv->profile_name);
		priv->profile_name = g_strdup(name);
		g_signal_emit((gpointer)profile_page, signals[RENAMED], 0);
	}
}

gchar *roccat_profile_page_get_name(RoccatProfilePage *profile_page) {
	return g_strdup(profile_page->priv->profile_name);
}

static void load_from_file_cb(GtkButton *button, gpointer user_data) {
	RoccatProfilePage *profile_page = ROCCAT_PROFILE_PAGE(user_data);
	g_signal_emit((gpointer)profile_page, signals[LOAD_FROM_FILE], 0);
}

static void save_to_file_cb(GtkButton *button, gpointer user_data) {
	RoccatProfilePage *profile_page = ROCCAT_PROFILE_PAGE(user_data);
	g_signal_emit((gpointer)profile_page, signals[SAVE_TO_FILE], 0);
}

static void rename_cb(GtkButton *button, gpointer user_data) {
	RoccatProfilePage *profile_page = ROCCAT_PROFILE_PAGE(user_data);
	RoccatProfilePagePrivate *priv = profile_page->priv;
	gchar *new_name;

	new_name = gaminggear_text_dialog(gtk_roccat_widget_get_gtk_window(GTK_WIDGET(button)),
			_("Enter new name"), _("Please enter new name"), priv->profile_name);
	if (new_name) {
		roccat_profile_page_set_name(profile_page, new_name);
		g_free(new_name);
	}
}

static void reset_cb(GtkButton *button, gpointer user_data) {
	RoccatProfilePage *profile_page = ROCCAT_PROFILE_PAGE(user_data);
	g_signal_emit((gpointer)profile_page, signals[RESET], 0);
}

static GtkWidget *buttons_new(RoccatProfilePage *profile_page) {
	GtkWidget *hbox1, *hbox2, *button;

	hbox1 = gtk_hbox_new(FALSE, 0);
	hbox2 = gtk_hbox_new(TRUE, 0);

	gtk_box_pack_start(GTK_BOX(hbox1), hbox2, FALSE, FALSE, 0);

	button = gtk_button_new_with_label(_("Import"));
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(load_from_file_cb), profile_page);
	gtk_box_pack_start(GTK_BOX(hbox2), button, TRUE, TRUE, 0);

	button = gtk_button_new_with_label(_("Export"));
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(save_to_file_cb), profile_page);
	gtk_box_pack_start(GTK_BOX(hbox2), button, TRUE, TRUE, 0);

	if (profile_page->priv->renameable) {
		button = gtk_button_new_with_label(_("Rename"));
		g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(rename_cb), profile_page);
		gtk_box_pack_start(GTK_BOX(hbox2), button, TRUE, TRUE, 0);
	}

	button = gtk_button_new_with_label(_("Reset"));
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(reset_cb), profile_page);
	gtk_box_pack_start(GTK_BOX(hbox2), button, TRUE, TRUE, 0);

	return hbox1;
}

static void roccat_profile_page_init(RoccatProfilePage *profile_page) {
	RoccatProfilePagePrivate *priv = ROCCAT_PROFILE_PAGE_GET_PRIVATE(profile_page);
	profile_page->priv = priv;
	priv->profile_name = NULL;
}

static GObject *constructor(GType gtype, guint n_properties, GObjectConstructParam *properties) {
	GObject *obj;
	GtkBox *box;
	RoccatProfilePage *profile_page;
	GtkWidget *buttons;

	obj = G_OBJECT_CLASS(roccat_profile_page_parent_class)->constructor(gtype, n_properties, properties);
	box = GTK_BOX(obj);
	profile_page = ROCCAT_PROFILE_PAGE(obj);

	gtk_box_set_homogeneous(box, FALSE);
	gtk_box_set_spacing(box, 0);

	buttons = buttons_new(profile_page);
	gtk_box_pack_end(box, buttons, FALSE, FALSE, 0);

	gtk_widget_show_all(GTK_WIDGET(profile_page));

	return obj;
}

static void set_property(GObject *object, guint prop_id, GValue const *value, GParamSpec *pspec) {
	RoccatProfilePage *profile_page = ROCCAT_PROFILE_PAGE(object);
	switch(prop_id) {
	case PROP_RENAMEABLE:
		profile_page->priv->renameable = g_value_get_boolean(value);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
}

static void get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec) {
	RoccatProfilePage *profile_page = ROCCAT_PROFILE_PAGE(object);
	switch(prop_id) {
	case PROP_RENAMEABLE:
		g_value_set_boolean(value, profile_page->priv->renameable);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
	}
}

static void finalize(GObject *object) {
	RoccatProfilePagePrivate *priv = ROCCAT_PROFILE_PAGE(object)->priv;
	g_free(priv->profile_name);
	G_OBJECT_CLASS(roccat_profile_page_parent_class)->finalize(object);
}

static void roccat_profile_page_class_init(RoccatProfilePageClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->constructor = constructor;
	gobject_class->set_property = set_property;
	gobject_class->get_property = get_property;
	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(RoccatProfilePagePrivate));

	g_object_class_install_property(gobject_class, PROP_RENAMEABLE,
			g_param_spec_boolean("renameable",
					"Renameable",
					"Indicates if page is renameable",
					TRUE,
					G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

	signals[LOAD_FROM_FILE] = g_signal_new("load-from-file",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION, 0,
			NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

	signals[SAVE_TO_FILE] = g_signal_new("save-to-file",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION, 0,
			NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

	signals[RESET] = g_signal_new("reset",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION, 0,
			NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

	signals[RENAMED] = g_signal_new("renamed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION, 0,
			NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}
