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

#include "suora_keyboard_selector.h"
#include "suora_key_selector.h"
#include "suora_key_relations.h"
#include "suora_gui_key_relations.h"
#include "roccat_key_button.h"
#include "roccat_multiwidget.h"
#include "g_roccat_helper.h"
#include "gdk_roccat_helper.h"
#include "g_cclosure_roccat_marshaller.h"
#include "config.h"
#include "i18n.h"
#include <gaminggear/gdk_key_translations.h>

#define SUORA_KEYBOARD_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), SUORA_KEYBOARD_SELECTOR_TYPE, SuoraKeyboardSelectorClass))
#define IS_SUORA_KEYBOARD_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), SUORA_KEYBOARD_SELECTOR_TYPE))
#define SUORA_KEYBOARD_SELECTOR_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), SUORA_KEYBOARD_SELECTOR_TYPE, SuoraKeyboardSelectorClass))
#define SUORA_KEYBOARD_SELECTOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), SUORA_KEYBOARD_SELECTOR_TYPE, SuoraKeyboardSelectorPrivate))

typedef struct _SuoraKeyboardSelectorClass SuoraKeyboardSelectorClass;
typedef struct _SuoraKeyboardSelectorPrivate SuoraKeyboardSelectorPrivate;
typedef struct _SuoraKeyboardSelectorPrivateData SuoraKeyboardSelectorPrivateData;

struct _SuoraKeyboardSelector {
	GtkFrame parent;
	SuoraKeyboardSelectorPrivate *priv;
};

struct _SuoraKeyboardSelectorClass {
	GtkFrameClass parent_class;
};

enum {
	TABLE_HEIGHT = 25,
	TABLE_WIDTH = 90,
};

static gchar const * const index_key = "index";

struct _SuoraKeyboardSelectorPrivate {
	RoccatKeyButton *keys[SUORA_KEYS_NUM];
	GtkScrolledWindow *scroll_window;
	GtkTable *table;

	GtkLabel *label;
	guint actual_index;

	RoccatMultiwidget *key_type_multiwidget;
	SuoraKeySelector *key_selector;
	GtkWidget *nothing_selector;

	/* private object data can only be 64kb in size */
	SuoraKeyboardSelectorPrivateData *data;
};

struct _SuoraKeyboardSelectorPrivateData {
	SuoraKeyComboBoxData datas[SUORA_KEYS_NUM];
};

G_DEFINE_TYPE(SuoraKeyboardSelector, suora_keyboard_selector, GTK_TYPE_FRAME);

GtkWidget *suora_keyboard_selector_new(void) {
	return GTK_WIDGET(g_object_new(SUORA_KEYBOARD_SELECTOR_TYPE,
			"label", _("Game mode keys"),
			NULL));
}

static guint get_active_index(SuoraKeyboardSelector *selector) {
	return selector->priv->actual_index;
}

static void set_active_index(SuoraKeyboardSelector *selector, guint index) {
	selector->priv->actual_index = index;
}

static void selector_label_update(SuoraKeyboardSelector *selector, guint index) {
	SuoraKeyboardSelectorPrivate *priv = selector->priv;
	gchar *string;

	string = suora_hid_to_special_keyname(suora_key_relations[index].value);
	if (!string)
		string = gaminggear_hid_to_name(suora_key_relations[index].value);

	gtk_label_set_text(priv->label, string);
	g_free(string);
}

static void selector_show(SuoraKeyboardSelector *selector) {
	SuoraKeyboardSelectorPrivate *priv = selector->priv;
	guint index = get_active_index(selector);

	if (suora_key_relations[index].type != SUORA_KEY_RELATION_TYPE_NONE) {
		roccat_multiwidget_show(priv->key_type_multiwidget, GTK_WIDGET(priv->key_selector));
		suora_key_selector_set_index(priv->key_selector, index, (suora_key_relations[index].type == SUORA_KEY_RELATION_TYPE_NORMAL) ? ROCCAT_KEY_COMBO_BOX_GROUP_MACRO : 0, suora_key_relations[index].value);
	} else
		roccat_multiwidget_show(priv->key_type_multiwidget, priv->nothing_selector);
}

static void button_update_active(SuoraKeyboardSelector *keyboard_selector) {
	guint key_index = get_active_index(keyboard_selector);

	selector_label_update(keyboard_selector, key_index);
	selector_show(keyboard_selector);
}

