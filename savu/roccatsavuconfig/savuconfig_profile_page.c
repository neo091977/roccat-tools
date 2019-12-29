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

#include "savuconfig_profile_page.h"
#include "savuconfig_buttons_frame.h"
#include "savuconfig_cpi_selector.h"
#include "savuconfig_light_frame.h"
#include "roccat_sensitivity_frame.h"
#include "roccat_polling_rate_frame.h"
#include "roccat_gamefile_selector.h"
#include "roccat_cpi_fixed_selector.h"
#include "i18n.h"

#define SAVUCONFIG_PROFILE_PAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), SAVUCONFIG_PROFILE_PAGE_TYPE, SavuconfigProfilePageClass))
#define IS_SAVUCONFIG_PROFILE_PAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), SAVUCONFIG_PROFILE_PAGE_TYPE))
#define SAVUCONFIG_PROFILE_PAGE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), SAVUCONFIG_PROFILE_PAGE_TYPE, SavuconfigProfilePagePrivate))

typedef struct _SavuconfigProfilePageClass SavuconfigProfilePageClass;
typedef struct _SavuconfigProfilePagePrivate SavuconfigProfilePagePrivate;

struct _SavuconfigProfilePage {
	RoccatProfilePage parent;
	SavuconfigProfilePagePrivate *priv;
};

struct _SavuconfigProfilePageClass {
	RoccatProfilePageClass parent_class;
};

struct _SavuconfigProfilePagePrivate {
	SavuRmp *rmp;
	SavuconfigButtonsFrame *buttons;
	RoccatGamefileSelector *gamefiles;
	RoccatPollingRateFrame *polling_rate;
	SavuconfigCpiSelector *cpi;
	RoccatSensitivityFrame *sensitivity;
	SavuconfigLightFrame *light;
};

G_DEFINE_TYPE(SavuconfigProfilePage, savuconfig_profile_page, ROCCAT_PROFILE_PAGE_TYPE);

GtkWidget *savuconfig_profile_page_new(void) {
	return GTK_WIDGET(g_object_new(SAVUCONFIG_PROFILE_PAGE_TYPE, NULL));
}

static void set_from_rmp(SavuconfigProfilePage *profile_page) {
	SavuconfigProfilePagePrivate *priv = profile_page->priv;
	guint i;

	roccat_profile_page_set_name(ROCCAT_PROFILE_PAGE(profile_page), savu_rmp_get_profile_name(priv->rmp));

	savuconfig_buttons_frame_set_from_rmp(priv->buttons, priv->rmp);
	savuconfig_cpi_selector_set_from_rmp(priv->cpi, priv->rmp);
	savuconfig_light_frame_set_from_rmp(priv->light, priv->rmp);
	roccat_polling_rate_frame_set_value(priv->polling_rate, savu_rmp_get_polling_rate(priv->rmp));

	roccat_sensitivity_frame_set_x_value(priv->sensitivity, savu_rmp_get_sensitivity_x(priv->rmp));
	roccat_sensitivity_frame_set_y_value(priv->sensitivity, savu_rmp_get_sensitivity_y(priv->rmp));
	roccat_sensitivity_frame_set_advanced(priv->sensitivity, savu_rmp_get_xy_synchronous(priv->rmp));

	for (i = 0; i < SAVU_GAMEFILE_NUM; ++i)
		roccat_gamefile_selector_set_text(priv->gamefiles, i, savu_rmp_get_game_file(priv->rmp, i));
}

