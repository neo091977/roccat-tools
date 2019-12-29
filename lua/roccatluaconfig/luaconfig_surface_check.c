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

#include "luaconfig_surface_check.h"
#include "roccat_warning_dialog.h"
#include "i18n.h"

#define LUACONFIG_SURFACE_CHECK_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), LUACONFIG_SURFACE_CHECK_TYPE, LuaconfigSurfaceCheckClass))
#define IS_LUACONFIG_SURFACE_CHECK_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), LUACONFIG_SURFACE_CHECK_TYPE))
#define LUACONFIG_SURFACE_CHECK_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), LUACONFIG_SURFACE_CHECK_TYPE, LuaconfigSurfaceCheckPrivate))

typedef struct _LuaconfigSurfaceCheckClass LuaconfigSurfaceCheckClass;
typedef struct _LuaconfigSurfaceCheckPrivate LuaconfigSurfaceCheckPrivate;

struct _LuaconfigSurfaceCheck {
	GtkDialog parent;
	LuaconfigSurfaceCheckPrivate *priv;
};

struct _LuaconfigSurfaceCheckClass {
	GtkDialogClass parent_class;
};

struct _LuaconfigSurfaceCheckPrivate {
	GtkProgressBar *squal;
	GtkProgressBar *average;
	RoccatDevice *device;
};

G_DEFINE_TYPE(LuaconfigSurfaceCheck, luaconfig_surface_check, GTK_TYPE_DIALOG);

void luaconfig_surface_check_set_device(LuaconfigSurfaceCheck *surface_check, RoccatDevice *device) {
	surface_check->priv->device = device;
}

GtkWidget *luaconfig_surface_check_new(GtkWindow *parent) {
	LuaconfigSurfaceCheck *surface_check;

	surface_check = LUACONFIG_SURFACE_CHECK(g_object_new(LUACONFIG_SURFACE_CHECK_TYPE,
			"has-separator", FALSE,
			NULL));

	gtk_dialog_add_buttons(GTK_DIALOG(surface_check),
			GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
			NULL);
	gtk_window_set_title(GTK_WINDOW(surface_check), _("Surface test"));
	gtk_window_set_transient_for(GTK_WINDOW(surface_check), parent);
	gtk_window_set_modal(GTK_WINDOW(surface_check), TRUE);
	gtk_window_set_destroy_with_parent(GTK_WINDOW(surface_check), TRUE);

	return GTK_WIDGET(surface_check);
}

static void luaconfig_surface_check_init(LuaconfigSurfaceCheck *surface_check) {
	LuaconfigSurfaceCheckPrivate *priv = LUACONFIG_SURFACE_CHECK_GET_PRIVATE(surface_check);
	surface_check->priv = priv;
	GtkWidget *content_area, *table;
	content_area = gtk_dialog_get_content_area(GTK_DIALOG(surface_check));

	table = gtk_table_new(2, 2, FALSE);
	gtk_box_pack_start(GTK_BOX(content_area), GTK_WIDGET(table), FALSE, FALSE, 0);

	priv->squal = GTK_PROGRESS_BAR(gtk_progress_bar_new());
	gtk_table_attach(GTK_TABLE(table), gtk_label_new(_("SQUAL")), 0, 1, 0, 1, 0, GTK_EXPAND, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(priv->squal), 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);

	priv->average = GTK_PROGRESS_BAR(gtk_progress_bar_new());
	gtk_table_attach(GTK_TABLE(table), gtk_label_new(_("Average pixel value")), 0, 1, 1, 2, 0, GTK_EXPAND, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(priv->average), 1, 2, 1, 2, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);

	gtk_widget_show_all(GTK_WIDGET(content_area));
}

static void luaconfig_surface_check_class_init(LuaconfigSurfaceCheckClass *klass) {
	g_type_class_add_private(klass, sizeof(LuaconfigSurfaceCheckPrivate));
}

void luaconfig_surface_check_set_squal(LuaconfigSurfaceCheck *surface_check, guint value) {
	gfloat squal;
	squal = (gfloat)(value << 1);
	gtk_progress_bar_set_fraction(surface_check->priv->squal, squal / (gfloat)0x1ff);
}

void luaconfig_surface_check_set_average(LuaconfigSurfaceCheck *surface_check, guint value) {
	gfloat average;
	average = (gfloat)value * 1.058;
	gtk_progress_bar_set_fraction(surface_check->priv->average, average / (gfloat)254);
}

static gboolean timeout_cb(gpointer user_data) {
	LuaconfigSurfaceCheck *surface_check;
	GError *error = NULL;
	LuaReport *report;

	if (g_source_is_destroyed(g_main_current_source()))
		return FALSE;

	surface_check = (LuaconfigSurfaceCheck *)user_data;

	report = lua_report_read(surface_check->priv->device, &error);
	if (error) {
		g_clear_error(&error);
		gtk_dialog_response(GTK_DIALOG(surface_check), GTK_RESPONSE_ACCEPT);
		return FALSE;
	}

	luaconfig_surface_check_set_squal(surface_check, report->maybe_squal);
	luaconfig_surface_check_set_average(surface_check, report->maybe_pix_accum);
	g_free(report);

	lua_write_request(surface_check->priv->device, &error);
	if (error) {
		g_clear_error(&error);
		gtk_dialog_response(GTK_DIALOG(surface_check), GTK_RESPONSE_ACCEPT);
		return FALSE;
	}

	return TRUE;
}

void luaconfig_surface_check(GtkWindow *parent, RoccatDevice *lua) {
	LuaconfigSurfaceCheck *surface_check;
	GSource *source;
	guint timeout;

	surface_check = LUACONFIG_SURFACE_CHECK(luaconfig_surface_check_new(parent));

	luaconfig_surface_check_set_device(surface_check, lua);
	timeout = g_timeout_add(500, timeout_cb, surface_check);

	(void)gtk_dialog_run(GTK_DIALOG(surface_check));

	source = g_main_context_find_source_by_id(g_main_context_default(), timeout);
	if (source)
		g_source_destroy(source);

	gtk_widget_destroy(GTK_WIDGET(surface_check));
}
