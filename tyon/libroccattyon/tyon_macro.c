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

#include "tyon_macro.h"
#include "tyon_device.h"
#include "roccat_helper.h"
#include "roccat.h"
#include "i18n-lib.h"
#include <string.h>

typedef struct _TyonMacro1 TyonMacro1;
typedef struct _TyonMacro2 TyonMacro2;

enum {
	TYON_MACRO_1_DATA_SIZE = 1024,
	TYON_MACRO_2_DATA_SIZE = sizeof(TyonMacro) - TYON_MACRO_1_DATA_SIZE,
	TYON_MACRO_2_UNUSED_SIZE = 1024 - TYON_MACRO_2_DATA_SIZE,
};

struct _TyonMacro1 {
	guint8 report_id;
	guint8 one;
	guint8 data[TYON_MACRO_1_DATA_SIZE];
} __attribute__ ((packed));

struct _TyonMacro2 {
	guint8 report_id;
	guint8 two;
	guint8 data[TYON_MACRO_2_DATA_SIZE];
	guint8 unused[TYON_MACRO_2_UNUSED_SIZE];
} __attribute__ ((packed));

void tyon_macro_set_macroset_name(TyonMacro *macro, gchar const *new_name) {
	g_strlcpy((gchar *)(macro->macroset_name), new_name, TYON_MACRO_MACROSET_NAME_LENGTH);
}

void tyon_macro_set_macro_name(TyonMacro *macro, gchar const *new_name) {
	g_strlcpy((gchar *)(macro->macro_name), new_name, TYON_MACRO_MACRO_NAME_LENGTH);
}

static void tyon_macro_finalize(TyonMacro *macro, guint profile_index, guint button_index) {
	macro->profile_index = profile_index;
	macro->button_index = button_index;
}

gboolean tyon_macro_write(RoccatDevice *tyon, guint profile_index, guint button_index, TyonMacro *macro, GError **error) {
	TyonMacro1 macro1;
	TyonMacro2 macro2;
	gboolean result;

	g_assert(profile_index < TYON_PROFILE_NUM);

	tyon_macro_finalize(macro, profile_index, button_index);

	macro1.report_id = TYON_REPORT_ID_MACRO;
	macro1.one = 1;
	memcpy(macro1.data, macro, TYON_MACRO_1_DATA_SIZE);

	macro2.report_id = TYON_REPORT_ID_MACRO;
	macro2.two = 2;
	memcpy(macro2.data, (guint8 *)macro + TYON_MACRO_1_DATA_SIZE, TYON_MACRO_2_DATA_SIZE);
	memset(macro2.unused, 0, TYON_MACRO_2_UNUSED_SIZE);

	result = tyon_device_write(tyon, (gchar const *)&macro1, sizeof(TyonMacro1), error);
	if (!result)
		return FALSE;

	return tyon_device_write(tyon, (gchar const *)&macro2, sizeof(TyonMacro2), error);
}

TyonMacro *tyon_macro_read(RoccatDevice *tyon, guint profile_index, guint button_index, GError **error) {
	TyonMacro1 *macro1;
	TyonMacro2 *macro2;
	TyonMacro *macro;

	g_assert(profile_index < TYON_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(tyon));

	if (!tyon_select(tyon, profile_index, TYON_CONTROL_DATA_INDEX_MACRO_1, button_index, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(tyon));
		return NULL;
	}

	macro1 = (TyonMacro1 *)tyon_device_read(tyon, TYON_REPORT_ID_MACRO, sizeof(TyonMacro1), error);
	if (!macro1) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(tyon));
		return NULL;
	}

	if (!tyon_select(tyon, profile_index, TYON_CONTROL_DATA_INDEX_MACRO_2, button_index, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(tyon));
		g_free(macro1);
		return NULL;
	}

	macro2 = (TyonMacro2 *)tyon_device_read(tyon, TYON_REPORT_ID_MACRO, sizeof(TyonMacro2), error);
	if (!macro2) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(tyon));
		g_free(macro1);
		return NULL;
	}

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(tyon));

	macro = (TyonMacro *)g_malloc(sizeof(TyonMacro));
	memcpy(macro, macro1->data, TYON_MACRO_1_DATA_SIZE);
	memcpy((guint8 *)macro + TYON_MACRO_1_DATA_SIZE, macro2->data, TYON_MACRO_2_DATA_SIZE);

	g_free(macro1);
	g_free(macro2);

	return macro;
}

gboolean gaminggear_macro_to_tyon_macro(GaminggearMacro const *gaminggear_macro, TyonMacro *tyon_macro, GError **error) {
	guint i, count;

	g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

	count = gaminggear_macro_keystrokes_get_count(&gaminggear_macro->keystrokes);
	if (count > TYON_MACRO_KEYSTROKES_NUM) {
		g_set_error(error, ROCCAT_ERROR, ROCCAT_ERROR_MACRO_TOO_LONG,
				_("Macro contains %u actions while device only supports %u actions"), count, TYON_MACRO_KEYSTROKES_NUM);
		return FALSE;
	}

	memset(tyon_macro, 0, sizeof(TyonMacro));
	tyon_macro_set_count(tyon_macro, count);
	tyon_macro->loop = gaminggear_macro->keystrokes.loop;
	tyon_macro_set_macroset_name(tyon_macro, gaminggear_macro->macroset);
	tyon_macro_set_macro_name(tyon_macro, gaminggear_macro->macro);

	for (i = 0; i < count; ++i)
		gaminggear_macro_keystroke_to_roccat_keystroke(&gaminggear_macro->keystrokes.keystrokes[i], &tyon_macro->keystrokes[i]);

	return TRUE;
}

GaminggearMacro *tyon_macro_to_gaminggear_macro(TyonMacro const *tyon_macro) {
	GaminggearMacro *gaminggear_macro;
	GaminggearMacroKeystroke keystroke;
	guint i, count;

	gaminggear_macro = gaminggear_macro_new((gchar const *)tyon_macro->macroset_name, (gchar const *)tyon_macro->macro_name, NULL);

	gaminggear_macro->keystrokes.loop = tyon_macro->loop;

	count = tyon_macro_get_count(tyon_macro);
	count = MIN(count, GAMINGGEAR_MACRO_KEYSTROKES_NUM);
	count = MIN(count, TYON_MACRO_KEYSTROKES_NUM);

	for (i = 0; i < count; ++i) {
		roccat_keystroke_to_gaminggear_macro_keystroke(&tyon_macro->keystrokes[i], &keystroke);
		gaminggear_macro_keystrokes_add(&gaminggear_macro->keystrokes, &keystroke);
	}

	return gaminggear_macro;
}

gboolean tyon_macro_equal(TyonMacro const *left, TyonMacro const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY_TO_END(left, right, TyonMacro, loop);
	return equal ? FALSE : TRUE;
}

void tyon_macro_copy(TyonMacro *destination, TyonMacro const *source) {
	memcpy(destination, source, sizeof(TyonMacro));
}
