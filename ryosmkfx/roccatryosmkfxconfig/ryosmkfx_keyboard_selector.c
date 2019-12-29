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

#include <gaminggear/gdk_key_translations.h>
#include "ryosmkfx_keyboard_selector.h"
#include "ryosmkfx_standard_selector.h"
#include "ryosmkfx_capslock_selector.h"
#include "ryosmkfx_key_illumination_selector.h"
#include "ryosmkfx_illumination_frame.h"
#include "roccat_key_button.h"
#include "ryosmkfx_key_relations.h"
#include "ryos_gui_key_relations.h"
#include "ryosmkfx_light_effects_emulator.h"
#include "ryos_fn_selector.h"
#include "roccat_multiwidget.h"
#include "g_cclosure_roccat_marshaller.h"
#include "g_roccat_helper.h"
#include "gdk_roccat_helper.h"
#include "config.h"
#include "i18n.h"

#define RYOSMKFX_KEYBOARD_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), RYOSMKFX_KEYBOARD_SELECTOR_TYPE, RyosmkfxKeyboardSelectorClass))
#define IS_RYOSMKFX_KEYBOARD_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), RYOSMKFX_KEYBOARD_SELECTOR_TYPE))
#define RYOSMKFX_KEYBOARD_SELECTOR_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), RYOSMKFX_KEYBOARD_SELECTOR_TYPE, RyosmkfxKeyboardSelectorClass))
#define RYOSMKFX_KEYBOARD_SELECTOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), RYOSMKFX_KEYBOARD_SELECTOR_TYPE, RyosmkfxKeyboardSelectorPrivate))

typedef struct _RyosmkfxKeyboardSelectorClass RyosmkfxKeyboardSelectorClass;
typedef struct _RyosmkfxKeyboardSelectorPrivate RyosmkfxKeyboardSelectorPrivate;
typedef struct _RyosmkfxKeyboardSelectorPrivateData RyosmkfxKeyboardSelectorPrivateData;

struct _RyosmkfxKeyboardSelector {
	GtkFrame parent;
	RyosmkfxKeyboardSelectorPrivate *priv;
};

struct _RyosmkfxKeyboardSelectorClass {
	GtkFrameClass parent_class;
};

enum {
	TABLE_HEIGHT = 30,
	TABLE_WIDTH = 95,
};

static gchar const * const index_key = "index";

struct _RyosmkfxKeyboardSelectorPrivate {
	RoccatKeyButton *keys[RYOS_KEY_RELATIONS_NUM];
	GtkScrolledWindow *scroll_window;
	GtkTable *table;

	GtkLabel *label;
	guint actual_index;

	RyosmkfxIlluminationFrame *illumination_frame;

	RoccatMultiwidget *key_type_multiwidget;
	RyosFnSelector *fn_selector;
	RyosmkfxCapslockSelector *capslock_selector;
	RyosmkfxStandardSelector *standard_selector;
	GtkWidget *nothing_selector;

	RoccatMultiwidget *key_illumination_multiwidget;
	RyosmkfxKeyIlluminationSelector *key_illumination_selector;
	GtkWidget *key_illumination_box;
	GtkWidget *key_illumination_nothing;

	/* private object data can only be 64kb in size */
	RyosmkfxKeyboardSelectorPrivateData *data;

	RyosmkfxLightEffectsEmulator *emulator;
	GtkToggleButton *emulation_button;
};

struct _RyosmkfxKeyboardSelectorPrivateData {
	RyosmkfxKeyComboBoxData standard_datas[RYOS_KEY_RELATIONS_NUM];
	RyosmkfxKeyComboBoxData easyshift_datas[RYOS_KEY_RELATIONS_NUM];
	RyosmkfxLightLayerData light_layer_data;
};

G_DEFINE_TYPE(RyosmkfxKeyboardSelector, ryosmkfx_keyboard_selector, GTK_TYPE_FRAME);

enum {
	LAYER_EDITOR,
	LAST_SIGNAL,
};

static guint signals[LAST_SIGNAL] = { 0 };

GtkWidget *ryosmkfx_keyboard_selector_new(void) {
	return GTK_WIDGET(g_object_new(RYOSMKFX_KEYBOARD_SELECTOR_TYPE,
			"label", _("Keys"),
			NULL));
}

static guint get_active_index(RyosmkfxKeyboardSelector *selector) {
	return selector->priv->actual_index;
}

static void set_active_index(RyosmkfxKeyboardSelector *selector, guint index) {
	selector->priv->actual_index = index;
}

static void selector_label_update(RyosmkfxKeyboardSelector *selector, guint index) {
	RyosmkfxKeyboardSelectorPrivate *priv = selector->priv;
	gchar *string;

	string = ryos_hid_to_special_keyname(ryos_key_relations[index].primary);
	if (!string)
		string = gaminggear_hid_to_name(ryos_key_relations[index].primary);

	gtk_label_set_text(priv->label, string);
	g_free(string);
}

