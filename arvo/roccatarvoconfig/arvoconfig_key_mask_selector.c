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

#include "arvoconfig_key_mask_selector.h"
#include "roccat_key_mask_selector.h"
#include "i18n.h"

#define ARVOCONFIG_KEY_MASK_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ARVOCONFIG_KEY_MASK_SELECTOR_TYPE, ArvoconfigKeyMaskSelectorClass))
#define IS_ARVOCONFIG_KEY_MASK_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ARVOCONFIG_KEY_MASK_SELECTOR_TYPE))

typedef struct _ArvoconfigKeyMaskSelectorClass ArvoconfigKeyMaskSelectorClass;

struct _ArvoconfigKeyMaskSelector {
	RoccatKeyMaskSelector parent;
};

struct _ArvoconfigKeyMaskSelectorClass {
	RoccatKeyMaskSelectorClass parent_class;
};

G_DEFINE_TYPE(ArvoconfigKeyMaskSelector, arvoconfig_key_mask_selector, ROCCAT_KEY_MASK_SELECTOR_TYPE);

enum {
	COUNT = 5,
};

static gchar *names[COUNT] = {
	N_("Left Windows"),
	N_("Right Windows"),
	N_("Application"),
	N_("Caps lock"),
	N_("Tab"),
};

static guint values[COUNT] = {
	ARVO_KEY_MASK_BIT_LEFT_WIN,
	ARVO_KEY_MASK_BIT_RIGHT_WIN,
	ARVO_KEY_MASK_BIT_APP_KEY,
	ARVO_KEY_MASK_BIT_CAPS_LOCK,
	ARVO_KEY_MASK_BIT_TAB,
};

void arvoconfig_key_mask_selector_set_from_rkp(ArvoconfigKeyMaskSelector *selector, ArvoRkp *rkp) {
	RoccatKeyMaskSelector *roccat_selector = ROCCAT_KEY_MASK_SELECTOR(selector);
	guint i;
	gboolean state;

	for (i = 0; i < COUNT; ++i) {
		state = (arvo_rkp_get_key(rkp, values[i]) == ARVO_RKP_KEY_STATE_ON) ? TRUE : FALSE;
		roccat_key_mask_selector_set_active(roccat_selector, i, state);
	}
}

void arvoconfig_key_mask_selector_update_rkp(ArvoconfigKeyMaskSelector *selector, ArvoRkp *rkp) {
	RoccatKeyMaskSelector *roccat_selector = ROCCAT_KEY_MASK_SELECTOR(selector);
	guint i;
	gboolean state;

	for (i = 0; i < COUNT; ++i) {
		state = roccat_key_mask_selector_get_active(roccat_selector, i);
		arvo_rkp_set_key(rkp, values[i], state ? ARVO_RKP_KEY_STATE_ON : ARVO_RKP_KEY_STATE_OFF);
	}
}

GtkWidget *arvoconfig_key_mask_selector_new(void) {
	ArvoconfigKeyMaskSelector *arvoconfig_key_mask_selector;

	arvoconfig_key_mask_selector = ARVOCONFIG_KEY_MASK_SELECTOR(g_object_new(ARVOCONFIG_KEY_MASK_SELECTOR_TYPE,
			"count", COUNT,
			"values", names,
			NULL));

	return GTK_WIDGET(arvoconfig_key_mask_selector);
}

static void arvoconfig_key_mask_selector_init(ArvoconfigKeyMaskSelector *arvoconfig_key_mask_selector) {}

static void arvoconfig_key_mask_selector_class_init(ArvoconfigKeyMaskSelectorClass *klass) {}
