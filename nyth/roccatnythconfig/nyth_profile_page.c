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

#include "nyth_profile_page.h"
#include "nyth_side_buttons_frame.h"
#include "nyth_top_buttons_frame.h"
#include "nyth_notification_frame.h"
#include "nyth_color_frame.h"
#include "nyth_light_effects_frame.h"
#include "roccat_sensitivity_frame.h"
#include "roccat_gamefile_selector.h"
#include "roccat_cpi_selector.h"
#include "roccat_polling_rate_frame.h"
#include "g_cclosure_roccat_marshaller.h"
#include "i18n.h"

#define NYTH_PROFILE_PAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), NYTH_PROFILE_PAGE_TYPE, NythProfilePageClass))
#define IS_NYTH_PROFILE_PAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), NYTH_PROFILE_PAGE_TYPE))
#define NYTH_PROFILE_PAGE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), NYTH_PROFILE_PAGE_TYPE, NythProfilePagePrivate))

typedef struct _NythProfilePageClass NythProfilePageClass;
typedef struct _NythProfilePagePrivate NythProfilePagePrivate;

struct _NythProfilePage {
	RoccatProfilePage parent;
	NythProfilePagePrivate *priv;
};

struct _NythProfilePageClass {
	RoccatProfilePageClass parent_class;
};

struct _NythProfilePagePrivate {
	NythProfileData *profile_data;
	GtkBox *misc_box;
	RoccatGamefileSelector *gamefiles;
	NythNotificationFrame *notifications;
	RoccatCpiSelector *cpi_selector;
	RoccatPollingRateFrame *polling_rate;
	NythColorFrame *colors;
	NythLightEffectsFrame *light_effects;
	RoccatSensitivityFrame *sensitivity;
	NythTopButtonsFrame *top_buttons;
	NythSideButtonsFrame *bottom_buttons;
};

G_DEFINE_TYPE(NythProfilePage, nyth_profile_page, ROCCAT_PROFILE_PAGE_TYPE);

GtkWidget *nyth_profile_page_new(void) {
	return GTK_WIDGET(g_object_new(NYTH_PROFILE_PAGE_TYPE, NULL));
}

static void set_from_profile_data(NythProfilePage *profile_page) {
	NythProfilePagePrivate *priv = profile_page->priv;
	guint i;

	roccat_profile_page_set_name(ROCCAT_PROFILE_PAGE(profile_page), priv->profile_data->eventhandler.profile_name);

	nyth_top_buttons_frame_set_from_profile_data(priv->top_buttons, priv->profile_data);
	nyth_side_buttons_frame_set_from_profile_data(priv->bottom_buttons, priv->profile_data);
	nyth_notification_frame_set_from_profile_data(priv->notifications, priv->profile_data);
	nyth_color_frame_set_from_profile_data(priv->colors, priv->profile_data);
	nyth_light_effects_frame_set_from_profile_data(priv->light_effects, priv->profile_data);

	roccat_polling_rate_frame_set_value(priv->polling_rate, priv->profile_data->hardware.profile_settings.polling_rate);

	roccat_sensitivity_frame_set_advanced(priv->sensitivity, priv->profile_data->hardware.profile_settings.advanced_sensitivity);
	roccat_sensitivity_frame_set_x_value(priv->sensitivity, priv->profile_data->hardware.profile_settings.sensitivity_x);
	roccat_sensitivity_frame_set_y_value(priv->sensitivity, priv->profile_data->hardware.profile_settings.sensitivity_y);

	roccat_cpi_selector_set_all_active(priv->cpi_selector, priv->profile_data->hardware.profile_settings.cpi_levels_enabled);
	roccat_cpi_selector_set_selected(priv->cpi_selector, priv->profile_data->hardware.profile_settings.cpi_active);
	for (i = 0; i < NYTH_PROFILE_SETTINGS_CPI_LEVELS_NUM; ++i)
		roccat_cpi_selector_set_value(priv->cpi_selector, i, nyth_profile_settings_get_cpi_level(&priv->profile_data->hardware.profile_settings, i));

	for (i = 0; i < ROCCAT_SWARM_RMP_GAMEFILE_NUM; ++i)
		roccat_gamefile_selector_set_text(priv->gamefiles, i, priv->profile_data->eventhandler.gamefile_names[i]);
}