static void update_rmp(SavuconfigProfilePage *profile_page, SavuRmp *rmp) {
	SavuconfigProfilePagePrivate *priv = profile_page->priv;
	gchar *profile_name;
	guint i;

	profile_name = roccat_profile_page_get_name(ROCCAT_PROFILE_PAGE(profile_page));
	savu_rmp_set_profile_name(rmp, profile_name);
	g_free(profile_name);

	savuconfig_buttons_frame_update_rmp(priv->buttons, rmp);
	savuconfig_cpi_selector_update_rmp(priv->cpi, rmp);
	savuconfig_light_frame_update_rmp(priv->light, rmp);
	savu_rmp_set_polling_rate(rmp, roccat_polling_rate_frame_get_value(priv->polling_rate));

	savu_rmp_set_sensitivity_x(rmp, roccat_sensitivity_frame_get_x_value(priv->sensitivity));
	savu_rmp_set_sensitivity_y(rmp, roccat_sensitivity_frame_get_y_value(priv->sensitivity));
	savu_rmp_set_xy_synchronous(rmp, roccat_sensitivity_frame_get_advanced(priv->sensitivity));

	for (i = 0; i < SAVU_GAMEFILE_NUM; ++i)
		savu_rmp_set_game_file(rmp, i, roccat_gamefile_selector_get_text(priv->gamefiles, i));
}

static void reset_cb(RoccatProfilePage *profile_page, gpointer user_data) {
	set_from_rmp(SAVUCONFIG_PROFILE_PAGE(profile_page));
}

static void savuconfig_profile_page_init(SavuconfigProfilePage *profile_page) {
	SavuconfigProfilePagePrivate *priv = SAVUCONFIG_PROFILE_PAGE_GET_PRIVATE(profile_page);
	GtkWidget *content;
	GtkWidget *left;
	GtkWidget *right;

	profile_page->priv = priv;

	content = gtk_hbox_new(FALSE, 0);
	left = gtk_vbox_new(FALSE, 0);
	right = gtk_vbox_new(FALSE, 0);

	priv->buttons = SAVUCONFIG_BUTTONS_FRAME(savuconfig_buttons_frame_new());
	priv->cpi = SAVUCONFIG_CPI_SELECTOR(savuconfig_cpi_selector_new());
	priv->gamefiles = ROCCAT_GAMEFILE_SELECTOR(roccat_gamefile_selector_new(SAVU_GAMEFILE_NUM));
	priv->polling_rate = ROCCAT_POLLING_RATE_FRAME(roccat_polling_rate_frame_new());
	priv->sensitivity = ROCCAT_SENSITIVITY_FRAME(roccat_sensitivity_frame_new());
	priv->light = SAVUCONFIG_LIGHT_FRAME(savuconfig_light_frame_new());

	gtk_box_pack_start(GTK_BOX(left), GTK_WIDGET(priv->light), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(left), GTK_WIDGET(priv->cpi), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(left), GTK_WIDGET(priv->polling_rate), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(right), GTK_WIDGET(priv->buttons), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(right), GTK_WIDGET(priv->sensitivity), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(right), GTK_WIDGET(priv->gamefiles), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(content), left, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(content), right, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(profile_page), content, TRUE, TRUE, 0);

	gtk_widget_show_all(content);
	g_signal_connect(G_OBJECT(profile_page), "reset", G_CALLBACK(reset_cb), NULL);
}

static void savuconfig_profile_page_class_init(SavuconfigProfilePageClass *klass) {
	g_type_class_add_private(klass, sizeof(SavuconfigProfilePagePrivate));
}

void savuconfig_profile_page_set_rmp(SavuconfigProfilePage *profile_page, SavuRmp *rmp) {
	SavuconfigProfilePagePrivate *priv = profile_page->priv;
	if (priv->rmp)
		savu_rmp_free(priv->rmp);
	priv->rmp = savu_rmp_dup(rmp);
	set_from_rmp(profile_page);
}

SavuRmp *savuconfig_profile_page_get_rmp(SavuconfigProfilePage *profile_page) {
	SavuconfigProfilePagePrivate *priv = profile_page->priv;
	SavuRmp *rmp;
	rmp = savu_rmp_dup(priv->rmp);
	update_rmp(profile_page, rmp);
	return rmp;
}
