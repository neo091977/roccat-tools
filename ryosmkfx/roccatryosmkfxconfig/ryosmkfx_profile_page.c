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

#include "ryosmkfx_profile_page.h"
#include "ryosmkfx_keyboard_selector.h"
#include "ryosmkfx_led_feedback_frame.h"
#include "ryosmkfx_light_frame.h"
#include "ryosmkfx_light_effects_frame.h"
#include "ryos_key_mask_selector.h"
#include "ryos_notification_frame.h"
#include "roccat_gamefile_selector.h"
#include "g_cclosure_roccat_marshaller.h"
#include "i18n.h"

#define RYOSMKFX_PROFILE_PAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), RYOSMKFX_PROFILE_PAGE_TYPE, RyosmkfxProfilePageClass))
#define IS_RYOSMKFX_PROFILE_PAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), RYOSMKFX_PROFILE_PAGE_TYPE))
#define RYOSMKFX_PROFILE_PAGE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), RYOSMKFX_PROFILE_PAGE_TYPE, RyosmkfxProfilePagePrivate))

typedef struct _RyosmkfxProfilePageClass RyosmkfxProfilePageClass;
typedef struct _RyosmkfxProfilePagePrivate RyosmkfxProfilePagePrivate;

struct _RyosmkfxProfilePage {
	RoccatProfilePage parent;
	RyosmkfxProfilePagePrivate *priv;
};

struct _RyosmkfxProfilePageClass {
	RoccatProfilePageClass parent_class;
};

struct _RyosmkfxProfilePagePrivate {
	RyosmkfxProfileData *profile_data;
	GtkBox *misc_box;
	RoccatGamefileSelector *gamefiles;
	RyosmkfxKeyboardSelector *keyboard_selector;
	RyosKeyMaskSelector *key_mask_selector;
	RyosNotificationFrame *notification_frame;
	RyosmkfxLedFeedbackFrame *feedback_frame;
	RyosmkfxLightFrame *light_frame;
	RyosmkfxLightEffectsFrame *light_effects_frame;
};

G_DEFINE_TYPE(RyosmkfxProfilePage, ryosmkfx_profile_page, ROCCAT_PROFILE_PAGE_TYPE);

