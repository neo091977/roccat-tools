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

#include "luaconfig_window.h"
#include "luaconfig_profile_page.h"
#include "luaconfig_surface_check.h"
#include "luaconfig_configuration_dialog.h"
#include "roccat_warning_dialog.h"
#include "roccat_save_dialog.h"
#include "roccat_helper.h"
#include "roccat.h"
#include "i18n.h"
#include "g_roccat_helper.h"

#define LUACONFIG_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), LUACONFIG_WINDOW_TYPE, LuaconfigWindowClass))
#define IS_LUACONFIG_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), LUACONFIG_WINDOW_TYPE))
#define LUACONFIG_WINDOW_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), LUACONFIG_WINDOW_TYPE, LuaconfigWindowPrivate))

typedef struct _LuaconfigWindowClass LuaconfigWindowClass;
typedef struct _LuaconfigWindowPrivate LuaconfigWindowPrivate;

struct _LuaconfigWindow {
	RoccatConfigWindowPages parent;
	LuaconfigWindowPrivate *priv;
};

struct _LuaconfigWindowClass {
	RoccatConfigWindowPagesClass parent_class;
};

struct _LuaconfigWindowPrivate {
	RoccatKeyFile *config;
};

G_DEFINE_TYPE(LuaconfigWindow, luaconfig_window, ROCCAT_CONFIG_WINDOW_PAGES_TYPE);

static gboolean save_single(LuaconfigWindow *window, LuaRmp *rmp, GError **error) {
	RoccatDevice *device = roccat_config_window_get_device(ROCCAT_CONFIG_WINDOW(window));

	if (device)
		lua_rmp_save(device, rmp, error);
	else
		lua_rmp_save_actual(rmp, error);

	if (*error)
		return FALSE;
	else {
		/* If data has been saved, set this as new actual state in profile_page */
		luaconfig_profile_page_set_rmp(LUACONFIG_PROFILE_PAGE(roccat_config_window_pages_get_page(ROCCAT_CONFIG_WINDOW_PAGES(window), 0)), rmp);
		return TRUE;
	}
}

static gboolean save_all(LuaconfigWindow *window, gboolean ask) {
	LuaRmp *rmp;
	GError *local_error = NULL;
	gboolean retval;

	rmp = luaconfig_profile_page_get_rmp(LUACONFIG_PROFILE_PAGE(roccat_config_window_pages_get_page(ROCCAT_CONFIG_WINDOW_PAGES(window), 0)));

	if (!lua_rmp_get_modified(rmp)) {
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

	retval = TRUE;
	retval = save_single(window, rmp, &local_error);
	roccat_handle_error_dialog(GTK_WINDOW(window), _("Could not save unsaved data"), &local_error);

exit:
	lua_rmp_free(rmp);
	return retval;
}

static gboolean delete_event_cb(GtkWidget *window, GdkEvent *event, gpointer user_data) {
	return !save_all(LUACONFIG_WINDOW(window), TRUE);
}

static void add_file_filter_to_file_chooser(GtkFileChooser *chooser) {
	GtkFileFilter *filter;
	filter = gtk_file_filter_new();
	gtk_file_filter_set_name(filter, _("Lua Windows profiles"));
	gtk_file_filter_add_pattern(filter, "*." ROCCAT_MOUSE_PROFILE_EXTENSION);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(chooser), filter);

	filter = gtk_file_filter_new();
	gtk_file_filter_set_name(filter, _("All files"));
	gtk_file_filter_add_pattern(filter, "*");
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(chooser), filter);
}

static void save_profile_to_file_cb(RoccatProfilePage *profile_page, gpointer user_data) {
	LuaconfigWindow *window = LUACONFIG_WINDOW(user_data);
	LuaconfigWindowPrivate *priv = window->priv;
	GtkWidget *dialog;
	gchar *filename, *path;
	gchar *temp_filename;
	GError *error = NULL;
	LuaRmp *rmp;

	rmp = luaconfig_profile_page_get_rmp(LUACONFIG_PROFILE_PAGE(profile_page));

	dialog = gtk_file_chooser_dialog_new(_("Save profile"),
			GTK_WINDOW(window),
			GTK_FILE_CHOOSER_ACTION_SAVE,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
			NULL);

	add_file_filter_to_file_chooser(GTK_FILE_CHOOSER(dialog));

	path = lua_configuration_get_rmp_path(priv->config);
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), path);
	g_free(path);

	gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), "Profile." ROCCAT_MOUSE_PROFILE_EXTENSION);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		temp_filename = roccat_create_filename_with_extension(filename, ROCCAT_MOUSE_PROFILE_EXTENSION);
		g_free(filename);

		path = g_path_get_dirname(temp_filename);
		lua_configuration_set_rmp_path(priv->config, path);
		g_free(path);

		lua_rmp_write_with_path(temp_filename, rmp, &error);
		g_free(temp_filename);
		if (error) {
			roccat_warning_dialog(GTK_WINDOW(dialog), _("Could not save profile"), error->message);
			g_error_free(error);
		}
	}
	gtk_widget_destroy(dialog);
	lua_rmp_free(rmp);
}

