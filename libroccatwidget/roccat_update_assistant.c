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

#include "roccat_update_assistant.h"
#include "roccat_warning_dialog.h"
#include "roccat_firmware.h"
#include "roccat_strings.h"
#include "g_roccat_helper.h"
#include "i18n-lib.h"

#define ROCCAT_UPDATE_ASSISTANT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ROCCAT_UPDATE_ASSISTANT_TYPE, RoccatUpdateAssistantPrivate))

typedef struct _RoccatUpdateAssistantFirmware RoccatUpdateAssistantFirmware;

struct _RoccatUpdateAssistantFirmware {
	RoccatUpdateAssistant *assistant;

	RoccatFirmwareState *state;

	GtkFileChooserButton *file_chooser;
	gboolean file_ok;
	GtkLabel *filename_label;
	GtkEntry *md5_entry;

	GtkProgressBar *progress_bar;
	gdouble fraction_size;
	gdouble fraction;
};

struct _RoccatUpdateAssistantPrivate {
	gint info_page_number;

	gint file_page_number;
	GtkLabel *file_page_label;
	GtkTable *file_page_table;

	gint check_page_number;
	GtkLabel *check_page_label;
	GtkTable *check_page_table;

	gint action_page_number;
	GtkTable *action_page_table;

	gint success_page_number;

	gint failure_page_number;
	GtkLabel *failure_label;

	RoccatDevice *device;
	guint endpoint;

	guint wait_prepare, wait_init_data, wait_data;
	gboolean final_write_check;

	GSList *firmwares;
	GSList *actual_firmware;
};

G_DEFINE_TYPE(RoccatUpdateAssistant, roccat_update_assistant, GTK_TYPE_ASSISTANT);

enum {
	PRE_ACTION,
	POST_ACTION,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

enum {
	PROP_0,
	PROP_DEVICE,
	PROP_ENDPOINT,
	PROP_WAIT_PREPARE,
	PROP_WAIT_INIT_DATA,
	PROP_WAIT_DATA,
	PROP_FINAL_WRITE_CHECK,
};

static void roccat_update_assistant_set_ok(RoccatUpdateAssistant *assistant) {
	GSList *iterator;
	gboolean all_ok = TRUE;

	for (iterator = assistant->priv->firmwares; iterator; iterator = g_slist_next(iterator)) {
		if (!((RoccatUpdateAssistantFirmware const *)iterator->data)->file_ok)
			all_ok = FALSE;
	}
	gtk_assistant_set_page_complete(GTK_ASSISTANT(assistant),
			gtk_assistant_get_nth_page(GTK_ASSISTANT(assistant), assistant->priv->file_page_number), all_ok);
}

static void roccat_update_assistant_firmware_set_table_row(RoccatUpdateAssistantFirmware *firmware, gchar const *path) {
	gchar *filename = g_path_get_basename(path);
	gchar *checksum = roccat_firmware_state_get_md5(firmware->state);

	gtk_label_set_text(firmware->filename_label, filename);
	gtk_entry_set_text(firmware->md5_entry, checksum);

	g_free(filename);
	g_free(checksum);
}

static void file_set_cb(GtkFileChooserButton *button, gpointer user_data) {
	RoccatUpdateAssistantFirmware *firmware = (RoccatUpdateAssistantFirmware *)user_data;
	gchar *path;
	GError *error = NULL;

	path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(firmware->file_chooser));

	if (!roccat_firmware_state_read_firmware(firmware->state, path, &error)) {
		roccat_handle_error_dialog(NULL, _("Could not read firmware image"), &error);
		firmware->file_ok = FALSE;
	} else {
		firmware->fraction_size = 1.0 / (gdouble)roccat_firmware_fractions(firmware->state);
		roccat_update_assistant_firmware_set_table_row(firmware, path);
		firmware->file_ok = TRUE;
		roccat_update_assistant_set_ok(firmware->assistant);
	}

	g_free(path);
}

static void add_file_filter(GtkFileChooser *chooser) {
	GtkFileFilter *filter;
	filter = gtk_file_filter_new();
	gtk_file_filter_set_name(filter, _("Roccat firmware images"));
	gtk_file_filter_add_pattern(filter, "*.bin");
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(chooser), filter);

	filter = gtk_file_filter_new();
	gtk_file_filter_set_name(filter, _("All files"));
	gtk_file_filter_add_pattern(filter, "*");
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(chooser), filter);
}

