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

#include "nyth_macro.h"
#include "nyth_device.h"
#include "roccat_helper.h"
#include "roccat.h"
#include "i18n-lib.h"
#include <string.h>

typedef struct _NythMacro1 NythMacro1;
typedef struct _NythMacro2 NythMacro2;

enum {
	NYTH_MACRO_1_DATA_SIZE = 1024,
	NYTH_MACRO_2_DATA_SIZE = sizeof(NythMacro) - NYTH_MACRO_1_DATA_SIZE,
	NYTH_MACRO_2_UNUSED_SIZE = 1024 - NYTH_MACRO_2_DATA_SIZE,
};

struct _NythMacro1 {
	guint8 report_id;
	guint8 one;
	guint8 data[NYTH_MACRO_1_DATA_SIZE];
} __attribute__ ((packed));

struct _NythMacro2 {
	guint8 report_id;
	guint8 two;
	guint8 data[NYTH_MACRO_2_DATA_SIZE];
	guint8 unused[NYTH_MACRO_2_UNUSED_SIZE];
} __attribute__ ((packed));

void nyth_macro_set_macroset_name(NythMacro *macro, gchar const *new_name) {
	g_strlcpy((gchar *)(macro->macroset_name), new_name, NYTH_MACRO_MACROSET_NAME_LENGTH);
}

void nyth_macro_set_macro_name(NythMacro *macro, gchar const *new_name) {
	g_strlcpy((gchar *)(macro->macro_name), new_name, NYTH_MACRO_MACRO_NAME_LENGTH);
}

static void nyth_macro_finalize(NythMacro *macro, guint profile_index, guint button_index) {
	macro->profile_index = profile_index;
	macro->button_index = button_index;
}

gboolean nyth_macro_write(RoccatDevice *nyth, guint profile_index, guint button_index, NythMacro *macro, GError **error) {
	NythMacro1 macro1;
	NythMacro2 macro2;
	gboolean result;

	g_assert(profile_index < NYTH_PROFILE_NUM);

	nyth_macro_finalize(macro, profile_index, button_index);

	macro1.report_id = NYTH_REPORT_ID_MACRO;
	macro1.one = 1;
	memcpy(macro1.data, macro, NYTH_MACRO_1_DATA_SIZE);

	macro2.report_id = NYTH_REPORT_ID_MACRO;
	macro2.two = 2;
	memcpy(macro2.data, (guint8 *)macro + NYTH_MACRO_1_DATA_SIZE, NYTH_MACRO_2_DATA_SIZE);
	memset(macro2.unused, 0, NYTH_MACRO_2_UNUSED_SIZE);

	result = nyth_device_write(nyth, (gchar const *)&macro1, sizeof(NythMacro1), error);
	if (!result)
		return FALSE;

	return nyth_device_write(nyth, (gchar const *)&macro2, sizeof(NythMacro2), error);
}

NythMacro *nyth_macro_read(RoccatDevice *nyth, guint profile_index, guint button_index, GError **error) {
	NythMacro1 *macro1;
	NythMacro2 *macro2;
	NythMacro *macro;

	g_assert(profile_index < NYTH_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(nyth));

	if (!nyth_select(nyth, profile_index, NYTH_CONTROL_DATA_INDEX_MACRO_1, button_index, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(nyth));
		return NULL;
	}

	macro1 = (NythMacro1 *)nyth_device_read(nyth, NYTH_REPORT_ID_MACRO, sizeof(NythMacro1), error);
	if (!macro1) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(nyth));
		return NULL;
	}

	if (!nyth_select(nyth, profile_index, NYTH_CONTROL_DATA_INDEX_MACRO_2, button_index, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(nyth));
		g_free(macro1);
		return NULL;
	}

	macro2 = (NythMacro2 *)nyth_device_read(nyth, NYTH_REPORT_ID_MACRO, sizeof(NythMacro2), error);
	if (!macro2) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(nyth));
		g_free(macro1);
		return NULL;
	}

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(nyth));

	macro = (NythMacro *)g_malloc(sizeof(NythMacro));
	memcpy(macro, macro1->data, NYTH_MACRO_1_DATA_SIZE);
	memcpy((guint8 *)macro + NYTH_MACRO_1_DATA_SIZE, macro2->data, NYTH_MACRO_2_DATA_SIZE);

	g_free(macro1);
	g_free(macro2);

	return macro;
}

gboolean gaminggear_macro_to_nyth_macro(GaminggearMacro const *gaminggear_macro, NythMacro *nyth_macro, GError **error) {
	guint i, count;

	g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

	count = gaminggear_macro_keystrokes_get_count(&gaminggear_macro->keystrokes);
	if (count > NYTH_MACRO_KEYSTROKES_NUM) {
		g_set_error(error, ROCCAT_ERROR, ROCCAT_ERROR_MACRO_TOO_LONG,
				_("Macro contains %u actions while device only supports %u actions"), count, NYTH_MACRO_KEYSTROKES_NUM);
		return FALSE;
	}

	memset(nyth_macro, 0, sizeof(NythMacro));
	nyth_macro_set_count(nyth_macro, count);
	nyth_macro->loop = gaminggear_macro->keystrokes.loop;
	nyth_macro_set_macroset_name(nyth_macro, gaminggear_macro->macroset);
	nyth_macro_set_macro_name(nyth_macro, gaminggear_macro->macro);

	for (i = 0; i < count; ++i)
		gaminggear_macro_keystroke_to_roccat_keystroke(&gaminggear_macro->keystrokes.keystrokes[i], &nyth_macro->keystrokes[i]);

	return TRUE;
}

GaminggearMacro *nyth_macro_to_gaminggear_macro(NythMacro const *nyth_macro) {
	GaminggearMacro *gaminggear_macro;
	GaminggearMacroKeystroke keystroke;
	guint i, count;

	gaminggear_macro = gaminggear_macro_new((gchar const *)nyth_macro->macroset_name, (gchar const *)nyth_macro->macro_name, NULL);

	gaminggear_macro->keystrokes.loop = nyth_macro->loop;

	count = nyth_macro_get_count(nyth_macro);
	count = MIN(count, GAMINGGEAR_MACRO_KEYSTROKES_NUM);
	count = MIN(count, NYTH_MACRO_KEYSTROKES_NUM);

	for (i = 0; i < count; ++i) {
		roccat_keystroke_to_gaminggear_macro_keystroke(&nyth_macro->keystrokes[i], &keystroke);
		gaminggear_macro_keystrokes_add(&gaminggear_macro->keystrokes, &keystroke);
	}

	return gaminggear_macro;
}

gboolean nyth_macro_equal(NythMacro const *left, NythMacro const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY_TO_END(left, right, NythMacro, loop);
	return equal ? FALSE : TRUE;
}

void nyth_macro_copy(NythMacro *destination, NythMacro const *source) {
	memcpy(destination, source, sizeof(NythMacro));
}