static void key_clicked_cb(GtkButton *button, gpointer user_data) {
	SuoraKeyboardSelector *keyboard_selector = SUORA_KEYBOARD_SELECTOR(user_data);
	guint index;

	if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button)))
		return;

	index = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(button), index_key));
	set_active_index(keyboard_selector, index);

	button_update_active(keyboard_selector);
}

static void button_set_active(SuoraKeyboardSelector *selector, guint index) {
	SuoraKeyboardSelectorPrivate *priv = selector->priv;
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

static void set_tooltip(SuoraKeyboardSelector *keyboard_selector, guint index) {
	SuoraKeyboardSelectorPrivate *priv = keyboard_selector->priv;
	gchar *standard_text;

	if (!priv->keys[index])
		return;

	standard_text = suora_key_selector_get_text_for_data(priv->key_selector, &priv->data->datas[index]);
	gtk_widget_set_tooltip_text(GTK_WIDGET(priv->keys[index]), standard_text);
	g_free(standard_text);
}

static void key_changed_cb(SuoraKeySelector *key_selector, gint index, gpointer user_data) {
	SuoraKeyboardSelector *keyboard_selector = SUORA_KEYBOARD_SELECTOR(user_data);
	if (index >= 0)
		set_tooltip(keyboard_selector, index);
}

static void update_keyboard(SuoraKeyboardSelector *keyboard_selector) {
	selector_show(keyboard_selector);
}

static void create_key_type_multiwidget(SuoraKeyboardSelector *keyboard_selector) {
	SuoraKeyboardSelectorPrivate *priv = keyboard_selector->priv;

	priv->key_type_multiwidget = ROCCAT_MULTIWIDGET(roccat_multiwidget_new());
	priv->key_selector = SUORA_KEY_SELECTOR(suora_key_selector_new(priv->data->datas));
	priv->nothing_selector = gtk_label_new(_("This key does not allow settings"));

	gtk_label_set_line_wrap(GTK_LABEL(priv->nothing_selector), TRUE);

	g_signal_connect(G_OBJECT(priv->key_selector), "key-changed", G_CALLBACK(key_changed_cb), keyboard_selector);

	roccat_multiwidget_add(priv->key_type_multiwidget, GTK_WIDGET(priv->key_selector));
	roccat_multiwidget_add(priv->key_type_multiwidget, priv->nothing_selector);
}

static void suora_keyboard_selector_init(SuoraKeyboardSelector *keyboard_selector) {
	SuoraKeyboardSelectorPrivate *priv = SUORA_KEYBOARD_SELECTOR_GET_PRIVATE(keyboard_selector);
	keyboard_selector->priv = priv;
	GtkWidget *keyboard_frame;
	GtkWidget *vbox;
	GtkWidget *key_frame;
	GtkWidget *hbox;
	GtkWidget *key_vbox;

	priv->data = (SuoraKeyboardSelectorPrivateData *)g_malloc(sizeof(SuoraKeyboardSelectorPrivateData));

	vbox = gtk_vbox_new(FALSE, 0);

	gtk_container_add(GTK_CONTAINER(keyboard_selector), vbox);

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

	gtk_widget_show_all(GTK_WIDGET(keyboard_selector));
}

static void finalize(GObject *object) {
	SuoraKeyboardSelector *selector = SUORA_KEYBOARD_SELECTOR(object);
	SuoraKeyboardSelectorPrivate *priv = selector->priv;

	g_free(priv->data);

	G_OBJECT_CLASS(suora_keyboard_selector_parent_class)->finalize(object);
}

static void suora_keyboard_selector_class_init(SuoraKeyboardSelectorClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = (GObjectClass*)klass;
	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(SuoraKeyboardSelectorPrivate));
}

