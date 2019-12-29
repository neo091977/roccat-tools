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

#include "ryostklconfig_profile_page.h"
#include "ryostklconfig_keyboard_selector.h"
#include "ryostklconfig_led_feedback_frame.h"
#include "ryostklconfig_light_frame.h"
#include "ryos_key_mask_selector.h"
#include "ryostklconfig_notification_frame.h"
#include "roccat_gamefile_selector.h"
#include "g_cclosure_roccat_marshaller.h"
#include "i18n.h"

#define RYOSTKLCONFIG_PROFILE_PAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), RYOSTKLCONFIG_PROFILE_PAGE_TYPE, RyostklconfigProfilePageClass))
#define IS_RYOSTKLCONFIG_PROFILE_PAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), RYOSTKLCONFIG_PROFILE_PAGE_TYPE))
#define RYOSTKLCONFIG_PROFILE_PAGE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), RYOSTKLCONFIG_PROFILE_PAGE_TYPE, RyostklconfigProfilePagePrivate))

typedef struct _RyostklconfigProfilePageClass RyostklconfigProfilePageClass;
typedef struct _RyostklconfigProfilePagePrivate RyostklconfigProfilePagePrivate;

struct _RyostklconfigProfilePage {
	RoccatProfilePage parent;
	RyostklconfigProfilePagePrivate *priv;
};

struct _RyostklconfigProfilePageClass {
	RoccatProfilePageClass parent_class;
};

struct _RyostklconfigProfilePagePrivate {
	RyostklProfileData *profile_data;
	GtkBox *misc_box;
	RyostklconfigLightFrame *light_frame;
	RyostklconfigKeyboardSelector *keyboard_selector;
	RoccatGamefileSelector *gamefiles;
	RyostklconfigLedFeedbackFrame *feedback_frame;
	RyosKeyMaskSelector *key_mask_selector;
	RyostklconfigNotificationFrame *notification_frame;
};

G_DEFINE_TYPE(RyostklconfigProfilePage, ryostklconfig_profile_page, ROCCAT_PROFILE_PAGE_TYPE);

