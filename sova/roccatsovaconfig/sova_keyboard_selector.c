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
#include "sova_keyboard_selector.h"
#include "sova_standard_selector.h"
#include "sova_capslock_selector.h"
#include "sova_primary_selector.h"
#include "roccat_key_button.h"
#include "roccat_talk.h"
#include "sova_key_relations.h"
#include "sova_gui_key_relations.h"
#include "roccat_multiwidget.h"
#include "g_cclosure_roccat_marshaller.h"
#include "g_roccat_helper.h"
#include "gdk_roccat_helper.h"
#include "config.h"
#include "i18n.h"

#define SOVA_KEYBOARD_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), SOVA_KEYBOARD_SELECTOR_TYPE, SovaKeyboardSelectorClass))
#define IS_SOVA_KEYBOARD_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), SOVA_KEYBOARD_SELECTOR_TYPE))
#define SOVA_KEYBOARD_SELECTOR_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), SOVA_KEYBOARD_SELECTOR_TYPE, SovaKeyboardSelectorClass))
#define SOVA_KEYBOARD_SELECTOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), SOVA_KEYBOARD_SELECTOR_TYPE, SovaKeyboardSelectorPrivate))

typedef struct _SovaKeyboardSelectorClass SovaKeyboardSelectorClass;
typedef struct _SovaKeyboardSelectorPrivate SovaKeyboardSelectorPrivate;
typedef struct _SovaKeyboardSelectorPrivateData SovaKeyboardSelectorPrivateData;

struct _SovaKeyboardSelector {
	GtkFrame parent;
	SovaKeyboardSelectorPrivate *priv;
};

struct _SovaKeyboardSelectorClass {
	GtkFrameClass parent_class;
};

enum {
	TABLE_HEIGHT = 25,
	TABLE_WIDTH = 64,
};

static gchar const * const index_key = "index";

struct _SovaKeyboardSelectorPrivate {
	RoccatKeyButton *keys[SOVA_KEYS_NUM];
	GtkScrolledWindow *scroll_window;
	GtkTable *table;

	GtkLabel *label;
	guint actual_index;

	RoccatMultiwidget *key_type_multiwidget;
	SovaPrimarySelector *primary_selector;
	SovaCapslockSelector *capslock_selector;
	SovaStandardSelector *standard_selector;
	GtkWidget *nothing_selector;

	/* private object data can only be 64kb in size */
	SovaKeyboardSelectorPrivateData *data;
};

struct _SovaKeyboardSelectorPrivateData {
	SovaKeyComboBoxData standard_datas[SOVA_KEYS_NUM];
	SovaKeyComboBoxData easyshift_datas[SOVA_KEYS_NUM];
};

G_DEFINE_TYPE(SovaKeyboardSelector, sova_keyboard_selector, GTK_TYPE_FRAME);

enum {
	LAYER_EDITOR,
	LAST_SIGNAL,
};

static guint signals[LAST_SIGNAL] = { 0 };

GtkWidget *sova_keyboard_selector_new(void) {
	return GTK_WIDGET(g_object_new(SOVA_KEYBOARD_SELECTOR_TYPE,
			"label", _("Keys"),
			NULL));
}

static guint get_active_index(SovaKeyboardSelector *selector) {
	return selector->priv->actual_index;
}

static void set_active_index(SovaKeyboardSelector *selector, guint index) {
	selector->priv->actual_index = index;
}

static void selector_label_update(SovaKeyboardSelector *selector, guint index) {
	SovaKeyboardSelectorPrivate *priv = selector->priv;
	gchar *string;

	string = gaminggear_hid_to_name(sova_key_relations[index].primary_default);
	gtk_label_set_text(priv->label, string);
	g_free(string);
}

