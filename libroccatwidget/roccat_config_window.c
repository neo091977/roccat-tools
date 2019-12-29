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

#include "roccat_config_window.h"
#include "roccat_profile_page_tab_label.h"
#include "roccat_about_dialog.h"
#include "roccat_timer_editor_dialog.h"
#include <gaminggear/gaminggear_macro_editor_dialog.h>
#include "roccat_warning_dialog.h"
#include "gtk_roccat_helper.h"
#include "g_roccat_helper.h"
#include "g_cclosure_roccat_marshaller.h"
#include "i18n-lib.h"

#define ROCCAT_CONFIG_WINDOW_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ROCCAT_CONFIG_WINDOW_TYPE, RoccatConfigWindowPrivate))

struct _RoccatConfigWindowPrivate {
	GtkMenuBar *menu_bar;
	GtkMenu *edit_menu;
	GtkMenu *device_menu;
	GtkBox *content;
	gchar *device_name;
	RoccatDeviceScannerInterface *device_scanner_interface;
	RoccatDevice *device;
	gboolean scanner_active;
	gulong device_remove_handler_id;
};

G_DEFINE_TYPE(RoccatConfigWindow, roccat_config_window, GTK_TYPE_WINDOW);

enum {
	PROP_0,
	PROP_DEVICE_NAME,
};

enum {
	SAVE_ALL,
	DEVICE_ADDED,
	DEVICE_REMOVED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

static void roccat_config_window_set_title(RoccatConfigWindow *config_window, gboolean connected, gchar const *device_name) {
	gchar *title;

	title = g_strdup_printf(_("%s configuration - %s connected"),
			config_window->priv->device_name,
			connected ? device_name : _("No device"));
	gtk_window_set_title(GTK_WINDOW(config_window), title);
	g_free(title);
}

static void roccat_config_window_set_title_disconnected(RoccatConfigWindow *config_window) {
	roccat_config_window_set_title(config_window, FALSE, "");
}

static void roccat_config_window_set_title_connected(RoccatConfigWindow *config_window, gchar const *device_name) {
	roccat_config_window_set_title(config_window, TRUE, device_name);
}

GtkWidget *roccat_config_window_new(gchar const *device_name) {
	return GTK_WIDGET(g_object_new(ROCCAT_CONFIG_WINDOW_TYPE,
			"device-name", device_name,
			NULL));
}

static void menu_file_save_all_cb(GtkMenuItem *item, gpointer user_data) {
	RoccatConfigWindow *config_window = ROCCAT_CONFIG_WINDOW(user_data);
	g_signal_emit((gpointer)config_window, signals[SAVE_ALL], 0);
}

static void menu_file_quit_cb(GtkMenuItem *item, gpointer user_data) {
	GdkEvent *event;
	gboolean retval;

	/* close window like the close button has been pressed
	 * which leads to user being asked to save work
	 */
	event = gdk_event_new(GDK_DELETE);
	g_signal_emit_by_name((gpointer)user_data, "delete-event", event, &retval);
	gdk_event_free(event);
	if (!retval)
		gtk_widget_destroy(GTK_WIDGET(user_data));
}

static void menu_edit_timers_cb(GtkMenuItem *item, gpointer user_data) {
	RoccatConfigWindow *config_window = ROCCAT_CONFIG_WINDOW(user_data);
	roccat_timer_editor_dialog(GTK_WINDOW(config_window));
}

static void menu_edit_macros_cb(GtkMenuItem *item, gpointer user_data) {
	RoccatConfigWindow *config_window = ROCCAT_CONFIG_WINDOW(user_data);
	gaminggear_macro_editor_dialog(GTK_WINDOW(config_window));
}

static void menu_help_about_cb(GtkMenuItem *item, gpointer user_data) {
	RoccatConfigWindow *config_window = ROCCAT_CONFIG_WINDOW(user_data);
	gchar *title;

	title = g_strdup_printf(_("Application to manipulate profiles and settings of Roccat %s devices"),
			config_window->priv->device_name);
	roccat_about_dialog(GTK_WINDOW(config_window), NULL, title);
	g_free(title);
}

static GtkWidget *menu_bar_new(RoccatConfigWindow *config_window) {
	RoccatConfigWindowPrivate *priv = config_window->priv;
	GtkWidget *menu_bar;
	GtkWidget *menu_item;
	GtkWidget *menu;

	menu_bar = gtk_menu_bar_new();

	menu_item = gtk_menu_item_new_with_label(_("File"));
	menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item), menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), menu_item);

	menu_item = gtk_image_menu_item_new_with_label(_("Store all in device"));
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
	g_signal_connect(G_OBJECT(menu_item), "activate", G_CALLBACK(menu_file_save_all_cb), config_window);

	/* separator */
	menu_item = gtk_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);

	menu_item = gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT, NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
	g_signal_connect(G_OBJECT(menu_item), "activate", G_CALLBACK(menu_file_quit_cb), config_window);


	menu_item = gtk_menu_item_new_with_label(_("Edit"));
	priv->edit_menu = GTK_MENU(gtk_menu_new());
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item), GTK_WIDGET(priv->edit_menu));
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), menu_item);

	menu_item = gtk_image_menu_item_new_with_label(_("Macros"));
	gtk_menu_shell_append(GTK_MENU_SHELL(priv->edit_menu), menu_item);
	g_signal_connect(G_OBJECT(menu_item), "activate", G_CALLBACK(menu_edit_macros_cb), config_window);

	menu_item = gtk_image_menu_item_new_with_label(_("Timers"));
	gtk_menu_shell_append(GTK_MENU_SHELL(priv->edit_menu), menu_item);
	g_signal_connect(G_OBJECT(menu_item), "activate", G_CALLBACK(menu_edit_timers_cb), config_window);


	menu_item = gtk_menu_item_new_with_label(_("Device"));
	priv->device_menu = GTK_MENU(gtk_menu_new());
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item), GTK_WIDGET(priv->device_menu));
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), menu_item);


	menu_item = gtk_menu_item_new_with_label(_("Help"));
	menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item), menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), menu_item);

	menu_item = gtk_image_menu_item_new_from_stock(GTK_STOCK_ABOUT, NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
	g_signal_connect(G_OBJECT(menu_item), "activate", G_CALLBACK(menu_help_about_cb), config_window);

	return menu_bar;
}