void roccat_update_assistant_add_firmware(RoccatUpdateAssistant *assistant, gchar const *name, gchar const *actual_version, guint size, guint8 number) {
	RoccatUpdateAssistantPrivate *priv = assistant->priv;
	GtkWidget *label;
	RoccatUpdateAssistantFirmware *firmware;
	guint line;

	firmware = (RoccatUpdateAssistantFirmware *)g_malloc(sizeof(RoccatUpdateAssistantFirmware));

	priv->firmwares = g_slist_append(priv->firmwares, firmware);
	line = g_slist_index(priv->firmwares, firmware) + 1; /* adding table header */

	firmware->assistant = assistant;

	firmware->state = roccat_firmware_state_new(priv->device, priv->endpoint, size, number);
	roccat_firmware_state_set_wait_prepare(firmware->state, priv->wait_prepare);
	roccat_firmware_state_set_wait_init_data(firmware->state, priv->wait_init_data);
	roccat_firmware_state_set_wait_data(firmware->state, priv->wait_data);
	roccat_firmware_state_set_final_write_check(firmware->state, priv->final_write_check);

	firmware->file_ok = FALSE;

	label = gtk_label_new(name);
	gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.5);
	gtk_table_attach(priv->file_page_table, label, 0, 1, line, line + 1, GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

	label = gtk_label_new(actual_version);
	gtk_misc_set_alignment(GTK_MISC(label), 0.5, 0.5);
	gtk_table_attach(priv->file_page_table, label, 1, 2, line, line + 1, GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

	firmware->file_chooser = GTK_FILE_CHOOSER_BUTTON(gtk_file_chooser_button_new(_("Select firmware image"), GTK_FILE_CHOOSER_ACTION_OPEN));
	add_file_filter(GTK_FILE_CHOOSER(firmware->file_chooser));
	g_signal_connect(G_OBJECT(firmware->file_chooser), "file-set", G_CALLBACK(file_set_cb), firmware);
	gtk_table_attach(priv->file_page_table, GTK_WIDGET(firmware->file_chooser), 2, 3, line, line + 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

	label = gtk_label_new(name);
	gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.5);
	gtk_table_attach(priv->check_page_table, label, 0, 1, line, line + 1, GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

	label = gtk_label_new(actual_version);
	gtk_misc_set_alignment(GTK_MISC(label), 0.5, 0.5);
	gtk_table_attach(priv->check_page_table, label, 1, 2, line, line + 1, GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

	firmware->filename_label = GTK_LABEL(gtk_label_new(NULL));
	gtk_misc_set_alignment(GTK_MISC(firmware->filename_label), 0.5, 0.5);
	gtk_table_attach(priv->check_page_table, GTK_WIDGET(firmware->filename_label), 2, 3, line, line + 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

	firmware->md5_entry = GTK_ENTRY(gtk_entry_new());
	gtk_editable_set_editable(GTK_EDITABLE(firmware->md5_entry), FALSE);
	gtk_table_attach(priv->check_page_table, GTK_WIDGET(firmware->md5_entry), 3, 4, line, line + 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

	label = gtk_label_new(name);
	gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.5);
	gtk_table_attach(priv->action_page_table, label, 0, 1, line, line + 1, GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

	firmware->progress_bar = GTK_PROGRESS_BAR(gtk_progress_bar_new());
	gtk_table_attach(priv->action_page_table, GTK_WIDGET(firmware->progress_bar), 1, 2, line, line + 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
}

static void append_info_page(RoccatUpdateAssistant *assistant) {
	RoccatUpdateAssistantPrivate *priv = assistant->priv;
	GtkWidget *page;
	GtkWidget *label;

	page = gtk_vbox_new(FALSE, 0);

	label = gtk_label_new(roccat_string_firmware_liability());
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);
	gtk_box_pack_start(GTK_BOX(page), label, TRUE, TRUE, 10);

	priv->info_page_number = gtk_assistant_append_page(GTK_ASSISTANT(assistant), page);
	gtk_assistant_set_page_title(GTK_ASSISTANT(assistant), page, _("Liability statement"));
	gtk_assistant_set_page_type(GTK_ASSISTANT(assistant), page, GTK_ASSISTANT_PAGE_INTRO);
	gtk_assistant_set_page_complete(GTK_ASSISTANT(assistant), page, TRUE);
}

static void append_file_page(RoccatUpdateAssistant *assistant) {
	RoccatUpdateAssistantPrivate *priv = assistant->priv;
	GtkWidget *page;
	GtkWidget *label;

	page = gtk_vbox_new(FALSE, 0);

	priv->file_page_label = GTK_LABEL(gtk_label_new(NULL));
	gtk_label_set_justify(priv->file_page_label, GTK_JUSTIFY_CENTER);
	gtk_box_pack_start(GTK_BOX(page), GTK_WIDGET(priv->file_page_label), TRUE, TRUE, 0);

	label = gtk_label_new(_("Please select firmware images that fit your device."));
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);
	gtk_box_pack_start(GTK_BOX(page), label, TRUE, TRUE, 10);

	priv->file_page_table = GTK_TABLE(gtk_table_new(1, 3, FALSE));
	gtk_box_pack_start(GTK_BOX(page), GTK_WIDGET(priv->file_page_table), TRUE, FALSE, 0);

	label = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(label), _("<b>Name</b>"));
	gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.5);
	gtk_table_attach(priv->file_page_table, label, 0, 1, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

	label = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(label), _("<b>Actual\nversion</b>"));
	gtk_misc_set_alignment(GTK_MISC(label), 0.5, 0.5);
	gtk_table_attach(priv->file_page_table, label, 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

	label = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(label), _("<b>File</b>"));
	gtk_misc_set_alignment(GTK_MISC(label), 0.5, 0.5);
	gtk_table_attach(priv->file_page_table, label, 2, 3, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

	priv->file_page_number = gtk_assistant_append_page(GTK_ASSISTANT(assistant), page);
	gtk_assistant_set_page_title(GTK_ASSISTANT(assistant), page, _("Select firmware"));
	gtk_assistant_set_page_type(GTK_ASSISTANT(assistant), page, GTK_ASSISTANT_PAGE_CONTENT);
	gtk_assistant_set_page_complete(GTK_ASSISTANT(assistant), page, FALSE);
}

static void append_check_page(RoccatUpdateAssistant *assistant) {
	RoccatUpdateAssistantPrivate *priv = assistant->priv;
	GtkWidget *page;
	GtkWidget *label;

	page = gtk_vbox_new(FALSE, 0);

	priv->check_page_label = GTK_LABEL(gtk_label_new(NULL));
	gtk_label_set_justify(priv->check_page_label, GTK_JUSTIFY_CENTER);
	gtk_box_pack_start(GTK_BOX(page), GTK_WIDGET(priv->check_page_label), TRUE, TRUE, 0);

	priv->check_page_table = GTK_TABLE(gtk_table_new(1, 4, FALSE));
	gtk_box_pack_start(GTK_BOX(page), GTK_WIDGET(priv->check_page_table), FALSE, FALSE, 0);

	label = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(label), _("<b>Name</b>"));
	gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.5);
	gtk_table_attach(priv->check_page_table, label, 0, 1, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

	label = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(label), _("<b>Actual\nversion</b>"));
	gtk_misc_set_alignment(GTK_MISC(label), 0.5, 0.5);
	gtk_table_attach(priv->check_page_table, label, 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

	label = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(label), _("<b>Filename</b>"));
	gtk_misc_set_alignment(GTK_MISC(label), 0.5, 0.5);
	gtk_table_attach(priv->check_page_table, label, 2, 3, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

	label = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(label), _("<b>MD5-Sum</b>"));
	gtk_misc_set_alignment(GTK_MISC(label), 0.5, 0.5);
	gtk_table_attach(priv->check_page_table, label, 3, 4, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

	label = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(label), _("<b>Please do not unplug the device\n"
			"or power down your computer while the update\nprocess is running.</b>"));
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);
	gtk_box_pack_start(GTK_BOX(page), label, TRUE, TRUE, 10);

	label = gtk_label_new(_("Please continue to update the firmware now."));
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);
	gtk_box_pack_start(GTK_BOX(page), label, TRUE, TRUE, 10);

	priv->check_page_number = gtk_assistant_append_page(GTK_ASSISTANT(assistant), page);
	gtk_assistant_set_page_title(GTK_ASSISTANT(assistant), page, _("Initiate update"));
	gtk_assistant_set_page_type(GTK_ASSISTANT(assistant), page, GTK_ASSISTANT_PAGE_CONFIRM);
	gtk_assistant_set_page_complete(GTK_ASSISTANT(assistant), page, TRUE);
}

