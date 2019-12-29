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

#include "kovaplusconfig_profile_page.h"
#include "kovaplusconfig_buttons_frame.h"
#include "kovaplusconfig_cpi_selector.h"
#include "kovaplusconfig_sensitivity_frame.h"
#include "kovaplusconfig_light_frame.h"
#include "roccat_polling_rate_frame.h"
#include "roccat_handedness_selector.h"
#include "roccat_gamefile_selector.h"
#include "i18n.h"

#define KOVAPLUSCONFIG_PROFILE_PAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KOVAPLUSCONFIG_PROFILE_PAGE_TYPE, KovaplusconfigProfilePageClass))
#define IS_KOVAPLUSCONFIG_PROFILE_PAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KOVAPLUSCONFIG_PROFILE_PAGE_TYPE))
#define KOVAPLUSCONFIG_PROFILE_PAGE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KOVAPLUSCONFIG_PROFILE_PAGE_TYPE, KovaplusconfigProfilePagePrivate))

typedef struct _KovaplusconfigProfilePageClass KovaplusconfigProfilePageClass;
typedef struct _KovaplusconfigProfilePagePrivate KovaplusconfigProfilePagePrivate;

struct _KovaplusconfigProfilePage {
	RoccatProfilePage parent;
	KovaplusconfigProfilePagePrivate *priv;
};

struct _KovaplusconfigProfilePageClass {
	RoccatProfilePageClass parent_class;
};

struct _KovaplusconfigProfilePagePrivate {
	KovaplusRmp *rmp;
	KovaplusconfigButtonsFrame *buttons;
	RoccatGamefileSelector *gamefiles;
	RoccatPollingRateFrame *polling_rate;
	RoccatHandednessSelector *handedness;
	KovaplusconfigCpiSelector *cpi;
	KovaplusconfigSensitivityFrame *sensitivity;
	KovaplusconfigLightFrame *light;
};

G_DEFINE_TYPE(KovaplusconfigProfilePage, kovaplusconfig_profile_page, ROCCAT_PROFILE_PAGE_TYPE);

GtkWidget *kovaplusconfig_profile_page_new(void) {
	return GTK_WIDGET(g_object_new(KOVAPLUSCONFIG_PROFILE_PAGE_TYPE, NULL));
}

static guint roccat_handedness_to_kovaplus_orientation(guint handedness) {
	return (handedness == ROCCAT_HANDEDNESS_SELECTOR_LEFT) ?
		KOVAPLUS_PROFILE_SETTINGS_ORIENTATION_LEFT : KOVAPLUS_PROFILE_SETTINGS_ORIENTATION_RIGHT;
}

static guint kovaplus_orientation_to_roccat_handedness(guint orientation) {
	return (orientation == KOVAPLUS_PROFILE_SETTINGS_ORIENTATION_LEFT) ?
		ROCCAT_HANDEDNESS_SELECTOR_LEFT : ROCCAT_HANDEDNESS_SELECTOR_RIGHT;
}

static void set_from_rmp(KovaplusconfigProfilePage *profile_page) {
	KovaplusconfigProfilePagePrivate *priv = profile_page->priv;
	guint i;

	roccat_profile_page_set_name(ROCCAT_PROFILE_PAGE(profile_page), kovaplus_rmp_get_profile_name(priv->rmp));

	kovaplusconfig_buttons_frame_set_from_rmp(priv->buttons, priv->rmp);
	kovaplusconfig_sensitivity_frame_set_from_rmp(priv->sensitivity, priv->rmp);
	kovaplusconfig_cpi_selector_set_from_rmp(priv->cpi, priv->rmp);
	kovaplusconfig_light_frame_set_from_rmp(priv->light, priv->rmp);
	roccat_polling_rate_frame_set_value(priv->polling_rate, kovaplus_rmp_get_polling_rate(priv->rmp));

	roccat_handedness_selector_set_value(priv->handedness, kovaplus_orientation_to_roccat_handedness(kovaplus_rmp_get_orientation(priv->rmp)));

	for (i = 0; i < KOVAPLUS_GAMEFILE_NUM; ++i)
		roccat_gamefile_selector_set_text(priv->gamefiles, i, kovaplus_rmp_get_game_file_name(priv->rmp, i));
}

