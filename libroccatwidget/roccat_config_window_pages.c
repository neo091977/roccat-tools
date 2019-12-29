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

#include "roccat_config_window_pages.h"
#include "roccat_profile_page_tab_label.h"
#include "roccat_warning_dialog.h"
#include "gtk_roccat_helper.h"
#include "g_roccat_helper.h"
#include "g_cclosure_roccat_marshaller.h"
#include "i18n-lib.h"

#define ROCCAT_CONFIG_WINDOW_PAGES_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ROCCAT_CONFIG_WINDOW_PAGES_TYPE, RoccatConfigWindowPagesPrivate))

struct _RoccatConfigWindowPagesPrivate {
	GtkNotebook *notebook;
	guint active_page;
	guint variable_pages;
	GtkButton *add_button;
};

G_DEFINE_TYPE(RoccatConfigWindowPages, roccat_config_window_pages, ROCCAT_CONFIG_WINDOW_TYPE);

enum {
	PROP_0,
	PROP_VARIABLE_PAGES,
};

enum {
	ACTIVE_CHANGED,
	ADD_PAGE,
	REMOVE_PAGE,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

GtkWidget *roccat_config_window_pages_new(gchar const *device_name) {
	return GTK_WIDGET(g_object_new(ROCCAT_CONFIG_WINDOW_PAGES_TYPE,
			"device-name", device_name,
			NULL));
}

static RoccatProfilePageTabLabel *get_tab_label_from_profile_page(RoccatConfigWindowPages *config_window, RoccatProfilePage *profile_page) {
	return ROCCAT_PROFILE_PAGE_TAB_LABEL(gtk_notebook_get_tab_label(config_window->priv->notebook, GTK_WIDGET(profile_page)));
}

static RoccatProfilePageTabLabel *get_tab_label(RoccatConfigWindowPages *config_window, gint index) {
	return get_tab_label_from_profile_page(config_window, roccat_config_window_pages_get_page(config_window, index));
}

static RoccatProfilePage *get_profile_page_from_tab_label(RoccatConfigWindowPages *config_window, RoccatProfilePageTabLabel const *tab_label) {
	RoccatProfilePage *profile_page;
	guint i;
	guint count;

	count = roccat_config_window_pages_get_num_pages(config_window);
	for (i = 0; i < count; ++i) {
		profile_page = roccat_config_window_pages_get_page(config_window, i);
		if (get_tab_label_from_profile_page(config_window, profile_page) == tab_label)
			return profile_page;
	}

	return NULL;
}

gboolean roccat_config_window_pages_get_page_moved(RoccatConfigWindowPages *config_window, RoccatProfilePage *profile_page) {
	gint is_index = roccat_config_window_pages_get_page_index(config_window, profile_page);
	gint was_index = roccat_profile_page_tab_label_get_index(get_tab_label_from_profile_page(config_window, profile_page));
	return (is_index == was_index) ? FALSE : TRUE;
}

void roccat_config_window_pages_set_page_unmoved(RoccatConfigWindowPages *config_window, RoccatProfilePage *profile_page) {
	gint is_index = roccat_config_window_pages_get_page_index(config_window, profile_page);
	roccat_profile_page_tab_label_set_index(get_tab_label_from_profile_page(config_window, profile_page), is_index);
}

static void active_changed_cb(RoccatProfilePageTabLabel *tab_label, gpointer user_data) {
	RoccatConfigWindowPages *config_window = ROCCAT_CONFIG_WINDOW_PAGES(user_data);
	RoccatConfigWindowPagesPrivate *priv = ROCCAT_CONFIG_WINDOW_PAGES(user_data)->priv;
	guint new_active;

	new_active = roccat_config_window_pages_get_page_index(config_window, get_profile_page_from_tab_label(config_window, tab_label));
	if (priv->active_page != new_active) {
		priv->active_page = new_active;
		g_signal_emit(user_data, signals[ACTIVE_CHANGED], 0);
	}
}

static void roccat_config_window_pages_set_active_page(RoccatConfigWindowPages *config_window, guint index) {
	RoccatProfilePageTabLabel *tab_label = get_tab_label(config_window, index);
	if (tab_label) {
		roccat_profile_page_tab_label_set_active(tab_label);
		config_window->priv->active_page = index;
	}
}

void roccat_config_window_pages_set_active_page_blocked(RoccatConfigWindowPages *config_window, guint index) {
	RoccatProfilePageTabLabel *tab_label = get_tab_label(config_window, index);
	if (tab_label) {
		g_signal_handlers_block_by_func(G_OBJECT(tab_label), G_CALLBACK(active_changed_cb), config_window);
		roccat_profile_page_tab_label_set_active(tab_label);
		g_signal_handlers_unblock_by_func(G_OBJECT(tab_label), G_CALLBACK(active_changed_cb), config_window);
		config_window->priv->active_page = index;
	}
}

gint roccat_config_window_pages_get_active_page(RoccatConfigWindowPages *config_window) {
	return config_window->priv->active_page;
}

static void page_reordered_cb(GtkNotebook *notebook, GtkWidget *child, guint page_num, gpointer user_data) {
	RoccatConfigWindowPages *config_window = ROCCAT_CONFIG_WINDOW_PAGES(user_data);
	roccat_config_window_pages_set_active_page(config_window, roccat_config_window_pages_get_active_page(config_window));
}

static void add_button_clicked_cb(GtkButton *button, gpointer user_data) {
	g_signal_emit(user_data, signals[ADD_PAGE], 0);
}

static void roccat_config_window_pages_init(RoccatConfigWindowPages *config_window) {
	RoccatConfigWindowPagesPrivate *priv = ROCCAT_CONFIG_WINDOW_PAGES_GET_PRIVATE(config_window);
	GtkWidget *image;

	config_window->priv = priv;

	priv->notebook = GTK_NOTEBOOK(gtk_notebook_new());
	g_signal_connect(G_OBJECT(priv->notebook), "page-reordered", G_CALLBACK(page_reordered_cb), config_window);

	image = gtk_image_new_from_stock(GTK_STOCK_ADD, GTK_ICON_SIZE_MENU);
	priv->add_button = GTK_BUTTON(gtk_button_new());
	gtk_button_set_image(priv->add_button, image);
	gtk_button_set_relief(priv->add_button, GTK_RELIEF_NONE);
	g_signal_connect(G_OBJECT(priv->add_button), "clicked", G_CALLBACK(add_button_clicked_cb), config_window);
	gtk_notebook_set_action_widget(priv->notebook, GTK_WIDGET(priv->add_button), GTK_PACK_END);

	roccat_config_window_add(ROCCAT_CONFIG_WINDOW(config_window), GTK_WIDGET(priv->notebook));

	gtk_widget_show_all(GTK_WIDGET(priv->notebook));
}

static void update_state_first_page(RoccatConfigWindowPages *config_window) {
	RoccatProfilePageTabLabel *tab_label;

	tab_label = get_tab_label(config_window, 0);

	if (config_window->priv->variable_pages && (roccat_config_window_pages_get_num_pages(config_window) > 1))
		roccat_profile_page_tab_label_set_closeable(tab_label, TRUE);
	else
		roccat_profile_page_tab_label_set_closeable(tab_label, FALSE);
}

static void update_state_show_tabs(RoccatConfigWindowPages *config_window) {
	RoccatConfigWindowPagesPrivate *priv = config_window->priv;
	gboolean show_tabs = (priv->variable_pages > 2) || (roccat_config_window_pages_get_num_pages(config_window) > 1);
	gtk_notebook_set_show_tabs(priv->notebook, show_tabs);
}

static void update_state_add_button(RoccatConfigWindowPages *config_window) {
	RoccatConfigWindowPagesPrivate *priv = config_window->priv;
	if (priv->variable_pages && (roccat_config_window_pages_get_num_pages(config_window) < priv->variable_pages))
		gtk_widget_show(GTK_WIDGET(priv->add_button));
	else
		gtk_widget_hide(GTK_WIDGET(priv->add_button));
}

static GObject *constructor(GType gtype, guint n_properties, GObjectConstructParam *properties) {
	GObject *obj;
	RoccatConfigWindowPages *config_window;

	obj = G_OBJECT_CLASS(roccat_config_window_pages_parent_class)->constructor(gtype, n_properties, properties);
	config_window = ROCCAT_CONFIG_WINDOW_PAGES(obj);

	update_state_show_tabs(config_window);
	update_state_add_button(config_window);

	return obj;
}

static void set_property(GObject *object, guint prop_id, GValue const *value, GParamSpec *pspec) {
	RoccatConfigWindowPagesPrivate *priv = ROCCAT_CONFIG_WINDOW_PAGES(object)->priv;
	switch(prop_id) {
	case PROP_VARIABLE_PAGES:
		priv->variable_pages = g_value_get_uint(value);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
}

static void get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec) {
	RoccatConfigWindowPagesPrivate *priv = ROCCAT_CONFIG_WINDOW_PAGES(object)->priv;
	switch(prop_id) {
	case PROP_VARIABLE_PAGES:
		g_value_set_uint(value, priv->variable_pages);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
}

static void roccat_config_window_pages_class_init(RoccatConfigWindowPagesClass *klass) {
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->constructor = constructor;
	gobject_class->set_property = set_property;
	gobject_class->get_property = get_property;

	g_type_class_add_private(klass, sizeof(RoccatConfigWindowPagesPrivate));

	 g_object_class_install_property(gobject_class, PROP_VARIABLE_PAGES,
			 g_param_spec_uint("variable-pages",
					 "Variable page count",
					 "Variable page count",
					 0, G_MAXUINT, 0,
					 G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));

	signals[ACTIVE_CHANGED] = g_signal_new("active-changed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION, 0,
			NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

	signals[ADD_PAGE] = g_signal_new("add-page",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION, 0,
			NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

	signals[REMOVE_PAGE] = g_signal_new("remove-page",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION, 0,
			NULL, NULL, g_cclosure_roccat_marshal_BOOLEAN__UINT, G_TYPE_BOOLEAN, 1, G_TYPE_UINT);
}

static void remove_page(RoccatConfigWindowPages *config_window, RoccatProfilePage *profile_page) {
	gint index = roccat_config_window_pages_get_page_index(config_window, profile_page);
	gboolean should_remove;
	g_signal_emit((gpointer)config_window, signals[REMOVE_PAGE], 0, index, &should_remove);
	if (should_remove) {
		gtk_notebook_remove_page(config_window->priv->notebook, index);
		roccat_config_window_pages_set_active_page(config_window, roccat_config_window_pages_get_active_page(config_window));
		update_state_first_page(config_window);
		update_state_add_button(config_window);
	}
}

void roccat_config_window_pages_remove_pages(RoccatConfigWindowPages *config_window) {
	while (roccat_config_window_pages_get_page(config_window, 0))
		gtk_notebook_remove_page(config_window->priv->notebook, 0);
	update_state_show_tabs(config_window);
	update_state_add_button(config_window);
}

static void tab_label_close_cb(RoccatProfilePageTabLabel *tab_label, gpointer user_data) {
	RoccatConfigWindowPages *config_window = ROCCAT_CONFIG_WINDOW_PAGES(user_data);
	RoccatProfilePage *profile_page;

	if (roccat_config_window_pages_get_num_pages(config_window) > 1) {
		profile_page = get_profile_page_from_tab_label(config_window, tab_label);
		remove_page(config_window, profile_page);
	}
}

RoccatProfilePage *roccat_config_window_pages_get_page(RoccatConfigWindowPages *config_window, guint index) {
	return ROCCAT_PROFILE_PAGE(gtk_notebook_get_nth_page(config_window->priv->notebook, index));
}

gint roccat_config_window_pages_get_page_index(RoccatConfigWindowPages *config_window, RoccatProfilePage *profile_page) {
	return gtk_notebook_page_num(config_window->priv->notebook, GTK_WIDGET(profile_page));
}

guint roccat_config_window_pages_get_num_pages(RoccatConfigWindowPages *config_window) {
	return gtk_notebook_get_n_pages(config_window->priv->notebook);
}

static void profile_renamed_cb(RoccatProfilePage *profile_page, gpointer user_data) {
	RoccatConfigWindowPages *config_window = ROCCAT_CONFIG_WINDOW_PAGES(user_data);
	RoccatProfilePageTabLabel *tab_label = get_tab_label_from_profile_page(config_window, profile_page);
	gchar *name = roccat_profile_page_get_name(profile_page);
	roccat_profile_page_tab_label_set_name(tab_label, name);
	g_free(name);
}

void roccat_config_window_pages_append_page(RoccatConfigWindowPages *config_window, RoccatProfilePage *profile_page) {
	RoccatConfigWindowPagesPrivate *priv = config_window->priv;
	RoccatProfilePageTabLabel *tab_label;
	gint index;

	tab_label = ROCCAT_PROFILE_PAGE_TAB_LABEL(roccat_profile_page_tab_label_new());

	index = gtk_notebook_append_page(priv->notebook, GTK_WIDGET(profile_page), GTK_WIDGET(tab_label));
	gtk_notebook_set_tab_reorderable(priv->notebook, GTK_WIDGET(profile_page), TRUE);

	if (index > 0)
		roccat_profile_page_tab_label_set_group(tab_label, roccat_profile_page_tab_label_get_group(get_tab_label(config_window, 0)));
	roccat_profile_page_tab_label_set_index(tab_label, index);
	roccat_profile_page_tab_label_set_closeable(tab_label, priv->variable_pages > 1);

	g_signal_connect(G_OBJECT(tab_label), "active-changed", G_CALLBACK(active_changed_cb), config_window);
	g_signal_connect(G_OBJECT(tab_label), "close", G_CALLBACK(tab_label_close_cb), config_window);

	g_signal_connect(G_OBJECT(profile_page), "renamed", G_CALLBACK(profile_renamed_cb), config_window);

	update_state_show_tabs(config_window);
	update_state_add_button(config_window);
	update_state_first_page(config_window);
}
