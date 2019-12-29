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

#include "sova_key_mask_selector.h"
#include "roccat_key_mask_selector.h"
#include "roccat_helper.h"
#include "sova_key_mask.h"
#include "i18n.h"

#define SOVA_KEY_MASK_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), SOVA_KEY_MASK_SELECTOR_TYPE, SovaKeyMaskSelectorClass))
#define IS_SOVA_KEY_MASK_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), SOVA_KEY_MASK_SELECTOR_TYPE))

typedef struct _SovaKeyMaskSelectorClass SovaKeyMaskSelectorClass;

struct _SovaKeyMaskSelector {
	RoccatKeyMaskSelector parent;
};

struct _SovaKeyMaskSelectorClass {
	RoccatKeyMaskSelectorClass parent_class;
};

G_DEFINE_TYPE(SovaKeyMaskSelector, sova_key_mask_selector, ROCCAT_KEY_MASK_SELECTOR_TYPE);

enum {
	COUNT = 6,
};

static gchar *names[COUNT] = {
	N_("Tab"),
	N_("Caps lock"),
	N_("Left Windows"),
	N_("Right Windows"),
	N_("Application"),
	N_("Left shift"),
};

static guint values[COUNT] = {
	SOVA_KEY_MASK_BIT_TAB,
	SOVA_KEY_MASK_BIT_CAPS_LOCK,
	SOVA_KEY_MASK_BIT_LEFT_WIN,
	SOVA_KEY_MASK_BIT_RIGHT_WIN,
	SOVA_KEY_MASK_BIT_APP,
	SOVA_KEY_MASK_BIT_LEFT_SHIFT,
};

void sova_key_mask_selector_set_mask(SovaKeyMaskSelector *selector, guint8 mask) {
	RoccatKeyMaskSelector *roccat_selector = ROCCAT_KEY_MASK_SELECTOR(selector);
	guint i;
	gboolean state;

	for (i = 0; i < COUNT; ++i) {
		state = roccat_get_bit8(mask, values[i]);
		roccat_key_mask_selector_set_active(roccat_selector, i, state);
	}
}

guint8 sova_key_mask_selector_get_mask(SovaKeyMaskSelector *selector) {
	RoccatKeyMaskSelector *roccat_selector = ROCCAT_KEY_MASK_SELECTOR(selector);
	guint i;
	gboolean state;
	guint8 mask = 0;

	for (i = 0; i < COUNT; ++i) {
		state = roccat_key_mask_selector_get_active(roccat_selector, i);
		roccat_set_bit8(&mask, values[i], state);
	}

	roccat_set_bit8(&mask, SOVA_KEY_MASK_BIT_CAPS_LOCK, TRUE);

	return mask;
}

GtkWidget *sova_key_mask_selector_new(void) {
	SovaKeyMaskSelector *sova_key_mask_selector;

	sova_key_mask_selector = SOVA_KEY_MASK_SELECTOR(g_object_new(SOVA_KEY_MASK_SELECTOR_TYPE,
			"count", COUNT,
			"values", names,
			NULL));

	return GTK_WIDGET(sova_key_mask_selector);
}

static void sova_key_mask_selector_init(SovaKeyMaskSelector *sova_key_mask_selector) {}

static void sova_key_mask_selector_class_init(SovaKeyMaskSelectorClass *klass) {}

void sova_key_mask_selector_set_from_profile_data(SovaKeyMaskSelector *selector, SovaProfileData const *profile_data) {
	sova_key_mask_selector_set_mask(selector, sova_profile_data_get_key_mask(profile_data)->mask);
}

void sova_key_mask_selector_update_profile_data(SovaKeyMaskSelector *selector, SovaProfileData *profile_data) {
	SovaKeyMask key_mask;

	sova_key_mask_copy(&key_mask, sova_profile_data_get_key_mask(profile_data));
	key_mask.mask = sova_key_mask_selector_get_mask(selector);
	sova_profile_data_set_key_mask(profile_data, &key_mask);
}