static void destroy_event_cb(GtkWidget *window, gpointer user_data) {
	gtk_main_quit();
}

static void button_apply_cb(GtkButton *button, gpointer user_data) {
	g_signal_emit((gpointer)user_data, signals[SAVE_ALL], 0);
}

static void button_ok_cb(GtkButton *button, gpointer user_data) {
	/* Data should be saved without asking user and window should be closed
	 * without triggering question again. */
	g_signal_emit((gpointer)user_data, signals[SAVE_ALL], 0);
	gtk_widget_destroy(GTK_WIDGET(user_data));
}

static void button_cancel_cb(GtkButton *button, gpointer user_data) {
	gtk_widget_destroy(GTK_WIDGET(user_data));
}

static GtkWidget *buttons_new(RoccatConfigWindow *config_window) {
	GtkWidget *hbox1, *hbox2, *button;

	hbox1 = gtk_hbox_new(FALSE, 0);
	hbox2 = gtk_hbox_new(TRUE, 0);

	gtk_box_pack_end(GTK_BOX(hbox1), hbox2, FALSE, FALSE, 0);

	button = gtk_button_new_from_stock(GTK_STOCK_CANCEL);
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(button_cancel_cb), config_window);
	gtk_box_pack_start(GTK_BOX(hbox2), button, TRUE, TRUE, 0);

	button = gtk_button_new_from_stock(GTK_STOCK_APPLY);
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(button_apply_cb), config_window);
	gtk_box_pack_start(GTK_BOX(hbox2), button, TRUE, TRUE, 0);

	button = gtk_button_new_from_stock(GTK_STOCK_OK);
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(button_ok_cb), config_window);
	gtk_box_pack_start(GTK_BOX(hbox2), button, TRUE, TRUE, 0);

	return hbox1;
}