enum {
	LAYER_EDITOR,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

void ryostklconfig_profile_page_set_device(RyostklconfigProfilePage *profile_page, RoccatDevice const *device) {
	RyostklconfigProfilePagePrivate *priv = profile_page->priv;
	ryostklconfig_keyboard_selector_set_device_type(priv->keyboard_selector, device);
	ryostklconfig_light_frame_set_device_type(priv->light_frame, device);
	ryostklconfig_led_feedback_frame_set_device_type(priv->feedback_frame, device);
}

void ryostklconfig_profile_page_update_brightness(RyostklconfigProfilePage *profile_page, guint new_brightness) {
	RyostklconfigProfilePagePrivate *priv = profile_page->priv;
	priv->profile_data->hardware.light.brightness = new_brightness;
	ryostklconfig_light_frame_set_brightness(priv->light_frame, new_brightness);
}

void ryostklconfig_profile_page_update_key(RyostklconfigProfilePage *profile_page, guint macro_index, RyosMacro *macro) {
	RyostklconfigProfilePagePrivate *priv = profile_page->priv;

	ryos_macro_copy(&priv->profile_data->hardware.macros[macro_index], macro);
	ryostkl_profile_data_hardware_set_key_to_macro_without_modified(&priv->profile_data->hardware, macro_index);

	ryostklconfig_keyboard_selector_set_macro(priv->keyboard_selector, macro_index, macro);
}

GtkWidget *ryostklconfig_profile_page_new(void) {
	return GTK_WIDGET(g_object_new(RYOSTKLCONFIG_PROFILE_PAGE_TYPE, NULL));
}

static void set_from_profile_data(RyostklconfigProfilePage *profile_page, RoccatKeyFile *config) {
	RyostklconfigProfilePagePrivate *priv = profile_page->priv;
	guint i;

	roccat_profile_page_set_name(ROCCAT_PROFILE_PAGE(profile_page), priv->profile_data->eventhandler.profile_name);

	ryostklconfig_light_frame_set_from_profile_data(priv->light_frame, priv->profile_data, config);
	ryostklconfig_led_feedback_frame_set_from_profile_data(priv->feedback_frame, priv->profile_data);
	ryostklconfig_keyboard_selector_set_from_profile_data(priv->keyboard_selector, priv->profile_data);
	ryostklconfig_notification_frame_set_from_profile_data(priv->notification_frame, priv->profile_data);
	ryos_key_mask_selector_set_mask(priv->key_mask_selector, priv->profile_data->hardware.key_mask.mask);

	for (i = 0; i < RYOS_RKP_PROFILE_GAMEFILE_NUM; ++i)
		roccat_gamefile_selector_set_text(priv->gamefiles, i, priv->profile_data->eventhandler.gamefile_names[i]);
}

static void update_profile_data(RyostklconfigProfilePage *profile_page, RyostklProfileData *profile_data, RoccatKeyFile *config) {
	RyostklconfigProfilePagePrivate *priv = profile_page->priv;
	RyosKeyMask key_mask;
	gchar *profile_name;
	guint i;

	profile_name = roccat_profile_page_get_name(ROCCAT_PROFILE_PAGE(profile_page));
	ryostkl_profile_data_eventhandler_set_profile_name(&profile_data->eventhandler, profile_name);
	g_free(profile_name);

	ryostklconfig_light_frame_update_profile_data(priv->light_frame, profile_data, config);
	ryostklconfig_led_feedback_frame_update_profile_data(priv->feedback_frame, profile_data);
	ryostklconfig_keyboard_selector_update_profile_data(priv->keyboard_selector, profile_data);
	ryostklconfig_notification_frame_update_profile_data(priv->notification_frame, profile_data);

	key_mask.mask = ryos_key_mask_selector_get_mask(priv->key_mask_selector);
	ryostkl_profile_data_hardware_set_key_mask(&profile_data->hardware, &key_mask);

	for (i = 0; i < RYOS_RKP_PROFILE_GAMEFILE_NUM; ++i)
		ryostkl_profile_data_eventhandler_set_gamefile_name(&profile_data->eventhandler, i, roccat_gamefile_selector_get_text(priv->gamefiles, i));
}

static void reset_cb(RoccatProfilePage *profile_page, gpointer user_data) {
	set_from_profile_data(RYOSTKLCONFIG_PROFILE_PAGE(profile_page), NULL);
}

static gboolean layer_editor_cb(RyostklconfigKeyboardSelector *keyboard_selector, gpointer light_layer, gpointer user_data) {
	RyostklconfigProfilePage *profile_page = RYOSTKLCONFIG_PROFILE_PAGE(user_data);
	gboolean result;
	g_signal_emit((gpointer)profile_page, signals[LAYER_EDITOR], 0, light_layer, &result);
	return result;
}

static void append_keys_page(RyostklconfigProfilePage *profile_page, GtkNotebook *notebook) {
	RyostklconfigProfilePagePrivate *priv = profile_page->priv;
	GtkWidget *vbox;

	vbox = gtk_vbox_new(FALSE, 0);
	priv->keyboard_selector = RYOSTKLCONFIG_KEYBOARD_SELECTOR(ryostklconfig_keyboard_selector_new());

	g_signal_connect(G_OBJECT(priv->keyboard_selector), "layer-editor", G_CALLBACK(layer_editor_cb), profile_page);

	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(priv->keyboard_selector), TRUE, TRUE, 0);
	gtk_notebook_append_page(notebook, vbox, gtk_label_new(_("Keys")));
}

