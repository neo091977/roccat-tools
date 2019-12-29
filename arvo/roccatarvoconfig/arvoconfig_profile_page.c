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

#include "arvoconfig_profile_page.h"
#include "arvoconfig_buttons_frame.h"
#include "roccat_gamefile_selector.h"
#include "arvoconfig_mode_selector.h"
#include "arvoconfig_key_mask_selector.h"

#define ARVOCONFIG_PROFILE_PAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ARVOCONFIG_PROFILE_PAGE_TYPE, ArvoconfigProfilePageClass))
#define IS_ARVOCONFIG_PROFILE_PAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ARVOCONFIG_PROFILE_PAGE_TYPE))
#define ARVOCONFIG_PROFILE_PAGE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ARVOCONFIG_PROFILE_PAGE_TYPE, ArvoconfigProfilePagePrivate))

typedef struct _ArvoconfigProfilePageClass ArvoconfigProfilePageClass;
typedef struct _ArvoconfigProfilePagePrivate ArvoconfigProfilePagePrivate;

struct _ArvoconfigProfilePage {
	RoccatProfilePage parent;
	ArvoconfigProfilePagePrivate *priv;
};

struct _ArvoconfigProfilePageClass {
	RoccatProfilePageClass parent_class;
};

struct _ArvoconfigProfilePagePrivate {
	ArvoRkp *rkp;
	ArvoconfigButtonsFrame *buttons;
	RoccatGamefileSelector *gamefiles;
	ArvoconfigModeSelector *mode;
	ArvoconfigKeyMaskSelector *key_mask;
};

G_DEFINE_TYPE(ArvoconfigProfilePage, arvoconfig_profile_page, ROCCAT_PROFILE_PAGE_TYPE);

GtkWidget *arvoconfig_profile_page_new(void) {
	return GTK_WIDGET(g_object_new(ARVOCONFIG_PROFILE_PAGE_TYPE, NULL));
}

static void set_from_rkp(ArvoconfigProfilePage *profile_page) {
	ArvoconfigProfilePagePrivate *priv = profile_page->priv;
	guint i;

	roccat_profile_page_set_name(ROCCAT_PROFILE_PAGE(profile_page), arvo_rkp_get_profile_name(priv->rkp));

	arvoconfig_buttons_frame_set_from_rkp(priv->buttons, priv->rkp);
	arvoconfig_key_mask_selector_set_from_rkp(priv->key_mask, priv->rkp);
	arvoconfig_mode_selector_set_from_rkp(priv->mode, priv->rkp);

	for (i = 0; i < ARVO_GAMEFILE_NUM; ++i)
		roccat_gamefile_selector_set_text(priv->gamefiles, i, arvo_rkp_get_game_file_name(priv->rkp, i));
}

static void update_rkp(ArvoconfigProfilePage *profile_page, ArvoRkp *rkp) {
	ArvoconfigProfilePagePrivate *priv = profile_page->priv;
	gchar *profile_name;
	guint i;

	profile_name = roccat_profile_page_get_name(ROCCAT_PROFILE_PAGE(profile_page));
	arvo_rkp_set_profile_name(rkp, profile_name);
	g_free(profile_name);

	arvoconfig_buttons_frame_update_rkp(priv->buttons, rkp);
	arvoconfig_key_mask_selector_update_rkp(priv->key_mask, rkp);
	arvoconfig_mode_selector_update_rkp(priv->mode, rkp);

	for (i = 0; i < ARVO_GAMEFILE_NUM; ++i)
		arvo_rkp_set_game_file_name(rkp, i, roccat_gamefile_selector_get_text(priv->gamefiles, i));
}

static void reset_cb(RoccatProfilePage *profile_page, gpointer user_data) {
	set_from_rkp(ARVOCONFIG_PROFILE_PAGE(profile_page));
}

static void arvoconfig_profile_page_init(ArvoconfigProfilePage *profile_page) {
	ArvoconfigProfilePagePrivate *priv = ARVOCONFIG_PROFILE_PAGE_GET_PRIVATE(profile_page);
	GtkWidget *content, *left, *right;

	profile_page->priv = priv;

	content = gtk_hbox_new(FALSE, 0);

	left = gtk_vbox_new(FALSE, 0);
	right = gtk_vbox_new(FALSE, 0);

	priv->buttons = ARVOCONFIG_BUTTONS_FRAME(arvoconfig_buttons_frame_new());
	priv->gamefiles = ROCCAT_GAMEFILE_SELECTOR(roccat_gamefile_selector_new(ARVO_GAMEFILE_NUM));
	priv->mode = ARVOCONFIG_MODE_SELECTOR(arvoconfig_mode_selector_new());
	priv->key_mask = ARVOCONFIG_KEY_MASK_SELECTOR(arvoconfig_key_mask_selector_new());

	gtk_box_pack_start(GTK_BOX(left), GTK_WIDGET(priv->buttons), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(left), GTK_WIDGET(priv->gamefiles), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(right), GTK_WIDGET(priv->mode), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(right), GTK_WIDGET(priv->key_mask), TRUE, TRUE, 0);

	gtk_container_add(GTK_CONTAINER(content), left);
	gtk_container_add(GTK_CONTAINER(content), right);

	gtk_box_pack_start(GTK_BOX(profile_page), content, TRUE, TRUE, 0);

	gtk_widget_show_all(content);
	g_signal_connect(G_OBJECT(profile_page), "reset", G_CALLBACK(reset_cb), NULL);
}

static void arvoconfig_profile_page_class_init(ArvoconfigProfilePageClass *klass) {
	g_type_class_add_private(klass, sizeof(ArvoconfigProfilePagePrivate));
}

void arvoconfig_profile_page_set_rkp(ArvoconfigProfilePage *profile_page, ArvoRkp *rkp) {
	ArvoconfigProfilePagePrivate *priv = profile_page->priv;
	if (priv->rkp)
		arvo_rkp_free(priv->rkp);
	priv->rkp = arvo_rkp_dup(rkp);
	set_from_rkp(profile_page);
}

ArvoRkp *arvoconfig_profile_page_get_rkp_orig(ArvoconfigProfilePage *profile_page) {
	return arvo_rkp_dup(profile_page->priv->rkp);
}

ArvoRkp *arvoconfig_profile_page_get_rkp(ArvoconfigProfilePage *profile_page) {
	ArvoRkp *rkp;
	rkp = arvoconfig_profile_page_get_rkp_orig(profile_page);
	update_rkp(profile_page, rkp);
	return rkp;
}
