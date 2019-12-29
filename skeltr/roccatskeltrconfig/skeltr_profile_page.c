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

#include "skeltr_profile_page.h"
#include "skeltr_color_frame.h"
#include "skeltr_keyboard_selector.h"
#include "skeltr_key_mask_selector.h"
#include "skeltr_light_effects_frame.h"
#include "skeltr_light_event_effect_frame.h"
#include "skeltr_notification_frame.h"
#include "roccat_gamefile_selector.h"
#include "roccat_swarm_rmp.h"
#include "g_cclosure_roccat_marshaller.h"
#include "i18n.h"

#define SKELTR_PROFILE_PAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), SKELTR_PROFILE_PAGE_TYPE, SkeltrProfilePageClass))
#define IS_SKELTR_PROFILE_PAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), SKELTR_PROFILE_PAGE_TYPE))
#define SKELTR_PROFILE_PAGE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), SKELTR_PROFILE_PAGE_TYPE, SkeltrProfilePagePrivate))

typedef struct _SkeltrProfilePageClass SkeltrProfilePageClass;
typedef struct _SkeltrProfilePagePrivate SkeltrProfilePagePrivate;

struct _SkeltrProfilePage {
	RoccatProfilePage parent;
	SkeltrProfilePagePrivate *priv;
};

struct _SkeltrProfilePageClass {
	RoccatProfilePageClass parent_class;
};

struct _SkeltrProfilePagePrivate {
	SkeltrProfileData *profile_data;
	GtkBox *misc_box;
	RoccatGamefileSelector *gamefiles;
	SkeltrColorFrame *color_frame;
	SkeltrKeyboardSelector *keyboard_selector;
	SkeltrKeyMaskSelector *key_mask_selector;
	SkeltrNotificationFrame *notification_frame;
	SkeltrLightEffectsFrame *light_effects_frame;
	SkeltrLightEventEffectFrame *light_event_effect_frame;
};

G_DEFINE_TYPE(SkeltrProfilePage, skeltr_profile_page, ROCCAT_PROFILE_PAGE_TYPE);

GtkWidget *skeltr_profile_page_new(void) {
	return GTK_WIDGET(g_object_new(SKELTR_PROFILE_PAGE_TYPE, NULL));
}

static void set_from_profile_data(SkeltrProfilePage *profile_page, RoccatKeyFile *config) {
	SkeltrProfilePagePrivate *priv = profile_page->priv;
	guint i;

	roccat_profile_page_set_name(ROCCAT_PROFILE_PAGE(profile_page), gaminggear_profile_data_get_name(GAMINGGEAR_PROFILE_DATA(priv->profile_data)));
	skeltr_keyboard_selector_set_from_profile_data(priv->keyboard_selector, priv->profile_data);
	skeltr_key_mask_selector_set_from_profile_data(priv->key_mask_selector, priv->profile_data);
	skeltr_notification_frame_set_from_profile_data(priv->notification_frame, priv->profile_data);
	skeltr_color_frame_set_from_profile_data(priv->color_frame, priv->profile_data);
	skeltr_light_effects_frame_set_from_profile_data(priv->light_effects_frame, priv->profile_data);
	skeltr_light_event_effect_frame_set_from_profile_data(priv->light_event_effect_frame, priv->profile_data);

	for (i = 0; i < GAMINGGEAR_PROFILE_DATA_GAMEFILE_NUM; ++i)
		roccat_gamefile_selector_set_text(priv->gamefiles, i, gaminggear_profile_data_get_gamefile(GAMINGGEAR_PROFILE_DATA(priv->profile_data), i));
}

static void update_profile_data(SkeltrProfilePage *profile_page, RoccatKeyFile *config) {
	SkeltrProfilePagePrivate *priv = profile_page->priv;
	gchar *profile_name;
	guint i;

	profile_name = roccat_profile_page_get_name(ROCCAT_PROFILE_PAGE(profile_page));
	gaminggear_profile_data_set_name(GAMINGGEAR_PROFILE_DATA(priv->profile_data), profile_name);
	g_free(profile_name);

	skeltr_keyboard_selector_update_profile_data(priv->keyboard_selector, priv->profile_data);
	skeltr_key_mask_selector_update_profile_data(priv->key_mask_selector, priv->profile_data);
	skeltr_notification_frame_update_profile_data(priv->notification_frame, priv->profile_data);
	skeltr_color_frame_update_profile_data(priv->color_frame, priv->profile_data);
	skeltr_light_effects_frame_update_profile_data(priv->light_effects_frame, priv->profile_data);
	skeltr_light_event_effect_frame_update_profile_data(priv->light_event_effect_frame, priv->profile_data);

	for (i = 0; i < GAMINGGEAR_PROFILE_DATA_GAMEFILE_NUM; ++i)
		gaminggear_profile_data_set_gamefile(GAMINGGEAR_PROFILE_DATA(priv->profile_data), i, roccat_gamefile_selector_get_text(priv->gamefiles, i));
}

static void reset_cb(RoccatProfilePage *profile_page, gpointer user_data) {
	SkeltrProfilePage *skeltr_profile_page = SKELTR_PROFILE_PAGE(profile_page);
	gaminggear_profile_data_reset(GAMINGGEAR_PROFILE_DATA(skeltr_profile_page->priv->profile_data));
	set_from_profile_data(skeltr_profile_page, NULL);
}

