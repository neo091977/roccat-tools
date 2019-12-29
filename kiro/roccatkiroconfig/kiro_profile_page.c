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

#include "kiro_profile_page.h"
#include "kiro_buttons_frame.h"
#include "kiro_notification_frame.h"
#include "kiro_color_frame.h"
#include "kiro_dcu_frame.h"
#include "kiro_light_effects_frame.h"
#include "kiro_sensitivity_frame.h"
#include "roccat_cpi_selector.h"
#include "roccat_handedness_selector.h"
#include "roccat_polling_rate_frame.h"
#include "roccat_helper.h"
#include "g_cclosure_roccat_marshaller.h"
#include "i18n.h"

#define KIRO_PROFILE_PAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KIRO_PROFILE_PAGE_TYPE, KiroProfilePageClass))
#define IS_KIRO_PROFILE_PAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KIRO_PROFILE_PAGE_TYPE))
#define KIRO_PROFILE_PAGE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KIRO_PROFILE_PAGE_TYPE, KiroProfilePagePrivate))

typedef struct _KiroProfilePageClass KiroProfilePageClass;
typedef struct _KiroProfilePagePrivate KiroProfilePagePrivate;

struct _KiroProfilePage {
	RoccatProfilePage parent;
	KiroProfilePagePrivate *priv;
};

struct _KiroProfilePageClass {
	RoccatProfilePageClass parent_class;
};

struct _KiroProfilePagePrivate {
	KiroProfileData *profile_data;
	GtkBox *misc_box;
	KiroNotificationFrame *notifications;
	RoccatCpiSelector *cpi_selector;
	RoccatPollingRateFrame *polling_rate;
	KiroColorFrame *colors;
	KiroLightEffectsFrame *light_effects;
	KiroSensitivityFrame *sensitivity;
	KiroButtonsFrame *buttons;
	RoccatHandednessSelector *handedness;
	KiroDcuFrame *dcu;
};

G_DEFINE_TYPE(KiroProfilePage, kiro_profile_page, ROCCAT_PROFILE_PAGE_TYPE);

GtkWidget *kiro_profile_page_new(void) {
	return GTK_WIDGET(g_object_new(KIRO_PROFILE_PAGE_TYPE, NULL));
}

static void set_from_profile_data(KiroProfilePage *profile_page) {
	KiroProfilePagePrivate *priv = profile_page->priv;
	KiroProfileDataHardware const *hardware = &priv->profile_data->hardware;
	guint i;

	roccat_profile_page_set_name(ROCCAT_PROFILE_PAGE(profile_page), priv->profile_data->eventhandler.profile_name);

	kiro_buttons_frame_set_from_profile_data(priv->buttons, priv->profile_data);
	kiro_notification_frame_set_from_profile_data(priv->notifications, priv->profile_data);
	kiro_color_frame_set_from_profile_data(priv->colors, priv->profile_data);
	kiro_light_effects_frame_set_from_profile_data(priv->light_effects, priv->profile_data);
	kiro_dcu_frame_set_value_blocked(priv->dcu, hardware->dcu);
	
	roccat_polling_rate_frame_set_value(priv->polling_rate, hardware->profile.misc & KIRO_PROFILE_MISC_POLLING_RATE_MASK);
	kiro_sensitivity_frame_set_value(priv->sensitivity, hardware->profile.sensitivity);

	roccat_cpi_selector_set_all_active(priv->cpi_selector, hardware->profile.cpi_levels_enabled);
	roccat_cpi_selector_set_selected(priv->cpi_selector, hardware->profile.cpi_active);
	for (i = 0; i < KIRO_PROFILE_CPI_LEVELS_NUM; ++i)
		roccat_cpi_selector_set_value(priv->cpi_selector, i, kiro_profile_cpi_level_to_cpi(hardware->profile.cpi_levels[i]));

	roccat_handedness_selector_set_value_blocked(priv->handedness,
			(roccat_get_bit8(hardware->profile.misc, KIRO_PROFILE_MISC_LEFT_HANDED_BIT)) ?
					ROCCAT_HANDEDNESS_SELECTOR_LEFT : ROCCAT_HANDEDNESS_SELECTOR_RIGHT);
}