static void append_misc_page(RyostklconfigProfilePage *profile_page, GtkNotebook *notebook) {
	RyostklconfigProfilePagePrivate *priv = profile_page->priv;
	GtkWidget *hbox1;
	GtkWidget *hbox2;
	GtkWidget *vbox;

	hbox1 = gtk_hbox_new(FALSE, 0);
	hbox2 = gtk_hbox_new(FALSE, 0);
	vbox = gtk_vbox_new(FALSE, 0);
	priv->misc_box = GTK_BOX(gtk_vbox_new(FALSE, 0));
	priv->light_frame = RYOSTKLCONFIG_LIGHT_FRAME(ryostklconfig_light_frame_new());
	priv->feedback_frame = RYOSTKLCONFIG_LED_FEEDBACK_FRAME(ryostklconfig_led_feedback_frame_new());
	priv->gamefiles = ROCCAT_GAMEFILE_SELECTOR(roccat_gamefile_selector_new(RYOS_RKP_PROFILE_GAMEFILE_NUM));
	priv->key_mask_selector = RYOS_KEY_MASK_SELECTOR(ryos_key_mask_selector_new());
	priv->notification_frame = RYOSTKLCONFIG_NOTIFICATION_FRAME(ryostklconfig_notification_frame_new());

	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(priv->light_frame), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(priv->feedback_frame), TRUE, TRUE, 0);

	gtk_box_pack_start(GTK_BOX(hbox1), vbox, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox1), GTK_WIDGET(priv->key_mask_selector), TRUE, TRUE, 0);

	gtk_box_pack_start(GTK_BOX(hbox2), GTK_WIDGET(priv->gamefiles), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox2), GTK_WIDGET(priv->notification_frame), TRUE, TRUE, 0);

	gtk_box_pack_start(priv->misc_box, hbox1, TRUE, TRUE, 0);
	gtk_box_pack_start(priv->misc_box, hbox2, TRUE, TRUE, 0);

	gtk_notebook_append_page(notebook, GTK_WIDGET(priv->misc_box), gtk_label_new(_("Misc")));
}

static void ryostklconfig_profile_page_init(RyostklconfigProfilePage *profile_page) {
	RyostklconfigProfilePagePrivate *priv = RYOSTKLCONFIG_PROFILE_PAGE_GET_PRIVATE(profile_page);
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

static void ryostklconfig_profile_page_class_init(RyostklconfigProfilePageClass *klass) {
	g_type_class_add_private(klass, sizeof(RyostklconfigProfilePagePrivate));

	signals[LAYER_EDITOR] = g_signal_new("layer-editor",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_BOOLEAN__POINTER, G_TYPE_BOOLEAN, 1, G_TYPE_POINTER);
}

void ryostklconfig_profile_page_set_keyboard_layout(RyostklconfigProfilePage *profile_page, gchar const *layout) {
	ryostklconfig_keyboard_selector_set_layout(profile_page->priv->keyboard_selector, layout);
}

void ryostklconfig_profile_page_set_profile_data(RyostklconfigProfilePage *profile_page, RyostklProfileData const *profile_data, RoccatKeyFile *config) {
	RyostklconfigProfilePagePrivate *priv = profile_page->priv;
	if (priv->profile_data)
		g_free(priv->profile_data);
	priv->profile_data = ryostkl_profile_data_dup(profile_data);
	set_from_profile_data(profile_page, config);
}

RyostklProfileData *ryostklconfig_profile_page_get_profile_data(RyostklconfigProfilePage *profile_page, RoccatKeyFile *config) {
	RyostklconfigProfilePagePrivate *priv = profile_page->priv;
	RyostklProfileData *profile_data;
	profile_data = ryostkl_profile_data_dup(priv->profile_data);
	update_profile_data(profile_page, profile_data, config);
	return profile_data;
}

void ryostklconfig_profile_page_update_keyboard_illumination(RyostklconfigProfilePage *profile_page, RyostklconfigBlink const *blink) {
	ryostklconfig_keyboard_selector_update_keyboard_illumination(profile_page->priv->keyboard_selector, blink);
}
