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

#include "roccat_talkfx_selector.h"
#include "i18n-lib.h"

#define ROCCAT_TALKFX_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ROCCAT_TALKFX_SELECTOR_TYPE, RoccatTalkFXSelectorClass))
#define IS_ROCCAT_TALKFX_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ROCCAT_TALKFX_SELECTOR_TYPE))
#define ROCCAT_TALKFX_SELECTOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ROCCAT_TALKFX_SELECTOR_TYPE, RoccatTalkFXSelectorPrivate))

typedef struct _RoccatTalkFXSelectorClass RoccatTalkFXSelectorClass;

struct _RoccatTalkFXSelectorClass {
	GtkFrameClass parent_class;
};

struct _RoccatTalkFXSelectorPrivate {
	GtkToggleButton *button;
};

G_DEFINE_TYPE(RoccatTalkFXSelector, roccat_talkfx_selector, GTK_TYPE_FRAME);

enum {
	CHANGED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

gboolean roccat_talkfx_selector_get_active(RoccatTalkFXSelector *selector) {
	return gtk_toggle_button_get_active(selector->priv->button);
}

void roccat_talkfx_selector_set_active(RoccatTalkFXSelector *selector, gboolean is_active) {
	gtk_toggle_button_set_active(selector->priv->button, is_active);
}

static void button_toggled_cb(GtkToggleButton *button, gpointer user_data) {
	RoccatTalkFXSelector *selector = ROCCAT_TALKFX_SELECTOR(user_data);
	g_signal_emit((gpointer)selector, signals[CHANGED], 0);
}

GtkWidget *roccat_talkfx_selector_new(void) {
	RoccatTalkFXSelector *selector;
	RoccatTalkFXSelectorPrivate *priv;

	selector = g_object_new(ROCCAT_TALKFX_SELECTOR_TYPE, NULL);

	priv = selector->priv;

	priv->button = GTK_TOGGLE_BUTTON(gtk_check_button_new_with_label(_("On")));
	g_signal_connect(G_OBJECT(priv->button), "toggled", G_CALLBACK(button_toggled_cb), selector);

	gtk_container_add(GTK_CONTAINER(selector), GTK_WIDGET(priv->button));

	gtk_widget_show(GTK_WIDGET(priv->button));

	return GTK_WIDGET(selector);
}

static void roccat_talkfx_selector_init(RoccatTalkFXSelector *selector) {
	RoccatTalkFXSelectorPrivate *priv = ROCCAT_TALKFX_SELECTOR_GET_PRIVATE(selector);

	selector->priv = priv;

	gtk_frame_set_label(GTK_FRAME(selector), _("Talk FX"));
}

static void roccat_talkfx_selector_class_init(RoccatTalkFXSelectorClass *klass) {
	g_type_class_add_private(klass, sizeof(RoccatTalkFXSelectorPrivate));

	signals[CHANGED] = g_signal_new("changed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}