static void append_action_page(RoccatUpdateAssistant *assistant) {
	RoccatUpdateAssistantPrivate *priv = assistant->priv;
	GtkWidget *label;
	GtkWidget *page;

	page = gtk_vbox_new(FALSE, 0);

	label = gtk_label_new(_("Please wait while the update process is running."));
	gtk_box_pack_start(GTK_BOX(page), label, TRUE, FALSE, 0);

	priv->action_page_table = GTK_TABLE(gtk_table_new(1, 2, FALSE));
	gtk_box_pack_start(GTK_BOX(page), GTK_WIDGET(priv->action_page_table), TRUE, FALSE, 0);

	label = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(label), _("<b>Name</b>"));
	gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.5);
	gtk_table_attach(priv->action_page_table, label, 0, 1, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

	label = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(label), _("<b>Progress</b>"));
	gtk_misc_set_alignment(GTK_MISC(label), 0.5, 0.5);
	gtk_table_attach(priv->action_page_table, label, 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

	priv->action_page_number = gtk_assistant_append_page(GTK_ASSISTANT(assistant), page);
	gtk_assistant_set_page_title(GTK_ASSISTANT(assistant), page, _("Updating..."));
	gtk_assistant_set_page_type(GTK_ASSISTANT(assistant), page, GTK_ASSISTANT_PAGE_PROGRESS);
	gtk_assistant_set_page_complete(GTK_ASSISTANT(assistant), page, FALSE);
}

