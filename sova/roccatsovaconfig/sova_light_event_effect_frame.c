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

#include "sova_light_event_effect_frame.h"
#include "sova_light.h"
#include "gtk_roccat_helper.h"
#include "i18n.h"

#define SOVA_LIGHT_EVENT_EFFECT_FRAME_CLASS(obj) (G_TYPE_CHECK_CLASS_CAST((obj), SOVA_LIGHT_EVENT_EFFECT_FRAME_TYPE, SovaLightEventEffectFrameClass))
#define IS_SOVA_LIGHT_EVENT_EFFECT_FRAME_CLASS(obj) (G_TYPE_CHECK_CLASS_TYPE((obj), SOVA_LIGHT_EVENT_EFFECT_FRAME_TYPE))
#define SOVA_LIGHT_EVENT_EFFECT_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), SOVA_LIGHT_EVENT_EFFECT_FRAME_TYPE, SovaLightEventEffectFramePrivate))
#define SOVA_LIGHT_EVENT_EFFECT_FRAME_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), SOVA_LIGHT_EVENT_EFFECT_FRAME_TYPE, SovaLightEventEffectFrameClass))

typedef struct _SovaLightEventEffectFrameClass SovaLightEventEffectFrameClass;
typedef struct _SovaLightEventEffectFramePrivate SovaLightEventEffectFramePrivate;

struct _SovaLightEventEffectFrame {
	GtkFrame parent;
	SovaLightEventEffectFramePrivate *priv;
};

struct _SovaLightEventEffectFrameClass {
	GtkFrameClass parent_class;
	GtkListStore *start_effect_store;
	GtkListStore *sleep_effect_store;
	GtkListStore *wakeup_effect_store;
	GtkListStore *profile_change_effect_store;
};

struct _SovaLightEventEffectFramePrivate {
	GtkComboBox *start_effect;
	GtkComboBox *sleep_effect;
	GtkComboBox *wakeup_effect;
	GtkComboBox *profile_change_effect;
	GtkSpinButton *sleep_time;
};

G_DEFINE_TYPE(SovaLightEventEffectFrame, sova_light_event_effect_frame, GTK_TYPE_FRAME);

enum {
	NAME_COLUMN,
	VALUE_COLUMN,
	N_COLUMNS
};

GtkWidget *sova_light_event_effect_frame_new(void) {
	return GTK_WIDGET(g_object_new(SOVA_LIGHT_EVENT_EFFECT_FRAME_TYPE, NULL));
}

static GtkListStore *start_effect_store_new(void) {
	GtkListStore *store = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_INT);
	GtkTreeIter iter;
	
	gtk_roccat_list_store_append_with_values(store, &iter, NAME_COLUMN, _("Off"), VALUE_COLUMN, SOVA_LIGHT_START_EFFECT_OFF, -1);
	gtk_roccat_list_store_append_with_values(store, &iter, NAME_COLUMN, _("Breathing"), VALUE_COLUMN, SOVA_LIGHT_START_EFFECT_BREATHING, -1);
	gtk_roccat_list_store_append_with_values(store, &iter, NAME_COLUMN, _("Blinking"), VALUE_COLUMN, SOVA_LIGHT_START_EFFECT_BLINKING, -1);
	gtk_roccat_list_store_append_with_values(store, &iter, NAME_COLUMN, _("Flickering"), VALUE_COLUMN, SOVA_LIGHT_START_EFFECT_FLICKERING, -1);

	return store;
}

static GtkListStore *sleep_effect_store_new(void) {
	GtkListStore *store = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_INT);
	GtkTreeIter iter;
	
	gtk_roccat_list_store_append_with_values(store, &iter, NAME_COLUMN, _("Off"), VALUE_COLUMN, SOVA_LIGHT_SLEEP_EFFECT_OFF, -1);
	gtk_roccat_list_store_append_with_values(store, &iter, NAME_COLUMN, _("Breathing"), VALUE_COLUMN, SOVA_LIGHT_SLEEP_EFFECT_BREATHING, -1);
	gtk_roccat_list_store_append_with_values(store, &iter, NAME_COLUMN, _("Blinking"), VALUE_COLUMN, SOVA_LIGHT_SLEEP_EFFECT_BLINKING, -1);

	return store;
}

