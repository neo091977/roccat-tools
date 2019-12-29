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

#include "suora_profile_page.h"
#include "suora_illumination_frame.h"
#include "suora_sound_feedback_frame.h"
#include "suora_keyboard_selector.h"
#include "roccat_gamefile_selector.h"
#include "i18n.h"

#define SUORA_PROFILE_PAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), SUORA_PROFILE_PAGE_TYPE, SuoraProfilePageClass))
#define IS_SUORA_PROFILE_PAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), SUORA_PROFILE_PAGE_TYPE))
#define SUORA_PROFILE_PAGE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), SUORA_PROFILE_PAGE_TYPE, SuoraProfilePagePrivate))

typedef struct _SuoraProfilePageClass SuoraProfilePageClass;
typedef struct _SuoraProfilePagePrivate SuoraProfilePagePrivate;

struct _SuoraProfilePage {
	GaminggearProfilePage parent;
	SuoraProfilePagePrivate *priv;
};

struct _SuoraProfilePageClass {
	GaminggearProfilePageClass parent_class;
};

struct _SuoraProfilePagePrivate {
	SuoraProfileData *profile_data;
	SuoraIlluminationFrame *illumination_frame;
	SuoraSoundFeedbackFrame *sound_feedback_frame;
	SuoraKeyboardSelector *keyboard_selector;
	RoccatGamefileSelector *gamefiles;
};

G_DEFINE_TYPE(SuoraProfilePage, suora_profile_page, GAMINGGEAR_PROFILE_PAGE_TYPE);

GtkWidget *suora_profile_page_new(void) {
	return GTK_WIDGET(g_object_new(SUORA_PROFILE_PAGE_TYPE, NULL));
}

static void append_keys_page(SuoraProfilePage *profile_page, GtkNotebook *notebook) {
	SuoraProfilePagePrivate *priv = profile_page->priv;
	GtkWidget *vbox;

	vbox = gtk_vbox_new(FALSE, 0);
	priv->keyboard_selector = SUORA_KEYBOARD_SELECTOR(suora_keyboard_selector_new());

	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(priv->keyboard_selector), TRUE, TRUE, 0);
	gtk_notebook_append_page(notebook, vbox, gtk_label_new(_("Keys")));
}

static void append_misc_page(SuoraProfilePage *profile_page, GtkNotebook *notebook) {
	SuoraProfilePagePrivate *priv = profile_page->priv;
	GtkWidget *hbox;
	GtkWidget *vbox;

	hbox = gtk_hbox_new(FALSE, 0);
	vbox = gtk_vbox_new(FALSE, 0);
	priv->illumination_frame = SUORA_ILLUMINATION_FRAME(suora_illumination_frame_new());
	priv->sound_feedback_frame = SUORA_SOUND_FEEDBACK_FRAME(suora_sound_feedback_frame_new());
	priv->gamefiles = ROCCAT_GAMEFILE_SELECTOR(roccat_gamefile_selector_new(GAMINGGEAR_PROFILE_DATA_GAMEFILE_NUM));

	gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(priv->illumination_frame), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(priv->sound_feedback_frame), TRUE, TRUE, 0);

	gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(priv->gamefiles), TRUE, TRUE, 0);

	gtk_notebook_append_page(notebook, vbox, gtk_label_new(_("Misc")));
}

static void suora_profile_page_init(SuoraProfilePage *profile_page) {
	SuoraProfilePagePrivate *priv = SUORA_PROFILE_PAGE_GET_PRIVATE(profile_page);
	GtkWidget *notebook;

	profile_page->priv = priv;

	priv->profile_data = NULL;

	notebook = gtk_notebook_new();

	append_keys_page(profile_page, GTK_NOTEBOOK(notebook));
	append_misc_page(profile_page, GTK_NOTEBOOK(notebook));

	gtk_box_pack_start(GTK_BOX(profile_page), notebook, TRUE, TRUE, 0);

	gtk_widget_show_all(notebook);
}

void set_from_data(GaminggearProfilePage *self, GaminggearProfileData *profile_data) {
	SuoraProfilePagePrivate *priv = SUORA_PROFILE_PAGE(self)->priv;
	SuoraProfileData *suora_data;
	guint i;

	if (!profile_data)
		return;

	suora_data = SUORA_PROFILE_DATA(profile_data);
	suora_illumination_frame_set_from_profile_data(priv->illumination_frame, suora_data);
	suora_sound_feedback_frame_set_from_profile_data(priv->sound_feedback_frame, suora_data);
	suora_keyboard_selector_set_from_profile_data(priv->keyboard_selector, suora_data);

	for (i = 0; i < GAMINGGEAR_PROFILE_DATA_GAMEFILE_NUM; ++i)
		roccat_gamefile_selector_set_text(priv->gamefiles, i, gaminggear_profile_data_get_gamefile(profile_data, i));
}

void update_data(GaminggearProfilePage *self, GaminggearProfileData *profile_data) {
	SuoraProfilePagePrivate *priv = SUORA_PROFILE_PAGE(self)->priv;
	SuoraProfileData *suora_data;
	guint i;

	if (!profile_data)
		return;

	suora_data = SUORA_PROFILE_DATA(profile_data);
	suora_illumination_frame_update_profile_data(priv->illumination_frame, suora_data);
	suora_sound_feedback_frame_update_profile_data(priv->sound_feedback_frame, suora_data);
	suora_keyboard_selector_update_profile_data(priv->keyboard_selector, suora_data);

	for (i = 0; i < GAMINGGEAR_PROFILE_DATA_GAMEFILE_NUM; ++i)
		gaminggear_profile_data_set_gamefile(profile_data, i, roccat_gamefile_selector_get_text(priv->gamefiles, i));
}

static void suora_profile_page_class_init(SuoraProfilePageClass *klass) {
	GaminggearProfilePageClass *parent_class = GAMINGGEAR_PROFILE_PAGE_CLASS(klass);

	parent_class->set_from_data = set_from_data;
	parent_class->update_data = update_data;

	g_type_class_add_private(klass, sizeof(SuoraProfilePagePrivate));
}

void suora_profile_page_set_keyboard_layout(SuoraProfilePage *profile_page, gchar const *layout) {
	suora_keyboard_selector_set_layout(profile_page->priv->keyboard_selector, layout);
}
