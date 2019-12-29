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
#include "skeltr_keyboard_selector.h"
#include "skeltr_fn_selector.h"
#include "skeltr_standard_selector.h"
#include "skeltr_capslock_selector.h"
#include "skeltr_primary_selector.h"
#include "roccat_key_button.h"
#include "roccat_talk.h"
#include "skeltr_key_relations.h"
#include "skeltr_gui_key_relations.h"
#include "roccat_multiwidget.h"
#include "g_cclosure_roccat_marshaller.h"
#include "g_roccat_helper.h"
#include "gdk_roccat_helper.h"
#include "config.h"
#include "i18n.h"

#define SKELTR_KEYBOARD_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), SKELTR_KEYBOARD_SELECTOR_TYPE, SkeltrKeyboardSelectorClass))
#define IS_SKELTR_KEYBOARD_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), SKELTR_KEYBOARD_SELECTOR_TYPE))
#define SKELTR_KEYBOARD_SELECTOR_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), SKELTR_KEYBOARD_SELECTOR_TYPE, SkeltrKeyboardSelectorClass))
#define SKELTR_KEYBOARD_SELECTOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), SKELTR_KEYBOARD_SELECTOR_TYPE, SkeltrKeyboardSelectorPrivate))

typedef struct _SkeltrKeyboardSelectorClass SkeltrKeyboardSelectorClass;
typedef struct _SkeltrKeyboardSelectorPrivate SkeltrKeyboardSelectorPrivate;
typedef struct _SkeltrKeyboardSelectorPrivateData SkeltrKeyboardSelectorPrivateData;

struct _SkeltrKeyboardSelector {
	GtkFrame parent;
	SkeltrKeyboardSelectorPrivate *priv;
};

struct _SkeltrKeyboardSelectorClass {
	GtkFrameClass parent_class;
};

enum {
	TABLE_HEIGHT = 29,
	TABLE_WIDTH = 95,
};

static gchar const * const index_key = "index";

struct _SkeltrKeyboardSelectorPrivate {
	RoccatKeyButton *keys[SKELTR_KEYS_NUM];
	GtkScrolledWindow *scroll_window;
	GtkTable *table;

	GtkLabel *label;
	guint actual_index;

	RoccatMultiwidget *key_type_multiwidget;
	SkeltrFnSelector *fn_selector;
	SkeltrPrimarySelector *primary_selector;
	SkeltrCapslockSelector *capslock_selector;
	SkeltrStandardSelector *standard_selector;
	GtkWidget *nothing_selector;

	/* private object data can only be 64kb in size */
	SkeltrKeyboardSelectorPrivateData *data;
};

struct _SkeltrKeyboardSelectorPrivateData {
	SkeltrKeyComboBoxData standard_datas[SKELTR_KEYS_NUM];
	SkeltrKeyComboBoxData easyshift_datas[SKELTR_KEYS_NUM];
};

G_DEFINE_TYPE(SkeltrKeyboardSelector, skeltr_keyboard_selector, GTK_TYPE_FRAME);

enum {
	LAYER_EDITOR,
	LAST_SIGNAL,
};

static guint signals[LAST_SIGNAL] = { 0 };

GtkWidget *skeltr_keyboard_selector_new(void) {
	return GTK_WIDGET(g_object_new(SKELTR_KEYBOARD_SELECTOR_TYPE,
			"label", _("Keys"),
			NULL));
}

static guint get_active_index(SkeltrKeyboardSelector *selector) {
	return selector->priv->actual_index;
}

static void set_active_index(SkeltrKeyboardSelector *selector, guint index) {
	selector->priv->actual_index = index;
}

static void selector_label_update(SkeltrKeyboardSelector *selector, guint index) {
	SkeltrKeyboardSelectorPrivate *priv = selector->priv;
	gchar *string;

	string = skeltr_hid_to_special_keyname(skeltr_key_relations[index].default_value);
	if (!string)
		string = gaminggear_hid_to_name(skeltr_key_relations[index].default_value);

	gtk_label_set_text(priv->label, string);
	g_free(string);
}