static GtkListStore *wakeup_effect_store_new(void) {
	GtkListStore *store = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_INT);
	GtkTreeIter iter;
	
	gtk_roccat_list_store_append_with_values(store, &iter, NAME_COLUMN, _("Off"), VALUE_COLUMN, SOVA_LIGHT_WAKEUP_EFFECT_OFF, -1);
	gtk_roccat_list_store_append_with_values(store, &iter, NAME_COLUMN, _("Impulse"), VALUE_COLUMN, SOVA_LIGHT_WAKEUP_EFFECT_IMPULSE, -1);
	gtk_roccat_list_store_append_with_values(store, &iter, NAME_COLUMN, _("Flickering"), VALUE_COLUMN, SOVA_LIGHT_WAKEUP_EFFECT_FLICKERING, -1);
	gtk_roccat_list_store_append_with_values(store, &iter, NAME_COLUMN, _("Heartbeat"), VALUE_COLUMN, SOVA_LIGHT_WAKEUP_EFFECT_HEARTBEAT, -1);

	return store;
}

static GtkListStore *profile_change_effect_store_new(void) {
	GtkListStore *store = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_INT);
	GtkTreeIter iter;
	
	gtk_roccat_list_store_append_with_values(store, &iter, NAME_COLUMN, _("Off"), VALUE_COLUMN, SOVA_LIGHT_PROFILE_CHANGE_EFFECT_OFF, -1);
	gtk_roccat_list_store_append_with_values(store, &iter, NAME_COLUMN, _("Impulse"), VALUE_COLUMN, SOVA_LIGHT_PROFILE_CHANGE_EFFECT_IMPULSE, -1);
	gtk_roccat_list_store_append_with_values(store, &iter, NAME_COLUMN, _("Flickering"), VALUE_COLUMN, SOVA_LIGHT_PROFILE_CHANGE_EFFECT_FLICKERING, -1);
	gtk_roccat_list_store_append_with_values(store, &iter, NAME_COLUMN, _("Heartbeat"), VALUE_COLUMN, SOVA_LIGHT_PROFILE_CHANGE_EFFECT_HEARTBEAT, -1);

	return store;
}

static gint sova_light_effect_combo_box_get_value(GtkComboBox *combo) {
	GtkTreeIter iter;
	GtkTreeModel *model;
	gboolean valid;
	gint type;

	model = gtk_combo_box_get_model(combo);
	
	valid = gtk_combo_box_get_active_iter(combo, &iter);
	if (!valid)
		valid = gtk_tree_model_get_iter_first(model, &iter);

	gtk_tree_model_get(model, &iter, VALUE_COLUMN, &type, -1);
	return type;
}

static void sova_light_effect_combo_box_set_value(GtkComboBox *combo, gint value) {
	GtkTreeIter iter;
	GtkTreeModel *model;
	gboolean valid;

	model = gtk_combo_box_get_model(combo);
	valid = gtk_roccat_tree_model_iter_find_int(model, VALUE_COLUMN, value, &iter, NULL);
	if (valid)
		gtk_combo_box_set_active_iter(combo, &iter);
}

static GtkWidget *sova_light_effect_combo_box_new(GtkListStore *store) {
	GtkWidget *combo;
	GtkCellRenderer *renderer;
		
	combo = gtk_combo_box_new_with_model(GTK_TREE_MODEL(store));
	
	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combo), renderer, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combo), renderer, "text", NAME_COLUMN, NULL);

	return combo;
}

static void sova_light_sleep_spin_button_set_value(GtkSpinButton *spin, guint value) {
	gtk_spin_button_set_value(spin, (gdouble)value);
}

static guint sova_light_sleep_spin_button_get_value(GtkSpinButton *spin) {
	return (guint)gtk_spin_button_get_value(spin); // FIXME + 0.5?
}

static GtkWidget *sova_light_sleep_spin_button_new(void) {
	return gtk_spin_button_new_with_range(0.0, 255.0, 1.0);
}