void roccat_config_window_add(RoccatConfigWindow *config_window, GtkWidget *content) {
	gtk_box_pack_start(config_window->priv->content, content, TRUE, TRUE, 0);
	gtk_widget_show(content);
}

static void roccat_config_window_init(RoccatConfigWindow *config_window) {
	RoccatConfigWindowPrivate *priv = ROCCAT_CONFIG_WINDOW_GET_PRIVATE(config_window);
	GtkWidget *vbox;

	config_window->priv = priv;
	priv->device_name = NULL;

	gtk_window_resize(GTK_WINDOW(config_window), 640, 480);

	gtk_window_set_default_icon_name("roccat");
	g_signal_connect(G_OBJECT(config_window), "destroy", G_CALLBACK(destroy_event_cb), NULL);

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(config_window), GTK_WIDGET(vbox));

	priv->menu_bar = GTK_MENU_BAR(menu_bar_new(config_window));
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(priv->menu_bar), FALSE, FALSE, 0);

	priv->content = GTK_BOX(gtk_vbox_new(FALSE, 0));
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(priv->content), TRUE, TRUE, 0);

	gtk_box_pack_start(GTK_BOX(vbox), buttons_new(config_window), FALSE, FALSE, 0);

	gtk_widget_show_all(vbox);
}

static GObject *constructor(GType gtype, guint n_properties, GObjectConstructParam *properties) {
	GObject *obj;
	RoccatConfigWindow *config_window;

	obj = G_OBJECT_CLASS(roccat_config_window_parent_class)->constructor(gtype, n_properties, properties);
	config_window = ROCCAT_CONFIG_WINDOW(obj);

	roccat_config_window_set_title_disconnected(config_window);

	return obj;
}