static void selector_show(SovaKeyboardSelector *selector) {
	SovaKeyboardSelectorPrivate *priv = selector->priv;
	guint index = get_active_index(selector);

	switch (sova_key_relations[index].type) {
	case SOVA_KEY_RELATION_TYPE_NOTHING:
		roccat_multiwidget_show(priv->key_type_multiwidget, priv->nothing_selector);
		break;
	case SOVA_KEY_RELATION_TYPE_CAPSLOCK:
		sova_capslock_selector_set_index(priv->capslock_selector, index,
				SOVA_KEY_COMBO_BOX_GROUP_PRIMARY | SOVA_KEY_COMBO_BOX_GROUP_EXTENDED | SOVA_KEY_COMBO_BOX_GROUP_KEY,
				sova_key_relations[index].primary_default);
		roccat_multiwidget_show(priv->key_type_multiwidget, GTK_WIDGET(priv->capslock_selector));
		break;
	case SOVA_KEY_RELATION_TYPE_FUNCTION:
		sova_standard_selector_set_secondary_text(priv->standard_selector, _("FN"));
		sova_standard_selector_set_index(priv->standard_selector, index,
				SOVA_KEY_COMBO_BOX_GROUP_TALK | SOVA_KEY_COMBO_BOX_GROUP_EXTENDED,
				sova_key_relations[index].primary_default,
				SOVA_KEY_COMBO_BOX_GROUP_TALK | SOVA_KEY_COMBO_BOX_GROUP_EXTENDED,
				sova_key_relations[index].fn_default);
		roccat_multiwidget_show(priv->key_type_multiwidget, GTK_WIDGET(priv->standard_selector));
		break;
	case SOVA_KEY_RELATION_TYPE_EASYZONE:
		sova_standard_selector_set_secondary_text(priv->standard_selector, _("Easyshift"));
		sova_standard_selector_set_index(priv->standard_selector, index,
				SOVA_KEY_COMBO_BOX_GROUP_TALK | SOVA_KEY_COMBO_BOX_GROUP_EXTENDED | SOVA_KEY_COMBO_BOX_GROUP_KEY,
				sova_key_relations[index].primary_default,
				SOVA_KEY_COMBO_BOX_GROUP_TALK,
				sova_key_relations[index].primary_default);
		roccat_multiwidget_show(priv->key_type_multiwidget, GTK_WIDGET(priv->standard_selector));
		break;
	case SOVA_KEY_RELATION_TYPE_PRIMARY:
		sova_primary_selector_set_index(priv->primary_selector, index,
				SOVA_KEY_COMBO_BOX_GROUP_TALK | SOVA_KEY_COMBO_BOX_GROUP_EXTENDED | SOVA_KEY_COMBO_BOX_GROUP_KEY,
				sova_key_relations[index].primary_default);
		roccat_multiwidget_show(priv->key_type_multiwidget, GTK_WIDGET(priv->primary_selector));
		break;
	default:
		// error
		break;
	}
}

static void button_update_active(SovaKeyboardSelector *keyboard_selector) {
	guint key_index = get_active_index(keyboard_selector);

	selector_label_update(keyboard_selector, key_index);
	selector_show(keyboard_selector);
}

static void key_clicked_cb(GtkButton *button, gpointer user_data) {
	SovaKeyboardSelector *keyboard_selector = SOVA_KEYBOARD_SELECTOR(user_data);
	guint index;

	if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button)))
		return;

	index = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(button), index_key));
	set_active_index(keyboard_selector, index);

	button_update_active(keyboard_selector);
}