static void create_key(RoccatKeyButton **group, SuoraKeyboardSelector *keyboard_selector, SuoraGuiKeyPosition const *position,
		guint column_offset, guint row_offset) {
	SuoraKeyboardSelectorPrivate *priv = keyboard_selector->priv;
	guint keys_index = position->keys_index;

	priv->keys[keys_index] = ROCCAT_KEY_BUTTON(roccat_key_button_new(*group, suora_key_relations[keys_index].value, suora_hid_to_special_keyname));
	g_object_set_data(G_OBJECT(priv->keys[keys_index]), index_key, GUINT_TO_POINTER(keys_index));
	g_signal_connect(G_OBJECT(priv->keys[keys_index]), "clicked", G_CALLBACK(key_clicked_cb), keyboard_selector);

	gtk_table_attach(priv->table, GTK_WIDGET(priv->keys[keys_index]),
			position->column + column_offset, position->column + column_offset + position->width,
			position->row + row_offset, position->row + row_offset + position->height,
			GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

	*group = priv->keys[keys_index];
}

static void create_block(RoccatKeyButton **group, SuoraKeyboardSelector *keyboard_selector, SuoraGuiKeyPosition const *positions,
		guint column_offset, guint row_offset) {
	guint i;

	for (i = 0; positions[i].keys_index != SUORA_GUI_KEY_POSITION_INDEX_END; ++i)
		create_key(group, keyboard_selector, &positions[i], column_offset, row_offset);
}

static void clear_keys(SuoraKeyboardSelector *keyboard_selector) {
	SuoraKeyboardSelectorPrivate *priv = keyboard_selector->priv;
	guint i;

	for (i = 0; i < SUORA_KEYS_NUM; ++i) {
		if (priv->keys[i]) {
			gtk_widget_destroy(GTK_WIDGET(priv->keys[i]));
			priv->keys[i] = NULL;
		}
	}
}

static void set_tooltips(SuoraKeyboardSelector *keyboard_selector) {
	guint i;

	for (i = 0; i < SUORA_KEYS_NUM; ++i) {
		if (suora_key_relations[i].type != SUORA_KEY_RELATION_TYPE_NONE)
			set_tooltip(keyboard_selector, i);
	}
}

void suora_keyboard_selector_set_layout(SuoraKeyboardSelector *keyboard_selector, gchar const *layout) {
	RoccatKeyButton *group = NULL;
	gint keys_index;

	clear_keys(keyboard_selector);
	create_block(&group, keyboard_selector, suora_gui_key_positions_general, 0, 0);

	// FIXME implement
	create_block(&group, keyboard_selector, suora_gui_key_positions_de, 0, 0);

	set_tooltips(keyboard_selector);
	gtk_widget_show_all(GTK_WIDGET(keyboard_selector->priv->table));

	keys_index = suora_key_relation_find_by_macro_index(5); // M1
	if (keys_index != -1)
		button_set_active(keyboard_selector, keys_index);

	update_keyboard(keyboard_selector);
}

void suora_keyboard_selector_set_from_profile_data(SuoraKeyboardSelector *selector, SuoraProfileData const *profile_data) {
	SuoraKeyboardSelectorPrivate *priv = selector->priv;
	SuoraKeyboardSelectorPrivateData *data = priv->data;
	SuoraKeyComboBoxData *combo_data;
	guint i;
	SuoraKeyRelation const *relation;
	SuoraRkpKeys const *keys;

	keys = suora_profile_data_get_keys(profile_data);
	for (i = 0; i < SUORA_KEYS_NUM; ++i) {
		relation = &suora_key_relations[i];

		if (relation->type == SUORA_KEY_RELATION_TYPE_NONE)
			continue;

		combo_data = &data->datas[i];

		roccat_button_copy(&combo_data->key, &keys->keys[i]);

		if (relation->macro_index != SUORA_KEY_RELATION_INVALID)
			suora_rkp_macro_copy(&combo_data->macro, suora_profile_data_get_macro(profile_data, relation->macro_index));
	}

	set_tooltips(selector);
	button_update_active(selector);
	update_keyboard(selector);
}

void suora_keyboard_selector_update_profile_data(SuoraKeyboardSelector *selector, SuoraProfileData *profile_data) {
	SuoraKeyboardSelectorPrivate *priv = selector->priv;
	SuoraKeyboardSelectorPrivateData *data = priv->data;
	SuoraRkpKeys rkp_keys;
	SuoraKeyComboBoxData *combo_data;
	guint i;
	SuoraKeyRelation const *relation;

	suora_rkp_keys_copy(&rkp_keys, suora_profile_data_get_keys(profile_data));

	for (i = 0; i < SUORA_KEYS_NUM; ++i) {
		relation = &suora_key_relations[i];

		if (relation->type == SUORA_KEY_RELATION_TYPE_NONE)
			continue;

		combo_data = &data->datas[i];

		roccat_button_copy(&rkp_keys.keys[i], &combo_data->key);

		if (relation->macro_index != SUORA_KEY_RELATION_INVALID)
			suora_profile_data_set_macro(profile_data, relation->macro_index, &combo_data->macro);
	}

	suora_profile_data_set_keys(profile_data, &rkp_keys);
}
