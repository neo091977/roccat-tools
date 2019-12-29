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

#include "ryos_key_mask_selector.h"
#include "roccat_key_mask_selector.h"
#include "roccat_helper.h"
#include "ryos_key_mask.h"
#include "i18n.h"

#define RYOS_KEY_MASK_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), RYOS_KEY_MASK_SELECTOR_TYPE, RyosKeyMaskSelectorClass))
#define IS_RYOS_KEY_MASK_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), RYOS_KEY_MASK_SELECTOR_TYPE))

typedef struct _RyosKeyMaskSelectorClass RyosKeyMaskSelectorClass;

struct _RyosKeyMaskSelector {
	RoccatKeyMaskSelector parent;
};

struct _RyosKeyMaskSelectorClass {
	RoccatKeyMaskSelectorClass parent_class;
};

G_DEFINE_TYPE(RyosKeyMaskSelector, ryos_key_mask_selector, ROCCAT_KEY_MASK_SELECTOR_TYPE);

enum {
	COUNT = 5,
};

static gchar *names[COUNT] = {
	N_("Tab"),
	N_("Left Windows"),
	N_("Right Windows"),
	N_("Application"),
	N_("Left shift"),
};

static guint values[COUNT] = {
	RYOS_KEY_MASK_BIT_TAB,
	RYOS_KEY_MASK_BIT_LEFT_WIN,
	RYOS_KEY_MASK_BIT_RIGHT_WIN,
	RYOS_KEY_MASK_BIT_APP,
	RYOS_KEY_MASK_BIT_LEFT_SHIFT,
};

void ryos_key_mask_selector_set_mask(RyosKeyMaskSelector *selector, guint8 mask) {
	RoccatKeyMaskSelector *roccat_selector = ROCCAT_KEY_MASK_SELECTOR(selector);
	guint i;
	gboolean state;

	for (i = 0; i < COUNT; ++i) {
		state = roccat_get_bit8(mask, values[i]);
		roccat_key_mask_selector_set_active(roccat_selector, i, state);
	}
}

guint8 ryos_key_mask_selector_get_mask(RyosKeyMaskSelector *selector) {
	RoccatKeyMaskSelector *roccat_selector = ROCCAT_KEY_MASK_SELECTOR(selector);
	guint i;
	gboolean state;
	guint8 mask = 0;

	for (i = 0; i < COUNT; ++i) {
		state = roccat_key_mask_selector_get_active(roccat_selector, i);
		roccat_set_bit8(&mask, values[i], state);
	}

	roccat_set_bit8(&mask, RYOS_KEY_MASK_BIT_CAPS_LOCK, TRUE);

	return mask;
}

GtkWidget *ryos_key_mask_selector_new(void) {
	RyosKeyMaskSelector *ryos_key_mask_selector;

	ryos_key_mask_selector = RYOS_KEY_MASK_SELECTOR(g_object_new(RYOS_KEY_MASK_SELECTOR_TYPE,
			"count", COUNT,
			"values", names,
			NULL));

	return GTK_WIDGET(ryos_key_mask_selector);
}

static void ryos_key_mask_selector_init(RyosKeyMaskSelector *ryos_key_mask_selector) {}

static void ryos_key_mask_selector_class_init(RyosKeyMaskSelectorClass *klass) {}
