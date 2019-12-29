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

#include "isku_key_mask_selector.h"
#include "roccat_key_mask_selector.h"
#include "i18n.h"

#define ISKU_KEY_MASK_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ISKU_KEY_MASK_SELECTOR_TYPE, IskuKeyMaskSelectorClass))
#define IS_ISKU_KEY_MASK_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ISKU_KEY_MASK_SELECTOR_TYPE))

typedef struct _IskuKeyMaskSelectorClass IskuKeyMaskSelectorClass;

struct _IskuKeyMaskSelector {
	RoccatKeyMaskSelector parent;
};

struct _IskuKeyMaskSelectorClass {
	RoccatKeyMaskSelectorClass parent_class;
};

G_DEFINE_TYPE(IskuKeyMaskSelector, isku_key_mask_selector, ROCCAT_KEY_MASK_SELECTOR_TYPE);

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
	ISKU_KEY_MASK_BIT_TAB,
	ISKU_KEY_MASK_BIT_LEFT_WIN,
	ISKU_KEY_MASK_BIT_RIGHT_WIN,
	ISKU_KEY_MASK_BIT_APP_KEY,
	ISKU_KEY_MASK_BIT_LEFT_SHIFT,
};

void isku_key_mask_selector_set_from_rkp(IskuKeyMaskSelector *selector, IskuRkp *rkp) {
	RoccatKeyMaskSelector *roccat_selector = ROCCAT_KEY_MASK_SELECTOR(selector);
	guint i;
	gboolean state;

	for (i = 0; i < COUNT; ++i) {
		state = isku_rkp_get_modify_key(rkp, values[i]);
		roccat_key_mask_selector_set_active(roccat_selector, i, state);
	}
}

void isku_key_mask_selector_update_rkp(IskuKeyMaskSelector *selector, IskuRkp *rkp) {
	RoccatKeyMaskSelector *roccat_selector = ROCCAT_KEY_MASK_SELECTOR(selector);
	guint i;
	gboolean state;

	for (i = 0; i < COUNT; ++i) {
		state = roccat_key_mask_selector_get_active(roccat_selector, i);
		isku_rkp_set_modify_key(rkp, values[i], state);
	}
}

GtkWidget *isku_key_mask_selector_new(void) {
	IskuKeyMaskSelector *isku_key_mask_selector;

	isku_key_mask_selector = ISKU_KEY_MASK_SELECTOR(g_object_new(ISKU_KEY_MASK_SELECTOR_TYPE,
			"count", COUNT,
			"values", names,
			NULL));

	return GTK_WIDGET(isku_key_mask_selector);
}

static void isku_key_mask_selector_init(IskuKeyMaskSelector *isku_key_mask_selector) {}

static void isku_key_mask_selector_class_init(IskuKeyMaskSelectorClass *klass) {}