static void sova_light_event_effect_frame_init(SovaLightEventEffectFrame *frame) {
	SovaLightEventEffectFramePrivate *priv = SOVA_LIGHT_EVENT_EFFECT_FRAME_GET_PRIVATE(frame);
	SovaLightEventEffectFrameClass *klass = SOVA_LIGHT_EVENT_EFFECT_FRAME_GET_CLASS(frame);
	GtkTable *table = GTK_TABLE(gtk_table_new(4, 5, FALSE));
	
	frame->priv = priv;

	priv->start_effect = GTK_COMBO_BOX(sova_light_effect_combo_box_new(klass->start_effect_store));
	priv->sleep_effect = GTK_COMBO_BOX(sova_light_effect_combo_box_new(klass->sleep_effect_store));
	priv->wakeup_effect = GTK_COMBO_BOX(sova_light_effect_combo_box_new(klass->wakeup_effect_store));
	priv->profile_change_effect = GTK_COMBO_BOX(sova_light_effect_combo_box_new(klass->profile_change_effect_store));
	priv->sleep_time = GTK_SPIN_BUTTON(sova_light_sleep_spin_button_new());
	
	gtk_table_attach(table, gtk_label_new(_("Start")), 0, 1, 0, 1, 0, GTK_EXPAND, 0, 0);
	gtk_table_attach(table, GTK_WIDGET(priv->start_effect), 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);
	
	gtk_table_attach(table, gtk_label_new(_("Sleep")), 0, 1, 1, 2, 0, GTK_EXPAND, 0, 0);
	gtk_table_attach(table, GTK_WIDGET(priv->sleep_effect), 1, 2, 1, 2, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);
	gtk_table_attach(table, gtk_label_new(_("after")), 2, 3, 1, 2, 0, GTK_EXPAND, 0, 0);
	gtk_table_attach(table, GTK_WIDGET(priv->sleep_time), 3, 4, 1, 2, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);
	gtk_table_attach(table, gtk_label_new(_("min")), 4, 5, 1, 2, 0, GTK_EXPAND, 0, 0);
	
	gtk_table_attach(table, gtk_label_new(_("Wakeup")), 0, 1, 2, 3, 0, GTK_EXPAND, 0, 0);
	gtk_table_attach(table, GTK_WIDGET(priv->wakeup_effect), 1, 2, 2, 3, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);
	
	gtk_table_attach(table, gtk_label_new(_("Profile change")), 0, 1, 3, 4, 0, GTK_EXPAND, 0, 0);
	gtk_table_attach(table, GTK_WIDGET(priv->profile_change_effect), 1, 2, 3, 4, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);
	
	gtk_container_add(GTK_CONTAINER(frame), GTK_WIDGET(table));
	gtk_frame_set_label(GTK_FRAME(frame), _("Event light effects"));
}

static void sova_light_event_effect_frame_class_init(SovaLightEventEffectFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(SovaLightEventEffectFramePrivate));
	
	klass->start_effect_store = start_effect_store_new();
	klass->sleep_effect_store = sleep_effect_store_new();
	klass->wakeup_effect_store = wakeup_effect_store_new();
	klass->profile_change_effect_store = profile_change_effect_store_new();
}

void sova_light_event_effect_frame_set_from_profile_data(SovaLightEventEffectFrame *frame, SovaProfileData const *profile_data) {
	SovaLightEventEffectFramePrivate *priv = frame->priv;
	SovaLight const *light;
	
	light = sova_profile_data_get_light(profile_data);
	
	sova_light_effect_combo_box_set_value(priv->start_effect, light->start_effect);
	sova_light_effect_combo_box_set_value(priv->sleep_effect, light->sleep_effect);
	sova_light_effect_combo_box_set_value(priv->wakeup_effect, light->wakeup_effect);
	sova_light_effect_combo_box_set_value(priv->profile_change_effect, light->profile_change_effect);
	sova_light_sleep_spin_button_set_value(priv->sleep_time, light->sleep_after);
}

void sova_light_event_effect_frame_update_profile_data(SovaLightEventEffectFrame *frame, SovaProfileData *profile_data) {
	SovaLightEventEffectFramePrivate *priv = frame->priv;
	SovaLight light;
	
	sova_light_copy(&light, sova_profile_data_get_light(profile_data));
	
	light.start_effect = sova_light_effect_combo_box_get_value(priv->start_effect);
	light.sleep_effect = sova_light_effect_combo_box_get_value(priv->sleep_effect);
	light.wakeup_effect = sova_light_effect_combo_box_get_value(priv->wakeup_effect);
	light.profile_change_effect = sova_light_effect_combo_box_get_value(priv->profile_change_effect);
	light.sleep_after = sova_light_sleep_spin_button_get_value(priv->sleep_time);
	
	sova_profile_data_set_light(profile_data, &light);
}