static void load_profile_from_file_cb(RoccatProfilePage *profile_page, gpointer user_data) {
	LuaconfigWindow *window = LUACONFIG_WINDOW(user_data);
	LuaconfigWindowPrivate *priv = window->priv;
	GtkWidget *dialog;
	gchar *filename, *path;
	GError *error = NULL;
	LuaRmp *rmp;

	dialog = gtk_file_chooser_dialog_new(_("Load profile"),
			GTK_WINDOW(window),
			GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
			NULL);

	add_file_filter_to_file_chooser(GTK_FILE_CHOOSER(dialog));
	path = lua_configuration_get_rmp_path(priv->config);
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), path);
	g_free(path);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		path = g_path_get_dirname(filename);
		lua_configuration_set_rmp_path(priv->config, path);
		g_free(path);

		rmp = lua_rmp_read_with_path(filename, &error);
		if (error) {
			roccat_warning_dialog(GTK_WINDOW(dialog), _("Could not load profile"), error->message);
			g_error_free(error);
		} else {
			/* Make sure it has to be saved */
			lua_rmp_set_modified(rmp);
			luaconfig_profile_page_set_rmp(LUACONFIG_PROFILE_PAGE(profile_page), rmp);
			lua_rmp_free(rmp);
		}

		g_free(filename);
	}

	gtk_widget_destroy(dialog);
}

static void save_all_cb(RoccatConfigWindow *roccat_window, gpointer user_data) {
	save_all(LUACONFIG_WINDOW(roccat_window), FALSE);
}

static void menu_device_surface_check_cb(GtkMenuItem *item, gpointer user_data) {
	RoccatConfigWindow *roccat_window = ROCCAT_CONFIG_WINDOW(user_data);

	if (!roccat_config_window_warn_if_no_device(roccat_window))
		return;

	luaconfig_surface_check(GTK_WINDOW(roccat_window), roccat_config_window_get_device(roccat_window));
}

GtkWidget *luaconfig_window_new(void) {
	return GTK_WIDGET(g_object_new(LUACONFIG_WINDOW_TYPE,
			"device-name", LUA_DEVICE_NAME,
			NULL));
}

static GObject *constructor(GType gtype, guint n_properties, GObjectConstructParam *properties) {
	GObject *obj;
	LuaconfigWindow *window;
	LuaconfigWindowPrivate *priv;
	RoccatConfigWindow *roccat_window;
	LuaRmp *rmp;
	GtkMenuItem *menu_item;
	GtkWidget *profile_page;

	obj = G_OBJECT_CLASS(luaconfig_window_parent_class)->constructor(gtype, n_properties, properties);
	window = LUACONFIG_WINDOW(obj);
	priv = window->priv;
	roccat_window = ROCCAT_CONFIG_WINDOW(obj);

	priv->config = lua_configuration_load();

	profile_page = luaconfig_profile_page_new();
	roccat_config_window_pages_append_page(ROCCAT_CONFIG_WINDOW_PAGES(roccat_window), ROCCAT_PROFILE_PAGE(profile_page));

	menu_item = GTK_MENU_ITEM(gtk_image_menu_item_new_with_label(_("Surface test")));
	g_signal_connect(G_OBJECT(menu_item), "activate", G_CALLBACK(menu_device_surface_check_cb), window);
	roccat_config_window_device_menu_append(roccat_window, menu_item);

	rmp = lua_rmp_load_actual();
	luaconfig_profile_page_set_rmp(LUACONFIG_PROFILE_PAGE(profile_page), rmp);
	lua_rmp_free(rmp);

	g_signal_connect(G_OBJECT(roccat_window), "delete-event", G_CALLBACK(delete_event_cb), NULL);
	g_signal_connect(G_OBJECT(roccat_window), "save-all", G_CALLBACK(save_all_cb), NULL);
	g_signal_connect(G_OBJECT(profile_page), "load-from-file", G_CALLBACK(load_profile_from_file_cb), window);
	g_signal_connect(G_OBJECT(profile_page), "save-to-file", G_CALLBACK(save_profile_to_file_cb), window);

	roccat_config_window_set_device_scanner(roccat_window, ROCCAT_DEVICE_SCANNER_INTERFACE(lua_device_scanner_new()));

	gtk_widget_show(GTK_WIDGET(window));

	/* if a device is already plugged in, add_cb has already been executed */
	roccat_config_window_warn_if_no_device(roccat_window);

	return obj;
}

static void luaconfig_window_init(LuaconfigWindow *window) {
	window->priv = LUACONFIG_WINDOW_GET_PRIVATE(window);
}

static void finalize(GObject *object) {
	LuaconfigWindowPrivate *priv = LUACONFIG_WINDOW(object)->priv;

	luaconfig_configuration_save(GTK_WINDOW(object), priv->config);
	g_clear_pointer(&priv->config, lua_configuration_free);

	G_OBJECT_CLASS(luaconfig_window_parent_class)->finalize(object);
}

static void luaconfig_window_class_init(LuaconfigWindowClass *klass) {
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->constructor = constructor;
	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(LuaconfigWindowClass));
}