static void update_rmp(KovaplusconfigProfilePage *profile_page, KovaplusRmp *rmp) {
	KovaplusconfigProfilePagePrivate *priv = profile_page->priv;
	gchar *profile_name;
	guint i;

	profile_name = roccat_profile_page_get_name(ROCCAT_PROFILE_PAGE(profile_page));
	kovaplus_rmp_set_profile_name(rmp, profile_name);
	g_free(profile_name);

	kovaplusconfig_buttons_frame_update_rmp(priv->buttons, rmp);
	kovaplusconfig_sensitivity_frame_update_rmp(priv->sensitivity, rmp);
	kovaplusconfig_cpi_selector_update_rmp(priv->cpi, rmp);
	kovaplusconfig_light_frame_update_rmp(priv->light, rmp);
	kovaplus_rmp_set_polling_rate(rmp, roccat_polling_rate_frame_get_value(priv->polling_rate));

	kovaplus_rmp_set_orientation(rmp, roccat_handedness_to_kovaplus_orientation(roccat_handedness_selector_get_value(priv->handedness)));

	for (i = 0; i < KOVAPLUS_GAMEFILE_NUM; ++i)
		kovaplus_rmp_set_game_file_name(rmp, i, roccat_gamefile_selector_get_text(priv->gamefiles, i));
}

static void reset_cb(RoccatProfilePage *profile_page, gpointer user_data) {
	set_from_rmp(KOVAPLUSCONFIG_PROFILE_PAGE(profile_page));
}

static void kovaplusconfig_profile_page_init(KovaplusconfigProfilePage *profile_page) {
	KovaplusconfigProfilePagePrivate *priv = KOVAPLUSCONFIG_PROFILE_PAGE_GET_PRIVATE(profile_page);
	GtkWidget *content;
	GtkWidget *left;
	GtkWidget *right;

	profile_page->priv = priv;

	content = gtk_hbox_new(FALSE, 0);
	left = gtk_vbox_new(FALSE, 0);
	right = gtk_vbox_new(FALSE, 0);
	priv->buttons = KOVAPLUSCONFIG_BUTTONS_FRAME(kovaplusconfig_buttons_frame_new());
	priv->gamefiles = ROCCAT_GAMEFILE_SELECTOR(roccat_gamefile_selector_new(KOVAPLUS_GAMEFILE_NUM));
	priv->polling_rate = ROCCAT_POLLING_RATE_FRAME(roccat_polling_rate_frame_new());
	priv->handedness = ROCCAT_HANDEDNESS_SELECTOR(roccat_handedness_selector_new());
	priv->cpi = KOVAPLUSCONFIG_CPI_SELECTOR(kovaplusconfig_cpi_selector_new());
	priv->sensitivity = KOVAPLUSCONFIG_SENSITIVITY_FRAME(kovaplusconfig_sensitivity_frame_new());
	priv->light = KOVAPLUSCONFIG_LIGHT_FRAME(kovaplusconfig_light_frame_new());

	gtk_box_pack_start(GTK_BOX(right), GTK_WIDGET(priv->buttons), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(right), GTK_WIDGET(priv->sensitivity), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(right), GTK_WIDGET(priv->gamefiles), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(left), GTK_WIDGET(priv->polling_rate), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(left), GTK_WIDGET(priv->light), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(left), GTK_WIDGET(priv->handedness), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(left), GTK_WIDGET(priv->cpi), TRUE, TRUE, 0);

	gtk_container_add(GTK_CONTAINER(content), left);
	gtk_container_add(GTK_CONTAINER(content), right);

	gtk_box_pack_start(GTK_BOX(profile_page), content, TRUE, TRUE, 0);

	gtk_widget_show_all(content);
	g_signal_connect(G_OBJECT(profile_page), "reset", G_CALLBACK(reset_cb), NULL);
}

static void kovaplusconfig_profile_page_class_init(KovaplusconfigProfilePageClass *klass) {
	g_type_class_add_private(klass, sizeof(KovaplusconfigProfilePagePrivate));
}

void kovaplusconfig_profile_page_set_rmp(KovaplusconfigProfilePage *profile_page, KovaplusRmp *rmp) {
	KovaplusconfigProfilePagePrivate *priv = profile_page->priv;
	if (priv->rmp)
		kovaplus_rmp_free(priv->rmp);
	priv->rmp = kovaplus_rmp_dup(rmp);
	set_from_rmp(profile_page);
}

KovaplusRmp *kovaplusconfig_profile_page_get_rmp(KovaplusconfigProfilePage *profile_page) {
	KovaplusconfigProfilePagePrivate *priv = profile_page->priv;
	KovaplusRmp *rmp;
	rmp = kovaplus_rmp_dup(priv->rmp);
	update_rmp(profile_page, rmp);
	return rmp;
}