static void update_profile_data(KiroProfilePage *profile_page, KiroProfileData *profile_data) {
	KiroProfilePagePrivate *priv = profile_page->priv;
	KiroProfileDataHardware *hardware = &profile_data->hardware;
	gchar *profile_name;
	guint i;
	guint8 misc;
	
	profile_name = roccat_profile_page_get_name(ROCCAT_PROFILE_PAGE(profile_page));
	kiro_profile_data_eventhandler_set_profile_name(&profile_data->eventhandler, profile_name);
	g_free(profile_name);

	kiro_buttons_frame_update_profile_data(priv->buttons, profile_data);
	
	kiro_notification_frame_update_profile_data(priv->notifications, profile_data);
	kiro_color_frame_update_profile_data(priv->colors, profile_data);
	kiro_light_effects_frame_update_profile_data(priv->light_effects, profile_data);
	kiro_profile_data_hardware_set_dcu(hardware, kiro_dcu_frame_get_value(priv->dcu));
	
	if (hardware->profile.sensitivity != kiro_sensitivity_frame_get_value(priv->sensitivity)) {
		hardware->profile.sensitivity = kiro_sensitivity_frame_get_value(priv->sensitivity);
		hardware->modified_profile = TRUE;
	}
	if (hardware->profile.cpi_levels_enabled != roccat_cpi_selector_get_all_active(priv->cpi_selector)) {
		hardware->profile.cpi_levels_enabled = roccat_cpi_selector_get_all_active(priv->cpi_selector);
		hardware->modified_profile = TRUE;
	}
	if (hardware->profile.cpi_active != roccat_cpi_selector_get_selected(priv->cpi_selector)) {
		hardware->profile.cpi_active = roccat_cpi_selector_get_selected(priv->cpi_selector);
		hardware->modified_profile = TRUE;
	}
	for (i = 0; i < KIRO_PROFILE_CPI_LEVELS_NUM; ++i) {
		if (hardware->profile.cpi_levels[i] == kiro_profile_cpi_to_cpi_level(roccat_cpi_selector_get_value(priv->cpi_selector, i))) {
			hardware->profile.cpi_levels[i] = kiro_profile_cpi_to_cpi_level(roccat_cpi_selector_get_value(priv->cpi_selector, i));
			hardware->modified_profile = TRUE;
		}
	}

	misc = hardware->profile.misc;
	misc &= ~KIRO_PROFILE_MISC_POLLING_RATE_MASK;
	misc |= roccat_polling_rate_frame_get_value(priv->polling_rate);
	roccat_set_bit8(&misc, KIRO_PROFILE_MISC_LEFT_HANDED_BIT, roccat_handedness_selector_get_value(priv->handedness) == ROCCAT_HANDEDNESS_SELECTOR_LEFT);
	
	if (hardware->profile.misc  != misc) {
		hardware->profile.misc = misc;
		hardware->modified_profile = TRUE;
	}
}

static void reset_cb(RoccatProfilePage *profile_page, gpointer user_data) {
	set_from_profile_data(KIRO_PROFILE_PAGE(profile_page));
}

static void handedness_toggled_cb(RoccatHandednessSelector *selector, gpointer user_data) {
	KiroProfilePage *profile_page = KIRO_PROFILE_PAGE(user_data);
	kiro_buttons_frame_change_handedness(profile_page->priv->buttons);
}

static void append_buttons_page(KiroProfilePage *profile_page, GtkNotebook *notebook) {
	KiroProfilePagePrivate *priv = profile_page->priv;
	GtkWidget *vbox;

	vbox = gtk_vbox_new(FALSE, 0);
	priv->buttons = KIRO_BUTTONS_FRAME(kiro_buttons_frame_new());

	priv->handedness = ROCCAT_HANDEDNESS_SELECTOR(roccat_handedness_selector_new());
	gtk_widget_set_tooltip_text(GTK_WIDGET(priv->handedness), _("Swaps the left and right mouse buttons and side buttons."));
	g_signal_connect(G_OBJECT(priv->handedness), "toggled", G_CALLBACK(handedness_toggled_cb), profile_page);

	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(priv->handedness), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(priv->buttons), TRUE, TRUE, 0);

	gtk_notebook_append_page(notebook, vbox, gtk_label_new(_("Buttons")));
}