static void append_keys_page(SkeltrProfilePage *profile_page, GtkNotebook *notebook) {
	SkeltrProfilePagePrivate *priv = profile_page->priv;
	GtkWidget *vbox;

	vbox = gtk_vbox_new(FALSE, 0);
	priv->keyboard_selector = SKELTR_KEYBOARD_SELECTOR(skeltr_keyboard_selector_new());

	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(priv->keyboard_selector), TRUE, TRUE, 0);
	gtk_notebook_append_page(notebook, vbox, gtk_label_new(_("Keys")));
}

static void append_misc_page(SkeltrProfilePage *profile_page, GtkNotebook *notebook) {
	SkeltrProfilePagePrivate *priv = profile_page->priv;
	GtkWidget *hbox1;
	GtkWidget *hbox2;

	hbox1 = gtk_hbox_new(FALSE, 0);
	hbox2 = gtk_hbox_new(FALSE, 0);

	priv->misc_box = GTK_BOX(gtk_vbox_new(FALSE, 0));
	priv->gamefiles = ROCCAT_GAMEFILE_SELECTOR(roccat_gamefile_selector_new(GAMINGGEAR_PROFILE_DATA_GAMEFILE_NUM));
	priv->key_mask_selector = SKELTR_KEY_MASK_SELECTOR(skeltr_key_mask_selector_new());
	priv->notification_frame = SKELTR_NOTIFICATION_FRAME(skeltr_notification_frame_new());
	priv->color_frame = SKELTR_COLOR_FRAME(skeltr_color_frame_new());
	priv->light_effects_frame = SKELTR_LIGHT_EFFECTS_FRAME(skeltr_light_effects_frame_new());
	priv->light_event_effect_frame = SKELTR_LIGHT_EVENT_EFFECT_FRAME(skeltr_light_event_effect_frame_new());

	gtk_box_pack_start(GTK_BOX(hbox1), GTK_WIDGET(priv->color_frame), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox1), GTK_WIDGET(priv->light_effects_frame), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox1), GTK_WIDGET(priv->light_event_effect_frame), TRUE, TRUE, 0);

	gtk_box_pack_start(GTK_BOX(hbox2), GTK_WIDGET(priv->key_mask_selector), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox2), GTK_WIDGET(priv->notification_frame), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox2), GTK_WIDGET(priv->gamefiles), TRUE, TRUE, 0);

	gtk_box_pack_start(priv->misc_box, hbox1, TRUE, TRUE, 0);
	gtk_box_pack_start(priv->misc_box, hbox2, TRUE, TRUE, 0);

	gtk_notebook_append_page(notebook, GTK_WIDGET(priv->misc_box), gtk_label_new(_("Misc")));
}

static void skeltr_profile_page_init(SkeltrProfilePage *profile_page) {
	SkeltrProfilePagePrivate *priv = SKELTR_PROFILE_PAGE_GET_PRIVATE(profile_page);
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
	SkeltrProfilePagePrivate *priv = SKELTR_PROFILE_PAGE(object)->priv;

	if (priv->profile_data)
		g_object_unref(priv->profile_data);

	G_OBJECT_CLASS(skeltr_profile_page_parent_class)->finalize(object);
}

static void skeltr_profile_page_class_init(SkeltrProfilePageClass *klass) {
	GObjectClass *gobject_class = (GObjectClass*)klass;

	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(SkeltrProfilePagePrivate));
}

void skeltr_profile_page_set_profile_data(SkeltrProfilePage *profile_page, SkeltrProfileData *profile_data, RoccatKeyFile *config) {
	SkeltrProfilePagePrivate *priv = profile_page->priv;
	if (priv->profile_data)
		g_object_unref(priv->profile_data);

	priv->profile_data = profile_data;
	g_object_ref(priv->profile_data);
	set_from_profile_data(profile_page, config);
}

SkeltrProfileData *skeltr_profile_page_get_profile_data(SkeltrProfilePage *profile_page, RoccatKeyFile *config) {
	SkeltrProfilePagePrivate *priv = profile_page->priv;
	update_profile_data(profile_page, config);
	g_object_ref(priv->profile_data);
	return priv->profile_data;
}

void skeltr_profile_page_set_keyboard_layout(SkeltrProfilePage *profile_page, gchar const *layout) {
	skeltr_keyboard_selector_set_layout(profile_page->priv->keyboard_selector, layout);
}

void skeltr_profile_page_update_brightness(SkeltrProfilePage *profile_page, guint new_brightness) {
	SkeltrProfilePagePrivate *priv = profile_page->priv;
	skeltr_profile_data_set_brightness_from_hardware(priv->profile_data, new_brightness);
	skeltr_light_effects_frame_set_brightness(priv->light_effects_frame, new_brightness);
}

void skeltr_profile_page_update_key(SkeltrProfilePage *profile_page, guint macro_index, SkeltrMacro *macro) {
	SkeltrProfilePagePrivate *priv = profile_page->priv;

	skeltr_profile_data_set_macro_from_hardware(priv->profile_data, macro_index, macro);
	skeltr_keyboard_selector_set_macro(priv->keyboard_selector, macro_index, macro);
}
