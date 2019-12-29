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

#include "kova2016_profile_page.h"
#include "kova2016_buttons_frame.h"
#include "kova2016_notification_frame.h"
#include "kova2016_color_frame.h"
#include "kova2016_light_effects_frame.h"
#include "roccat_gamefile_selector.h"
#include "roccat_cpi_selector.h"
#include "roccat_handedness_selector.h"
#include "roccat_polling_rate_frame.h"
#include "g_cclosure_roccat_marshaller.h"
#include "i18n.h"

#define KOVA2016_PROFILE_PAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KOVA2016_PROFILE_PAGE_TYPE, Kova2016ProfilePageClass))
#define IS_KOVA2016_PROFILE_PAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KOVA2016_PROFILE_PAGE_TYPE))
#define KOVA2016_PROFILE_PAGE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KOVA2016_PROFILE_PAGE_TYPE, Kova2016ProfilePagePrivate))

typedef struct _Kova2016ProfilePageClass Kova2016ProfilePageClass;
typedef struct _Kova2016ProfilePagePrivate Kova2016ProfilePagePrivate;

struct _Kova2016ProfilePage {
	RoccatProfilePage parent;
	Kova2016ProfilePagePrivate *priv;
};

struct _Kova2016ProfilePageClass {
	RoccatProfilePageClass parent_class;
};

struct _Kova2016ProfilePagePrivate {
	Kova2016ProfileData *profile_data;
	GtkBox *misc_box;
	RoccatGamefileSelector *gamefiles;
	Kova2016NotificationFrame *notifications;
	RoccatCpiSelector *cpi_selector;
	RoccatPollingRateFrame *polling_rate;
	Kova2016ColorFrame *colors;
	Kova2016LightEffectsFrame *light_effects;
	Kova2016ButtonsFrame *buttons;
	RoccatHandednessSelector *handedness;
};

G_DEFINE_TYPE(Kova2016ProfilePage, kova2016_profile_page, ROCCAT_PROFILE_PAGE_TYPE);

GtkWidget *kova2016_profile_page_new(void) {
	return GTK_WIDGET(g_object_new(KOVA2016_PROFILE_PAGE_TYPE, NULL));
}

static void set_from_profile_data(Kova2016ProfilePage *profile_page) {
	Kova2016ProfilePagePrivate *priv = profile_page->priv;
	Kova2016ProfileDataHardware *hardware = &priv->profile_data->hardware;
	guint i;

	roccat_profile_page_set_name(ROCCAT_PROFILE_PAGE(profile_page), priv->profile_data->eventhandler.profile_name);

	kova2016_buttons_frame_set_from_profile_data(priv->buttons, priv->profile_data);
	kova2016_notification_frame_set_from_profile_data(priv->notifications, priv->profile_data);
	kova2016_color_frame_set_from_profile_data(priv->colors, priv->profile_data);
	kova2016_light_effects_frame_set_from_profile_data(priv->light_effects, priv->profile_data);

	roccat_polling_rate_frame_set_value(priv->polling_rate, hardware->profile_settings.polling_rate);

	roccat_cpi_selector_set_all_active(priv->cpi_selector, hardware->profile_settings.cpi_levels_enabled);
	roccat_cpi_selector_set_selected(priv->cpi_selector, hardware->profile_settings.cpi_active);
	for (i = 0; i < KOVA2016_PROFILE_SETTINGS_CPI_LEVELS_NUM; ++i)
		roccat_cpi_selector_set_value(priv->cpi_selector, i, kova2016_profile_settings_cpi_level_to_cpi(hardware->profile_settings.cpi_levels[i]));

	for (i = 0; i < ROCCAT_SWARM_RMP_GAMEFILE_NUM; ++i)
		roccat_gamefile_selector_set_text(priv->gamefiles, i, priv->profile_data->eventhandler.gamefile_names[i]);

	roccat_handedness_selector_set_value_blocked(priv->handedness,
			(hardware->profile_settings.handedness == KOVA2016_PROFILE_SETTINGS_HANDEDNESS_LEFT) ?
					ROCCAT_HANDEDNESS_SELECTOR_LEFT : ROCCAT_HANDEDNESS_SELECTOR_RIGHT);
}