static void button_set_active(SovaKeyboardSelector *selector, guint index) {
	SovaKeyboardSelectorPrivate *priv = selector->priv;
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

static void set_capslock_tooltip(SovaKeyboardSelector *keyboard_selector, guint index) {
	SovaKeyboardSelectorPrivate *priv = keyboard_selector->priv;
	gchar *text;

	if (!priv->keys[index])
		return;

	text = sova_capslock_selector_get_text_for_data(priv->capslock_selector, &priv->data->standard_datas[index]);
	gtk_widget_set_tooltip_text(GTK_WIDGET(priv->keys[index]), text);
	g_free(text);
}

static void set_primary_tooltip(SovaKeyboardSelector *keyboard_selector, guint index) {
	SovaKeyboardSelectorPrivate *priv = keyboard_selector->priv;
	gchar *primary_text;
	gchar *text;

	if (!priv->keys[index])
		return;

	primary_text = sova_primary_selector_get_text_for_data(priv->primary_selector, &priv->data->standard_datas[index]);
	text = g_strdup_printf("%s", primary_text);
	gtk_widget_set_tooltip_text(GTK_WIDGET(priv->keys[index]), text);
	g_free(primary_text);
	g_free(text);
}

static void set_standard_tooltip(SovaKeyboardSelector *keyboard_selector, guint index) {
	SovaKeyboardSelectorPrivate *priv = keyboard_selector->priv;
	gchar *standard_text;
	gchar *easyshift_text;
	gchar *text;

	if (!priv->keys[index])
		return;

	standard_text = sova_standard_selector_get_text_for_data(priv->standard_selector, &priv->data->standard_datas[index]);
	easyshift_text = sova_standard_selector_get_text_for_data(priv->standard_selector, &priv->data->easyshift_datas[index]);
	text = g_strdup_printf(_("Standard: %s\nEasyshift: %s"), standard_text, easyshift_text);
	gtk_widget_set_tooltip_text(GTK_WIDGET(priv->keys[index]), text);
	g_free(standard_text);
	g_free(easyshift_text);
	g_free(text);
}

static void capslock_key_changed_cb(SovaCapslockSelector *capslock_selector, gint index, gpointer user_data) {
	SovaKeyboardSelector *keyboard_selector = SOVA_KEYBOARD_SELECTOR(user_data);
	if (index >= 0)
		set_capslock_tooltip(keyboard_selector, index);
}

static void standard_key_changed_cb(SovaStandardSelector *standard_selector, gint index, gpointer user_data) {
	SovaKeyboardSelector *keyboard_selector = SOVA_KEYBOARD_SELECTOR(user_data);
	if (index >= 0)
		set_standard_tooltip(keyboard_selector, index);
}

static void primary_key_changed_cb(SovaPrimarySelector *primary_selector, gint index, gpointer user_data) {
	SovaKeyboardSelector *keyboard_selector = SOVA_KEYBOARD_SELECTOR(user_data);
	if (index >= 0)
		set_primary_tooltip(keyboard_selector, index);
}

static void update_keyboard(SovaKeyboardSelector *keyboard_selector) {
	selector_show(keyboard_selector);
}

static void create_key_type_multiwidget(SovaKeyboardSelector *keyboard_selector) {
	SovaKeyboardSelectorPrivate *priv = keyboard_selector->priv;

	priv->key_type_multiwidget = ROCCAT_MULTIWIDGET(roccat_multiwidget_new());
	priv->capslock_selector = SOVA_CAPSLOCK_SELECTOR(sova_capslock_selector_new(priv->data->standard_datas));
	priv->primary_selector = SOVA_PRIMARY_SELECTOR(sova_primary_selector_new(priv->data->standard_datas));
	priv->standard_selector = SOVA_STANDARD_SELECTOR(sova_standard_selector_new(priv->data->standard_datas, priv->data->easyshift_datas));
	priv->nothing_selector = gtk_label_new(_("This key does not allow settings"));

	gtk_label_set_line_wrap(GTK_LABEL(priv->nothing_selector), TRUE);

	g_signal_connect(G_OBJECT(priv->capslock_selector), "key-changed", G_CALLBACK(capslock_key_changed_cb), keyboard_selector);
	g_signal_connect(G_OBJECT(priv->standard_selector), "key-changed", G_CALLBACK(standard_key_changed_cb), keyboard_selector);
	g_signal_connect(G_OBJECT(priv->primary_selector), "key-changed", G_CALLBACK(primary_key_changed_cb), keyboard_selector);

	roccat_multiwidget_add(priv->key_type_multiwidget, GTK_WIDGET(priv->capslock_selector));
	roccat_multiwidget_add(priv->key_type_multiwidget, GTK_WIDGET(priv->standard_selector));
	roccat_multiwidget_add(priv->key_type_multiwidget, GTK_WIDGET(priv->primary_selector));
	roccat_multiwidget_add(priv->key_type_multiwidget, priv->nothing_selector);
}

static void sova_keyboard_selector_init(SovaKeyboardSelector *keyboard_selector) {
	SovaKeyboardSelectorPrivate *priv = SOVA_KEYBOARD_SELECTOR_GET_PRIVATE(keyboard_selector);
	keyboard_selector->priv = priv;
	GtkWidget *keyboard_frame;
	GtkWidget *vbox;
	GtkWidget *key_frame;
	GtkWidget *hbox;
	GtkWidget *key_vbox;

	priv->data = (SovaKeyboardSelectorPrivateData *)g_malloc(sizeof(SovaKeyboardSelectorPrivateData));

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
	SovaKeyboardSelector *selector = SOVA_KEYBOARD_SELECTOR(object);
	SovaKeyboardSelectorPrivate *priv = selector->priv;

	g_free(priv->data);

	G_OBJECT_CLASS(sova_keyboard_selector_parent_class)->finalize(object);
}

static void sova_keyboard_selector_class_init(SovaKeyboardSelectorClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = (GObjectClass*)klass;
	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(SovaKeyboardSelectorPrivate));

	signals[LAYER_EDITOR] = g_signal_new("layer-editor",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_BOOLEAN__POINTER, G_TYPE_BOOLEAN, 1, G_TYPE_POINTER);
}