static void roccat_update_assistant_set_success(RoccatUpdateAssistant *assistant) {
	RoccatUpdateAssistantPrivate *priv = assistant->priv;
	gtk_assistant_set_current_page(GTK_ASSISTANT(assistant), priv->success_page_number);
}

static void append_success_page(RoccatUpdateAssistant *assistant) {
	RoccatUpdateAssistantPrivate *priv = assistant->priv;
	GtkWidget *page;
	GtkWidget *label;

	page = gtk_vbox_new(FALSE, 0);

	label = gtk_label_new(_("Firmware update successful!"));
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);
	gtk_box_pack_start(GTK_BOX(page), label, TRUE, FALSE, 0);

	label = gtk_label_new(_("Please unplug and replug your device now."));
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);
	gtk_box_pack_start(GTK_BOX(page), label, TRUE, FALSE, 0);

	priv->success_page_number = gtk_assistant_append_page(GTK_ASSISTANT(assistant), page);
	gtk_assistant_set_page_title(GTK_ASSISTANT(assistant), page, _("Success"));
	gtk_assistant_set_page_type(GTK_ASSISTANT(assistant), page, GTK_ASSISTANT_PAGE_SUMMARY);
	gtk_assistant_set_page_complete(GTK_ASSISTANT(assistant), page, TRUE);
}

static void roccat_update_assistant_set_failed(RoccatUpdateAssistant *assistant, gchar const *text) {
	RoccatUpdateAssistantPrivate *priv = assistant->priv;
	gtk_label_set_text(priv->failure_label, text);
	gtk_assistant_set_current_page(GTK_ASSISTANT(assistant), priv->failure_page_number);
}

/* This page is inserted after success page and not reachable in normal flow */
static void append_failure_page(RoccatUpdateAssistant *assistant) {
	RoccatUpdateAssistantPrivate *priv = assistant->priv;
	GtkWidget *page;
	GtkWidget *label;

	page = gtk_vbox_new(FALSE, 0);

	label = gtk_label_new(_("Firmware update failed!"));
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);
	gtk_box_pack_start(GTK_BOX(page), label, TRUE, FALSE, 0);

	priv->failure_label = GTK_LABEL(gtk_label_new(NULL));
	gtk_label_set_justify(priv->failure_label, GTK_JUSTIFY_CENTER);
	gtk_box_pack_start(GTK_BOX(page), GTK_WIDGET(priv->failure_label), TRUE, FALSE, 0);

	label = gtk_label_new(_("Please try again."));
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);
	gtk_box_pack_start(GTK_BOX(page), label, TRUE, FALSE, 0);

	priv->failure_page_number = gtk_assistant_append_page(GTK_ASSISTANT(assistant), page);
	gtk_assistant_set_page_title(GTK_ASSISTANT(assistant), page, _("Failure"));
	gtk_assistant_set_page_type(GTK_ASSISTANT(assistant), page, GTK_ASSISTANT_PAGE_SUMMARY);
	gtk_assistant_set_page_complete(GTK_ASSISTANT(assistant), page, TRUE);
}