static void update_profile_data(NythProfilePage *profile_page, NythProfileData *profile_data) {
	NythProfilePagePrivate *priv = profile_page->priv;
	NythProfileDataHardware *hardware = &profile_data->hardware;
	gchar *profile_name;
	guint i;
	
	profile_name = roccat_profile_page_get_name(ROCCAT_PROFILE_PAGE(profile_page));
	nyth_profile_data_eventhandler_set_profile_name(&profile_data->eventhandler, profile_name);
	g_free(profile_name);

	nyth_top_buttons_frame_update_profile_data(priv->top_buttons, profile_data);
	nyth_side_buttons_frame_update_profile_data(priv->bottom_buttons, profile_data);
	
	/* As button sets are not supported, button set gets reset to 12-button default
	 * for Windows compatibility if buttons have been modified.
	 */
	if (hardware->modified_profile_buttons == TRUE)
		nyth_profile_data_hardware_set_button_set(hardware, nyth_button_set_default());
	
	nyth_notification_frame_update_profile_data(priv->notifications, profile_data);
	nyth_color_frame_update_profile_data(priv->colors, profile_data);
	nyth_light_effects_frame_update_profile_data(priv->light_effects, profile_data);

	if (hardware->profile_settings.polling_rate != roccat_polling_rate_frame_get_value(priv->polling_rate)) {
		hardware->profile_settings.polling_rate = roccat_polling_rate_frame_get_value(priv->polling_rate);
		hardware->modified_profile_settings = TRUE;
	}

	if (hardware->profile_settings.advanced_sensitivity != roccat_sensitivity_frame_get_advanced(priv->sensitivity)) {
		hardware->profile_settings.advanced_sensitivity = roccat_sensitivity_frame_get_advanced(priv->sensitivity);
		hardware->modified_profile_settings = TRUE;
	}
	if (hardware->profile_settings.sensitivity_x != roccat_sensitivity_frame_get_x_value(priv->sensitivity)) {
		hardware->profile_settings.sensitivity_x = roccat_sensitivity_frame_get_x_value(priv->sensitivity);
		hardware->modified_profile_settings = TRUE;
	}
	if (hardware->profile_settings.sensitivity_y != roccat_sensitivity_frame_get_y_value(priv->sensitivity)) {
		hardware->profile_settings.sensitivity_y = roccat_sensitivity_frame_get_y_value(priv->sensitivity);
		hardware->modified_profile_settings = TRUE;
	}

	if (hardware->profile_settings.cpi_levels_enabled != roccat_cpi_selector_get_all_active(priv->cpi_selector)) {
		hardware->profile_settings.cpi_levels_enabled = roccat_cpi_selector_get_all_active(priv->cpi_selector);
		hardware->modified_profile_settings = TRUE;
	}
	if (hardware->profile_settings.cpi_active != roccat_cpi_selector_get_selected(priv->cpi_selector)) {
		hardware->profile_settings.cpi_active = roccat_cpi_selector_get_selected(priv->cpi_selector);
		hardware->modified_profile_settings = TRUE;
	}
	for (i = 0; i < NYTH_PROFILE_SETTINGS_CPI_LEVELS_NUM; ++i) {
		if (nyth_profile_settings_get_cpi_level(&hardware->profile_settings, i) != roccat_cpi_selector_get_value(priv->cpi_selector, i)) {
			nyth_profile_settings_set_cpi_level(&hardware->profile_settings, i, roccat_cpi_selector_get_value(priv->cpi_selector, i));
			hardware->modified_profile_settings = TRUE;
		}
	}

	for (i = 0; i < ROCCAT_SWARM_RMP_GAMEFILE_NUM; ++i)
		nyth_profile_data_eventhandler_set_gamefile_name(&profile_data->eventhandler, i, roccat_gamefile_selector_get_text(priv->gamefiles, i));
}

static void reset_cb(RoccatProfilePage *profile_page, gpointer user_data) {
	set_from_profile_data(NYTH_PROFILE_PAGE(profile_page));
}

static void append_buttons_page(NythProfilePage *profile_page, GtkNotebook *notebook) {
	NythProfilePagePrivate *priv = profile_page->priv;
	GtkWidget *vbox;

	vbox = gtk_vbox_new(FALSE, 0);
	priv->top_buttons = NYTH_TOP_BUTTONS_FRAME(nyth_top_buttons_frame_new());
	priv->bottom_buttons = NYTH_SIDE_BUTTONS_FRAME(nyth_side_buttons_frame_new());

	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(priv->top_buttons), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(priv->bottom_buttons), TRUE, TRUE, 0);

	gtk_notebook_append_page(notebook, vbox, gtk_label_new(_("Buttons")));
}