static void update_profile_data(Kova2016ProfilePage *profile_page, Kova2016ProfileData *profile_data) {
	Kova2016ProfilePagePrivate *priv = profile_page->priv;
	Kova2016ProfileDataHardware *hardware = &profile_data->hardware;
	gchar *profile_name;
	guint i;
	guint handedness;
	
	profile_name = roccat_profile_page_get_name(ROCCAT_PROFILE_PAGE(profile_page));
	kova2016_profile_data_eventhandler_set_profile_name(&profile_data->eventhandler, profile_name);
	g_free(profile_name);

	kova2016_buttons_frame_update_profile_data(priv->buttons, profile_data);
	
	kova2016_notification_frame_update_profile_data(priv->notifications, profile_data);
	kova2016_color_frame_update_profile_data(priv->colors, profile_data);
	kova2016_light_effects_frame_update_profile_data(priv->light_effects, profile_data);

	if (hardware->profile_settings.polling_rate != roccat_polling_rate_frame_get_value(priv->polling_rate)) {
		hardware->profile_settings.polling_rate = roccat_polling_rate_frame_get_value(priv->polling_rate);
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
	for (i = 0; i < KOVA2016_PROFILE_SETTINGS_CPI_LEVELS_NUM; ++i) {
		if (kova2016_profile_settings_cpi_level_to_cpi(hardware->profile_settings.cpi_levels[i]) != roccat_cpi_selector_get_value(priv->cpi_selector, i)) {
			hardware->profile_settings.cpi_levels[i] = kova2016_profile_settings_cpi_to_cpi_level(roccat_cpi_selector_get_value(priv->cpi_selector, i));
			hardware->modified_profile_settings = TRUE;
		}
	}

	for (i = 0; i < ROCCAT_SWARM_RMP_GAMEFILE_NUM; ++i)
		kova2016_profile_data_eventhandler_set_gamefile_name(&profile_data->eventhandler, i, roccat_gamefile_selector_get_text(priv->gamefiles, i));

	handedness = (roccat_handedness_selector_get_value(priv->handedness) == ROCCAT_HANDEDNESS_SELECTOR_LEFT) ?
			KOVA2016_PROFILE_SETTINGS_HANDEDNESS_LEFT : KOVA2016_PROFILE_SETTINGS_HANDEDNESS_RIGHT;
	if (hardware->profile_settings.handedness != handedness) {
		hardware->profile_settings.handedness = handedness;
		hardware->modified_profile_settings = TRUE;
	}
}

static void reset_cb(RoccatProfilePage *profile_page, gpointer user_data) {
	set_from_profile_data(KOVA2016_PROFILE_PAGE(profile_page));
}

static void handedness_toggled_cb(RoccatHandednessSelector *selector, gpointer user_data) {
	Kova2016ProfilePage *profile_page = KOVA2016_PROFILE_PAGE(user_data);
	kova2016_buttons_frame_change_handedness(profile_page->priv->buttons);
}

static void append_buttons_page(Kova2016ProfilePage *profile_page, GtkNotebook *notebook) {
	Kova2016ProfilePagePrivate *priv = profile_page->priv;
	GtkWidget *vbox;

	vbox = gtk_vbox_new(FALSE, 0);
	priv->buttons = KOVA2016_BUTTONS_FRAME(kova2016_buttons_frame_new());

	priv->handedness = ROCCAT_HANDEDNESS_SELECTOR(roccat_handedness_selector_new());
	gtk_widget_set_tooltip_text(GTK_WIDGET(priv->handedness), _("Swaps the left and right mouse buttons including Smart Cast and side buttons."));
	g_signal_connect(G_OBJECT(priv->handedness), "toggled", G_CALLBACK(handedness_toggled_cb), profile_page);

	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(priv->handedness), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(priv->buttons), TRUE, TRUE, 0);

	gtk_notebook_append_page(notebook, vbox, gtk_label_new(_("Buttons")));
}

