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

#include "arvoconfig_mode_selector.h"
#include "i18n.h"

#define ARVOCONFIG_MODE_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ARVOCONFIG_MODE_SELECTOR_TYPE, ArvoconfigModeSelectorClass))
#define IS_ARVOCONFIG_MODE_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ARVOCONFIG_MODE_SELECTOR_TYPE))
#define ARVOCONFIG_MODE_SELECTOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ARVOCONFIG_MODE_SELECTOR_TYPE, ArvoconfigModeSelectorPrivate))

typedef struct _ArvoconfigModeSelectorClass ArvoconfigModeSelectorClass;
typedef struct _ArvoconfigModeSelectorPrivate ArvoconfigModeSelectorPrivate;

struct _ArvoconfigModeSelector {
	GtkFrame parent;
	ArvoconfigModeSelectorPrivate *priv;
};

struct _ArvoconfigModeSelectorClass {
	GtkFrameClass parent_class;
};

struct _ArvoconfigModeSelectorPrivate {
	GtkToggleButton *button;
};

G_DEFINE_TYPE(ArvoconfigModeSelector, arvoconfig_mode_selector, GTK_TYPE_FRAME);

void arvoconfig_mode_selector_set_from_rkp(ArvoconfigModeSelector *selector, ArvoRkp *rkp) {
	guint value = arvo_rkp_get_mode_key(rkp);
	gtk_toggle_button_set_active(selector->priv->button, value == ARVO_MODE_KEY_STATE_ON);
}

void arvoconfig_mode_selector_update_rkp(ArvoconfigModeSelector *selector, ArvoRkp *rkp) {
	guint value = gtk_toggle_button_get_active(selector->priv->button) ? ARVO_MODE_KEY_STATE_ON : ARVO_MODE_KEY_STATE_OFF;
	arvo_rkp_set_mode_key(rkp, value);
}

GtkWidget *arvoconfig_mode_selector_new(void) {
	ArvoconfigModeSelector *arvoconfig_mode_selector;

	arvoconfig_mode_selector = ARVOCONFIG_MODE_SELECTOR(g_object_new(ARVOCONFIG_MODE_SELECTOR_TYPE,
			NULL));

	return GTK_WIDGET(arvoconfig_mode_selector);
}

static void arvoconfig_mode_selector_init(ArvoconfigModeSelector *selector) {
	ArvoconfigModeSelectorPrivate *priv = ARVOCONFIG_MODE_SELECTOR_GET_PRIVATE(selector);
	selector->priv = priv;

	priv->button = GTK_TOGGLE_BUTTON(gtk_check_button_new_with_label(_("Active")));
	gtk_container_add(GTK_CONTAINER(selector), GTK_WIDGET(priv->button));

	gtk_frame_set_label(GTK_FRAME(selector), _("Gaming mode"));
}

static void arvoconfig_mode_selector_class_init(ArvoconfigModeSelectorClass *klass) {
	g_type_class_add_private(klass, sizeof(ArvoconfigModeSelectorPrivate));
}