static void selector_show(SkeltrKeyboardSelector *selector) {
	SkeltrKeyboardSelectorPrivate *priv = selector->priv;
	guint index = get_active_index(selector);

	switch (skeltr_key_relations[index].keys_type) {
	case SKELTR_KEY_RELATION_TYPE_NOTHING:
		roccat_multiwidget_show(priv->key_type_multiwidget, priv->nothing_selector);
		break;
	case SKELTR_KEY_RELATION_TYPE_FN:
		roccat_multiwidget_show(priv->key_type_multiwidget, GTK_WIDGET(priv->fn_selector));
		break;
	case SKELTR_KEY_RELATION_TYPE_CAPSLOCK:
		skeltr_capslock_selector_set_index(priv->capslock_selector, index,
				SKELTR_KEY_COMBO_BOX_GROUP_PRIMARY | SKELTR_KEY_COMBO_BOX_GROUP_EXTENDED | SKELTR_KEY_COMBO_BOX_GROUP_KEY,
				skeltr_key_relations[index].default_value);
		roccat_multiwidget_show(priv->key_type_multiwidget, GTK_WIDGET(priv->capslock_selector));
		break;
	case SKELTR_KEY_RELATION_TYPE_MACRO:
	case SKELTR_KEY_RELATION_TYPE_THUMBSTER:
		skeltr_standard_selector_set_secondary_text(priv->standard_selector, _("Easyshift"));
		skeltr_standard_selector_set_index(priv->standard_selector, index,
				0,
				skeltr_key_relations[index].default_value,
				SKELTR_KEY_COMBO_BOX_GROUP_TALK,
				skeltr_key_relations[index].default_value_easyshift);
		roccat_multiwidget_show(priv->key_type_multiwidget, GTK_WIDGET(priv->standard_selector));
		break;
	case SKELTR_KEY_RELATION_TYPE_EASYZONE:
		skeltr_standard_selector_set_secondary_text(priv->standard_selector, _("Easyshift"));
		skeltr_standard_selector_set_index(priv->standard_selector, index,
				SKELTR_KEY_COMBO_BOX_GROUP_TALK | SKELTR_KEY_COMBO_BOX_GROUP_EXTENDED | SKELTR_KEY_COMBO_BOX_GROUP_KEY,
				skeltr_key_relations[index].default_value,
				SKELTR_KEY_COMBO_BOX_GROUP_TALK,
				skeltr_key_relations[index].default_value_easyshift);
		roccat_multiwidget_show(priv->key_type_multiwidget, GTK_WIDGET(priv->standard_selector));
		break;
	case SKELTR_KEY_RELATION_TYPE_FUNCTION:
		skeltr_standard_selector_set_secondary_text(priv->standard_selector, _("FN"));
		skeltr_standard_selector_set_index(priv->standard_selector, index,
				SKELTR_KEY_COMBO_BOX_GROUP_TALK | SKELTR_KEY_COMBO_BOX_GROUP_EXTENDED,
				skeltr_key_relations[index].default_value,
				SKELTR_KEY_COMBO_BOX_GROUP_TALK | SKELTR_KEY_COMBO_BOX_GROUP_EXTENDED,
				skeltr_key_relations[index].default_value_easyshift);
		roccat_multiwidget_show(priv->key_type_multiwidget, GTK_WIDGET(priv->standard_selector));
		break;
	case SKELTR_KEY_RELATION_TYPE_PRIMARY:
		skeltr_primary_selector_set_index(priv->primary_selector, index,
				SKELTR_KEY_COMBO_BOX_GROUP_TALK | SKELTR_KEY_COMBO_BOX_GROUP_EXTENDED | SKELTR_KEY_COMBO_BOX_GROUP_KEY,
				skeltr_key_relations[index].default_value);
		roccat_multiwidget_show(priv->key_type_multiwidget, GTK_WIDGET(priv->primary_selector));
		break;
	default:
		// error
		break;
	}
}

static void button_update_active(SkeltrKeyboardSelector *keyboard_selector) {
	guint key_index = get_active_index(keyboard_selector);

	selector_label_update(keyboard_selector, key_index);
	selector_show(keyboard_selector);
}

