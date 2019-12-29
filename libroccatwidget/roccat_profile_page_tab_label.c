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

#include "roccat_profile_page_tab_label.h"
#include "i18n-lib.h"

#define ROCCAT_PROFILE_PAGE_TAB_LABEL_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ROCCAT_PROFILE_PAGE_TAB_LABEL_TYPE, RoccatProfilePageTabLabelPrivate))

struct _RoccatProfilePageTabLabelPrivate {
	gint index;
	gchar *text;
	GtkRadioButton *active;
	GtkLabel *label;
	GtkButton *close;
	gboolean closeable;
};

G_DEFINE_TYPE(RoccatProfilePageTabLabel, roccat_profile_page_tab_label, GTK_TYPE_HBOX);

enum {
	PROP_0,
	PROP_CLOSEABLE,
};

enum {
	ACTIVE_CHANGED,
	CLOSE,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

static void update_label_text(RoccatProfilePageTabLabel *tab_label) {
	RoccatProfilePageTabLabelPrivate *priv = tab_label->priv;
	gchar *string;

	if (priv->text && strlen(priv->text) != 0)
		gtk_label_set_text(priv->label, priv->text);
	else {
		string = g_strdup_printf(_("Profile %u"), priv->index + 1);
		gtk_label_set_text(priv->label, string);
		g_free(string);
	}
}

void roccat_profile_page_tab_label_set_name(RoccatProfilePageTabLabel *tab_label, gchar const *name) {
	RoccatProfilePageTabLabelPrivate *priv = tab_label->priv;
	if (g_strcmp0(priv->text, name)) {
		g_free(priv->text);
		priv->text = g_strdup(name);
		update_label_text(tab_label);
	}
}

gchar *roccat_profile_page_tab_label_get_name(RoccatProfilePageTabLabel *tab_label) {
	return g_strdup(tab_label->priv->text);
}

void roccat_profile_page_tab_label_set_index(RoccatProfilePageTabLabel *tab_label, gint index) {
	RoccatProfilePageTabLabelPrivate *priv = tab_label->priv;
	if (priv->index != index) {
		priv->index = index;
		update_label_text(tab_label);
	}
}

gint roccat_profile_page_tab_label_get_index(RoccatProfilePageTabLabel *tab_label) {
	return tab_label->priv->index;
}

void roccat_profile_page_tab_label_set_active(RoccatProfilePageTabLabel *tab_label) {
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tab_label->priv->active), TRUE);
}

void roccat_profile_page_tab_label_set_group(RoccatProfilePageTabLabel *tab_label, GSList *group) {
	gtk_radio_button_set_group(tab_label->priv->active, group);
}

GSList *roccat_profile_page_tab_label_get_group(RoccatProfilePageTabLabel *tab_label) {
	return gtk_radio_button_get_group(tab_label->priv->active);
}

GtkWidget *roccat_profile_page_tab_label_new(void) {
	return GTK_WIDGET(g_object_new(ROCCAT_PROFILE_PAGE_TAB_LABEL_TYPE, NULL));
}

static void active_toggled_cb(GtkToggleButton *togglebutton, gpointer user_data) {
	if (gtk_toggle_button_get_active(togglebutton))
		g_signal_emit(user_data, signals[ACTIVE_CHANGED], 0);
}

static void close_clicked_cb(GtkButton *button, gpointer user_data) {
	RoccatProfilePageTabLabel *tab_label = ROCCAT_PROFILE_PAGE_TAB_LABEL(user_data);
	if (tab_label->priv->closeable)
		g_signal_emit(user_data, signals[CLOSE], 0);
}

static void update_state_closeable(RoccatProfilePageTabLabel *tab_label) {
	RoccatProfilePageTabLabelPrivate *priv = tab_label->priv;
	if (priv->closeable)
		gtk_widget_show(GTK_WIDGET(priv->close));
	else
		gtk_widget_hide(GTK_WIDGET(priv->close));
}

static void roccat_profile_page_tab_label_init(RoccatProfilePageTabLabel *tab_label) {
	RoccatProfilePageTabLabelPrivate *priv = ROCCAT_PROFILE_PAGE_TAB_LABEL_GET_PRIVATE(tab_label);
	GtkWidget *image;

	tab_label->priv = priv;
	priv->text = NULL;
	priv->index = -1;

	priv->active = GTK_RADIO_BUTTON(gtk_radio_button_new(NULL));
	priv->label = GTK_LABEL(gtk_label_new(NULL));
	priv->close = GTK_BUTTON(gtk_button_new());

	image = gtk_image_new_from_stock(GTK_STOCK_CLOSE, GTK_ICON_SIZE_MENU);
	gtk_button_set_image(priv->close, image); // NOTE image shown or not is configuration dependent, maybe container_add
	gtk_button_set_relief(priv->close, GTK_RELIEF_NONE);
	gtk_button_set_focus_on_click(priv->close, FALSE);
	
	g_signal_connect(G_OBJECT(priv->active), "toggled", G_CALLBACK(active_toggled_cb), tab_label);
	g_signal_connect(G_OBJECT(priv->close), "clicked", G_CALLBACK(close_clicked_cb), tab_label);

	gtk_box_pack_start(GTK_BOX(tab_label), GTK_WIDGET(priv->active), FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(tab_label), GTK_WIDGET(priv->label), FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(tab_label), GTK_WIDGET(priv->close), FALSE, FALSE, 0);
	
	gtk_widget_show(GTK_WIDGET(priv->active));
	gtk_widget_show(GTK_WIDGET(priv->label));
	update_state_closeable(tab_label);
}

gboolean roccat_profile_page_tab_label_get_closeable(RoccatProfilePageTabLabel *tab_label) {
	return tab_label->priv->closeable;
}

void roccat_profile_page_tab_label_set_closeable(RoccatProfilePageTabLabel *tab_label, gboolean closeable) {
	tab_label->priv->closeable = closeable;
	update_state_closeable(tab_label);
}

static void set_property(GObject *object, guint prop_id, GValue const *value, GParamSpec *pspec) {
	RoccatProfilePageTabLabel *tab_label = ROCCAT_PROFILE_PAGE_TAB_LABEL(object);
	switch(prop_id) {
	case PROP_CLOSEABLE:
		roccat_profile_page_tab_label_set_closeable(tab_label, g_value_get_boolean(value));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
}

static void get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec) {
	RoccatProfilePageTabLabel *tab_label = ROCCAT_PROFILE_PAGE_TAB_LABEL(object);
	switch(prop_id) {
	case PROP_CLOSEABLE:
		g_value_set_boolean(value, roccat_profile_page_tab_label_get_closeable(tab_label));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
	}
}

static void roccat_profile_page_tab_label_class_init(RoccatProfilePageTabLabelClass *klass) {
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
	gobject_class->set_property = set_property;
	gobject_class->get_property = get_property;

	g_type_class_add_private(klass, sizeof(RoccatProfilePageTabLabelPrivate));

	g_object_class_install_property(gobject_class, PROP_CLOSEABLE,
		g_param_spec_boolean("closeable",
				"Closeable",
				"Closeable",
				FALSE,
				G_PARAM_READWRITE));

	signals[ACTIVE_CHANGED] = g_signal_new("active-changed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION, 0,
			NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

	signals[CLOSE] = g_signal_new("close",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION, 0,
			NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}