static void selector_show(RyosmkfxKeyboardSelector *selector) {
	RyosmkfxKeyboardSelectorPrivate *priv = selector->priv;
	GtkWidget *possible_illumination;
	guint index = get_active_index(selector);

	if (ryosmkfx_illumination_frame_get_mode(priv->illumination_frame) == RYOSMKFX_LIGHT_MODE_PLAIN)
		possible_illumination = GTK_WIDGET(priv->key_illumination_nothing);
	else
		possible_illumination = GTK_WIDGET(priv->key_illumination_box);

	switch (ryos_key_relations[index].keys_type) {
	case RYOS_KEY_RELATION_KEYS_TYPE_CAPSLOCK:
		ryosmkfx_capslock_selector_set_index(priv->capslock_selector, index, ryos_gui_key_relations[index].mask, ryos_gui_key_relations[index].default_normal);
		roccat_multiwidget_show(priv->key_type_multiwidget, GTK_WIDGET(priv->capslock_selector));
		roccat_multiwidget_show(priv->key_illumination_multiwidget, possible_illumination);
		break;
	case RYOS_KEY_RELATION_KEYS_TYPE_FN:
		roccat_multiwidget_show(priv->key_type_multiwidget, GTK_WIDGET(priv->fn_selector));
		roccat_multiwidget_show(priv->key_illumination_multiwidget, possible_illumination);
		break;
	case RYOS_KEY_RELATION_KEYS_TYPE_NOTHING:
		roccat_multiwidget_show(priv->key_type_multiwidget, priv->nothing_selector);
		roccat_multiwidget_show(priv->key_illumination_multiwidget, possible_illumination);
		break;
	case RYOS_KEY_RELATION_KEYS_TYPE_THUMBSTER:
		ryosmkfx_standard_selector_set_secondary_text(priv->standard_selector, _("Easyshift"));
		ryosmkfx_standard_selector_set_index(priv->standard_selector, index,
				ryos_gui_key_relations[index].mask, ryos_gui_key_relations[index].default_normal, ryos_gui_key_relations[index].mask_easyshift, ryos_gui_key_relations[index].default_easyshift);
		roccat_multiwidget_show(priv->key_type_multiwidget, GTK_WIDGET(priv->standard_selector));
		roccat_multiwidget_show(priv->key_illumination_multiwidget, GTK_WIDGET(priv->key_illumination_nothing));
		break;
	case RYOS_KEY_RELATION_KEYS_TYPE_MACRO:
	case RYOS_KEY_RELATION_KEYS_TYPE_EASYZONE:
		ryosmkfx_standard_selector_set_secondary_text(priv->standard_selector, _("Easyshift"));
		ryosmkfx_standard_selector_set_index(priv->standard_selector, index,
				ryos_gui_key_relations[index].mask, ryos_gui_key_relations[index].default_normal, ryos_gui_key_relations[index].mask_easyshift, ryos_gui_key_relations[index].default_easyshift);
		roccat_multiwidget_show(priv->key_type_multiwidget, GTK_WIDGET(priv->standard_selector));
		roccat_multiwidget_show(priv->key_illumination_multiwidget, possible_illumination);
		break;
	case RYOS_KEY_RELATION_KEYS_TYPE_FUNCTION:
		ryosmkfx_standard_selector_set_secondary_text(priv->standard_selector, _("FN"));
		ryosmkfx_standard_selector_set_index(priv->standard_selector, index,
				ryos_gui_key_relations[index].mask, ryos_gui_key_relations[index].default_normal, ryos_gui_key_relations[index].mask_easyshift, ryos_gui_key_relations[index].default_easyshift);
		roccat_multiwidget_show(priv->key_type_multiwidget, GTK_WIDGET(priv->standard_selector));
		roccat_multiwidget_show(priv->key_illumination_multiwidget, possible_illumination);
		break;
	default:
		// error
		break;
	}
}

static void key_illumination_changed_cb(RyosmkfxKeyIlluminationSelector *selector, gpointer user_data) {
	RyosmkfxKeyboardSelector *keyboard_selector = RYOSMKFX_KEYBOARD_SELECTOR(user_data);
	RyosmkfxKeyboardSelectorPrivate *priv = keyboard_selector->priv;
	guint key_index;
	guint light_layer_index;
	RyosmkfxLightLayerKey *light_layer_key;
	gboolean state;
	GdkColor color;

	key_index = get_active_index(keyboard_selector);
	light_layer_index = ryos_key_relations[key_index].primary_index;
	light_layer_key = &priv->data->light_layer_data.keys[light_layer_index];
	state = ryosmkfx_key_illumination_selector_get_state(priv->key_illumination_selector);
	ryosmkfx_key_illumination_selector_get_color(priv->key_illumination_selector, &color);

	light_layer_key->state = state;
	light_layer_key->red = color.red / GDK_ROCCAT_BYTE_TO_COLOR_FACTOR;
	light_layer_key->green = color.green / GDK_ROCCAT_BYTE_TO_COLOR_FACTOR;
	light_layer_key->blue = color.blue / GDK_ROCCAT_BYTE_TO_COLOR_FACTOR;

	if (ryosmkfx_light_effects_emulator_uses_temp_layer_data(priv->emulator))
		return;

	if (priv->keys[key_index]) {
		roccat_key_button_set_light(priv->keys[key_index], state);
		roccat_key_button_set_color(priv->keys[key_index], &color);
	}
}

static void update_key_illumination(RyosmkfxKeyboardSelector *keyboard_selector) {
	RyosmkfxKeyboardSelectorPrivate *priv = keyboard_selector->priv;
	guint key_index = get_active_index(keyboard_selector);
	guint light_layer_index = ryos_key_relations[key_index].primary_index;
	RyosmkfxLightLayerKey *light_layer_key = &priv->data->light_layer_data.keys[light_layer_index];
	gboolean state;
	GdkColor color;

	state = light_layer_key->state;
	color.red = light_layer_key->red * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR;
	color.green = light_layer_key->green * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR;
	color.blue = light_layer_key->blue * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR;

	g_signal_handlers_block_by_func(G_OBJECT(priv->key_illumination_selector), key_illumination_changed_cb, keyboard_selector);
	ryosmkfx_key_illumination_selector_set_state(priv->key_illumination_selector, state);
	ryosmkfx_key_illumination_selector_set_color(priv->key_illumination_selector, &color);
	g_signal_handlers_unblock_by_func(G_OBJECT(priv->key_illumination_selector), key_illumination_changed_cb, keyboard_selector);
}

static void button_update_active(RyosmkfxKeyboardSelector *keyboard_selector) {
	RyosmkfxKeyboardSelectorPrivate *priv = keyboard_selector->priv;
	guint key_index = get_active_index(keyboard_selector);
	guint sdk_index;

	selector_label_update(keyboard_selector, key_index);
	selector_show(keyboard_selector);
	update_key_illumination(keyboard_selector);

	sdk_index = ryos_hid_to_sdk_index(ryos_key_relations[key_index].primary);
	if (sdk_index != RYOS_KEY_RELATIONS_INDEX_INVALID) {
		ryosmkfx_light_effects_emulator_key_event(priv->emulator, sdk_index, TRUE);
		ryosmkfx_light_effects_emulator_key_event(priv->emulator, sdk_index, FALSE);
	}
}