static void append_misc_page(Kova2016ProfilePage *profile_page, GtkNotebook *notebook) {
	Kova2016ProfilePagePrivate *priv = profile_page->priv;
	GtkWidget *hbox1;
	GtkWidget *hbox2;
	GtkWidget *vbox1;
	GtkWidget *vbox2;

	hbox1 = gtk_hbox_new(FALSE, 0);
	hbox2 = gtk_hbox_new(FALSE, 0);
	vbox1 = gtk_vbox_new(FALSE, 0);
	vbox2 = gtk_vbox_new(FALSE, 0);
	priv->misc_box = GTK_BOX(gtk_vbox_new(FALSE, 0));
	priv->gamefiles = ROCCAT_GAMEFILE_SELECTOR(roccat_gamefile_selector_new(ROCCAT_SWARM_RMP_GAMEFILE_NUM));
	priv->notifications = KOVA2016_NOTIFICATION_FRAME(kova2016_notification_frame_new());
	priv->cpi_selector = ROCCAT_CPI_SELECTOR(roccat_cpi_selector_new(KOVA2016_PROFILE_SETTINGS_CPI_LEVELS_NUM, KOVA2016_CPI_MIN, KOVA2016_CPI_MAX, KOVA2016_CPI_STEP));
	priv->colors = KOVA2016_COLOR_FRAME(kova2016_color_frame_new());
	priv->polling_rate = ROCCAT_POLLING_RATE_FRAME(roccat_polling_rate_frame_new());
	priv->light_effects = KOVA2016_LIGHT_EFFECTS_FRAME(kova2016_light_effects_frame_new());
	
	gtk_box_pack_start(GTK_BOX(hbox1), GTK_WIDGET(priv->colors), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox1), GTK_WIDGET(priv->light_effects), TRUE, TRUE, 0);

	gtk_box_pack_start(GTK_BOX(vbox1), hbox1, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox1), GTK_WIDGET(priv->polling_rate), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox1), GTK_WIDGET(priv->gamefiles), TRUE, TRUE, 0);

	gtk_box_pack_start(GTK_BOX(vbox2), GTK_WIDGET(priv->cpi_selector), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox2), GTK_WIDGET(priv->notifications), TRUE, TRUE, 0);

	gtk_box_pack_start(GTK_BOX(hbox2), vbox1, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox2), vbox2, TRUE, TRUE, 0);

	gtk_box_pack_start(priv->misc_box, hbox2, TRUE, TRUE, 0);

	gtk_notebook_append_page(notebook, GTK_WIDGET(priv->misc_box), gtk_label_new(_("Misc")));
}

static void kova2016_profile_page_init(Kova2016ProfilePage *profile_page) {
	Kova2016ProfilePagePrivate *priv = KOVA2016_PROFILE_PAGE_GET_PRIVATE(profile_page);
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

static void kova2016_profile_page_class_init(Kova2016ProfilePageClass *klass) {
	g_type_class_add_private(klass, sizeof(Kova2016ProfilePagePrivate));
}

void kova2016_profile_page_set_profile_data(Kova2016ProfilePage *profile_page, Kova2016ProfileData const *profile_data) {
	Kova2016ProfilePagePrivate *priv = profile_page->priv;
	if (priv->profile_data)
		g_free(priv->profile_data);
	priv->profile_data = kova2016_profile_data_dup(profile_data);
	set_from_profile_data(profile_page);
}

Kova2016ProfileData *kova2016_profile_page_get_profile_data(Kova2016ProfilePage *profile_page) {
	Kova2016ProfilePagePrivate *priv = profile_page->priv;
	Kova2016ProfileData *profile_data;
	profile_data = kova2016_profile_data_dup(priv->profile_data);
	update_profile_data(profile_page, profile_data);
	return profile_data;
}