static void key_clicked_cb(GtkButton *button, gpointer user_data) {
	SkeltrKeyboardSelector *keyboard_selector = SKELTR_KEYBOARD_SELECTOR(user_data);
	guint index;

	if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button)))
		return;

	index = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(button), index_key));
	set_active_index(keyboard_selector, index);

	button_update_active(keyboard_selector);
}

static void button_set_active(SkeltrKeyboardSelector *selector, guint index) {
	SkeltrKeyboardSelectorPrivate *priv = selector->priv;
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

static void set_capslock_tooltip(SkeltrKeyboardSelector *keyboard_selector, guint index) {
	SkeltrKeyboardSelectorPrivate *priv = keyboard_selector->priv;
	gchar *text;

	if (!priv->keys[index])
		return;

	text = skeltr_capslock_selector_get_text_for_data(priv->capslock_selector, &priv->data->standard_datas[index]);
	gtk_widget_set_tooltip_text(GTK_WIDGET(priv->keys[index]), text);
	g_free(text);
}

static void set_fn_tooltip(SkeltrKeyboardSelector *keyboard_selector, guint index) {
	SkeltrKeyboardSelectorPrivate *priv = keyboard_selector->priv;
	gchar *text;
	guint value;

	if (!priv->keys[index])
		return;

	value = skeltr_fn_selector_get_value(priv->fn_selector);
	text = skeltr_fn_selector_get_text_for_value(priv->fn_selector, value);
	gtk_widget_set_tooltip_text(GTK_WIDGET(priv->keys[index]), text);
	g_free(text);
}

static void set_primary_tooltip(SkeltrKeyboardSelector *keyboard_selector, guint index) {
	SkeltrKeyboardSelectorPrivate *priv = keyboard_selector->priv;
	gchar *primary_text;
	gchar *text;

	if (!priv->keys[index])
		return;

	primary_text = skeltr_primary_selector_get_text_for_data(priv->primary_selector, &priv->data->standard_datas[index]);
	text = g_strdup_printf("%s", primary_text);
	gtk_widget_set_tooltip_text(GTK_WIDGET(priv->keys[index]), text);
	g_free(primary_text);
	g_free(text);
}

static void set_standard_tooltip(SkeltrKeyboardSelector *keyboard_selector, guint index) {
	SkeltrKeyboardSelectorPrivate *priv = keyboard_selector->priv;
	gchar *standard_text;
	gchar *easyshift_text;
	gchar *text;

	if (!priv->keys[index])
		return;

	standard_text = skeltr_standard_selector_get_text_for_data(priv->standard_selector, &priv->data->standard_datas[index]);
	easyshift_text = skeltr_standard_selector_get_text_for_data(priv->standard_selector, &priv->data->easyshift_datas[index]);
	text = g_strdup_printf(_("Standard: %s\nEasyshift: %s"), standard_text, easyshift_text);
	gtk_widget_set_tooltip_text(GTK_WIDGET(priv->keys[index]), text);
	g_free(standard_text);
	g_free(easyshift_text);
	g_free(text);
}

static gint find_index_by_type(guint type) {
	guint i;

	for (i = 0; i < SKELTR_KEYS_NUM; ++i)
		if (skeltr_key_relations[i].keys_type == type)
			return i;
	return -1;
}

static void fn_key_changed_cb(SkeltrFnSelector *fn_selector, gpointer user_data) {
	gint index;

	/* assuming there's only one */
	index = find_index_by_type(SKELTR_KEY_RELATION_TYPE_FN);
	if (index < 0)
		return;

	set_fn_tooltip(SKELTR_KEYBOARD_SELECTOR(user_data), index);
}

static void capslock_key_changed_cb(SkeltrCapslockSelector *capslock_selector, gint index, gpointer user_data) {
	SkeltrKeyboardSelector *keyboard_selector = SKELTR_KEYBOARD_SELECTOR(user_data);
	if (index >= 0)
		set_capslock_tooltip(keyboard_selector, index);
}

static void standard_key_changed_cb(SkeltrStandardSelector *standard_selector, gint index, gpointer user_data) {
	SkeltrKeyboardSelector *keyboard_selector = SKELTR_KEYBOARD_SELECTOR(user_data);
	if (index >= 0)
		set_standard_tooltip(keyboard_selector, index);
}

static void primary_key_changed_cb(SkeltrPrimarySelector *primary_selector, gint index, gpointer user_data) {
	SkeltrKeyboardSelector *keyboard_selector = SKELTR_KEYBOARD_SELECTOR(user_data);
	if (index >= 0)
		set_primary_tooltip(keyboard_selector, index);
}

static void update_keyboard(SkeltrKeyboardSelector *keyboard_selector) {
	selector_show(keyboard_selector);
}

static void create_key_type_multiwidget(SkeltrKeyboardSelector *keyboard_selector) {
	SkeltrKeyboardSelectorPrivate *priv = keyboard_selector->priv;

	priv->key_type_multiwidget = ROCCAT_MULTIWIDGET(roccat_multiwidget_new());
	priv->fn_selector = SKELTR_FN_SELECTOR(skeltr_fn_selector_new());
	priv->capslock_selector = SKELTR_CAPSLOCK_SELECTOR(skeltr_capslock_selector_new(priv->data->standard_datas));
	priv->primary_selector = SKELTR_PRIMARY_SELECTOR(skeltr_primary_selector_new(priv->data->standard_datas));
	priv->standard_selector = SKELTR_STANDARD_SELECTOR(skeltr_standard_selector_new(priv->data->standard_datas, priv->data->easyshift_datas));
	priv->nothing_selector = gtk_label_new(_("This key does not allow settings"));

	gtk_label_set_line_wrap(GTK_LABEL(priv->nothing_selector), TRUE);

	g_signal_connect(G_OBJECT(priv->fn_selector), "key-changed", G_CALLBACK(fn_key_changed_cb), keyboard_selector);
	g_signal_connect(G_OBJECT(priv->capslock_selector), "key-changed", G_CALLBACK(capslock_key_changed_cb), keyboard_selector);
	g_signal_connect(G_OBJECT(priv->standard_selector), "key-changed", G_CALLBACK(standard_key_changed_cb), keyboard_selector);
	g_signal_connect(G_OBJECT(priv->primary_selector), "key-changed", G_CALLBACK(primary_key_changed_cb), keyboard_selector);

	roccat_multiwidget_add(priv->key_type_multiwidget, GTK_WIDGET(priv->fn_selector));
	roccat_multiwidget_add(priv->key_type_multiwidget, GTK_WIDGET(priv->capslock_selector));
	roccat_multiwidget_add(priv->key_type_multiwidget, GTK_WIDGET(priv->standard_selector));
	roccat_multiwidget_add(priv->key_type_multiwidget, GTK_WIDGET(priv->primary_selector));
	roccat_multiwidget_add(priv->key_type_multiwidget, priv->nothing_selector);
}

static void skeltr_keyboard_selector_init(SkeltrKeyboardSelector *keyboard_selector) {
	SkeltrKeyboardSelectorPrivate *priv = SKELTR_KEYBOARD_SELECTOR_GET_PRIVATE(keyboard_selector);
	keyboard_selector->priv = priv;
	GtkWidget *keyboard_frame;
	GtkWidget *vbox;
	GtkWidget *key_frame;
	GtkWidget *hbox;
	GtkWidget *key_vbox;

	priv->data = (SkeltrKeyboardSelectorPrivateData *)g_malloc(sizeof(SkeltrKeyboardSelectorPrivateData));

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
	SkeltrKeyboardSelector *selector = SKELTR_KEYBOARD_SELECTOR(object);
	SkeltrKeyboardSelectorPrivate *priv = selector->priv;

	g_free(priv->data);

	G_OBJECT_CLASS(skeltr_keyboard_selector_parent_class)->finalize(object);
}

static void skeltr_keyboard_selector_class_init(SkeltrKeyboardSelectorClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = (GObjectClass*)klass;
	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(SkeltrKeyboardSelectorPrivate));

	signals[LAYER_EDITOR] = g_signal_new("layer-editor",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_BOOLEAN__POINTER, G_TYPE_BOOLEAN, 1, G_TYPE_POINTER);
}