static void key_clicked_cb(GtkButton *button, gpointer user_data) {
	RyosmkfxKeyboardSelector *keyboard_selector = RYOSMKFX_KEYBOARD_SELECTOR(user_data);
	guint index;

	if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button)))
		return;

	index = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(button), index_key));
	set_active_index(keyboard_selector, index);

	button_update_active(keyboard_selector);
}

static void button_set_active(RyosmkfxKeyboardSelector *selector, guint index) {
	RyosmkfxKeyboardSelectorPrivate *priv = selector->priv;
	GtkToggleButton *toggle_button;

	set_active_index(selector, index);

	toggle_button = GTK_TOGGLE_BUTTON(priv->keys[index]);

	if (!gtk_toggle_button_get_active(toggle_button)) {
		g_signal_handlers_block_by_func(G_OBJECT(toggle_button), key_clicked_cb, selector);
		gtk_toggle_button_set_active(toggle_button, TRUE);
		g_signal_handlers_unblock_by_func(G_OBJECT(toggle_button), key_clicked_cb, selector);
	}

	button_update_active(selector);
}

/* Fixing size request width
 * AspectFrame forces Table to shape, but does not correct its size_request,
 * leaving an empty stripe on the side.
 */
static void table_size_request_cb(GtkWidget *table, GtkRequisition *requisition, gpointer user_data) {
	requisition->width = (gfloat)TABLE_WIDTH / (gfloat)TABLE_HEIGHT * (gfloat)requisition->height;
}

static gint find_index_by_type(guint type) {
	guint i;

	for (i = 0; i < RYOS_KEY_RELATIONS_NUM; ++i)
		if (ryos_key_relations[i].keys_type == type)
			return i;
	return -1;
}

static void set_fn_tooltip(RyosmkfxKeyboardSelector *keyboard_selector, guint index) {
	RyosmkfxKeyboardSelectorPrivate *priv = keyboard_selector->priv;
	gchar *text;
	guint value;

	if (!priv->keys[index])
		return;

	value = ryos_fn_selector_get_value(priv->fn_selector);
	text = ryos_fn_selector_get_text_for_value(priv->fn_selector, value);
	gtk_widget_set_tooltip_text(GTK_WIDGET(priv->keys[index]), text);
	g_free(text);
}

static void set_capslock_tooltip(RyosmkfxKeyboardSelector *keyboard_selector, guint index) {
	RyosmkfxKeyboardSelectorPrivate *priv = keyboard_selector->priv;
	gchar *text;

	if (!priv->keys[index])
		return;

	text = ryosmkfx_capslock_selector_get_text_for_data(priv->capslock_selector, &priv->data->standard_datas[index]);
	gtk_widget_set_tooltip_text(GTK_WIDGET(priv->keys[index]), text);
	g_free(text);
}

static void set_standard_tooltip(RyosmkfxKeyboardSelector *keyboard_selector, guint index) {
	RyosmkfxKeyboardSelectorPrivate *priv = keyboard_selector->priv;
	gchar *standard_text;
	gchar *easyshift_text;
	gchar *text;

	if (!priv->keys[index])
		return;

	standard_text = ryosmkfx_standard_selector_get_text_for_data(priv->standard_selector, &priv->data->standard_datas[index]);
	easyshift_text = ryosmkfx_standard_selector_get_text_for_data(priv->standard_selector, &priv->data->easyshift_datas[index]);
	text = g_strdup_printf(_("Standard: %s\nEasyshift: %s"), standard_text, easyshift_text);
	gtk_widget_set_tooltip_text(GTK_WIDGET(priv->keys[index]), text);
	g_free(standard_text);
	g_free(easyshift_text);
	g_free(text);
}

static void fn_key_changed_cb(RyosFnSelector *fn_selector, gpointer user_data) {
	gint index;

	/* assuming there's only one */
	index = find_index_by_type(RYOS_KEY_RELATION_KEYS_TYPE_FN);
	if (index < 0)
		return;

	set_fn_tooltip(RYOSMKFX_KEYBOARD_SELECTOR(user_data), index);
}

static void capslock_key_changed_cb(RyosmkfxCapslockSelector *capslock_selector, gint index, gpointer user_data) {
	RyosmkfxKeyboardSelector *keyboard_selector = RYOSMKFX_KEYBOARD_SELECTOR(user_data);
	if (index >= 0)
		set_capslock_tooltip(keyboard_selector, index);
}

static void standard_key_changed_cb(RyosmkfxStandardSelector *standard_selector, gint index, gpointer user_data) {
	RyosmkfxKeyboardSelector *keyboard_selector = RYOSMKFX_KEYBOARD_SELECTOR(user_data);
	if (index >= 0)
		set_standard_tooltip(keyboard_selector, index);
}

static gboolean set_thumbster_key(RoccatKeyButton *keys[], guint index) {
	GdkColor white;

	gdk_color_parse("white", &white);

	if (ryos_key_relations[index].keys_type == RYOS_KEY_RELATION_KEYS_TYPE_THUMBSTER) {
		roccat_key_button_set_light(keys[index], TRUE);
		roccat_key_button_set_color(keys[index], &white);
		roccat_key_button_set_intensity(keys[index], 1.0);
		return TRUE;
	} else
		return FALSE;
}

static void set_plain(RyosmkfxKeyboardSelector *keyboard_selector) {
	RyosmkfxKeyboardSelectorPrivate *priv = keyboard_selector->priv;
	GdkColor color;
	guint i;

	ryosmkfx_illumination_frame_get_plain_color(priv->illumination_frame, &color);
	for (i = 0; i < RYOS_KEY_RELATIONS_NUM; ++i) {
		if (priv->keys[i]) {
			if (!set_thumbster_key(priv->keys, i)) {
				roccat_key_button_set_light(priv->keys[i], TRUE);
				roccat_key_button_set_color(priv->keys[i], &color);
			}
		}
	}
}

