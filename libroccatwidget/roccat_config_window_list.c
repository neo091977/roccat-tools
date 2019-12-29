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

#include "roccat_config_window_list.h"
#include "roccat_warning_dialog.h"
#include "roccat_save_dialog.h"
#include "gtk_roccat_helper.h"
#include "g_roccat_helper.h"
#include "g_cclosure_roccat_marshaller.h"
#include "i18n-lib.h"
#include <gaminggear/gaminggear_profile_table.h>
#include <gaminggear/gaminggear_profile_data.h>

#define ROCCAT_CONFIG_WINDOW_LIST_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ROCCAT_CONFIG_WINDOW_LIST_TYPE, RoccatConfigWindowListPrivate))

struct _RoccatConfigWindowListPrivate {
	gchar *path;
	GType profile_type;
	GtkPaned *paned;
	GaminggearProfileTable *profile_table;
	GaminggearProfilePage *profile_page;
	guint num_profiles;
};

G_DEFINE_TYPE(RoccatConfigWindowList, roccat_config_window_list, ROCCAT_CONFIG_WINDOW_TYPE);

enum {
	PROP_0,
	PROP_NUM_PROFILES,
	PROP_PROFILE_TYPE,
	PROP_PROFILE_PAGE,
	PROP_PATH,
};

enum {
	PROFILE_DATA_CHANGED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

gchar const *roccat_config_window_list_get_default_profile_name(RoccatConfigWindowList *window) {
	return gaminggear_profile_table_get_default_profile_name(window->priv->profile_table);
}

void roccat_config_window_list_set_default_profile_name(RoccatConfigWindowList *window, gchar const *name) {
	gaminggear_profile_table_set_default_profile_name(window->priv->profile_table, name);
}

GaminggearProfilePage *roccat_config_window_list_get_profile_page(RoccatConfigWindowList *window) {
	RoccatConfigWindowListPrivate *priv = window->priv;
	g_object_ref(priv->profile_page);
	return priv->profile_page;
}

static void roccat_config_window_list_init(RoccatConfigWindowList *config_window) {
	config_window->priv = ROCCAT_CONFIG_WINDOW_LIST_GET_PRIVATE(config_window);
}

static void profile_selected_cb(GaminggearProfileTable *tree_view, GaminggearProfileData *profile_data, gpointer user_data) {
	gaminggear_profile_page_set_profile_data(ROCCAT_CONFIG_WINDOW_LIST(user_data)->priv->profile_page, profile_data);
}

static void profile_removed_cb(GaminggearProfileTable *tree_view, GaminggearProfileData *profile_data, gpointer user_data) {
	RoccatConfigWindowListPrivate *priv = ROCCAT_CONFIG_WINDOW_LIST(user_data)->priv;
	GaminggearProfileData *actual;

	actual = gaminggear_profile_page_get_profile_data(priv->profile_page);
	if (actual == profile_data)
		gaminggear_profile_page_set_profile_data(priv->profile_page, NULL);

	g_object_unref(actual);
}

static void add_profile_clicked_cb(GtkButton *button, gpointer user_data) {
	gaminggear_profile_table_add_profile(ROCCAT_CONFIG_WINDOW_LIST(user_data)->priv->profile_table);
}

static gboolean save_all(RoccatConfigWindowList *window, gboolean ask) {
	RoccatConfigWindowListPrivate *priv = window->priv;
	GError *local_error = NULL;
	gboolean retval;

	gaminggear_profile_page_update_data(priv->profile_page);

	if (!gaminggear_profile_table_get_modified(priv->profile_table)) {
		retval = TRUE;
		goto exit;
	}

	if (ask) {
		switch (roccat_save_unsaved_dialog(GTK_WINDOW(window), TRUE)) {
		case GTK_RESPONSE_CANCEL:
			/* not saving by user request */
			retval = FALSE;
			goto exit;
			break;
		case GTK_RESPONSE_ACCEPT:
			break;
		case GTK_RESPONSE_REJECT:
			/* discarding by user request */
			retval = TRUE;
			goto exit;
			break;
		default:
			break;
		}
	}

	roccat_config_window_warn_if_no_device(ROCCAT_CONFIG_WINDOW(window));

	retval = gaminggear_profile_table_store(priv->profile_table,
			GAMINGGEAR_DEVICE(roccat_config_window_get_device(ROCCAT_CONFIG_WINDOW(window))), &local_error);
	roccat_handle_error_dialog(GTK_WINDOW(window), _("Could not save unsaved data"), &local_error);

	/* On error, there might have been partial save, so emit always */
	g_signal_emit(window, signals[PROFILE_DATA_CHANGED], 0);
exit:
	return retval;
}

static gboolean delete_event_cb(GtkWidget *window, GdkEvent *event, gpointer user_data) {
	return !save_all(ROCCAT_CONFIG_WINDOW_LIST(window), TRUE);
}

static void save_all_cb(RoccatConfigWindow *roccat_window, gpointer user_data) {
	save_all(ROCCAT_CONFIG_WINDOW_LIST(roccat_window), FALSE);
}

static GObject *constructor(GType gtype, guint n_properties, GObjectConstructParam *properties) {
	GObject *obj;
	RoccatConfigWindowList *config_window;
	RoccatConfigWindowListPrivate *priv;
	GtkWidget *list_box;
	GtkWidget *button;
	GtkWidget *scrolled_window;

	obj = G_OBJECT_CLASS(roccat_config_window_list_parent_class)->constructor(gtype, n_properties, properties);
	config_window = ROCCAT_CONFIG_WINDOW_LIST(obj);
	priv = config_window->priv;

	priv->paned = GTK_PANED(gtk_hpaned_new());
	list_box = gtk_vbox_new(FALSE, 0);
	scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	priv->profile_table = GAMINGGEAR_PROFILE_TABLE(gaminggear_profile_table_new(priv->profile_type, priv->path, priv->num_profiles));
	button = gtk_button_new_with_label(_("Add profile"));

	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

	gtk_container_add(GTK_CONTAINER(scrolled_window), GTK_WIDGET(priv->profile_table));
	gtk_box_pack_start(GTK_BOX(list_box), GTK_WIDGET(scrolled_window), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(list_box), button, FALSE, FALSE, 0);
	gtk_paned_pack1(priv->paned, list_box, FALSE, FALSE);
	roccat_config_window_add(ROCCAT_CONFIG_WINDOW(config_window), GTK_WIDGET(priv->paned));

	gtk_paned_pack2(priv->paned, GTK_WIDGET(priv->profile_page), TRUE, FALSE);

	gtk_widget_show_all(GTK_WIDGET(priv->paned));

	g_signal_connect(G_OBJECT(priv->profile_table), "selected", G_CALLBACK(profile_selected_cb), config_window);
	g_signal_connect(G_OBJECT(priv->profile_table), "removed", G_CALLBACK(profile_removed_cb), config_window);
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(add_profile_clicked_cb), config_window);
	g_signal_connect(G_OBJECT(config_window), "delete-event", G_CALLBACK(delete_event_cb), NULL);
	g_signal_connect(G_OBJECT(config_window), "save-all", G_CALLBACK(save_all_cb), NULL);

