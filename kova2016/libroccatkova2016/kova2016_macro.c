/*
 * This file is part of roccat-tools.
 *
 * roccat-tools is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Macro Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * roccat-tools is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Macro Public License for more details.
 *
 * You should have received a copy of the GNU Macro Public License
 * along with roccat-tools. If not, see <http://www.gnu.org/licenses/>.
 */

#include "kova2016_macro.h"
#include "kova2016_device.h"
#include "roccat_helper.h"
#include "roccat.h"
#include "i18n-lib.h"
#include <string.h>

typedef struct _Kova2016Macro1 Kova2016Macro1;
typedef struct _Kova2016Macro2 Kova2016Macro2;

enum {
	KOVA2016_MACRO_1_DATA_SIZE = 1024,
	KOVA2016_MACRO_2_DATA_SIZE = sizeof(Kova2016Macro) - KOVA2016_MACRO_1_DATA_SIZE,
	KOVA2016_MACRO_2_UNUSED_SIZE = 1024 - KOVA2016_MACRO_2_DATA_SIZE,
};

struct _Kova2016Macro1 {
	guint8 report_id;
	guint8 one;
	guint8 data[KOVA2016_MACRO_1_DATA_SIZE];
} __attribute__ ((packed));

struct _Kova2016Macro2 {
	guint8 report_id;
	guint8 two;
	guint8 data[KOVA2016_MACRO_2_DATA_SIZE];
	guint8 unused[KOVA2016_MACRO_2_UNUSED_SIZE];
} __attribute__ ((packed));

void kova2016_macro_set_macroset_name(Kova2016Macro *macro, gchar const *new_name) {
	g_strlcpy((gchar *)(macro->macroset_name), new_name, KOVA2016_MACRO_MACROSET_NAME_LENGTH);
}

void kova2016_macro_set_macro_name(Kova2016Macro *macro, gchar const *new_name) {
	g_strlcpy((gchar *)(macro->macro_name), new_name, KOVA2016_MACRO_MACRO_NAME_LENGTH);
}

static void kova2016_macro_finalize(Kova2016Macro *macro, guint profile_index, guint button_index) {
	macro->profile_index = profile_index;
	macro->button_index = button_index;
}

gboolean kova2016_macro_write(RoccatDevice *kova2016, guint profile_index, guint button_index, Kova2016Macro *macro, GError **error) {
	Kova2016Macro1 macro1;
	Kova2016Macro2 macro2;
	gboolean result;

	g_assert(profile_index < KOVA2016_PROFILE_NUM);

	kova2016_macro_finalize(macro, profile_index, button_index);

	macro1.report_id = KOVA2016_REPORT_ID_MACRO;
	macro1.one = 1;
	memcpy(macro1.data, macro, KOVA2016_MACRO_1_DATA_SIZE);

	macro2.report_id = KOVA2016_REPORT_ID_MACRO;
	macro2.two = 2;
	memcpy(macro2.data, (guint8 *)macro + KOVA2016_MACRO_1_DATA_SIZE, KOVA2016_MACRO_2_DATA_SIZE);
	memset(macro2.unused, 0, KOVA2016_MACRO_2_UNUSED_SIZE);

	result = kova2016_device_write(kova2016, (gchar const *)&macro1, sizeof(Kova2016Macro1), error);
	if (!result)
		return FALSE;

	return kova2016_device_write(kova2016, (gchar const *)&macro2, sizeof(Kova2016Macro2), error);
}