static void set_layer(RyosmkfxKeyboardSelector *keyboard_selector, RyosmkfxLightLayerData const *light_layer_data) {
	RyosmkfxKeyboardSelectorPrivate *priv = keyboard_selector->priv;
	GdkColor color;
	gboolean state;
	guint light_layer_index;
	guint i;

	for (i = 0; i < RYOS_KEY_RELATIONS_NUM; ++i) {
		if (priv->keys[i]) {
			if (!set_thumbster_key(priv->keys, i)) {
				light_layer_index = ryos_key_relations[i].primary_index;
				color.pixel = 0;
				color.red = light_layer_data->keys[light_layer_index].red * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR;
				color.green = light_layer_data->keys[light_layer_index].green * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR;
				color.blue = light_layer_data->keys[light_layer_index].blue * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR;
				state = light_layer_data->keys[light_layer_index].state;
				roccat_key_button_set_light(priv->keys[i], state);
				roccat_key_button_set_color(priv->keys[i], &color);
			}
		}
	}
}

static void set_illumination(RyosmkfxKeyboardSelector *keyboard_selector) {
	RyosmkfxKeyboardSelectorPrivate *priv = keyboard_selector->priv;

	if (ryosmkfx_illumination_frame_get_mode(priv->illumination_frame) == RYOSMKFX_LIGHT_MODE_PLAIN)
		set_plain(keyboard_selector);
	else
		set_layer(keyboard_selector, &priv->data->light_layer_data);
}

static void update_keyboard(RyosmkfxKeyboardSelector *keyboard_selector) {
	set_illumination(keyboard_selector);
	selector_show(keyboard_selector);
}

static void illumination_plain_color_changed_cb(RyosmkfxIlluminationFrame *illumination_frame, gpointer user_data) {
	set_plain(RYOSMKFX_KEYBOARD_SELECTOR(user_data));
}

static void illumination_mode_changed_cb(RyosmkfxIlluminationFrame *illumination_frame, gpointer user_data) {
	update_keyboard(RYOSMKFX_KEYBOARD_SELECTOR(user_data));
}

static void layer_editor_cb(RyosmkfxIlluminationFrame *illumination_frame, gpointer user_data) {
	RyosmkfxKeyboardSelector *keyboard_selector = RYOSMKFX_KEYBOARD_SELECTOR(user_data);
	RyosmkfxKeyboardSelectorPrivateData *data = keyboard_selector->priv->data;
	gboolean result;
	g_signal_emit(user_data, signals[LAYER_EDITOR], 0, &data->light_layer_data, &result);
	if (result)
		update_keyboard(keyboard_selector);
}

static void create_key_type_multiwidget(RyosmkfxKeyboardSelector *keyboard_selector) {
	RyosmkfxKeyboardSelectorPrivate *priv = keyboard_selector->priv;

	priv->key_type_multiwidget = ROCCAT_MULTIWIDGET(roccat_multiwidget_new());
	priv->fn_selector = RYOS_FN_SELECTOR(ryos_fn_selector_new());
	priv->capslock_selector = RYOSMKFX_CAPSLOCK_SELECTOR(ryosmkfx_capslock_selector_new(priv->data->standard_datas));
	priv->standard_selector = RYOSMKFX_STANDARD_SELECTOR(ryosmkfx_standard_selector_new(priv->data->standard_datas, priv->data->easyshift_datas));
	priv->nothing_selector = gtk_label_new(_("This key does not allow settings"));

	gtk_label_set_line_wrap(GTK_LABEL(priv->nothing_selector), TRUE);

	g_signal_connect(G_OBJECT(priv->fn_selector), "key-changed", G_CALLBACK(fn_key_changed_cb), keyboard_selector);
	g_signal_connect(G_OBJECT(priv->capslock_selector), "key-changed", G_CALLBACK(capslock_key_changed_cb), keyboard_selector);
	g_signal_connect(G_OBJECT(priv->standard_selector), "key-changed", G_CALLBACK(standard_key_changed_cb), keyboard_selector);

	roccat_multiwidget_add(priv->key_type_multiwidget, GTK_WIDGET(priv->fn_selector));
	roccat_multiwidget_add(priv->key_type_multiwidget, GTK_WIDGET(priv->capslock_selector));
	roccat_multiwidget_add(priv->key_type_multiwidget, GTK_WIDGET(priv->standard_selector));
	roccat_multiwidget_add(priv->key_type_multiwidget, priv->nothing_selector);
}

static void create_key_illumination_multiwidget(RyosmkfxKeyboardSelector *keyboard_selector) {
	RyosmkfxKeyboardSelectorPrivate *priv = keyboard_selector->priv;

	priv->key_illumination_multiwidget = ROCCAT_MULTIWIDGET(roccat_multiwidget_new());
	priv->key_illumination_box = gtk_hbox_new(FALSE, 0);
	priv->key_illumination_selector = RYOSMKFX_KEY_ILLUMINATION_SELECTOR(ryosmkfx_key_illumination_selector_new());
	priv->key_illumination_nothing = gtk_label_new(_("This key or mode does not support per-key light settings"));

	gtk_label_set_line_wrap(GTK_LABEL(priv->key_illumination_nothing), TRUE);

	g_signal_connect(G_OBJECT(priv->key_illumination_selector), "changed", G_CALLBACK(key_illumination_changed_cb), keyboard_selector);

	gtk_box_pack_start(GTK_BOX(priv->key_illumination_box), gtk_label_new(_("Light")), FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(priv->key_illumination_box), GTK_WIDGET(priv->key_illumination_selector), TRUE, TRUE, 0);

	roccat_multiwidget_add(priv->key_illumination_multiwidget, priv->key_illumination_box);
	roccat_multiwidget_add(priv->key_illumination_multiwidget, priv->key_illumination_nothing);
}

static void emulation_toggled_cb(GtkToggleButton *togglebutton, gpointer user_data) {
	RyosmkfxKeyboardSelectorPrivate *priv = RYOSMKFX_KEYBOARD_SELECTOR(user_data)->priv;

	ryosmkfx_light_effects_emulator_set_state(priv->emulator, gtk_toggle_button_get_active(priv->emulation_button));
}