static void append_misc_page(KiroProfilePage *profile_page, GtkNotebook *notebook) {
	KiroProfilePagePrivate *priv = profile_page->priv;
	GtkWidget *hbox1;
	GtkWidget *hbox2;
	GtkWidget *vbox1;
	GtkWidget *vbox2;

	hbox1 = gtk_hbox_new(FALSE, 0);
	hbox2 = gtk_hbox_new(FALSE, 0);
	vbox1 = gtk_vbox_new(FALSE, 0);
	vbox2 = gtk_vbox_new(FALSE, 0);
	priv->misc_box = GTK_BOX(gtk_vbox_new(FALSE, 0));
	priv->notifications = KIRO_NOTIFICATION_FRAME(kiro_notification_frame_new());
	priv->cpi_selector = ROCCAT_CPI_SELECTOR(roccat_cpi_selector_new(KIRO_PROFILE_CPI_LEVELS_NUM, KIRO_CPI_MIN, KIRO_CPI_MAX, KIRO_CPI_STEP));
	priv->colors = KIRO_COLOR_FRAME(kiro_color_frame_new());
	priv->polling_rate = ROCCAT_POLLING_RATE_FRAME(roccat_polling_rate_frame_new());
	priv->light_effects = KIRO_LIGHT_EFFECTS_FRAME(kiro_light_effects_frame_new());
	priv->sensitivity = KIRO_SENSITIVITY_FRAME(kiro_sensitivity_frame_new());
	priv->dcu = KIRO_DCU_FRAME(kiro_dcu_frame_new());
	
	gtk_box_pack_start(GTK_BOX(hbox1), GTK_WIDGET(priv->colors), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox1), GTK_WIDGET(priv->light_effects), TRUE, TRUE, 0);

	gtk_box_pack_start(GTK_BOX(vbox1), hbox1, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox1), GTK_WIDGET(priv->sensitivity), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox1), GTK_WIDGET(priv->polling_rate), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox1), GTK_WIDGET(priv->dcu), TRUE, TRUE, 0);

	gtk_box_pack_start(GTK_BOX(vbox2), GTK_WIDGET(priv->cpi_selector), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox2), GTK_WIDGET(priv->notifications), TRUE, TRUE, 0);

	gtk_box_pack_start(GTK_BOX(hbox2), vbox1, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox2), vbox2, TRUE, TRUE, 0);

	gtk_box_pack_start(priv->misc_box, hbox2, TRUE, TRUE, 0);

	gtk_notebook_append_page(notebook, GTK_WIDGET(priv->misc_box), gtk_label_new(_("Misc")));
}

static void kiro_profile_page_init(KiroProfilePage *profile_page) {
	KiroProfilePagePrivate *priv = KIRO_PROFILE_PAGE_GET_PRIVATE(profile_page);
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

static void kiro_profile_page_class_init(KiroProfilePageClass *klass) {
	g_type_class_add_private(klass, sizeof(KiroProfilePagePrivate));
}

void kiro_profile_page_set_profile_data(KiroProfilePage *profile_page, KiroProfileData const *profile_data) {
	KiroProfilePagePrivate *priv = profile_page->priv;
	if (priv->profile_data)
		g_free(priv->profile_data);
	priv->profile_data = kiro_profile_data_dup(profile_data);
	set_from_profile_data(profile_page);
}

KiroProfileData *kiro_profile_page_get_profile_data(KiroProfilePage *profile_page) {
	KiroProfilePagePrivate *priv = profile_page->priv;
	KiroProfileData *profile_data;
	profile_data = kiro_profile_data_dup(priv->profile_data);
	update_profile_data(profile_page, profile_data);
	return profile_data;
}
