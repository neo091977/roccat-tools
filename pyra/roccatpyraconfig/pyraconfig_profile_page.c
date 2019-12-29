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

#include "pyraconfig_profile_page.h"
#include "pyraconfig_buttons_frame.h"
#include "pyraconfig_cpi_selector.h"
#include "pyraconfig_light_frame.h"
#include "roccat_gamefile_selector.h"
#include "roccat_handedness_selector.h"
#include "i18n.h"
#include "pyraconfig_sensitivity_frame.h"

#define PYRACONFIG_PROFILE_PAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), PYRACONFIG_PROFILE_PAGE_TYPE, PyraconfigProfilePageClass))
#define IS_PYRACONFIG_PROFILE_PAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), PYRACONFIG_PROFILE_PAGE_TYPE))
#define PYRACONFIG_PROFILE_PAGE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), PYRACONFIG_PROFILE_PAGE_TYPE, PyraconfigProfilePagePrivate))

typedef struct _PyraconfigProfilePageClass PyraconfigProfilePageClass;
typedef struct _PyraconfigProfilePagePrivate PyraconfigProfilePagePrivate;

struct _PyraconfigProfilePage {
	RoccatProfilePage parent;
	PyraconfigProfilePagePrivate *priv;
};

struct _PyraconfigProfilePageClass {
	RoccatProfilePageClass parent_class;
};

struct _PyraconfigProfilePagePrivate {
	PyraRMP *rmp;
	PyraconfigButtonsFrame *buttons;
	PyraconfigCpiSelector *cpi;
	RoccatGamefileSelector *gamefiles;
	RoccatHandednessSelector *handedness;
	PyraconfigSensitivityFrame *sensitivity;
	PyraconfigLightFrame *light;
};

G_DEFINE_TYPE(PyraconfigProfilePage, pyraconfig_profile_page, ROCCAT_PROFILE_PAGE_TYPE);

GtkWidget *pyraconfig_profile_page_new(void) {
	return GTK_WIDGET(g_object_new(PYRACONFIG_PROFILE_PAGE_TYPE, NULL));
}

static guint roccat_handedness_to_pyra_orientation(guint handedness) {
	return (handedness == ROCCAT_HANDEDNESS_SELECTOR_LEFT) ?
		PYRA_ORIENTATION_LEFT : PYRA_ORIENTATION_RIGHT;
}

static guint pyra_orientation_to_roccat_handedness(guint orientation) {
	return (orientation == PYRA_ORIENTATION_LEFT) ?
		ROCCAT_HANDEDNESS_SELECTOR_LEFT : ROCCAT_HANDEDNESS_SELECTOR_RIGHT;
}

static void set_from_rmp(PyraconfigProfilePage *profile_page) {
	PyraconfigProfilePagePrivate *priv = profile_page->priv;
	guint i;

	roccat_profile_page_set_name(ROCCAT_PROFILE_PAGE(profile_page), pyra_rmp_get_profile_name(priv->rmp));

	pyraconfig_buttons_frame_set_from_rmp(priv->buttons, priv->rmp);
	pyraconfig_sensitivity_frame_set_from_rmp(priv->sensitivity, priv->rmp);
	pyraconfig_cpi_selector_set_from_rmp(priv->cpi, priv->rmp);
	pyraconfig_light_frame_set_from_rmp(priv->light, priv->rmp);
	roccat_handedness_selector_set_value(priv->handedness, pyra_orientation_to_roccat_handedness(pyra_rmp_get_orientation(priv->rmp)));

	for (i = 0; i < PYRA_GAMEFILE_NUM; ++i)
		roccat_gamefile_selector_set_text(priv->gamefiles, i, pyra_rmp_get_game_file_name(priv->rmp, i));
}

