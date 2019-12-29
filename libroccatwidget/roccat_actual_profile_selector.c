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

#include "roccat_actual_profile_selector.h"
#include "i18n-lib.h"

#define ROCCAT_ACTUAL_PROFILE_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ROCCAT_ACTUAL_PROFILE_SELECTOR_TYPE, RoccatActualProfileSelectorClass))
#define IS_ROCCAT_ACTUAL_PROFILE_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ROCCAT_ACTUAL_PROFILE_SELECTOR_TYPE))
#define ROCCAT_ACTUAL_PROFILE_SELECTOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ROCCAT_ACTUAL_PROFILE_SELECTOR_TYPE, RoccatActualProfileSelectorPrivate))

typedef struct _RoccatActualProfileSelectorClass RoccatActualProfileSelectorClass;

struct _RoccatActualProfileSelectorClass {
	GtkFrameClass parent_class;
	void(*changed)(RoccatActualProfileSelector *selector);
};

struct _RoccatActualProfileSelectorPrivate {
	GtkSpinButton *spin;
};

G_DEFINE_TYPE(RoccatActualProfileSelector, roccat_actual_profile_selector, GTK_TYPE_FRAME);

enum {
	CHANGED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

guint roccat_actual_profile_selector_get_value(RoccatActualProfileSelector *selector) {
	return (guint)gtk_spin_button_get_value(selector->priv->spin);
}

void roccat_actual_profile_selector_set_value(RoccatActualProfileSelector *selector, guint profile_number) {
	gtk_spin_button_set_value(selector->priv->spin, (gdouble)profile_number);
}

static void spin_value_changed_cb(GtkSpinButton *spin, gpointer user_data) {
	RoccatActualProfileSelector *selector = ROCCAT_ACTUAL_PROFILE_SELECTOR(user_data);
	g_signal_emit((gpointer)selector, signals[CHANGED], 0);
}

/* TODO Can't be inherited */
GtkWidget *roccat_actual_profile_selector_new(guint max) {
	RoccatActualProfileSelector *selector;
	RoccatActualProfileSelectorPrivate *priv;

	selector = g_object_new(ROCCAT_ACTUAL_PROFILE_SELECTOR_TYPE, NULL);

	priv = selector->priv;

	priv->spin = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(1.0, (gdouble)max, 1.0));
	g_signal_connect(G_OBJECT(priv->spin), "value-changed", G_CALLBACK(spin_value_changed_cb), selector);

	gtk_container_add(GTK_CONTAINER(selector), GTK_WIDGET(priv->spin));

	gtk_widget_show(GTK_WIDGET(priv->spin));

	return GTK_WIDGET(selector);
}

static void roccat_actual_profile_selector_init(RoccatActualProfileSelector *selector) {
	RoccatActualProfileSelectorPrivate *priv = ROCCAT_ACTUAL_PROFILE_SELECTOR_GET_PRIVATE(selector);

	selector->priv = priv;

	gtk_frame_set_label(GTK_FRAME(selector), _("Actual profile"));
}

static void roccat_actual_profile_selector_class_init(RoccatActualProfileSelectorClass *klass) {
	g_type_class_add_private(klass, sizeof(RoccatActualProfileSelectorPrivate));

	signals[CHANGED] = g_signal_new("changed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			G_STRUCT_OFFSET(RoccatActualProfileSelectorClass, changed),
			NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}
