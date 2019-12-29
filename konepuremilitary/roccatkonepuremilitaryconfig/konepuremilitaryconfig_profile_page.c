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

#include "konepuremilitaryconfig_profile_page.h"
#include "konepuremilitary_anglesnap_frame.h"
#include "konepuremilitary_sensor_alignment_frame.h"
#include "konepure_buttons_frame.h"
#include "konepure_light_effects_frame.h"
#include "konepure_color_frame.h"
#include "konepuremilitary_cpi_selector.h"
#include "roccat_sensitivity_frame.h"
#include "roccat_polling_rate_frame.h"
#include "roccat_gamefile_selector.h"
#include "i18n.h"

#define KONEPUREMILITARYCONFIG_PROFILE_PAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KONEPUREMILITARYCONFIG_PROFILE_PAGE_TYPE, KonepuremilitaryconfigProfilePageClass))
#define IS_KONEPUREMILITARYCONFIG_PROFILE_PAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KONEPUREMILITARYCONFIG_PROFILE_PAGE_TYPE))
#define KONEPUREMILITARYCONFIG_PROFILE_PAGE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KONEPUREMILITARYCONFIG_PROFILE_PAGE_TYPE, KonepuremilitaryconfigProfilePagePrivate))

typedef struct _KonepuremilitaryconfigProfilePageClass KonepuremilitaryconfigProfilePageClass;
typedef struct _KonepuremilitaryconfigProfilePagePrivate KonepuremilitaryconfigProfilePagePrivate;

struct _KonepuremilitaryconfigProfilePage {
	RoccatProfilePage parent;
	KonepuremilitaryconfigProfilePagePrivate *priv;
};

struct _KonepuremilitaryconfigProfilePageClass {
	RoccatProfilePageClass parent_class;
};

struct _KonepuremilitaryconfigProfilePagePrivate {
	KoneplusRmp *rmp;
	KonepureButtonsFrame *buttons;
	RoccatGamefileSelector *gamefiles;
	RoccatPollingRateFrame *polling_rate;
	KonepuremilitaryCpiSelector *cpi;
	RoccatSensitivityFrame *sensitivity;
	KonepureLightEffectsFrame *light_effects;
	KonepureColorFrame *color;
	KonepuremilitaryAnglesnapFrame *anglesnap;
	KonepuremilitarySensorAlignmentFrame *sensoralign;
};

G_DEFINE_TYPE(KonepuremilitaryconfigProfilePage, konepuremilitaryconfig_profile_page, ROCCAT_PROFILE_PAGE_TYPE);

GtkWidget *konepuremilitaryconfig_profile_page_new(void) {
	return GTK_WIDGET(g_object_new(KONEPUREMILITARYCONFIG_PROFILE_PAGE_TYPE, NULL));
}

static void set_from_rmp(KonepuremilitaryconfigProfilePage *profile_page) {
	KonepuremilitaryconfigProfilePagePrivate *priv = profile_page->priv;
	guint i;

	roccat_profile_page_set_name(ROCCAT_PROFILE_PAGE(profile_page), koneplus_rmp_get_profile_name(priv->rmp));

	konepure_buttons_frame_set_from_rmp(priv->buttons, priv->rmp);
	konepure_light_effects_frame_set_from_rmp(priv->light_effects, priv->rmp);
	konepure_color_frame_set_from_rmp(priv->color, priv->rmp);
	konepuremilitary_cpi_selector_set_from_rmp(priv->cpi, priv->rmp);
	roccat_polling_rate_frame_set_value(priv->polling_rate, koneplus_rmp_get_polling_rate(priv->rmp));
	konepuremilitary_anglesnap_frame_set_from_rmp(priv->anglesnap, priv->rmp);
	konepuremilitary_sensor_alignment_frame_set_from_rmp(priv->sensoralign, priv->rmp);

	roccat_sensitivity_frame_set_advanced(priv->sensitivity, koneplus_rmp_get_xy_synchronous(priv->rmp));
	if (koneplus_rmp_get_xy_synchronous(priv->rmp) == ROCCAT_SENSITIVITY_ADVANCED_ON) {
		roccat_sensitivity_frame_set_x_value(priv->sensitivity, koneplus_rmp_get_sensitivity_x(priv->rmp));
		roccat_sensitivity_frame_set_y_value(priv->sensitivity, koneplus_rmp_get_sensitivity_y(priv->rmp));
	} else
		roccat_sensitivity_frame_set_x_value(priv->sensitivity, koneplus_rmp_get_sensitivity(priv->rmp));

	for (i = 0; i < KONEPLUS_GAMEFILE_NUM; ++i)
		roccat_gamefile_selector_set_text(priv->gamefiles, i, koneplus_rmp_get_game_file_name(priv->rmp, i));
}