static void update_rmp(PyraconfigProfilePage *profile_page, PyraRMP *rmp) {
	PyraconfigProfilePagePrivate *priv = profile_page->priv;
	gchar *profile_name;
	guint i;

	profile_name = roccat_profile_page_get_name(ROCCAT_PROFILE_PAGE(profile_page));
	pyra_rmp_set_profile_name(rmp, profile_name);
	g_free(profile_name);

	pyraconfig_buttons_frame_update_rmp(priv->buttons, rmp);
	pyraconfig_sensitivity_frame_update_rmp(priv->sensitivity, rmp);
	pyraconfig_cpi_selector_update_rmp(priv->cpi, rmp);
	pyraconfig_light_frame_update_rmp(priv->light, rmp);
	pyra_rmp_set_orientation(rmp, roccat_handedness_to_pyra_orientation(roccat_handedness_selector_get_value(priv->handedness)));

	for (i = 0; i < PYRA_GAMEFILE_NUM; ++i)
		pyra_rmp_set_game_file_name(rmp, i, roccat_gamefile_selector_get_text(priv->gamefiles, i));
}

static void reset_cb(RoccatProfilePage *profile_page, gpointer user_data) {
	set_from_rmp(PYRACONFIG_PROFILE_PAGE(profile_page));
}

static void pyraconfig_profile_page_init(PyraconfigProfilePage *profile_page) {
	PyraconfigProfilePagePrivate *priv = PYRACONFIG_PROFILE_PAGE_GET_PRIVATE(profile_page);
	GtkWidget *content;
	GtkWidget *left;
	GtkWidget *right;

	profile_page->priv = priv;

	content = gtk_hbox_new(FALSE, 0);
	left = gtk_vbox_new(FALSE, 0);
	right = gtk_vbox_new(FALSE, 0);
	priv->buttons = PYRACONFIG_BUTTONS_FRAME(pyraconfig_buttons_frame_new());
	priv->gamefiles = ROCCAT_GAMEFILE_SELECTOR(roccat_gamefile_selector_new(PYRA_GAMEFILE_NUM));
	priv->handedness = ROCCAT_HANDEDNESS_SELECTOR(roccat_handedness_selector_new());
	priv->cpi = PYRACONFIG_CPI_SELECTOR(pyraconfig_cpi_selector_new());
	priv->sensitivity = PYRACONFIG_SENSITIVITY_FRAME(pyraconfig_sensitivity_frame_new());
	priv->light = PYRACONFIG_LIGHT_FRAME(pyraconfig_light_frame_new());

	gtk_box_pack_start(GTK_BOX(left), GTK_WIDGET(priv->light), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(left), GTK_WIDGET(priv->cpi), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(left), GTK_WIDGET(priv->sensitivity), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(left), GTK_WIDGET(priv->handedness), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(right), GTK_WIDGET(priv->buttons), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(right), GTK_WIDGET(priv->gamefiles), TRUE, TRUE, 0);

	gtk_container_add(GTK_CONTAINER(content), left);
	gtk_container_add(GTK_CONTAINER(content), right);

	gtk_box_pack_start(GTK_BOX(profile_page), content, TRUE, TRUE, 0);

	gtk_widget_show_all(content);
	g_signal_connect(G_OBJECT(profile_page), "reset", G_CALLBACK(reset_cb), NULL);
}

static void pyraconfig_profile_page_class_init(PyraconfigProfilePageClass *klass) {
	g_type_class_add_private(klass, sizeof(PyraconfigProfilePagePrivate));
}

void pyraconfig_profile_page_set_rmp(PyraconfigProfilePage *profile_page, PyraRMP *rmp) {
	PyraconfigProfilePagePrivate *priv = profile_page->priv;
	if (priv->rmp)
		pyra_rmp_free(priv->rmp);
	priv->rmp = pyra_rmp_dup(rmp);
	set_from_rmp(profile_page);
}

PyraRMP *pyraconfig_profile_page_get_rmp(PyraconfigProfilePage *profile_page) {
	PyraconfigProfilePagePrivate *priv = profile_page->priv;
	PyraRMP *rmp;
	rmp = pyra_rmp_dup(priv->rmp);
	update_rmp(profile_page, rmp);
	return rmp;
}
