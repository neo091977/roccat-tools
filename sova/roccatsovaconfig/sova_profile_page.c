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

#include "sova_profile_page.h"
#include "sova_keyboard_selector.h"
#include "sova_key_mask_selector.h"
#include "sova_light_effects_frame.h"
#include "sova_light_event_effect_frame.h"
#include "sova_notification_frame.h"
#include "roccat_gamefile_selector.h"
#include "roccat_swarm_rmp.h"
#include "g_cclosure_roccat_marshaller.h"
#include "i18n.h"

#define SOVA_PROFILE_PAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), SOVA_PROFILE_PAGE_TYPE, SovaProfilePageClass))
#define IS_SOVA_PROFILE_PAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), SOVA_PROFILE_PAGE_TYPE))
#define SOVA_PROFILE_PAGE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), SOVA_PROFILE_PAGE_TYPE, SovaProfilePagePrivate))

typedef struct _SovaProfilePageClass SovaProfilePageClass;
typedef struct _SovaProfilePagePrivate SovaProfilePagePrivate;

struct _SovaProfilePage {
	RoccatProfilePage parent;
	SovaProfilePagePrivate *priv;
};

struct _SovaProfilePageClass {
	RoccatProfilePageClass parent_class;
};

struct _SovaProfilePagePrivate {
	SovaProfileData *profile_data;
	GtkBox *misc_box;
	RoccatGamefileSelector *gamefiles;
	SovaKeyboardSelector *keyboard_selector;
	SovaKeyMaskSelector *key_mask_selector;
	SovaLightEventEffectFrame *light_event_effect_frame;
	SovaLightEffectsFrame *light_effects_frame;
	SovaNotificationFrame *notification_frame;
};

G_DEFINE_TYPE(SovaProfilePage, sova_profile_page, ROCCAT_PROFILE_PAGE_TYPE);

GtkWidget *sova_profile_page_new(void) {
	return GTK_WIDGET(g_object_new(SOVA_PROFILE_PAGE_TYPE, NULL));
}

static void set_from_profile_data(SovaProfilePage *profile_page, RoccatKeyFile *config) {
	SovaProfilePagePrivate *priv = profile_page->priv;
	guint i;

	roccat_profile_page_set_name(ROCCAT_PROFILE_PAGE(profile_page), gaminggear_profile_data_get_name(GAMINGGEAR_PROFILE_DATA(priv->profile_data)));
	sova_keyboard_selector_set_from_profile_data(priv->keyboard_selector, priv->profile_data);
	sova_key_mask_selector_set_from_profile_data(priv->key_mask_selector, priv->profile_data);
	sova_light_event_effect_frame_set_from_profile_data(priv->light_event_effect_frame, priv->profile_data);
	sova_light_effects_frame_set_from_profile_data(priv->light_effects_frame, priv->profile_data);
	sova_notification_frame_set_from_profile_data(priv->notification_frame, priv->profile_data);
	
	for (i = 0; i < GAMINGGEAR_PROFILE_DATA_GAMEFILE_NUM; ++i)
		roccat_gamefile_selector_set_text(priv->gamefiles, i, gaminggear_profile_data_get_gamefile(GAMINGGEAR_PROFILE_DATA(priv->profile_data), i));
}

static void update_profile_data(SovaProfilePage *profile_page, RoccatKeyFile *config) {
	SovaProfilePagePrivate *priv = profile_page->priv;
	gchar *profile_name;
	guint i;

	profile_name = roccat_profile_page_get_name(ROCCAT_PROFILE_PAGE(profile_page));
	gaminggear_profile_data_set_name(GAMINGGEAR_PROFILE_DATA(priv->profile_data), profile_name);
	g_free(profile_name);

	sova_keyboard_selector_update_profile_data(priv->keyboard_selector, priv->profile_data);
	sova_key_mask_selector_update_profile_data(priv->key_mask_selector, priv->profile_data);
	sova_light_event_effect_frame_update_profile_data(priv->light_event_effect_frame, priv->profile_data);
	sova_light_effects_frame_update_profile_data(priv->light_effects_frame, priv->profile_data);
	sova_notification_frame_update_profile_data(priv->notification_frame, priv->profile_data);
	
	for (i = 0; i < GAMINGGEAR_PROFILE_DATA_GAMEFILE_NUM; ++i)
		gaminggear_profile_data_set_gamefile(GAMINGGEAR_PROFILE_DATA(priv->profile_data), i, roccat_gamefile_selector_get_text(priv->gamefiles, i));
}

static void reset_cb(RoccatProfilePage *profile_page, gpointer user_data) {
	SovaProfilePage *sova_profile_page = SOVA_PROFILE_PAGE(profile_page);
	gaminggear_profile_data_reset(GAMINGGEAR_PROFILE_DATA(sova_profile_page->priv->profile_data));
	set_from_profile_data(sova_profile_page, NULL);
}