static void create_key(RoccatKeyButton **group, SovaKeyboardSelector *keyboard_selector, SovaGuiKeyPosition const *position,
		guint column_offset, guint row_offset) {
	SovaKeyboardSelectorPrivate *priv = keyboard_selector->priv;
	guint relation_index = position->relation_index;

	priv->keys[relation_index] = ROCCAT_KEY_BUTTON(roccat_key_button_new(*group, sova_key_relations[relation_index].primary_default, NULL));
	g_object_set_data(G_OBJECT(priv->keys[relation_index]), index_key, GUINT_TO_POINTER(relation_index));
	g_signal_connect(G_OBJECT(priv->keys[relation_index]), "clicked", G_CALLBACK(key_clicked_cb), keyboard_selector);

	gtk_table_attach(priv->table, GTK_WIDGET(priv->keys[relation_index]),
			position->column + column_offset, position->column + column_offset + position->width,
			position->row + row_offset, position->row + row_offset + position->height,
			GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

	*group = priv->keys[relation_index];
}

static void create_block(RoccatKeyButton **group, SovaKeyboardSelector *keyboard_selector, SovaGuiKeyPosition const *positions,
		guint column_offset, guint row_offset) {
	guint i;

	for (i = 0; positions[i].relation_index != SOVA_GUI_KEY_POSITION_INDEX_END; ++i)
		create_key(group, keyboard_selector, &positions[i], column_offset, row_offset);
}

static void clear_keys(SovaKeyboardSelector *keyboard_selector) {
	SovaKeyboardSelectorPrivate *priv = keyboard_selector->priv;
	guint i;

	for (i = 0; i < SOVA_KEYS_NUM; ++i) {
		if (priv->keys[i]) {
			gtk_widget_destroy(GTK_WIDGET(priv->keys[i]));
			priv->keys[i] = NULL;
		}
	}
}

static void set_tooltips(SovaKeyboardSelector *keyboard_selector) {
	guint i;

	for (i = 0; i < SOVA_KEYS_NUM; ++i) {
		switch (sova_key_relations[i].type) {
		case SOVA_KEY_RELATION_TYPE_PRIMARY:
			set_primary_tooltip(keyboard_selector, i);
			break;
		case SOVA_KEY_RELATION_TYPE_EASYZONE:
		case SOVA_KEY_RELATION_TYPE_FUNCTION:
			set_standard_tooltip(keyboard_selector, i);
			break;
		case SOVA_KEY_RELATION_TYPE_CAPSLOCK:
			set_capslock_tooltip(keyboard_selector, i);
			break;
		default:
			break;
		};
	}
}

void sova_keyboard_selector_set_layout(SovaKeyboardSelector *keyboard_selector, gchar const *layout) {
	RoccatKeyButton *group = NULL;

	clear_keys(keyboard_selector);
	create_block(&group, keyboard_selector, sova_gui_key_positions_general, 0, 0);
	create_block(&group, keyboard_selector, sova_gui_key_positions_get(layout), 0, 0);

	set_tooltips(keyboard_selector);
	gtk_widget_show_all(GTK_WIDGET(keyboard_selector->priv->table));
	button_set_active(keyboard_selector, 0);
	update_keyboard(keyboard_selector);
}

void sova_keyboard_selector_set_from_profile_data(SovaKeyboardSelector *selector, SovaProfileData const *profile_data) {
	SovaKeyboardSelectorPrivate *priv = selector->priv;
	SovaKeyboardSelectorPrivateData *data = priv->data;
	SovaKeyComboBoxData *standarddata;
	SovaKeyComboBoxData *easyshiftdata;
	guint i;
	SovaKeyRelation const *key;

	for (i = 0; i < SOVA_KEYS_NUM; ++i) {
		standarddata = &data->standard_datas[i];
		easyshiftdata = &data->easyshift_datas[i];
		key = &sova_key_relations[i];

		switch (key->type) {
		case SOVA_KEY_RELATION_TYPE_EASYZONE:
			roccat_button_set_to_normal(&standarddata->key, sova_profile_data_get_keys_primary(profile_data)->keys[key->primary_index]);
			roccat_button_copy(&easyshiftdata->key, &sova_profile_data_get_keys_easyzone(profile_data)->keys[key->easyzone_index]);
			sova_macro_copy(&easyshiftdata->macro, sova_profile_data_get_macro(profile_data, key->easyzone_index));
			g_strlcpy(easyshiftdata->opener, sova_profile_data_get_opener(profile_data, key->easyzone_index), ROCCAT_SWARM_RMP_OPENER_LENGTH);
			roccat_timer_copy(&easyshiftdata->timer, sova_profile_data_get_timer(profile_data, key->easyzone_index));
			break;
		case SOVA_KEY_RELATION_TYPE_FUNCTION:
			roccat_button_copy(&standarddata->key, &sova_profile_data_get_keys_function(profile_data)->keys_normal[key->function_index]);
			roccat_button_copy(&easyshiftdata->key, &sova_profile_data_get_keys_function(profile_data)->keys_fn[key->function_index]);
			break;
		case SOVA_KEY_RELATION_TYPE_CAPSLOCK:
			roccat_button_set_to_normal(&standarddata->key, sova_profile_data_get_keys_extra(profile_data)->capslock);
			standarddata->talk_target = sova_profile_data_get_capslock_talk_target(profile_data);
			break;
		case SOVA_KEY_RELATION_TYPE_PRIMARY:
			roccat_button_set_to_normal(&standarddata->key, sova_profile_data_get_keys_primary(profile_data)->keys[key->primary_index]);
			break;
		default:
			break;
		};
	}

	set_tooltips(selector);
	button_update_active(selector);
	update_keyboard(selector);
}

void sova_keyboard_selector_update_profile_data(SovaKeyboardSelector *selector, SovaProfileData *profile_data) {
	SovaKeyboardSelectorPrivate *priv = selector->priv;
	SovaKeyboardSelectorPrivateData *data = priv->data;
	SovaKeysPrimary keys_primary;
	SovaKeysEasyzone keys_easyzone;
	SovaKeysFunction keys_function;
	SovaKeysExtra keys_extra;
	SovaKeyComboBoxData *standarddata;
	SovaKeyComboBoxData *easyshiftdata;
	guint i;
	SovaKeyRelation const *key;

	sova_keys_primary_copy(&keys_primary, sova_profile_data_get_keys_primary(profile_data));
	sova_keys_easyzone_copy(&keys_easyzone, sova_profile_data_get_keys_easyzone(profile_data));
	sova_keys_function_copy(&keys_function, sova_profile_data_get_keys_function(profile_data));
	sova_keys_extra_copy(&keys_extra, sova_profile_data_get_keys_extra(profile_data));

	for (i = 0; i < SOVA_KEYS_NUM; ++i) {
		standarddata = &data->standard_datas[i];
		easyshiftdata = &data->easyshift_datas[i];
		key = &sova_key_relations[i];

		switch (key->type) {
		case SOVA_KEY_RELATION_TYPE_EASYZONE:
			keys_primary.keys[key->primary_index] = standarddata->key.type;
			roccat_button_copy(&keys_easyzone.keys[key->easyzone_index], &easyshiftdata->key);
			sova_profile_data_set_macro(profile_data, key->easyzone_index, &easyshiftdata->macro);
			sova_profile_data_set_opener(profile_data, key->easyzone_index, easyshiftdata->opener);
			sova_profile_data_set_timer(profile_data, key->easyzone_index, &easyshiftdata->timer);
			break;
		case SOVA_KEY_RELATION_TYPE_FUNCTION:
			roccat_button_copy(&keys_function.keys_normal[key->function_index], &standarddata->key);
			roccat_button_copy(&keys_function.keys_fn[key->function_index], &easyshiftdata->key);
			break;
		case SOVA_KEY_RELATION_TYPE_CAPSLOCK:
			keys_extra.capslock = standarddata->key.type;
			sova_profile_data_set_capslock_talk_target(profile_data, ROCCAT_TALK_DEVICE_MOUSE);
			break;
		case SOVA_KEY_RELATION_TYPE_PRIMARY:
			keys_primary.keys[key->primary_index] = standarddata->key.type;
			break;
		};
	}

	sova_profile_data_set_keys_primary(profile_data, &keys_primary);
	sova_profile_data_set_keys_easyzone(profile_data, &keys_easyzone);
	sova_profile_data_set_keys_function(profile_data, &keys_function);
	sova_profile_data_set_keys_extra(profile_data, &keys_extra);
}

void sova_keyboard_selector_set_macro(SovaKeyboardSelector *selector, guint macro_index, SovaMacro *macro) {
	SovaKeyboardSelectorPrivate *priv = selector->priv;
	guint i;

	i = sova_key_relation_find_by_easyzone_index(macro_index);
	if (i == SOVA_KEY_RELATION_INVALID)
		return;

	roccat_button_set_to_normal(&priv->data->easyshift_datas[i].key, SOVA_KEY_TYPE_MACRO);
	sova_macro_copy(&priv->data->easyshift_datas[i].macro, macro);
	set_standard_tooltip(selector, i);
	if (get_active_index(selector) == i)
		sova_standard_selector_update(priv->standard_selector);
}