static void emulator_timeout_cb(RyosmkfxLightEffectsEmulator *emulator, gfloat intensity, gpointer user_data) {
	RyosmkfxKeyboardSelectorPrivate *priv = RYOSMKFX_KEYBOARD_SELECTOR(user_data)->priv;
	guint i;

	for (i = 0; i < RYOS_KEY_RELATIONS_NUM; ++i) {
		if (priv->keys[i]) {
			if (!set_thumbster_key(priv->keys, i)) {
				roccat_key_button_set_intensity(priv->keys[i], intensity);
			}
		}
	}
}

static void ryosmkfx_keyboard_selector_init(RyosmkfxKeyboardSelector *keyboard_selector) {
	RyosmkfxKeyboardSelectorPrivate *priv = RYOSMKFX_KEYBOARD_SELECTOR_GET_PRIVATE(keyboard_selector);
	keyboard_selector->priv = priv;
	GtkWidget *keyboard_frame;
	GtkWidget *vbox;
	GtkWidget *key_frame;
	GtkWidget *hbox;
	GtkWidget *key_vbox;

	priv->data = (RyosmkfxKeyboardSelectorPrivateData *)g_malloc(sizeof(RyosmkfxKeyboardSelectorPrivateData));

	vbox = gtk_vbox_new(FALSE, 0);

	gtk_container_add(GTK_CONTAINER(keyboard_selector), vbox);

	priv->emulation_button = GTK_TOGGLE_BUTTON(gtk_check_button_new_with_label(_("Emulate light effects")));
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(priv->emulation_button), FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(priv->emulation_button), "toggled", G_CALLBACK(emulation_toggled_cb), keyboard_selector);

	priv->scroll_window = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(NULL, NULL));
	gtk_scrolled_window_set_policy(priv->scroll_window, GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(priv->scroll_window), TRUE, TRUE, 0);

	keyboard_frame = gtk_aspect_frame_new(NULL, 0.0, 0.0, (gfloat)TABLE_WIDTH / (gfloat)TABLE_HEIGHT, FALSE);
	gtk_frame_set_shadow_type(GTK_FRAME(keyboard_frame), GTK_SHADOW_NONE);
	gtk_scrolled_window_add_with_viewport(priv->scroll_window, keyboard_frame);

	priv->table = GTK_TABLE(gtk_table_new(TABLE_HEIGHT, TABLE_WIDTH, TRUE));
	g_signal_connect(G_OBJECT(priv->table), "size-request", G_CALLBACK(table_size_request_cb), NULL);
	gtk_container_add(GTK_CONTAINER(keyboard_frame), GTK_WIDGET(priv->table));

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

	key_frame = gtk_frame_new(NULL);
	gtk_box_pack_start(GTK_BOX(hbox), key_frame, TRUE, TRUE, 0);

	key_vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(key_frame), key_vbox);

	priv->label = GTK_LABEL(gtk_label_new(NULL));
	gtk_frame_set_label_widget(GTK_FRAME(key_frame), GTK_WIDGET(priv->label));

	create_key_type_multiwidget(keyboard_selector);
	gtk_box_pack_start(GTK_BOX(key_vbox), GTK_WIDGET(priv->key_type_multiwidget), TRUE, TRUE, 0);

	create_key_illumination_multiwidget(keyboard_selector);
	gtk_box_pack_start(GTK_BOX(key_vbox), GTK_WIDGET(priv->key_illumination_multiwidget), TRUE, TRUE, 0);

	priv->illumination_frame = RYOSMKFX_ILLUMINATION_FRAME(ryosmkfx_illumination_frame_new());
	g_signal_connect(G_OBJECT(priv->illumination_frame), "mode-changed", G_CALLBACK(illumination_mode_changed_cb), keyboard_selector);
	g_signal_connect(G_OBJECT(priv->illumination_frame), "plain-color-changed", G_CALLBACK(illumination_plain_color_changed_cb), keyboard_selector);
	g_signal_connect(G_OBJECT(priv->illumination_frame), "layer-editor", G_CALLBACK(layer_editor_cb), keyboard_selector);
	gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(priv->illumination_frame), TRUE, TRUE, 0);

	priv->emulator = ryosmkfx_light_effects_emulator_new(keyboard_selector);
	g_signal_connect(G_OBJECT(priv->emulator), "timeout", G_CALLBACK(emulator_timeout_cb), keyboard_selector);

	gtk_toggle_button_set_active(priv->emulation_button, TRUE);

	gtk_widget_show_all(GTK_WIDGET(keyboard_selector));
}

static void finalize(GObject *object) {
	RyosmkfxKeyboardSelector *selector = RYOSMKFX_KEYBOARD_SELECTOR(object);
	RyosmkfxKeyboardSelectorPrivate *priv = selector->priv;

	g_free(priv->data);

	G_OBJECT_CLASS(ryosmkfx_keyboard_selector_parent_class)->finalize(object);
}

static void ryosmkfx_keyboard_selector_class_init(RyosmkfxKeyboardSelectorClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = (GObjectClass*)klass;
	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(RyosmkfxKeyboardSelectorPrivate));

	signals[LAYER_EDITOR] = g_signal_new("layer-editor",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_BOOLEAN__POINTER, G_TYPE_BOOLEAN, 1, G_TYPE_POINTER);
}