enum {
	LAYER_EDITOR,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

GtkWidget *ryosmkfx_profile_page_new(void) {
	return GTK_WIDGET(g_object_new(RYOSMKFX_PROFILE_PAGE_TYPE, NULL));
}

static void ryos_notification_frame_set_from_profile_data(RyosNotificationFrame *notification_frame, RyosmkfxProfileData const *profile_data) {
	ryos_notification_frame_set_timer_type(notification_frame, profile_data->eventhandler.timer_notification_type);
	ryos_notification_frame_set_profile_type(notification_frame, profile_data->eventhandler.profile_notification_type);
	ryos_notification_frame_set_live_recording_type(notification_frame, profile_data->eventhandler.live_recording_notification_type);
	ryos_notification_frame_set_volume(notification_frame, profile_data->eventhandler.notification_volume);
}

static void set_from_profile_data(RyosmkfxProfilePage *profile_page, RoccatKeyFile *config) {
	RyosmkfxProfilePagePrivate *priv = profile_page->priv;
	guint i;

	roccat_profile_page_set_name(ROCCAT_PROFILE_PAGE(profile_page), priv->profile_data->eventhandler.profile_name);
	ryosmkfx_keyboard_selector_set_from_profile_data(priv->keyboard_selector, priv->profile_data);
	ryosmkfx_led_feedback_frame_set_from_profile_data(priv->feedback_frame, priv->profile_data);
	ryosmkfx_light_frame_set_from_profile_data(priv->light_frame, priv->profile_data);
	ryosmkfx_light_effects_frame_set_from_profile_data(priv->light_effects_frame, priv->profile_data, config);
	ryos_key_mask_selector_set_mask(priv->key_mask_selector, priv->profile_data->hardware.key_mask.mask);
	ryos_notification_frame_set_from_profile_data(priv->notification_frame, priv->profile_data);

	for (i = 0; i < ROCCAT_SWARM_RMP_GAMEFILE_NUM; ++i)
		roccat_gamefile_selector_set_text(priv->gamefiles, i, priv->profile_data->eventhandler.gamefile_names[i]);
}

static void ryos_notification_frame_update_profile_data(RyosNotificationFrame *notification_frame, RyosmkfxProfileData *profile_data) {
	guint8 int_val;
	gdouble double_val;

	int_val = ryos_notification_frame_get_timer_type(notification_frame);
	if (int_val != profile_data->eventhandler.timer_notification_type) {
		profile_data->eventhandler.timer_notification_type = int_val;
		profile_data->eventhandler.modified = TRUE;
	}

	int_val = ryos_notification_frame_get_profile_type(notification_frame);
	if (int_val != profile_data->eventhandler.profile_notification_type) {
		profile_data->eventhandler.profile_notification_type = int_val;
		profile_data->eventhandler.modified = TRUE;
	}

	int_val = ryos_notification_frame_get_live_recording_type(notification_frame);
	if (int_val != profile_data->eventhandler.live_recording_notification_type) {
		profile_data->eventhandler.live_recording_notification_type = int_val;
		profile_data->eventhandler.modified = TRUE;
	}

	double_val = ryos_notification_frame_get_volume(notification_frame);
	if (double_val != profile_data->eventhandler.notification_volume) {
		profile_data->eventhandler.notification_volume = double_val;
		profile_data->eventhandler.modified = TRUE;
	}
}

static void update_profile_data(RyosmkfxProfilePage *profile_page, RyosmkfxProfileData *profile_data, RoccatKeyFile *config) {
	RyosmkfxProfilePagePrivate *priv = profile_page->priv;
	RyosKeyMask key_mask;
	gchar *profile_name;
	guint i;

	profile_name = roccat_profile_page_get_name(ROCCAT_PROFILE_PAGE(profile_page));
	ryosmkfx_profile_data_eventhandler_set_profile_name(&profile_data->eventhandler, profile_name);
	g_free(profile_name);

	ryosmkfx_keyboard_selector_update_profile_data(priv->keyboard_selector, profile_data);
	ryosmkfx_led_feedback_frame_update_profile_data(priv->feedback_frame, profile_data);
	ryosmkfx_light_frame_update_profile_data(priv->light_frame, profile_data);
	ryosmkfx_light_effects_frame_update_profile_data(priv->light_effects_frame, profile_data, config);
	ryos_notification_frame_update_profile_data(priv->notification_frame, profile_data);

	key_mask.mask = ryos_key_mask_selector_get_mask(priv->key_mask_selector);
	ryosmkfx_profile_data_hardware_set_key_mask(&profile_data->hardware, &key_mask);

	for (i = 0; i < ROCCAT_SWARM_RMP_GAMEFILE_NUM; ++i)
		ryosmkfx_profile_data_eventhandler_set_gamefile_name(&profile_data->eventhandler, i, roccat_gamefile_selector_get_text(priv->gamefiles, i));
}

static void reset_cb(RoccatProfilePage *profile_page, gpointer user_data) {
	set_from_profile_data(RYOSMKFX_PROFILE_PAGE(profile_page), NULL);
}

static gboolean layer_editor_cb(RyosmkfxKeyboardSelector *keyboard_selector, gpointer light_layer, gpointer user_data) {
	gboolean result;
	g_signal_emit(user_data, signals[LAYER_EDITOR], 0, light_layer, &result);
	return result;
}

static void append_keys_page(RyosmkfxProfilePage *profile_page, GtkNotebook *notebook) {
	RyosmkfxProfilePagePrivate *priv = profile_page->priv;
	GtkWidget *vbox;

	vbox = gtk_vbox_new(FALSE, 0);
	priv->keyboard_selector = RYOSMKFX_KEYBOARD_SELECTOR(ryosmkfx_keyboard_selector_new());
	g_signal_connect(G_OBJECT(priv->keyboard_selector), "layer-editor", G_CALLBACK(layer_editor_cb), profile_page);

	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(priv->keyboard_selector), TRUE, TRUE, 0);
	gtk_notebook_append_page(notebook, vbox, gtk_label_new(_("Keys")));
}

static void light_effect_changed_cb(RyosmkfxLightEffectsFrame *effects_frame, gpointer user_data) {
	RyosmkfxProfilePagePrivate *priv = RYOSMKFX_PROFILE_PAGE(user_data)->priv;
	ryosmkfx_keyboard_selector_effect_set_effect(priv->keyboard_selector, ryosmkfx_light_effects_frame_get_effect(priv->light_effects_frame));
}

static void light_script_changed_cb(RyosmkfxLightEffectsFrame *effects_frame, gpointer user_data) {
	RyosmkfxProfilePagePrivate *priv = RYOSMKFX_PROFILE_PAGE(user_data)->priv;
	ryosmkfx_keyboard_selector_effect_set_script(priv->keyboard_selector, ryosmkfx_light_effects_frame_get_script_path(priv->light_effects_frame));
}

static void light_speed_changed_cb(RyosmkfxLightEffectsFrame *effects_frame, gpointer user_data) {
	RyosmkfxProfilePagePrivate *priv = RYOSMKFX_PROFILE_PAGE(user_data)->priv;
	ryosmkfx_keyboard_selector_effect_set_speed(priv->keyboard_selector, ryosmkfx_light_effects_frame_get_speed(priv->light_effects_frame));
}