static gboolean timeout_cb(gpointer user_data) {
	RoccatUpdateAssistant *assistant = ROCCAT_UPDATE_ASSISTANT(user_data);
	RoccatUpdateAssistantPrivate *priv = assistant->priv;
	RoccatUpdateAssistantFirmware *firmware = (RoccatUpdateAssistantFirmware *)priv->actual_firmware->data;
	GError *local_error = NULL;

	if (!roccat_firmware_state_tick(firmware->state, &local_error)) {
		if (local_error) {
			roccat_update_assistant_set_failed(assistant, _("Could not write firmware data"));
			g_warning(_("Could not update firmware: %s"), local_error->message);
			return FALSE;
		}

		priv->actual_firmware = g_slist_next(priv->actual_firmware);
		if (priv->actual_firmware == NULL) {
			roccat_update_assistant_set_success(assistant);
			return FALSE;
		}

	}

	firmware->fraction += firmware->fraction_size;
	gtk_progress_bar_set_fraction(firmware->progress_bar, MIN(1.0, firmware->fraction));

	return TRUE;
}

static void roccat_update_assistant_firmware_reset(gpointer data, gpointer user_data) {
	RoccatUpdateAssistantFirmware *firmware = (RoccatUpdateAssistantFirmware *)data;
	roccat_firmware_state_reset(firmware->state);
	firmware->fraction = 0.0;
}

static void prepare_cb(GtkAssistant *assistant, GtkWidget *page, gpointer user_data) {
	RoccatUpdateAssistant *roccat_update_assistant = ROCCAT_UPDATE_ASSISTANT(assistant);
	RoccatUpdateAssistantPrivate *priv = roccat_update_assistant->priv;

	gtk_widget_show_all(page);

	if (page == gtk_assistant_get_nth_page(GTK_ASSISTANT(assistant), priv->action_page_number)) {
		g_slist_foreach(priv->firmwares, roccat_update_assistant_firmware_reset, NULL);
		priv->actual_firmware = priv->firmwares;

#if (GTK_CHECK_VERSION(2,22,0))
		gtk_assistant_commit(assistant);
#endif

		g_signal_emit((gpointer)assistant, signals[PRE_ACTION], 0);

		(void)g_idle_add(timeout_cb, roccat_update_assistant);
	} else if (page == gtk_assistant_get_nth_page(GTK_ASSISTANT(assistant), priv->success_page_number) ||
			page == gtk_assistant_get_nth_page(GTK_ASSISTANT(assistant), priv->failure_page_number)) {
		g_signal_emit((gpointer)assistant, signals[POST_ACTION], 0);
	}
}

static void close_cb(GtkAssistant *assistant, gpointer user_data) {
	// FIXME deactivate while updating
	gtk_widget_destroy(GTK_WIDGET(assistant));
}

static void cancel_cb(GtkAssistant *assistant, gpointer user_data) {
	// FIXME deactivate while updating
	gtk_widget_destroy(GTK_WIDGET(assistant));
}

static void apply_cb(GtkAssistant *assistant, gpointer user_data) {
}

static void roccat_update_assistant_init(RoccatUpdateAssistant *assistant) {
	RoccatUpdateAssistantPrivate *priv = ROCCAT_UPDATE_ASSISTANT_GET_PRIVATE(assistant);
	assistant->priv = priv;

	priv->firmwares = NULL;

	append_info_page(assistant);
	append_file_page(assistant);
	append_check_page(assistant);
	append_action_page(assistant);
	append_success_page(assistant);
	append_failure_page(assistant);

	g_signal_connect(G_OBJECT(assistant), "prepare", G_CALLBACK(prepare_cb), NULL);
	g_signal_connect(G_OBJECT(assistant), "close", G_CALLBACK(close_cb), NULL);
	g_signal_connect(G_OBJECT(assistant), "cancel", G_CALLBACK(cancel_cb), NULL);
	g_signal_connect(G_OBJECT(assistant), "apply", G_CALLBACK(apply_cb), NULL);
}