static void append_keys_page(SovaProfilePage *profile_page, GtkNotebook *notebook) {
	SovaProfilePagePrivate *priv = profile_page->priv;
	GtkWidget *vbox;

	vbox = gtk_vbox_new(FALSE, 0);
	priv->keyboard_selector = SOVA_KEYBOARD_SELECTOR(sova_keyboard_selector_new());

	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(priv->keyboard_selector), TRUE, TRUE, 0);
	gtk_notebook_append_page(notebook, vbox, gtk_label_new(_("Keys")));
}

static void append_misc_page(SovaProfilePage *profile_page, GtkNotebook *notebook) {
	SovaProfilePagePrivate *priv = profile_page->priv;
	GtkWidget *hbox1;
	GtkWidget *hbox2;

	hbox1 = gtk_hbox_new(FALSE, 0);
	hbox2 = gtk_hbox_new(FALSE, 0);

	priv->misc_box = GTK_BOX(gtk_vbox_new(FALSE, 0));
	priv->gamefiles = ROCCAT_GAMEFILE_SELECTOR(roccat_gamefile_selector_new(GAMINGGEAR_PROFILE_DATA_GAMEFILE_NUM));
	priv->key_mask_selector = SOVA_KEY_MASK_SELECTOR(sova_key_mask_selector_new());
	priv->light_event_effect_frame = SOVA_LIGHT_EVENT_EFFECT_FRAME(sova_light_event_effect_frame_new());
	priv->light_effects_frame = SOVA_LIGHT_EFFECTS_FRAME(sova_light_effects_frame_new());
	priv->notification_frame = SOVA_NOTIFICATION_FRAME(sova_notification_frame_new());
	
	gtk_box_pack_start(GTK_BOX(hbox1), GTK_WIDGET(priv->notification_frame), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox1), GTK_WIDGET(priv->light_event_effect_frame), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox1), GTK_WIDGET(priv->light_effects_frame), TRUE, TRUE, 0);

	gtk_box_pack_start(GTK_BOX(hbox2), GTK_WIDGET(priv->key_mask_selector), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox2), GTK_WIDGET(priv->gamefiles), TRUE, TRUE, 0);

	gtk_box_pack_start(priv->misc_box, hbox1, TRUE, TRUE, 0);
	gtk_box_pack_start(priv->misc_box, hbox2, TRUE, TRUE, 0);

	gtk_notebook_append_page(notebook, GTK_WIDGET(priv->misc_box), gtk_label_new(_("Misc")));
}

static void sova_profile_page_init(SovaProfilePage *profile_page) {
	SovaProfilePagePrivate *priv = SOVA_PROFILE_PAGE_GET_PRIVATE(profile_page);
	GtkWidget *notebook;

	profile_page->priv = priv;

	priv->profile_data = NULL;

	notebook = gtk_notebook_new();

	append_keys_page(profile_page, GTK_NOTEBOOK(notebook));
	append_misc_page(profile_page, GTK_NOTEBOOK(notebook));

	gtk_box_pack_start(GTK_BOX(profile_page), notebook, TRUE, TRUE, 0);

	gtk_widget_show_all(notebook);
	g_signal_connect(G_OBJECT(profile_page), "reset", G_CALLBACK(reset_cb), NULL);
}

static void finalize(GObject *object) {
	SovaProfilePagePrivate *priv = SOVA_PROFILE_PAGE(object)->priv;

	if (priv->profile_data)
		g_object_unref(priv->profile_data);

	G_OBJECT_CLASS(sova_profile_page_parent_class)->finalize(object);
}

static void sova_profile_page_class_init(SovaProfilePageClass *klass) {
	GObjectClass *gobject_class = (GObjectClass*)klass;

	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(SovaProfilePagePrivate));
}

void sova_profile_page_set_profile_data(SovaProfilePage *profile_page, SovaProfileData *profile_data, RoccatKeyFile *config) {
	SovaProfilePagePrivate *priv = profile_page->priv;
	if (priv->profile_data)
		g_object_unref(priv->profile_data);

	priv->profile_data = profile_data;
	g_object_ref(priv->profile_data);
	set_from_profile_data(profile_page, config);
}

SovaProfileData *sova_profile_page_get_profile_data(SovaProfilePage *profile_page, RoccatKeyFile *config) {
	SovaProfilePagePrivate *priv = profile_page->priv;
	update_profile_data(profile_page, config);
	g_object_ref(priv->profile_data);
	return priv->profile_data;
}

void sova_profile_page_set_keyboard_layout(SovaProfilePage *profile_page, gchar const *layout) {
	sova_keyboard_selector_set_layout(profile_page->priv->keyboard_selector, layout);
}

void sova_profile_page_update_brightness(SovaProfilePage *profile_page, guint new_brightness) {
	SovaProfilePagePrivate *priv = profile_page->priv;
	sova_profile_data_set_brightness_from_hardware(priv->profile_data, new_brightness);
	sova_light_effects_frame_set_brightness(priv->light_effects_frame, new_brightness);
}

void sova_profile_page_update_key(SovaProfilePage *profile_page, guint macro_index, SovaMacro *macro) {
	SovaProfilePagePrivate *priv = profile_page->priv;

	sova_profile_data_set_macro_from_hardware(priv->profile_data, macro_index, macro);
	sova_keyboard_selector_set_macro(priv->keyboard_selector, macro_index, macro);
}
