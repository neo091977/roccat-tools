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

#include "koneconfig_profile_page.h"
#include "koneconfig_buttons_frame.h"
#include "koneconfig_polling_rate_frame.h"
#include "koneconfig_cpi_selector.h"
#include "koneconfig_color_frame.h"
#include "koneconfig_light_effects_frame.h"
#include "koneconfig_sensitivity_frame.h"
#include "roccat_gamefile_selector.h"
#include "i18n.h"

#define KONECONFIG_PROFILE_PAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KONECONFIG_PROFILE_PAGE_TYPE, KoneconfigProfilePageClass))
#define IS_KONECONFIG_PROFILE_PAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KONECONFIG_PROFILE_PAGE_TYPE))
#define KONECONFIG_PROFILE_PAGE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KONECONFIG_PROFILE_PAGE_TYPE, KoneconfigProfilePagePrivate))

typedef struct _KoneconfigProfilePageClass KoneconfigProfilePageClass;
typedef struct _KoneconfigProfilePagePrivate KoneconfigProfilePagePrivate;

struct _KoneconfigProfilePage {
	RoccatProfilePage parent;
	KoneconfigProfilePagePrivate *priv;
};

struct _KoneconfigProfilePageClass {
	RoccatProfilePageClass parent_class;
};

struct _KoneconfigProfilePagePrivate {
	KoneRMP *rmp;
	KoneconfigButtonsFrame *buttons;
	KoneconfigPollingRateFrame *polling_rate;
	RoccatGamefileSelector *gamefile;
	KoneconfigCpiSelector *cpi;
	KoneconfigSensitivityFrame *sensitivity;
	KoneconfigLightEffectsFrame *light_effects;
	KoneconfigColorFrame *colors;
};

G_DEFINE_TYPE(KoneconfigProfilePage, koneconfig_profile_page, ROCCAT_PROFILE_PAGE_TYPE);

GtkWidget *koneconfig_profile_page_new(void) {
	return GTK_WIDGET(g_object_new(KONECONFIG_PROFILE_PAGE_TYPE, NULL));
}

static void set_from_rmp(KoneconfigProfilePage *profile_page) {
	KoneconfigProfilePagePrivate *priv = profile_page->priv;

	roccat_profile_page_set_name(ROCCAT_PROFILE_PAGE(profile_page), kone_rmp_get_profile_name(priv->rmp));
	
	koneconfig_buttons_frame_set_from_rmp(priv->buttons, priv->rmp);
	koneconfig_sensitivity_frame_set_from_rmp(priv->sensitivity, priv->rmp);
	koneconfig_color_frame_set_from_rmp(priv->colors, priv->rmp);
	koneconfig_light_effects_frame_set_from_rmp(priv->light_effects, priv->rmp);
	koneconfig_cpi_selector_set_from_rmp(priv->cpi, priv->rmp);
	roccat_gamefile_selector_set_text(priv->gamefile, 0, kone_rmp_get_game_file_name(priv->rmp));
	koneconfig_polling_rate_frame_set_value(priv->polling_rate, kone_rmp_get_polling_rate(priv->rmp));
}

static void update_rmp(KoneconfigProfilePage *profile_page, KoneRMP *rmp) {
	KoneconfigProfilePagePrivate *priv = profile_page->priv;
	gchar *profile_name;

	profile_name = roccat_profile_page_get_name(ROCCAT_PROFILE_PAGE(profile_page));
	kone_rmp_set_profile_name(rmp, profile_name);
	g_free(profile_name);
	
	koneconfig_buttons_frame_update_rmp(priv->buttons, rmp);
	koneconfig_sensitivity_frame_update_rmp(priv->sensitivity, rmp);
	koneconfig_color_frame_update_rmp(priv->colors, rmp);
	koneconfig_light_effects_frame_update_rmp(priv->light_effects, rmp);
	koneconfig_cpi_selector_update_rmp(priv->cpi, rmp);
	kone_rmp_set_game_file_name(rmp, roccat_gamefile_selector_get_text(priv->gamefile, 0));
	kone_rmp_set_polling_rate(rmp, koneconfig_polling_rate_frame_get_value(priv->polling_rate));
}

static void reset_cb(RoccatProfilePage *profile_page, gpointer user_data) {
	set_from_rmp(KONECONFIG_PROFILE_PAGE(profile_page));
}

static void koneconfig_profile_page_init(KoneconfigProfilePage *profile_page) {
	KoneconfigProfilePagePrivate *priv = KONECONFIG_PROFILE_PAGE_GET_PRIVATE(profile_page);
	GtkWidget *content;
	GtkWidget *left;
	GtkWidget *middle;
	GtkWidget *right;

	profile_page->priv = priv;

	content = gtk_hbox_new(FALSE, 0);
	left = gtk_vbox_new(FALSE, 0);
	middle = gtk_vbox_new(FALSE, 0);
	right = gtk_vbox_new(FALSE, 0);
	priv->buttons = KONECONFIG_BUTTONS_FRAME(koneconfig_buttons_frame_new());
	priv->polling_rate = KONECONFIG_POLLING_RATE_FRAME(koneconfig_polling_rate_frame_new());
	priv->gamefile = ROCCAT_GAMEFILE_SELECTOR(roccat_gamefile_selector_new(KONE_GAMEFILE_NUM));
	priv->cpi = KONECONFIG_CPI_SELECTOR(koneconfig_cpi_selector_new());
	priv->sensitivity = KONECONFIG_SENSITIVITY_FRAME(koneconfig_sensitivity_frame_new());
	priv->colors = KONECONFIG_COLOR_FRAME(koneconfig_color_frame_new());
	priv->light_effects = KONECONFIG_LIGHT_EFFECTS_FRAME(koneconfig_light_effects_frame_new());

	gtk_box_pack_start(GTK_BOX(left), GTK_WIDGET(priv->colors), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(left), GTK_WIDGET(priv->cpi), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(left), GTK_WIDGET(priv->sensitivity), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(middle), GTK_WIDGET(priv->light_effects), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(right), GTK_WIDGET(priv->buttons), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(right), GTK_WIDGET(priv->polling_rate), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(right), GTK_WIDGET(priv->gamefile), TRUE, TRUE, 0);

	gtk_container_add(GTK_CONTAINER(content), left);
	gtk_container_add(GTK_CONTAINER(content), middle);
	gtk_container_add(GTK_CONTAINER(content), right);

	gtk_box_pack_start(GTK_BOX(profile_page), content, TRUE, TRUE, 0);

	gtk_widget_show_all(content);
	g_signal_connect(G_OBJECT(profile_page), "reset", G_CALLBACK(reset_cb), NULL);
}

static void koneconfig_profile_page_class_init(KoneconfigProfilePageClass *klass) {
	g_type_class_add_private(klass, sizeof(KoneconfigProfilePagePrivate));
}

void koneconfig_profile_page_set_rmp(KoneconfigProfilePage *profile_page, KoneRMP *rmp) {
	KoneconfigProfilePagePrivate *priv = profile_page->priv;
	if (priv->rmp)
		kone_rmp_free(priv->rmp);
	priv->rmp = kone_rmp_dup(rmp);
	set_from_rmp(profile_page);
}

KoneRMP *koneconfig_profile_page_get_rmp(KoneconfigProfilePage *profile_page) {
	KoneconfigProfilePagePrivate *priv = profile_page->priv;
	KoneRMP *rmp;
	rmp = kone_rmp_dup(priv->rmp);
	update_rmp(profile_page, rmp);
	return rmp;
}