static void create_key(RoccatKeyButton **group, SkeltrKeyboardSelector *keyboard_selector, SkeltrGuiKeyPosition const *position,
		guint column_offset, guint row_offset) {
	SkeltrKeyboardSelectorPrivate *priv = keyboard_selector->priv;
	guint relation_index = position->relation_index;

	priv->keys[relation_index] = ROCCAT_KEY_BUTTON(roccat_key_button_new(*group, skeltr_key_relations[relation_index].default_value, skeltr_hid_to_special_keyname));
	g_object_set_data(G_OBJECT(priv->keys[relation_index]), index_key, GUINT_TO_POINTER(relation_index));
	g_signal_connect(G_OBJECT(priv->keys[relation_index]), "clicked", G_CALLBACK(key_clicked_cb), keyboard_selector);

	gtk_table_attach(priv->table, GTK_WIDGET(priv->keys[relation_index]),
			position->column + column_offset, position->column + column_offset + position->width,
			position->row + row_offset, position->row + row_offset + position->height,
			GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

	*group = priv->keys[relation_index];
}

static void create_block(RoccatKeyButton **group, SkeltrKeyboardSelector *keyboard_selector, SkeltrGuiKeyPosition const *positions,
		guint column_offset, guint row_offset) {
	guint i;

	for (i = 0; positions[i].relation_index != SKELTR_GUI_KEY_POSITION_INDEX_END; ++i)
		create_key(group, keyboard_selector, &positions[i], column_offset, row_offset);
}

static void clear_keys(SkeltrKeyboardSelector *keyboard_selector) {
	SkeltrKeyboardSelectorPrivate *priv = keyboard_selector->priv;
	guint i;

	for (i = 0; i < SKELTR_KEYS_NUM; ++i) {
		if (priv->keys[i]) {
			gtk_widget_destroy(GTK_WIDGET(priv->keys[i]));
			priv->keys[i] = NULL;
		}
	}
}

static void set_tooltips(SkeltrKeyboardSelector *keyboard_selector) {
	guint i;

	for (i = 0; i < SKELTR_KEYS_NUM; ++i) {
		switch (skeltr_key_relations[i].keys_type) {
		case SKELTR_KEY_RELATION_TYPE_NOTHING:
			break;
		case SKELTR_KEY_RELATION_TYPE_FN:
			set_fn_tooltip(keyboard_selector, i);
			__attribute__ ((fallthrough));
		case SKELTR_KEY_RELATION_TYPE_CAPSLOCK:
			set_capslock_tooltip(keyboard_selector, i);
			break;
		case SKELTR_KEY_RELATION_TYPE_MACRO:
		case SKELTR_KEY_RELATION_TYPE_THUMBSTER:
		case SKELTR_KEY_RELATION_TYPE_FUNCTION:
		case SKELTR_KEY_RELATION_TYPE_EASYZONE:
			set_standard_tooltip(keyboard_selector, i);
			break;
		case SKELTR_KEY_RELATION_TYPE_PRIMARY:
			set_primary_tooltip(keyboard_selector, i);
			break;
		default:
			break;
		};
	}
}

void skeltr_keyboard_selector_set_layout(SkeltrKeyboardSelector *keyboard_selector, gchar const *layout) {
	RoccatKeyButton *group = NULL;

	clear_keys(keyboard_selector);
	create_block(&group, keyboard_selector, skeltr_gui_key_positions_general, 0, 0);
	create_block(&group, keyboard_selector, skeltr_gui_key_positions_get(layout), 0, 0);

	set_tooltips(keyboard_selector);
	gtk_widget_show_all(GTK_WIDGET(keyboard_selector->priv->table));
	button_set_active(keyboard_selector, 0);
	update_keyboard(keyboard_selector);
}

void skeltr_keyboard_selector_set_from_profile_data(SkeltrKeyboardSelector *selector, SkeltrProfileData const *profile_data) {
	SkeltrKeyboardSelectorPrivate *priv = selector->priv;
	SkeltrKeyboardSelectorPrivateData *data = priv->data;
	SkeltrKeyComboBoxData *standarddata;
	SkeltrKeyComboBoxData *easyshiftdata;
	guint i;
	SkeltrKeyRelation const *key;

	for (i = 0; i < SKELTR_KEYS_NUM; ++i) {
		standarddata = &data->standard_datas[i];
		easyshiftdata = &data->easyshift_datas[i];
		key = &skeltr_key_relations[i];

		switch (key->keys_type) {
		case SKELTR_KEY_RELATION_TYPE_NOTHING:
			break;
		case SKELTR_KEY_RELATION_TYPE_FN:
			skeltr_fn_selector_set_value(priv->fn_selector, skeltr_profile_data_get_keys_extra(profile_data)->fn);
			break;
		case SKELTR_KEY_RELATION_TYPE_CAPSLOCK:
			roccat_button_set_to_normal(&standarddata->key, skeltr_profile_data_get_keys_extra(profile_data)->capslock);
			standarddata->talk_target = skeltr_profile_data_get_capslock_talk_target(profile_data);
			break;
		case SKELTR_KEY_RELATION_TYPE_MACRO:
			roccat_button_copy(&standarddata->key, &skeltr_profile_data_get_keys_macro(profile_data)->keys[key->keys_index]);
			roccat_button_copy(&easyshiftdata->key, &skeltr_profile_data_get_keys_macro(profile_data)->keys[key->keys_index_easyshift]);
			skeltr_macro_copy(&standarddata->macro, skeltr_profile_data_get_macro(profile_data, key->macro_index));
			skeltr_macro_copy(&easyshiftdata->macro, skeltr_profile_data_get_macro(profile_data, key->macro_index_easyshift));
			g_strlcpy(standarddata->opener, skeltr_profile_data_get_opener(profile_data, key->macro_index), ROCCAT_SWARM_RMP_OPENER_LENGTH);
			g_strlcpy(easyshiftdata->opener, skeltr_profile_data_get_opener(profile_data, key->macro_index_easyshift), ROCCAT_SWARM_RMP_OPENER_LENGTH);
			roccat_timer_copy(&standarddata->timer, skeltr_profile_data_get_timer(profile_data, key->macro_index));
			roccat_timer_copy(&easyshiftdata->timer, skeltr_profile_data_get_timer(profile_data, key->macro_index_easyshift));
			break;
		case SKELTR_KEY_RELATION_TYPE_THUMBSTER:
			roccat_button_copy(&standarddata->key, &skeltr_profile_data_get_keys_thumbster(profile_data)->keys[key->keys_index]);
			roccat_button_copy(&easyshiftdata->key, &skeltr_profile_data_get_keys_thumbster(profile_data)->keys[key->keys_index_easyshift]);
			skeltr_macro_copy(&standarddata->macro, skeltr_profile_data_get_macro(profile_data, key->macro_index));
			skeltr_macro_copy(&easyshiftdata->macro, skeltr_profile_data_get_macro(profile_data, key->macro_index_easyshift));
			g_strlcpy(standarddata->opener, skeltr_profile_data_get_opener(profile_data, key->macro_index), ROCCAT_SWARM_RMP_OPENER_LENGTH);
			g_strlcpy(easyshiftdata->opener, skeltr_profile_data_get_opener(profile_data, key->macro_index_easyshift), ROCCAT_SWARM_RMP_OPENER_LENGTH);
			roccat_timer_copy(&standarddata->timer, skeltr_profile_data_get_timer(profile_data, key->macro_index));
			roccat_timer_copy(&easyshiftdata->timer, skeltr_profile_data_get_timer(profile_data, key->macro_index_easyshift));
			break;
		case SKELTR_KEY_RELATION_TYPE_FUNCTION:
			roccat_button_copy(&standarddata->key, &skeltr_profile_data_get_keys_function(profile_data)->keys[key->keys_index]);
			roccat_button_copy(&easyshiftdata->key, &skeltr_profile_data_get_keys_function(profile_data)->keys[key->keys_index_easyshift]);
			break;
		case SKELTR_KEY_RELATION_TYPE_EASYZONE:
			roccat_button_set_to_normal(&standarddata->key, skeltr_profile_data_get_keys_primary(profile_data)->keys[key->keys_index]);
			roccat_button_copy(&easyshiftdata->key, &skeltr_profile_data_get_keys_easyzone(profile_data)->keys[key->keys_index_easyshift]);
			skeltr_macro_copy(&easyshiftdata->macro, skeltr_profile_data_get_macro(profile_data, key->macro_index_easyshift));
			g_strlcpy(easyshiftdata->opener, skeltr_profile_data_get_opener(profile_data, key->macro_index_easyshift), ROCCAT_SWARM_RMP_OPENER_LENGTH);
			roccat_timer_copy(&easyshiftdata->timer, skeltr_profile_data_get_timer(profile_data, key->macro_index_easyshift));
			break;
		case SKELTR_KEY_RELATION_TYPE_PRIMARY:
			roccat_button_set_to_normal(&standarddata->key, skeltr_profile_data_get_keys_primary(profile_data)->keys[key->keys_index]);
			break;
		default:
			break;
		};
	}

	set_tooltips(selector);
	button_update_active(selector);
	update_keyboard(selector);
}

void skeltr_keyboard_selector_update_profile_data(SkeltrKeyboardSelector *selector, SkeltrProfileData *profile_data) {
	SkeltrKeyboardSelectorPrivate *priv = selector->priv;
	SkeltrKeyboardSelectorPrivateData *data = priv->data;
	SkeltrKeysPrimary keys_primary;
	SkeltrKeysEasyzone keys_easyzone;
	SkeltrKeysFunction keys_function;
	SkeltrKeysThumbster keys_thumbster;
	SkeltrKeysMacro keys_macro;
	SkeltrKeysExtra keys_extra;
	SkeltrKeyComboBoxData *standarddata;
	SkeltrKeyComboBoxData *easyshiftdata;
	guint i;
	SkeltrKeyRelation const *key;

	skeltr_keys_primary_copy(&keys_primary, skeltr_profile_data_get_keys_primary(profile_data));
	skeltr_keys_easyzone_copy(&keys_easyzone, skeltr_profile_data_get_keys_easyzone(profile_data));
	skeltr_keys_function_copy(&keys_function, skeltr_profile_data_get_keys_function(profile_data));
	skeltr_keys_extra_copy(&keys_extra, skeltr_profile_data_get_keys_extra(profile_data));
	skeltr_keys_macro_copy(&keys_macro, skeltr_profile_data_get_keys_macro(profile_data));
	skeltr_keys_thumbster_copy(&keys_thumbster, skeltr_profile_data_get_keys_thumbster(profile_data));

	for (i = 0; i < SKELTR_KEYS_NUM; ++i) {
		standarddata = &data->standard_datas[i];
		easyshiftdata = &data->easyshift_datas[i];
		key = &skeltr_key_relations[i];

		switch (key->keys_type) {
		case SKELTR_KEY_RELATION_TYPE_NOTHING:
			break;
		case SKELTR_KEY_RELATION_TYPE_FN:
			keys_extra.fn = skeltr_fn_selector_get_value(priv->fn_selector);
			break;
		case SKELTR_KEY_RELATION_TYPE_MACRO:
			roccat_button_copy(&keys_macro.keys[key->keys_index], &standarddata->key);
			roccat_button_copy(&keys_macro.keys[key->keys_index_easyshift], &easyshiftdata->key);
			skeltr_profile_data_set_macro(profile_data, key->macro_index, &standarddata->macro);
			skeltr_profile_data_set_macro(profile_data, key->macro_index_easyshift, &easyshiftdata->macro);
			skeltr_profile_data_set_opener(profile_data, key->macro_index, standarddata->opener);
			skeltr_profile_data_set_opener(profile_data, key->macro_index_easyshift, easyshiftdata->opener);
			skeltr_profile_data_set_timer(profile_data, key->macro_index, &standarddata->timer);
			skeltr_profile_data_set_timer(profile_data, key->macro_index_easyshift, &easyshiftdata->timer);
			break;
		case SKELTR_KEY_RELATION_TYPE_THUMBSTER:
			roccat_button_copy(&keys_thumbster.keys[key->keys_index], &standarddata->key);
			roccat_button_copy(&keys_thumbster.keys[key->keys_index_easyshift], &easyshiftdata->key);
			skeltr_profile_data_set_macro(profile_data, key->macro_index, &standarddata->macro);
			skeltr_profile_data_set_macro(profile_data, key->macro_index_easyshift, &easyshiftdata->macro);
			skeltr_profile_data_set_opener(profile_data, key->macro_index, standarddata->opener);
			skeltr_profile_data_set_opener(profile_data, key->macro_index_easyshift, easyshiftdata->opener);
			skeltr_profile_data_set_timer(profile_data, key->macro_index, &standarddata->timer);
			skeltr_profile_data_set_timer(profile_data, key->macro_index_easyshift, &easyshiftdata->timer);
			break;
		case SKELTR_KEY_RELATION_TYPE_EASYZONE:
			keys_primary.keys[key->keys_index] = standarddata->key.type;
			roccat_button_copy(&keys_easyzone.keys[key->keys_index_easyshift], &easyshiftdata->key);
			skeltr_profile_data_set_macro(profile_data, key->macro_index_easyshift, &easyshiftdata->macro);
			skeltr_profile_data_set_opener(profile_data, key->macro_index_easyshift, easyshiftdata->opener);
			skeltr_profile_data_set_timer(profile_data, key->macro_index_easyshift, &easyshiftdata->timer);
			break;
		case SKELTR_KEY_RELATION_TYPE_FUNCTION:
			roccat_button_copy(&keys_function.keys[key->keys_index], &standarddata->key);
			roccat_button_copy(&keys_function.keys[key->keys_index_easyshift], &easyshiftdata->key);
			break;
		case SKELTR_KEY_RELATION_TYPE_CAPSLOCK:
			keys_extra.capslock = standarddata->key.type;
			skeltr_profile_data_set_capslock_talk_target(profile_data, ROCCAT_TALK_DEVICE_MOUSE);
			break;
		case SKELTR_KEY_RELATION_TYPE_PRIMARY:
			keys_primary.keys[key->keys_index] = standarddata->key.type;
			break;
		};
	}

	skeltr_profile_data_set_keys_primary(profile_data, &keys_primary);
	skeltr_profile_data_set_keys_easyzone(profile_data, &keys_easyzone);
	skeltr_profile_data_set_keys_function(profile_data, &keys_function);
	skeltr_profile_data_set_keys_extra(profile_data, &keys_extra);
	skeltr_profile_data_set_keys_macro(profile_data, &keys_macro);
	skeltr_profile_data_set_keys_thumbster(profile_data, &keys_thumbster);
}

void skeltr_keyboard_selector_set_macro(SkeltrKeyboardSelector *selector, guint macro_index, SkeltrMacro *macro) {
	SkeltrKeyboardSelectorPrivate *priv = selector->priv;
	guint i;

	for (i = 0; i < SKELTR_KEYS_NUM; ++i) {
		if (macro_index == skeltr_key_relations[i].macro_index) {
			roccat_button_set_to_normal(&priv->data->standard_datas[i].key, SKELTR_KEY_TYPE_MACRO);
			skeltr_macro_copy(&priv->data->standard_datas[i].macro, macro);
			break;
		}
		else if (macro_index == skeltr_key_relations[i].macro_index_easyshift) {
			roccat_button_set_to_normal(&priv->data->easyshift_datas[i].key, SKELTR_KEY_TYPE_MACRO);
			skeltr_macro_copy(&priv->data->easyshift_datas[i].macro, macro);
			break;
		}
	}

	if (i >= SKELTR_KEYS_NUM)
		return; // FIXME error

	set_standard_tooltip(selector, i);
	if (get_active_index(selector) == i)
		skeltr_standard_selector_update(priv->standard_selector);
}