	return obj;
}

static void finalize(GObject *object) {
	RoccatConfigWindowListPrivate *priv = ROCCAT_CONFIG_WINDOW_LIST(object)->priv;
	g_clear_pointer(&priv->path, g_free);
	G_OBJECT_CLASS(roccat_config_window_list_parent_class)->finalize(object);
}

static void set_property(GObject *object, guint prop_id, GValue const *value, GParamSpec *pspec) {
	RoccatConfigWindowListPrivate *priv = ROCCAT_CONFIG_WINDOW_LIST(object)->priv;
	switch(prop_id) {
	case PROP_PROFILE_TYPE:
		priv->profile_type = g_value_get_gtype(value);
		break;
	case PROP_NUM_PROFILES:
		priv->num_profiles = g_value_get_uint(value);
		break;
	case PROP_PROFILE_PAGE:
		priv->profile_page = g_value_get_object(value);
		g_object_ref(G_OBJECT(priv->profile_page));
		break;
	case PROP_PATH:
		priv->path = g_strdup(g_value_get_string(value));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
}

static void get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec) {
	RoccatConfigWindowListPrivate *priv = ROCCAT_CONFIG_WINDOW_LIST(object)->priv;
	switch(prop_id) {
	case PROP_PROFILE_TYPE:
		g_value_set_gtype(value, priv->profile_type);
		break;
	case PROP_NUM_PROFILES:
		g_value_set_uint(value, priv->num_profiles);
		break;
	case PROP_PROFILE_PAGE:
		g_value_set_object(value, priv->profile_page);
		break;
	case PROP_PATH:
		g_value_set_string(value, priv->path);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
}

static void roccat_config_window_list_class_init(RoccatConfigWindowListClass *klass) {
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->constructor = constructor;
	gobject_class->set_property = set_property;
	gobject_class->get_property = get_property;
	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(RoccatConfigWindowListPrivate));

	g_object_class_install_property(gobject_class, PROP_NUM_PROFILES,
			g_param_spec_uint("num-profiles",
					"num-profiles",
					"Number of profiles",
					0, G_MAXUINT, 1,
					G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

	g_object_class_install_property(gobject_class, PROP_PROFILE_TYPE,
			g_param_spec_gtype("profile-type",
					"profile type",
					"Profile type",
					GAMINGGEAR_PROFILE_DATA_TYPE,
					G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

	g_object_class_install_property(gobject_class, PROP_PROFILE_PAGE,
			g_param_spec_object("profile-page",
					"profile page",
					"Profile page",
					GAMINGGEAR_PROFILE_PAGE_TYPE,
					G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

	g_object_class_install_property(gobject_class, PROP_PATH,
			g_param_spec_string("path",
					"path",
					"Path",
					"",
					G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

	signals[PROFILE_DATA_CHANGED] = g_signal_new("profile-data-changed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION, 0,
			NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}

gboolean roccat_config_window_list_load(RoccatConfigWindowList *window, GError **error) {
	return gaminggear_profile_table_load(window->priv->profile_table, error);
}

gboolean roccat_config_window_list_read(RoccatConfigWindowList *window, RoccatDevice *device, guint count, GError **error) {
	RoccatConfigWindowListPrivate *priv = window->priv;
	GaminggearProfileData *actual;

	if (!gaminggear_profile_table_read(priv->profile_table, GAMINGGEAR_DEVICE(device), error))
		return FALSE;

	if (!gaminggear_profile_table_fill(priv->profile_table, GAMINGGEAR_DEVICE(device), count, error))
		return FALSE;

	actual = gaminggear_profile_page_get_profile_data(priv->profile_page);

	if (!actual)
		return TRUE;

	if (gaminggear_profile_data_get_hardware_index(actual) != GAMINGGEAR_PROFILE_DATA_GAMEFILE_PROFILE_INVALID)
		gaminggear_profile_page_set_profile_data(priv->profile_page, actual);
	g_object_unref(actual);

	return TRUE;
}