static void set_property(GObject *object, guint prop_id, GValue const *value, GParamSpec *pspec) {
	RoccatConfigWindowPrivate *priv = ROCCAT_CONFIG_WINDOW(object)->priv;
	switch(prop_id) {
	case PROP_DEVICE_NAME:
		g_free(priv->device_name);
		priv->device_name = g_value_dup_string(value);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
}

static void get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec) {
	RoccatConfigWindowPrivate *priv = ROCCAT_CONFIG_WINDOW(object)->priv;
	switch(prop_id) {
	case PROP_DEVICE_NAME:
		g_value_set_string(value, priv->device_name);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
}

static void finalize(GObject *object) {
	RoccatConfigWindowPrivate *priv = ROCCAT_CONFIG_WINDOW(object)->priv;

	g_clear_object(&priv->device_scanner_interface);

	G_OBJECT_CLASS(roccat_config_window_parent_class)->finalize(object);
}

static void roccat_config_window_class_init(RoccatConfigWindowClass *klass) {
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->constructor = constructor;
	gobject_class->set_property = set_property;
	gobject_class->get_property = get_property;
	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(RoccatConfigWindowPrivate));

	g_object_class_install_property(gobject_class, PROP_DEVICE_NAME,
			g_param_spec_string("device-name",
					"Device name",
					"Name of device",
					"Undefined",
					G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

	signals[SAVE_ALL] = g_signal_new("save-all",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION, 0,
			NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

	signals[DEVICE_ADDED] = g_signal_new("device-added",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION, 0,
			NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

	signals[DEVICE_REMOVED] = g_signal_new("device-removed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION, 0,
			NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}

void roccat_config_window_menu_insert(RoccatConfigWindow *config_window, GtkMenuItem *menu_item) {
	RoccatConfigWindowPrivate *priv = config_window->priv;
	gint position;

	/* second to last */
	position = gtk_roccat_container_get_n_children(GTK_CONTAINER(priv->menu_bar)) - 1;
	if (position < 0)
		position = 0;

	gtk_menu_shell_insert(GTK_MENU_SHELL(priv->menu_bar), GTK_WIDGET(menu_item), position);
}

void roccat_config_window_edit_menu_append(RoccatConfigWindow *config_window, GtkMenuItem *menu_item) {
	RoccatConfigWindowPrivate *priv = config_window->priv;
	gtk_widget_show(GTK_WIDGET(menu_item));
	gtk_menu_shell_append(GTK_MENU_SHELL(priv->edit_menu), GTK_WIDGET(menu_item));
}

void roccat_config_window_device_menu_append(RoccatConfigWindow *config_window, GtkMenuItem *menu_item) {
	RoccatConfigWindowPrivate *priv = config_window->priv;
	gtk_widget_show(GTK_WIDGET(menu_item));
	gtk_menu_shell_append(GTK_MENU_SHELL(priv->device_menu), GTK_WIDGET(menu_item));
}

static void remove_device_remove_handler(RoccatConfigWindow *config_window) {
	RoccatConfigWindowPrivate *priv = config_window->priv;

	if (!priv->device_remove_handler_id)
		return;

	g_signal_handler_disconnect(G_OBJECT(priv->device_scanner_interface), priv->device_remove_handler_id);
	priv->device_remove_handler_id = 0;
}

static void device_remove_cb(RoccatDeviceScannerInterface *interface, RoccatDevice *device, gpointer user_data) {
	RoccatConfigWindow *config_window = ROCCAT_CONFIG_WINDOW(user_data);
	RoccatConfigWindowPrivate *priv = config_window->priv;

	if (!priv->device || !gaminggear_device_equal(GAMINGGEAR_DEVICE(device), GAMINGGEAR_DEVICE(priv->device)))
		return;

	remove_device_remove_handler(config_window);

	g_clear_object(&priv->device);

	roccat_config_window_set_title_disconnected(config_window);

	g_signal_emit((gpointer)config_window, signals[DEVICE_REMOVED], 0);
}

static void set_device_remove_handler(RoccatConfigWindow *config_window) {
	RoccatConfigWindowPrivate *priv = config_window->priv;

	if (priv->device_remove_handler_id)
		return;

	priv->device_remove_handler_id = g_signal_connect(G_OBJECT(priv->device_scanner_interface),
			"device-removed", G_CALLBACK(device_remove_cb), config_window);
}

static void device_add_cb(RoccatDeviceScannerInterface *interface, RoccatDevice *device, gpointer user_data) {
	RoccatConfigWindow *config_window = ROCCAT_CONFIG_WINDOW(user_data);
	RoccatConfigWindowPrivate *priv = config_window->priv;

	if (!priv->scanner_active)
		return;
	
	roccat_device_debug(device);

	if (priv->device) {
		roccat_multiple_device_warning_dialog(GTK_WINDOW(config_window), roccat_device_get_name_static(device));
		return;
	}

	priv->device = device;
	g_object_ref(G_OBJECT(priv->device));

	roccat_config_window_set_title_connected(config_window, roccat_device_get_name_static(priv->device));
	set_device_remove_handler(config_window);

	g_signal_emit((gpointer)config_window, signals[DEVICE_ADDED], 0);
}

void roccat_config_window_set_device_scanner(RoccatConfigWindow *config_window, RoccatDeviceScannerInterface *device_scanner_interface) {
	RoccatConfigWindowPrivate *priv = config_window->priv;

	priv->scanner_active = TRUE;
	
	if (priv->device_scanner_interface)
		return;

	priv->device_scanner_interface = device_scanner_interface;
	g_object_ref(G_OBJECT(priv->device_scanner_interface));

	g_signal_connect(G_OBJECT(priv->device_scanner_interface), "device-added", G_CALLBACK(device_add_cb), config_window);
	roccat_device_scanner_interface_start(priv->device_scanner_interface);
}

RoccatDevice *roccat_config_window_get_device(RoccatConfigWindow *config_window) {
	return config_window->priv->device;
}

gboolean roccat_config_window_warn_if_no_device(RoccatConfigWindow *config_window) {
	RoccatConfigWindowPrivate *priv = config_window->priv;

	return roccat_no_device_warning_dialog(GTK_WINDOW(config_window),
			priv->device, priv->device_name);
}

void roccat_config_window_device_scanner_suspend(RoccatConfigWindow *config_window) {
	config_window->priv->scanner_active = FALSE;
}

void roccat_config_window_device_scanner_continue(RoccatConfigWindow *config_window) {
	config_window->priv->scanner_active = TRUE;
}