Kova2016Macro *kova2016_macro_read(RoccatDevice *kova2016, guint profile_index, guint button_index, GError **error) {
	Kova2016Macro1 *macro1;
	Kova2016Macro2 *macro2;
	Kova2016Macro *macro;

	g_assert(profile_index < KOVA2016_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(kova2016));

	if (!kova2016_select(kova2016, profile_index, KOVA2016_CONTROL_DATA_INDEX_MACRO_1, button_index, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(kova2016));
		return NULL;
	}

	macro1 = (Kova2016Macro1 *)kova2016_device_read(kova2016, KOVA2016_REPORT_ID_MACRO, sizeof(Kova2016Macro1), error);
	if (!macro1) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(kova2016));
		return NULL;
	}

	if (!kova2016_select(kova2016, profile_index, KOVA2016_CONTROL_DATA_INDEX_MACRO_2, button_index, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(kova2016));
		g_free(macro1);
		return NULL;
	}

	macro2 = (Kova2016Macro2 *)kova2016_device_read(kova2016, KOVA2016_REPORT_ID_MACRO, sizeof(Kova2016Macro2), error);
	if (!macro2) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(kova2016));
		g_free(macro1);
		return NULL;
	}

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(kova2016));

	macro = (Kova2016Macro *)g_malloc(sizeof(Kova2016Macro));
	memcpy(macro, macro1->data, KOVA2016_MACRO_1_DATA_SIZE);
	memcpy((guint8 *)macro + KOVA2016_MACRO_1_DATA_SIZE, macro2->data, KOVA2016_MACRO_2_DATA_SIZE);

	g_free(macro1);
	g_free(macro2);

	return macro;
}

gboolean gaminggear_macro_to_kova2016_macro(GaminggearMacro const *gaminggear_macro, Kova2016Macro *kova2016_macro, GError **error) {
	guint i, count;

	g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

	count = gaminggear_macro_keystrokes_get_count(&gaminggear_macro->keystrokes);
	if (count > KOVA2016_MACRO_KEYSTROKES_NUM) {
		g_set_error(error, ROCCAT_ERROR, ROCCAT_ERROR_MACRO_TOO_LONG,
				_("Macro contains %u actions while device only supports %u actions"), count, KOVA2016_MACRO_KEYSTROKES_NUM);
		return FALSE;
	}

	memset(kova2016_macro, 0, sizeof(Kova2016Macro));
	kova2016_macro_set_count(kova2016_macro, count);
	kova2016_macro->loop = gaminggear_macro->keystrokes.loop;
	kova2016_macro_set_macroset_name(kova2016_macro, gaminggear_macro->macroset);
	kova2016_macro_set_macro_name(kova2016_macro, gaminggear_macro->macro);

	for (i = 0; i < count; ++i)
		gaminggear_macro_keystroke_to_roccat_keystroke(&gaminggear_macro->keystrokes.keystrokes[i], &kova2016_macro->keystrokes[i]);

	return TRUE;
}

GaminggearMacro *kova2016_macro_to_gaminggear_macro(Kova2016Macro const *kova2016_macro) {
	GaminggearMacro *gaminggear_macro;
	GaminggearMacroKeystroke keystroke;
	guint i, count;

	gaminggear_macro = gaminggear_macro_new((gchar const *)kova2016_macro->macroset_name, (gchar const *)kova2016_macro->macro_name, NULL);

	gaminggear_macro->keystrokes.loop = kova2016_macro->loop;

	count = kova2016_macro_get_count(kova2016_macro);
	count = MIN(count, GAMINGGEAR_MACRO_KEYSTROKES_NUM);
	count = MIN(count, KOVA2016_MACRO_KEYSTROKES_NUM);

	for (i = 0; i < count; ++i) {
		roccat_keystroke_to_gaminggear_macro_keystroke(&kova2016_macro->keystrokes[i], &keystroke);
		gaminggear_macro_keystrokes_add(&gaminggear_macro->keystrokes, &keystroke);
	}

	return gaminggear_macro;
}

gboolean kova2016_macro_equal(Kova2016Macro const *left, Kova2016Macro const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY_TO_END(left, right, Kova2016Macro, loop);
	return equal ? FALSE : TRUE;
}

void kova2016_macro_copy(Kova2016Macro *destination, Kova2016Macro const *source) {
	memcpy(destination, source, sizeof(Kova2016Macro));
}