static void append_misc_page(RyosmkfxProfilePage *profile_page, GtkNotebook *notebook) {
	RyosmkfxProfilePagePrivate *priv = profile_page->priv;
	GtkWidget *hbox;
	GtkWidget *vbox1;
	GtkWidget *vbox2;

	hbox = gtk_hbox_new(FALSE, 0);
	vbox1 = gtk_vbox_new(FALSE, 0);
	vbox2 = gtk_vbox_new(FALSE, 0);
	priv->misc_box = GTK_BOX(gtk_vbox_new(FALSE, 0));
	priv->gamefiles = ROCCAT_GAMEFILE_SELECTOR(roccat_gamefile_selector_new(ROCCAT_SWARM_RMP_GAMEFILE_NUM));
	priv->key_mask_selector = RYOS_KEY_MASK_SELECTOR(ryos_key_mask_selector_new());
	priv->feedback_frame = RYOSMKFX_LED_FEEDBACK_FRAME(ryosmkfx_led_feedback_frame_new());
	priv->light_frame = RYOSMKFX_LIGHT_FRAME(ryosmkfx_light_frame_new());
	priv->light_effects_frame = RYOSMKFX_LIGHT_EFFECTS_FRAME(ryosmkfx_light_effects_frame_new());
	priv->notification_frame = RYOS_NOTIFICATION_FRAME(ryos_notification_frame_new());

	g_signal_connect(G_OBJECT(priv->light_effects_frame), "effect-changed", G_CALLBACK(light_effect_changed_cb), profile_page);
	g_signal_connect(G_OBJECT(priv->light_effects_frame), "script-changed", G_CALLBACK(light_script_changed_cb), profile_page);
	g_signal_connect(G_OBJECT(priv->light_effects_frame), "speed-changed", G_CALLBACK(light_speed_changed_cb), profile_page);

	gtk_box_pack_start(GTK_BOX(vbox2), GTK_WIDGET(priv->key_mask_selector), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox2), GTK_WIDGET(priv->notification_frame), TRUE, TRUE, 0);

	gtk_box_pack_start(GTK_BOX(vbox1), GTK_WIDGET(priv->feedback_frame), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox1), GTK_WIDGET(priv->light_frame), TRUE, TRUE, 0);

	gtk_box_pack_start(GTK_BOX(hbox), vbox2, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(priv->light_effects_frame), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), vbox1, TRUE, TRUE, 0);

	gtk_box_pack_start(priv->misc_box, hbox, TRUE, TRUE, 0);
	gtk_box_pack_start(priv->misc_box, GTK_WIDGET(priv->gamefiles), TRUE, TRUE, 0);

	gtk_notebook_append_page(notebook, GTK_WIDGET(priv->misc_box), gtk_label_new(_("Misc")));
}

static void ryosmkfx_profile_page_init(RyosmkfxProfilePage *profile_page) {
	RyosmkfxProfilePagePrivate *priv = RYOSMKFX_PROFILE_PAGE_GET_PRIVATE(profile_page);
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

static void ryosmkfx_profile_page_class_init(RyosmkfxProfilePageClass *klass) {
	g_type_class_add_private(klass, sizeof(RyosmkfxProfilePagePrivate));

	signals[LAYER_EDITOR] = g_signal_new("layer-editor",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_BOOLEAN__POINTER, G_TYPE_BOOLEAN, 1, G_TYPE_POINTER);
}

void ryosmkfx_profile_page_set_profile_data(RyosmkfxProfilePage *profile_page, RyosmkfxProfileData const *profile_data, RoccatKeyFile *config) {
	RyosmkfxProfilePagePrivate *priv = profile_page->priv;
	if (priv->profile_data)
		g_free(priv->profile_data);
	priv->profile_data = ryosmkfx_profile_data_dup(profile_data);
	set_from_profile_data(profile_page, config);
}

RyosmkfxProfileData *ryosmkfx_profile_page_get_profile_data(RyosmkfxProfilePage *profile_page, RoccatKeyFile *config) {
	RyosmkfxProfilePagePrivate *priv = profile_page->priv;
	RyosmkfxProfileData *profile_data;
	profile_data = ryosmkfx_profile_data_dup(priv->profile_data);
	update_profile_data(profile_page, profile_data, config);
	return profile_data;
}

void ryosmkfx_profile_page_set_keyboard_layout(RyosmkfxProfilePage *profile_page, gchar const *layout) {
	ryosmkfx_keyboard_selector_set_layout(profile_page->priv->keyboard_selector, layout);
}

void ryosmkfx_profile_page_update_brightness(RyosmkfxProfilePage *profile_page, guint new_brightness) {
	RyosmkfxProfilePagePrivate *priv = profile_page->priv;
	priv->profile_data->hardware.light.brightness = new_brightness;
	ryosmkfx_light_frame_set_brightness(priv->light_frame, new_brightness);
}

void ryosmkfx_profile_page_update_key(RyosmkfxProfilePage *profile_page, guint macro_index, RyosmkfxMacro *macro) {
	RyosmkfxProfilePagePrivate *priv = profile_page->priv;

	ryosmkfx_macro_copy(&priv->profile_data->hardware.macros[macro_index], macro);
	ryosmkfx_profile_data_hardware_set_key_to_macro_without_modified(&priv->profile_data->hardware, macro_index);

	ryosmkfx_keyboard_selector_set_macro(priv->keyboard_selector, macro_index, macro);
}
