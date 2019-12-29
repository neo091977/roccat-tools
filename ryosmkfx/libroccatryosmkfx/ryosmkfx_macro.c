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

#include "ryosmkfx_macro.h"
#include "ryosmkfx_device.h"
#include "ryosmkfx.h"
#include "roccat_helper.h"
#include "roccat.h"
#include "i18n-lib.h"
#include <string.h>

typedef struct _RyosmkfxMacro1 RyosmkfxMacro1;
typedef struct _RyosmkfxMacro2 RyosmkfxMacro2;

enum {
	RYOSMKFX_MACRO_1_DATA_SIZE = 1024,
	RYOSMKFX_MACRO_2_DATA_SIZE = sizeof(RyosmkfxMacro) - RYOSMKFX_MACRO_1_DATA_SIZE,
	RYOSMKFX_MACRO_2_UNUSED_SIZE = 1024 - RYOSMKFX_MACRO_2_DATA_SIZE,
};

typedef enum {
	RYOSMKFX_CONTROL_DATA_INDEX_MACRO_1 = 0x10,
	RYOSMKFX_CONTROL_DATA_INDEX_MACRO_2 = 0x20,
} RyosmkfxControlDataIndex;

struct _RyosmkfxMacro1 {
	guint8 report_id;
	guint8 one;
	guint8 data[RYOSMKFX_MACRO_1_DATA_SIZE];
} __attribute__ ((packed));

struct _RyosmkfxMacro2 {
	guint8 report_id;
	guint8 two;
	guint8 data[RYOSMKFX_MACRO_2_DATA_SIZE];
	guint8 unused[RYOSMKFX_MACRO_2_UNUSED_SIZE];
} __attribute__ ((packed));

void ryosmkfx_macro_set_macroset_name(RyosmkfxMacro *macro, gchar const *new_name) {
	g_strlcpy((gchar *)(macro->macroset_name), new_name, RYOSMKFX_MACRO_MACROSET_NAME_LENGTH);
}

void ryosmkfx_macro_set_macro_name(RyosmkfxMacro *macro, gchar const *new_name) {
	g_strlcpy((gchar *)(macro->macro_name), new_name, RYOSMKFX_MACRO_MACRO_NAME_LENGTH);
}

static void ryosmkfx_macro_finalize(RyosmkfxMacro *macro, guint profile_index, guint button_index) {
	macro->profile_index = profile_index;
	macro->button_index = button_index;
}

gboolean ryosmkfx_macro_write(RoccatDevice *ryosmkfx, guint profile_index, guint button_index, RyosmkfxMacro *macro, GError **error) {
	RyosmkfxMacro1 macro1;
	RyosmkfxMacro2 macro2;
	gboolean result;

	g_assert(profile_index < RYOSMKFX_PROFILE_NUM);

	ryosmkfx_macro_finalize(macro, profile_index, button_index);

	macro1.report_id = RYOS_REPORT_ID_MACRO;
	macro1.one = 1;
	memcpy(macro1.data, macro, RYOSMKFX_MACRO_1_DATA_SIZE);

	macro2.report_id = RYOS_REPORT_ID_MACRO;
	macro2.two = 2;
	memcpy(macro2.data, (guint8 *)macro + RYOSMKFX_MACRO_1_DATA_SIZE, RYOSMKFX_MACRO_2_DATA_SIZE);
	memset(macro2.unused, 0, RYOSMKFX_MACRO_2_UNUSED_SIZE);
	//macro2.unused[] = 0x41; // FIXME

	result = ryos_device_write(ryosmkfx, (gchar const *)&macro1, sizeof(RyosmkfxMacro1), error);
	if (!result)
		return FALSE;

	return ryos_device_write(ryosmkfx, (gchar const *)&macro2, sizeof(RyosmkfxMacro2), error);
}

