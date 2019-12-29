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

#include "roccat_file_entry.h"
#include "i18n-lib.h"

#define ROCCAT_FILE_ENTRY_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ROCCAT_FILE_ENTRY_TYPE, RoccatFileEntryClass))
#define IS_ROCCAT_FILE_ENTRY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ROCCAT_FILE_ENTRY_TYPE))
#define ROCCAT_FILE_ENTRY_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ROCCAT_FILE_ENTRY_TYPE, RoccatFileEntryPrivate))

typedef struct _RoccatFileEntryClass RoccatFileEntryClass;
typedef struct _RoccatFileEntryPrivate RoccatFileEntryPrivate;

struct _RoccatFileEntry {
	GtkHBox hbox;
	RoccatFileEntryPrivate *priv;
};

struct _RoccatFileEntryClass {
	GtkHBoxClass parent_class;
	void(*changed)(RoccatFileEntry *entry);
};

struct _RoccatFileEntryPrivate {
	GtkEntry *entry;
	GtkButton *button;
};

enum {
	PROP_0,
	PROP_TEXT,
};

G_DEFINE_TYPE(RoccatFileEntry, roccat_file_entry, GTK_TYPE_HBOX);

gchar const *roccat_file_entry_get_text(RoccatFileEntry *file_entry) {
	return (gtk_entry_get_text(file_entry->priv->entry));
}

void roccat_file_entry_set_text(RoccatFileEntry *file_entry, gchar const *new_value) {
	gtk_entry_set_text(file_entry->priv->entry, new_value);
}

GtkWidget *roccat_file_entry_new(void) {
	return GTK_WIDGET(g_object_new(ROCCAT_FILE_ENTRY_TYPE, NULL));
}

static void roccat_file_entry_set_property(GObject *object, guint prop_id, GValue const *new_value, GParamSpec *pspec) {
	RoccatFileEntry *file_entry = ROCCAT_FILE_ENTRY(object);

	switch(prop_id) {
	case PROP_TEXT:
		roccat_file_entry_set_text(file_entry, g_value_get_string(new_value));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
	}
}

static void roccat_file_entry_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec) {
	RoccatFileEntry *file_entry = ROCCAT_FILE_ENTRY(object);

	switch(prop_id) {
	case PROP_TEXT:
		g_value_set_string(value, roccat_file_entry_get_text(file_entry));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
	}
}

static void roccat_file_entry_entry_changed_cb(GtkEntry *entry, RoccatFileEntry *file_entry) {
	g_signal_emit_by_name((gpointer)file_entry, "changed");
}

static void roccat_file_entry_button_clicked_cb(GtkButton *button, RoccatFileEntry *file_entry) {
	GtkWidget *dialog;
	gchar *filename;

	dialog = gtk_file_chooser_dialog_new(_("Choose file"),
			NULL,
			GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
			NULL);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		gtk_entry_set_text(file_entry->priv->entry, filename);
	}
	gtk_widget_destroy(dialog);
}

static void roccat_file_entry_init(RoccatFileEntry *file_entry) {
	RoccatFileEntryPrivate *priv = ROCCAT_FILE_ENTRY_GET_PRIVATE(file_entry);

	file_entry->priv = priv;

	priv->entry = GTK_ENTRY(gtk_entry_new());
	gtk_box_pack_start(GTK_BOX(file_entry), GTK_WIDGET(priv->entry), TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(priv->entry), "changed", G_CALLBACK(roccat_file_entry_entry_changed_cb), file_entry);

	priv->button = GTK_BUTTON(gtk_button_new_from_stock(GTK_STOCK_OPEN));
	gtk_box_pack_start(GTK_BOX(file_entry), GTK_WIDGET(priv->button), FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(priv->button), "clicked", G_CALLBACK(roccat_file_entry_button_clicked_cb), file_entry);
}

static void roccat_file_entry_class_init(RoccatFileEntryClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = (GObjectClass*)klass;

	gobject_class->set_property = roccat_file_entry_set_property;
	gobject_class->get_property = roccat_file_entry_get_property;

	g_type_class_add_private(klass, sizeof(RoccatFileEntryPrivate));

	g_signal_new("changed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			G_STRUCT_OFFSET(RoccatFileEntryClass, changed),
			NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

	g_object_class_install_property(gobject_class, PROP_TEXT,
			g_param_spec_string("text",
					"text",
					"Reads or sets value",
					"", G_PARAM_READWRITE));
}