static void create_key(RoccatKeyButton **group, RyosmkfxKeyboardSelector *keyboard_selector, RyosGuiKeyPosition const *position,
		guint column_offset, guint row_offset) {
	RyosmkfxKeyboardSelectorPrivate *priv = keyboard_selector->priv;
	guint relation_index = position->relation_index;

	priv->keys[relation_index] = ROCCAT_KEY_BUTTON(roccat_key_button_new(*group, ryos_key_relations[relation_index].primary, ryos_hid_to_special_keyname));
	g_object_set_data(G_OBJECT(priv->keys[relation_index]), index_key, GUINT_TO_POINTER(relation_index));
	g_signal_connect(G_OBJECT(priv->keys[relation_index]), "clicked", G_CALLBACK(key_clicked_cb), keyboard_selector);

	gtk_table_attach(priv->table, GTK_WIDGET(priv->keys[relation_index]),
			position->column + column_offset, position->column + column_offset + position->width,
			position->row + row_offset, position->row + row_offset + position->height,
			GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

	*group = priv->keys[relation_index];
}

static void create_block(RoccatKeyButton **group, RyosmkfxKeyboardSelector *keyboard_selector, RyosGuiKeyPosition const *positions,
		guint column_offset, guint row_offset) {
	guint i;

	for (i = 0; positions[i].relation_index != RYOS_GUI_KEY_POSITIONS_END; ++i)
		create_key(group, keyboard_selector, &positions[i], column_offset, row_offset);
}

static void clear_keys(RyosmkfxKeyboardSelector *keyboard_selector) {
	RyosmkfxKeyboardSelectorPrivate *priv = keyboard_selector->priv;
	guint i;

	for (i = 0; i < RYOS_KEY_RELATIONS_NUM; ++i) {
		if (priv->keys[i]) {
			gtk_widget_destroy(GTK_WIDGET(priv->keys[i]));
			priv->keys[i] = NULL;
		}
	}
}

static void set_tooltips(RyosmkfxKeyboardSelector *keyboard_selector) {
	guint i;

	for (i = 0; i < RYOS_KEY_RELATIONS_NUM; ++i) {
		switch (ryos_key_relations[i].keys_type) {
		case RYOS_KEY_RELATION_KEYS_TYPE_MACRO:
		case RYOS_KEY_RELATION_KEYS_TYPE_EASYZONE:
		case RYOS_KEY_RELATION_KEYS_TYPE_FUNCTION:
		case RYOS_KEY_RELATION_KEYS_TYPE_THUMBSTER:
			set_standard_tooltip(keyboard_selector, i);
			break;
		case RYOS_KEY_RELATION_KEYS_TYPE_CAPSLOCK:
			set_capslock_tooltip(keyboard_selector, i);
			break;
		case RYOS_KEY_RELATION_KEYS_TYPE_FN:
			set_fn_tooltip(keyboard_selector, i);
			break;
		default:
			break;
		};
	}
}

void ryosmkfx_keyboard_selector_set_layout(RyosmkfxKeyboardSelector *keyboard_selector, gchar const *layout) {
	RoccatKeyButton *group = NULL;

	clear_keys(keyboard_selector);
	create_block(&group, keyboard_selector, ryos_gui_key_positions_escape, 5, 0);
	create_block(&group, keyboard_selector, ryos_gui_key_positions_function, 13, 0);
	create_block(&group, keyboard_selector, ryos_gui_key_positions_macro, 0, 5);
	create_block(&group, keyboard_selector, ryos_gui_key_positions_get_main(layout), 5, 5);
	create_block(&group, keyboard_selector, ryos_gui_key_positions_block_above_arrows, 66, 0);
	create_block(&group, keyboard_selector, ryos_gui_key_positions_arrows, 66, 17);
	create_block(&group, keyboard_selector, ryos_gui_key_positions_numpad, 79, 5);
	create_block(&group, keyboard_selector, ryos_gui_key_positions_thumbster, 26, 26);

	set_tooltips(keyboard_selector);
	gtk_widget_show_all(GTK_WIDGET(keyboard_selector->priv->table));
	button_set_active(keyboard_selector, ryos_gui_key_positions_escape[0].relation_index);
	update_keyboard(keyboard_selector);
}

void ryosmkfx_keyboard_selector_set_from_profile_data(RyosmkfxKeyboardSelector *selector, RyosmkfxProfileData const *profile_data) {
	RyosmkfxKeyboardSelectorPrivate *priv = selector->priv;
	RyosmkfxKeyboardSelectorPrivateData *data = priv->data;
	RyosmkfxKeyComboBoxData *standarddata;
	RyosmkfxKeyComboBoxData *easyshiftdata;
	guint i;
	RyosKeyRelation const *key;
	GdkColor color;

	for (i = 0; i < RYOS_KEY_RELATIONS_NUM; ++i) {
		standarddata = &data->standard_datas[i];
		easyshiftdata = &data->easyshift_datas[i];
		key = &ryos_key_relations[i];

		if (key->macro_index != RYOS_KEY_RELATIONS_INDEX_INVALID) {
			ryos_led_macro_copy(&standarddata->led_macro, &profile_data->eventhandler.led_macros[key->macro_index]);
			ryosmkfx_macro_copy(&standarddata->macro, &profile_data->hardware.macros[key->macro_index]);
			g_strlcpy(standarddata->opener, profile_data->eventhandler.openers[key->macro_index], ROCCAT_SWARM_RMP_OPENER_LENGTH);
			roccat_timer_copy(&standarddata->timer, &profile_data->eventhandler.timers[key->macro_index]);
		}

		if (key->macro_index_easyshift != RYOS_KEY_RELATIONS_INDEX_INVALID) {
			ryos_led_macro_copy(&easyshiftdata->led_macro, &profile_data->eventhandler.led_macros[key->macro_index_easyshift]);
			ryosmkfx_macro_copy(&easyshiftdata->macro, &profile_data->hardware.macros[key->macro_index_easyshift]);
			g_strlcpy(easyshiftdata->opener, profile_data->eventhandler.openers[key->macro_index_easyshift], ROCCAT_SWARM_RMP_OPENER_LENGTH);
			roccat_timer_copy(&easyshiftdata->timer, &profile_data->eventhandler.timers[key->macro_index_easyshift]);
		}

		switch (key->keys_type) {
		case RYOS_KEY_RELATION_KEYS_TYPE_MACRO:
			roccat_button_copy(&standarddata->key, &profile_data->hardware.keys_macro.keys[key->keys_index]);
			roccat_button_copy(&easyshiftdata->key, &profile_data->hardware.keys_macro.keys[key->keys_index_easyshift]);
			standarddata->talk_target = profile_data->eventhandler.talk_targets[ryosmkfx_talk_index_from_ryos_talk_index(key->talk_index)];
			break;
		case RYOS_KEY_RELATION_KEYS_TYPE_EASYZONE:
			roccat_button_set_to_normal(&standarddata->key, profile_data->hardware.keys_primary.keys[key->primary_index]);
			roccat_button_copy(&easyshiftdata->key, &profile_data->hardware.keys_easyzone.keys[key->keys_index_easyshift]);
			break;
		case RYOS_KEY_RELATION_KEYS_TYPE_FUNCTION:
			roccat_button_copy(&standarddata->key, &profile_data->hardware.keys_function.keys[key->keys_index]);
			roccat_button_copy(&easyshiftdata->key, &profile_data->hardware.keys_function.keys[key->keys_index_easyshift]);
			break;
		case RYOS_KEY_RELATION_KEYS_TYPE_THUMBSTER:
			roccat_button_copy(&standarddata->key, &profile_data->hardware.keys_thumbster.keys[key->keys_index]);
			roccat_button_copy(&easyshiftdata->key, &profile_data->hardware.keys_thumbster.keys[key->keys_index_easyshift]);
			standarddata->talk_target = profile_data->eventhandler.talk_targets[ryosmkfx_talk_index_from_ryos_talk_index(key->talk_index)];
			break;
		case RYOS_KEY_RELATION_KEYS_TYPE_CAPSLOCK:
			roccat_button_set_to_normal(&standarddata->key, profile_data->hardware.keys_extra.capslock);
			standarddata->talk_target = profile_data->eventhandler.talk_targets[ryosmkfx_talk_index_from_ryos_talk_index(key->talk_index)];
			break;
		case RYOS_KEY_RELATION_KEYS_TYPE_FN:
			ryos_fn_selector_set_value(RYOS_FN_SELECTOR(priv->fn_selector), profile_data->hardware.keys_extra.fn);
			break;
		default:
			break;
		};
	}

	ryosmkfx_light_layer_get_data(&profile_data->hardware.stored_lights.light_layer, &priv->data->light_layer_data);

	color.red = (guint16)ryosmkfx_light_get_red(&profile_data->hardware.light) * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR;
	color.green = (guint16)ryosmkfx_light_get_green(&profile_data->hardware.light) * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR;
	color.blue = (guint16)ryosmkfx_light_get_blue(&profile_data->hardware.light) * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR;
	ryosmkfx_illumination_frame_set_plain_color(priv->illumination_frame, &color);

	ryosmkfx_illumination_frame_set_mode(priv->illumination_frame, profile_data->hardware.light.mode);

	ryosmkfx_keyboard_selector_effect_set_script(selector, profile_data->eventhandler.effect_script);
	ryosmkfx_keyboard_selector_effect_set_effect(selector, profile_data->hardware.light.effect);
	ryosmkfx_keyboard_selector_effect_set_speed(selector, profile_data->hardware.light.effect_speed);

	set_tooltips(selector);
	button_update_active(selector);
	update_keyboard(selector);
}

void ryosmkfx_keyboard_selector_update_profile_data(RyosmkfxKeyboardSelector *selector, RyosmkfxProfileData *profile_data) {
	RyosmkfxKeyboardSelectorPrivate *priv = selector->priv;
	RyosmkfxKeyboardSelectorPrivateData *data = priv->data;
	RyosKeysPrimary keys_primary;
	RyosKeysMacro keys_macro;
	RyosKeysThumbster keys_thumbster;
	RyosKeysEasyzone keys_easyzone;
	RyosKeysFunction keys_function;
	RyosKeysExtra keys_extra;
	RyosmkfxStoredLights stored_lights;
	RyosmkfxLight light;
	RyosmkfxKeyComboBoxData *standarddata;
	RyosmkfxKeyComboBoxData *easyshiftdata;
	guint i;
	RyosKeyRelation const *key;
	GdkColor color;

	ryos_keys_primary_copy(&keys_primary, &profile_data->hardware.keys_primary);
	ryos_keys_thumbster_copy(&keys_thumbster, &profile_data->hardware.keys_thumbster);
	ryos_keys_easyzone_copy(&keys_easyzone, &profile_data->hardware.keys_easyzone);
	ryos_keys_function_copy(&keys_function, &profile_data->hardware.keys_function);
	ryos_keys_extra_copy(&keys_extra, &profile_data->hardware.keys_extra);
	ryos_keys_macro_copy(&keys_macro, &profile_data->hardware.keys_macro);

	for (i = 0; i < RYOS_KEY_RELATIONS_NUM; ++i) {
		standarddata = &data->standard_datas[i];
		easyshiftdata = &data->easyshift_datas[i];
		key = &ryos_key_relations[i];

		switch (key->keys_type) {
		case RYOS_KEY_RELATION_KEYS_TYPE_MACRO:
			roccat_button_copy(&keys_macro.keys[key->keys_index], &standarddata->key);
			roccat_button_copy(&keys_macro.keys[key->keys_index_easyshift], &easyshiftdata->key);
			ryosmkfx_profile_data_eventhandler_set_talk_target(&profile_data->eventhandler, ryosmkfx_talk_index_from_ryos_talk_index(key->talk_index), standarddata->talk_target);
			break;
		case RYOS_KEY_RELATION_KEYS_TYPE_EASYZONE:
			keys_primary.keys[key->primary_index] = standarddata->key.type;
			roccat_button_copy(&keys_easyzone.keys[key->keys_index_easyshift], &easyshiftdata->key);
			break;
		case RYOS_KEY_RELATION_KEYS_TYPE_FUNCTION:
			roccat_button_copy(&keys_function.keys[key->keys_index], &standarddata->key);
			roccat_button_copy(&keys_function.keys[key->keys_index_easyshift], &easyshiftdata->key);
			break;
		case RYOS_KEY_RELATION_KEYS_TYPE_THUMBSTER:
			roccat_button_copy(&keys_thumbster.keys[key->keys_index], &standarddata->key);
			roccat_button_copy(&keys_thumbster.keys[key->keys_index_easyshift], &easyshiftdata->key);
			ryosmkfx_profile_data_eventhandler_set_talk_target(&profile_data->eventhandler, ryosmkfx_talk_index_from_ryos_talk_index(key->talk_index), standarddata->talk_target);
			break;
		case RYOS_KEY_RELATION_KEYS_TYPE_CAPSLOCK:
			keys_extra.capslock = standarddata->key.type;
			ryosmkfx_profile_data_eventhandler_set_talk_target(&profile_data->eventhandler, ryosmkfx_talk_index_from_ryos_talk_index(key->talk_index), standarddata->talk_target);
			break;
		case RYOS_KEY_RELATION_KEYS_TYPE_FN:
			keys_extra.fn = ryos_fn_selector_get_value(RYOS_FN_SELECTOR(priv->fn_selector));
			break;
		};

		if (key->macro_index != RYOS_KEY_RELATIONS_INDEX_INVALID) {
			ryosmkfx_profile_data_hardware_set_macro(&profile_data->hardware, key->macro_index, &standarddata->macro);
			ryosmkfx_profile_data_eventhandler_set_opener(&profile_data->eventhandler, key->macro_index, standarddata->opener);
			ryosmkfx_profile_data_eventhandler_set_timer(&profile_data->eventhandler, key->macro_index, &standarddata->timer);
			ryosmkfx_profile_data_eventhandler_set_led_macro(&profile_data->eventhandler, key->macro_index, &standarddata->led_macro);
		}

		if (key->macro_index_easyshift != RYOS_KEY_RELATIONS_INDEX_INVALID) {
			ryosmkfx_profile_data_hardware_set_macro(&profile_data->hardware, key->macro_index_easyshift, &easyshiftdata->macro);
			ryosmkfx_profile_data_eventhandler_set_opener(&profile_data->eventhandler, key->macro_index_easyshift, easyshiftdata->opener);
			ryosmkfx_profile_data_eventhandler_set_timer(&profile_data->eventhandler, key->macro_index_easyshift, &easyshiftdata->timer);
			ryosmkfx_profile_data_eventhandler_set_led_macro(&profile_data->eventhandler, key->macro_index_easyshift, &easyshiftdata->led_macro);
		}
	}

	ryosmkfx_profile_data_hardware_set_keys_primary(&profile_data->hardware, &keys_primary);
	ryosmkfx_profile_data_hardware_set_keys_macro(&profile_data->hardware, &keys_macro);
	ryosmkfx_profile_data_hardware_set_keys_thumbster(&profile_data->hardware, &keys_thumbster);
	ryosmkfx_profile_data_hardware_set_keys_easyzone(&profile_data->hardware, &keys_easyzone);
	ryosmkfx_profile_data_hardware_set_keys_function(&profile_data->hardware, &keys_function);
	ryosmkfx_profile_data_hardware_set_keys_extra(&profile_data->hardware, &keys_extra);

	ryosmkfx_stored_lights_copy(&stored_lights, &profile_data->hardware.stored_lights);
	ryosmkfx_light_layer_set_data(&stored_lights.light_layer, &priv->data->light_layer_data);
	ryosmkfx_profile_data_hardware_set_stored_lights(&profile_data->hardware, &stored_lights);

	ryosmkfx_light_copy(&light, &profile_data->hardware.light);
	ryosmkfx_illumination_frame_get_plain_color(priv->illumination_frame, &color);
	ryosmkfx_light_set_color(&light,
			color.red / GDK_ROCCAT_BYTE_TO_COLOR_FACTOR,
			color.green / GDK_ROCCAT_BYTE_TO_COLOR_FACTOR,
			color.blue / GDK_ROCCAT_BYTE_TO_COLOR_FACTOR);
	light.mode = ryosmkfx_illumination_frame_get_mode(priv->illumination_frame);
	ryosmkfx_profile_data_hardware_set_light(&profile_data->hardware, &light);
}

void ryosmkfx_keyboard_selector_set_macro(RyosmkfxKeyboardSelector *selector, guint macro_index, RyosmkfxMacro *macro) {
	RyosmkfxKeyboardSelectorPrivate *priv = selector->priv;
	guint i;
	for (i = 0; i < RYOS_KEY_RELATIONS_NUM; ++i) {
		if (macro_index == ryos_key_relations[i].macro_index) {
			roccat_button_set_to_normal(&priv->data->standard_datas[i].key, RYOS_KEY_TYPE_MACRO);
			ryosmkfx_macro_copy(&priv->data->standard_datas[i].macro, macro);
			break;
		} else if (macro_index == ryos_key_relations[i].macro_index_easyshift) {
			roccat_button_set_to_normal(&priv->data->easyshift_datas[i].key, RYOS_KEY_TYPE_MACRO);
			ryosmkfx_macro_copy(&priv->data->easyshift_datas[i].macro, macro);
			break;
		}
	}

	if (i >= RYOS_KEY_RELATIONS_NUM)
		return;

	set_standard_tooltip(selector, i);
	if (get_active_index(selector) == i)
		ryosmkfx_standard_selector_update(priv->standard_selector);
}

void ryosmkfx_keyboard_selector_effect_show_stored_layer_data(RyosmkfxKeyboardSelector *selector) {
	set_illumination(selector);
}

void ryosmkfx_keyboard_selector_effect_show_temp_layer_data(RyosmkfxKeyboardSelector *selector, RyosmkfxLightLayerData const *data) {
	set_layer(selector, data);
}

void ryosmkfx_keyboard_selector_effect_set_script(RyosmkfxKeyboardSelector *selector, gchar const *script) {
	ryosmkfx_light_effects_emulator_set_script(selector->priv->emulator, script);
}

void ryosmkfx_keyboard_selector_effect_set_effect(RyosmkfxKeyboardSelector *selector, guint effect) {
	ryosmkfx_light_effects_emulator_set_effect(selector->priv->emulator, effect);
}

void ryosmkfx_keyboard_selector_effect_set_speed(RyosmkfxKeyboardSelector *selector, guint speed) {
	ryosmkfx_light_effects_emulator_set_speed(selector->priv->emulator, speed);
}