static void set_property(GObject *object, guint prop_id, GValue const *value, GParamSpec *pspec) {
	RoccatUpdateAssistantPrivate *priv = ROCCAT_UPDATE_ASSISTANT(object)->priv;
	gchar *text;
	gchar const *device_name;

	switch(prop_id) {
	case PROP_DEVICE:
		priv->device = ROCCAT_DEVICE(g_value_get_object(value));
		g_object_ref(G_OBJECT(priv->device));

		device_name = roccat_device_get_name_static(priv->device);
		text = g_strdup_printf(_("%s firmware update"), device_name);
		g_object_set(G_OBJECT(object), "title", text, NULL);
		g_free(text);

		text = g_strdup_printf(_("You are about to update a %s."), device_name);
		gtk_label_set_text(priv->file_page_label, text);
		gtk_label_set_text(priv->check_page_label, text);
		g_free(text);

		break;
	case PROP_ENDPOINT:
		priv->endpoint = g_value_get_uint(value);
		break;
	case PROP_WAIT_PREPARE:
		priv->wait_prepare = g_value_get_uint(value);
		break;
	case PROP_WAIT_INIT_DATA:
		priv->wait_init_data = g_value_get_uint(value);
		break;
	case PROP_WAIT_DATA:
		priv->wait_data = g_value_get_uint(value);
		break;
	case PROP_FINAL_WRITE_CHECK:
		priv->final_write_check = g_value_get_boolean(value);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
	}
}

static void roccat_update_assistant_firmware_free(gpointer data) {
	RoccatUpdateAssistantFirmware *firmware = (RoccatUpdateAssistantFirmware *)data;
	roccat_firmware_state_free(firmware->state);
	g_free(firmware);
}

static void finalize(GObject *object) {
	RoccatUpdateAssistantPrivate *priv = ROCCAT_UPDATE_ASSISTANT(object)->priv;
	g_slist_free_full(priv->firmwares, roccat_update_assistant_firmware_free);
	g_object_unref(G_OBJECT(priv->device));
	G_OBJECT_CLASS(roccat_update_assistant_parent_class)->finalize(object);
}

static void roccat_update_assistant_class_init(RoccatUpdateAssistantClass *klass) {
	GObjectClass *gobject_class = (GObjectClass*)klass;

	gobject_class->set_property = set_property;
	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(RoccatUpdateAssistantPrivate));

	signals[PRE_ACTION] = g_signal_new("pre-action", G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE,
			0);

	signals[POST_ACTION] = g_signal_new("post-action", G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE,
			0);

	g_object_class_install_property(gobject_class, PROP_DEVICE,
			g_param_spec_object("device",
					"device",
					"Device",
					ROCCAT_DEVICE_TYPE,
					G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));

	g_object_class_install_property(gobject_class, PROP_ENDPOINT,
			g_param_spec_uint("endpoint",
					"endpoint",
					"Endpoint",
					0, G_MAXUINT, 0,
					G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));

	g_object_class_install_property(gobject_class, PROP_WAIT_PREPARE,
			g_param_spec_uint("wait-prepare",
					"wait-prepare",
					"wait-prepare",
					0, G_MAXUINT, ROCCAT_FIRMWARE_UPDATE_WAIT_PREPARE_DEFAULT,
					G_PARAM_WRITABLE));

	g_object_class_install_property(gobject_class, PROP_WAIT_INIT_DATA,
			g_param_spec_uint("wait-init-data",
					"wait-init-data",
					"wait-init-data",
					0, G_MAXUINT, ROCCAT_FIRMWARE_UPDATE_WAIT_INIT_DATA_DEFAULT,
					G_PARAM_WRITABLE));

	g_object_class_install_property(gobject_class, PROP_WAIT_DATA,
			g_param_spec_uint("wait-data",
					"wait-data",
					"wait-data",
					0, G_MAXUINT, ROCCAT_FIRMWARE_UPDATE_WAIT_DATA_DEFAULT,
					G_PARAM_WRITABLE));

	g_object_class_install_property(gobject_class, PROP_FINAL_WRITE_CHECK,
			g_param_spec_boolean("final-write-check",
					"final-write-check",
					"final-write-check",
					ROCCAT_FIRMWARE_UPDATE_FINAL_WRITE_CHECK_DEFAULT,
					G_PARAM_WRITABLE));
}

GtkWidget *roccat_update_assistant_new(GtkWindow *parent, RoccatDevice *device, guint endpoint) {
	RoccatUpdateAssistant *assistant;
	assistant = ROCCAT_UPDATE_ASSISTANT(g_object_new(ROCCAT_UPDATE_ASSISTANT_TYPE,
			"transient-for", parent,
			"modal", TRUE,
			"device", device,
			"endpoint", endpoint,
			NULL));
	return GTK_WIDGET(assistant);
}