static void append_misc_page(NythProfilePage *profile_page, GtkNotebook *notebook) {
	NythProfilePagePrivate *priv = profile_page->priv;
	GtkWidget *hbox1;
	GtkWidget *hbox2;
	GtkWidget *hbox3;
	GtkWidget *vbox1;
	GtkWidget *vbox2;

	hbox1 = gtk_hbox_new(FALSE, 0);
	hbox2 = gtk_hbox_new(FALSE, 0);
	hbox3 = gtk_hbox_new(FALSE, 0);
	vbox1 = gtk_vbox_new(FALSE, 0);
	vbox2 = gtk_vbox_new(FALSE, 0);
	priv->misc_box = GTK_BOX(gtk_vbox_new(FALSE, 0));
	priv->gamefiles = ROCCAT_GAMEFILE_SELECTOR(roccat_gamefile_selector_new(ROCCAT_SWARM_RMP_GAMEFILE_NUM));
	priv->notifications = NYTH_NOTIFICATION_FRAME(nyth_notification_frame_new());
	priv->cpi_selector = ROCCAT_CPI_SELECTOR(roccat_cpi_selector_new(NYTH_PROFILE_SETTINGS_CPI_LEVELS_NUM, NYTH_CPI_MIN, NYTH_CPI_MAX, NYTH_CPI_STEP));
	priv->colors = NYTH_COLOR_FRAME(nyth_color_frame_new());
	priv->polling_rate = ROCCAT_POLLING_RATE_FRAME(roccat_polling_rate_frame_new());
	priv->light_effects = NYTH_LIGHT_EFFECTS_FRAME(nyth_light_effects_frame_new());
	priv->sensitivity = ROCCAT_SENSITIVITY_FRAME(roccat_sensitivity_frame_new());

	gtk_box_pack_start(GTK_BOX(hbox1), GTK_WIDGET(priv->colors), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox1), GTK_WIDGET(priv->light_effects), TRUE, TRUE, 0);

	gtk_box_pack_start(GTK_BOX(vbox1), hbox1, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox1), GTK_WIDGET(priv->sensitivity), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox1), GTK_WIDGET(priv->polling_rate), TRUE, TRUE, 0);

	gtk_box_pack_start(GTK_BOX(hbox2), vbox1, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox2), GTK_WIDGET(priv->cpi_selector), TRUE, TRUE, 0);

	gtk_box_pack_start(GTK_BOX(hbox3), GTK_WIDGET(priv->gamefiles), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox3), GTK_WIDGET(priv->notifications), TRUE, TRUE, 0);

	gtk_box_pack_start(GTK_BOX(vbox2), hbox2, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox2), hbox3, TRUE, TRUE, 0);

	gtk_box_pack_start(priv->misc_box, vbox2, TRUE, TRUE, 0);

	gtk_notebook_append_page(notebook, GTK_WIDGET(priv->misc_box), gtk_label_new(_("Misc")));
}

static void nyth_profile_page_init(NythProfilePage *profile_page) {
	NythProfilePagePrivate *priv = NYTH_PROFILE_PAGE_GET_PRIVATE(profile_page);
	GtkWidget *notebook;

	profile_page->priv = priv;

	priv->profile_data = NULL;

	notebook = gtk_notebook_new();

	append_buttons_page(profile_page, GTK_NOTEBOOK(notebook));
	append_misc_page(profile_page, GTK_NOTEBOOK(notebook));

	gtk_box_pack_start(GTK_BOX(profile_page), notebook, TRUE, TRUE, 0);

	gtk_widget_show_all(notebook);
	g_signal_connect(G_OBJECT(profile_page), "reset", G_CALLBACK(reset_cb), NULL);
}

static void nyth_profile_page_class_init(NythProfilePageClass *klass) {
	g_type_class_add_private(klass, sizeof(NythProfilePagePrivate));
}

void nyth_profile_page_set_profile_data(NythProfilePage *profile_page, NythProfileData const *profile_data) {
	NythProfilePagePrivate *priv = profile_page->priv;
	if (priv->profile_data)
		g_free(priv->profile_data);
	priv->profile_data = nyth_profile_data_dup(profile_data);
	set_from_profile_data(profile_page);
}

NythProfileData *nyth_profile_page_get_profile_data(NythProfilePage *profile_page) {
	NythProfilePagePrivate *priv = profile_page->priv;
	NythProfileData *profile_data;
	profile_data = nyth_profile_data_dup(priv->profile_data);
	update_profile_data(profile_page, profile_data);
	return profile_data;
}