static void update_rmp(KonepuremilitaryconfigProfilePage *profile_page, KoneplusRmp *rmp) {
	KonepuremilitaryconfigProfilePagePrivate *priv = profile_page->priv;
	gchar *profile_name;
	guint i;

	profile_name = roccat_profile_page_get_name(ROCCAT_PROFILE_PAGE(profile_page));
	koneplus_rmp_set_profile_name(rmp, profile_name);
	g_free(profile_name);

	konepure_buttons_frame_update_rmp(priv->buttons, rmp);
	konepure_light_effects_frame_update_rmp(priv->light_effects, rmp);
	konepure_color_frame_update_rmp(priv->color, rmp);
	konepuremilitary_cpi_selector_update_rmp(priv->cpi, rmp);
	koneplus_rmp_set_polling_rate(rmp, roccat_polling_rate_frame_get_value(priv->polling_rate));
	konepuremilitary_anglesnap_frame_update_rmp(priv->anglesnap, rmp);
	konepuremilitary_sensor_alignment_frame_update_rmp(priv->sensoralign, rmp);

	koneplus_rmp_set_xy_synchronous(rmp, roccat_sensitivity_frame_get_advanced(priv->sensitivity));
	if (roccat_sensitivity_frame_get_advanced(priv->sensitivity) == ROCCAT_SENSITIVITY_ADVANCED_ON) {
		koneplus_rmp_set_sensitivity_x(rmp, roccat_sensitivity_frame_get_x_value(priv->sensitivity));
		koneplus_rmp_set_sensitivity_y(rmp, roccat_sensitivity_frame_get_y_value(priv->sensitivity));
	} else
		koneplus_rmp_set_sensitivity(rmp, roccat_sensitivity_frame_get_x_value(priv->sensitivity));

	for (i = 0; i < KONEPLUS_GAMEFILE_NUM; ++i)
		koneplus_rmp_set_game_file_name(rmp, i, roccat_gamefile_selector_get_text(priv->gamefiles, i));
}

static void reset_cb(RoccatProfilePage *profile_page, gpointer user_data) {
	set_from_rmp(KONEPUREMILITARYCONFIG_PROFILE_PAGE(profile_page));
}

static void konepuremilitaryconfig_profile_page_init(KonepuremilitaryconfigProfilePage *profile_page) {
	KonepuremilitaryconfigProfilePagePrivate *priv = KONEPUREMILITARYCONFIG_PROFILE_PAGE_GET_PRIVATE(profile_page);
	GtkWidget *content;
	GtkWidget *left;
	GtkWidget *light;
	GtkWidget *right;

	profile_page->priv = priv;

	content = gtk_hbox_new(FALSE, 0);
	left = gtk_vbox_new(FALSE, 0);
	light = gtk_hbox_new(FALSE, 0);
	right = gtk_vbox_new(FALSE, 0);
	priv->buttons = KONEPURE_BUTTONS_FRAME(konepure_buttons_frame_new());
	priv->gamefiles = ROCCAT_GAMEFILE_SELECTOR(roccat_gamefile_selector_new(KONEPLUS_GAMEFILE_NUM));
	priv->polling_rate = ROCCAT_POLLING_RATE_FRAME(roccat_polling_rate_frame_new());
	priv->cpi = KONEPUREMILITARY_CPI_SELECTOR(konepuremilitary_cpi_selector_new(
			KONEPUREMILITARY_CPI_MIN, KONEPUREMILITARY_CPI_MAX, KONEPUREMILITARY_CPI_STEP));
	priv->sensitivity = ROCCAT_SENSITIVITY_FRAME(roccat_sensitivity_frame_new());
	priv->light_effects = KONEPURE_LIGHT_EFFECTS_FRAME(konepure_light_effects_frame_new());
	priv->color = KONEPURE_COLOR_FRAME(konepure_color_frame_new());
	priv->anglesnap = KONEPUREMILITARY_ANGLESNAP_FRAME(konepuremilitary_anglesnap_frame_new());
	priv->sensoralign = KONEPUREMILITARY_SENSOR_ALIGNMENT_FRAME(konepuremilitary_sensor_alignment_frame_new());

	gtk_box_pack_start(GTK_BOX(light), GTK_WIDGET(priv->color), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(light), GTK_WIDGET(priv->light_effects), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(right), GTK_WIDGET(priv->buttons), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(right), GTK_WIDGET(priv->sensitivity), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(right), GTK_WIDGET(priv->anglesnap), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(right), GTK_WIDGET(priv->sensoralign), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(right), GTK_WIDGET(priv->gamefiles), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(left), light, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(left), GTK_WIDGET(priv->cpi), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(left), GTK_WIDGET(priv->polling_rate), TRUE, TRUE, 0);

	gtk_container_add(GTK_CONTAINER(content), left);
	gtk_container_add(GTK_CONTAINER(content), right);

	gtk_box_pack_start(GTK_BOX(profile_page), content, TRUE, TRUE, 0);

	gtk_widget_show_all(content);
	g_signal_connect(G_OBJECT(profile_page), "reset", G_CALLBACK(reset_cb), NULL);
}

static void konepuremilitaryconfig_profile_page_class_init(KonepuremilitaryconfigProfilePageClass *klass) {
	g_type_class_add_private(klass, sizeof(KonepuremilitaryconfigProfilePagePrivate));
}

void konepuremilitaryconfig_profile_page_set_rmp(KonepuremilitaryconfigProfilePage *profile_page, KoneplusRmp *rmp) {
	KonepuremilitaryconfigProfilePagePrivate *priv = profile_page->priv;
	if (priv->rmp)
		koneplus_rmp_free(priv->rmp);
	priv->rmp = koneplus_rmp_dup(rmp);
	set_from_rmp(profile_page);
}

KoneplusRmp *konepuremilitaryconfig_profile_page_get_rmp(KonepuremilitaryconfigProfilePage *profile_page) {
	KonepuremilitaryconfigProfilePagePrivate *priv = profile_page->priv;
	KoneplusRmp *rmp;
	rmp = koneplus_rmp_dup(priv->rmp);
	update_rmp(profile_page, rmp);
	return rmp;
}