RyosmkfxMacro *ryosmkfx_macro_read(RoccatDevice *ryosmkfx, guint profile_index, guint button_index, GError **error) {
	RyosmkfxMacro1 *macro1;
	RyosmkfxMacro2 *macro2;
	RyosmkfxMacro *macro;

	g_assert(profile_index < RYOSMKFX_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(ryosmkfx));

	if (!ryos_select(ryosmkfx, profile_index | RYOSMKFX_CONTROL_DATA_INDEX_MACRO_1, button_index, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(ryosmkfx));
		return NULL;
	}

	macro1 = (RyosmkfxMacro1 *)ryos_device_read(ryosmkfx, RYOS_REPORT_ID_MACRO, sizeof(RyosmkfxMacro1), error);
	if (!macro1) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(ryosmkfx));
		return NULL;
	}

	if (!ryos_select(ryosmkfx, profile_index | RYOSMKFX_CONTROL_DATA_INDEX_MACRO_2, button_index, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(ryosmkfx));
		g_free(macro1);
		return NULL;
	}

	macro2 = (RyosmkfxMacro2 *)ryos_device_read(ryosmkfx, RYOS_REPORT_ID_MACRO, sizeof(RyosmkfxMacro2), error);
	if (!macro2) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(ryosmkfx));
		g_free(macro1);
		return NULL;
	}

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(ryosmkfx));

	macro = (RyosmkfxMacro *)g_malloc(sizeof(RyosmkfxMacro));
	memcpy(macro, macro1->data, RYOSMKFX_MACRO_1_DATA_SIZE);
	memcpy((guint8 *)macro + RYOSMKFX_MACRO_1_DATA_SIZE, macro2->data, RYOSMKFX_MACRO_2_DATA_SIZE);

	g_free(macro1);
	g_free(macro2);

	return macro;
}

gboolean gaminggear_macro_to_ryosmkfx_macro(GaminggearMacro const *gaminggear_macro, RyosmkfxMacro *ryosmkfx_macro, GError **error) {
	guint i, count;

	g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

	count = gaminggear_macro_keystrokes_get_count(&gaminggear_macro->keystrokes);
	if (count > RYOSMKFX_MACRO_KEYSTROKES_NUM) {
		g_set_error(error, ROCCAT_ERROR, ROCCAT_ERROR_MACRO_TOO_LONG,
				_("Macro contains %u actions while device only supports %u actions"), count, RYOSMKFX_MACRO_KEYSTROKES_NUM);
		return FALSE;
	}

	memset(ryosmkfx_macro, 0, sizeof(RyosmkfxMacro));
	ryosmkfx_macro_set_count(ryosmkfx_macro, count);
	ryosmkfx_macro->loop = gaminggear_macro->keystrokes.loop;
	ryosmkfx_macro_set_macroset_name(ryosmkfx_macro, gaminggear_macro->macroset);
	ryosmkfx_macro_set_macro_name(ryosmkfx_macro, gaminggear_macro->macro);

	for (i = 0; i < count; ++i)
		gaminggear_macro_keystroke_to_roccat_keystroke(&gaminggear_macro->keystrokes.keystrokes[i], &ryosmkfx_macro->keystrokes[i]);

	return TRUE;
}

GaminggearMacro *ryosmkfx_macro_to_gaminggear_macro(RyosmkfxMacro const *ryosmkfx_macro) {
	GaminggearMacro *gaminggear_macro;
	GaminggearMacroKeystroke keystroke;
	guint i, count;

	gaminggear_macro = gaminggear_macro_new((gchar const *)ryosmkfx_macro->macroset_name, (gchar const *)ryosmkfx_macro->macro_name, NULL);

	gaminggear_macro->keystrokes.loop = ryosmkfx_macro->loop;

	count = ryosmkfx_macro_get_count(ryosmkfx_macro);
	count = MIN(count, GAMINGGEAR_MACRO_KEYSTROKES_NUM);
	count = MIN(count, RYOSMKFX_MACRO_KEYSTROKES_NUM);

	for (i = 0; i < count; ++i) {
		roccat_keystroke_to_gaminggear_macro_keystroke(&ryosmkfx_macro->keystrokes[i], &keystroke);
		gaminggear_macro_keystrokes_add(&gaminggear_macro->keystrokes, &keystroke);
	}

	return gaminggear_macro;
}

gboolean ryosmkfx_macro_equal(RyosmkfxMacro const *left, RyosmkfxMacro const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY_TO_END(left, right, RyosmkfxMacro, loop);
	return equal ? FALSE : TRUE;
}

void ryosmkfx_macro_copy(RyosmkfxMacro *destination, RyosmkfxMacro const *source) {
	memcpy(destination, source, sizeof(RyosmkfxMacro));
}
