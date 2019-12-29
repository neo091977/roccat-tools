#ifndef __ROCCAT_SOVA_MACRO_H__
#define __ROCCAT_SOVA_MACRO_H__

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

#include "sova_device.h"
#include "roccat_keystroke.h"
#include <gaminggear/macro.h>

G_BEGIN_DECLS

enum {
	SOVA_MACRO_EDITOR_MACROSET_NAME_LENGTH = 40,
	SOVA_MACRO_EDITOR_MACRO_NAME_LENGTH = 32,
	SOVA_MACRO_RECORD_DEVICE_LENGTH = 24,
	SOVA_MACRO_RECORD_MACROSET_NAME_LENGTH = 24,
	SOVA_MACRO_RECORD_MACRO_NAME_LENGTH = 24,
	SOVA_MACRO_KEYSTROKES_NUM = 480,
};

typedef struct _SovaMacro SovaMacro;
typedef struct _SovaMacroRecordNames SovaMacroRecordNames;
typedef struct _SovaMacroEditorNames SovaMacroEditorNames;

struct _SovaMacroEditorNames {
	guint8 macroset_name[SOVA_MACRO_EDITOR_MACROSET_NAME_LENGTH];
	guint8 macro_name[SOVA_MACRO_EDITOR_MACRO_NAME_LENGTH];
} __attribute__ ((packed));

struct _SovaMacroRecordNames {
	guint8 device[SOVA_MACRO_RECORD_DEVICE_LENGTH];
	guint8 macroset_name[SOVA_MACRO_RECORD_MACROSET_NAME_LENGTH];
	guint8 macro_name[SOVA_MACRO_RECORD_MACRO_NAME_LENGTH];
} __attribute__ ((packed));

struct _SovaMacro {
	guint8 report_id; /* RYOS_REPORT_ID_MACRO */
	guint16 size; /* 2002 */
	guint8 profile_index;
	guint8 button_index;
	guint8 loop;
	union {
		SovaMacroEditorNames editor;
		SovaMacroRecordNames record;
	};
	guint16 count;
	RoccatKeystroke keystrokes[SOVA_MACRO_KEYSTROKES_NUM];
	guint16 checksum;
} __attribute__ ((packed));

enum {
	SOVA_MACRO_CONST_SIZE = sizeof(SovaMacro) - SOVA_MACRO_KEYSTROKES_NUM * sizeof(RoccatKeystroke),
};

static inline guint16 sova_macro_get_size(SovaMacro const *macro) {
	return GUINT16_FROM_LE(macro->size);
}

static inline void sova_macro_set_size(SovaMacro *macro, guint16 new_value) {
	macro->size = GUINT16_TO_LE(new_value);
}

static inline guint16 sova_macro_get_count(SovaMacro const *macro) {
	return GUINT16_FROM_LE(macro->count);
}

static inline void sova_macro_set_count(SovaMacro *macro, guint16 new_value) {
	macro->count = GUINT16_TO_LE(new_value);
}

gchar const *sova_macro_get_macroset_name(SovaMacro const *macro);
gchar const *sova_macro_get_macro_name(SovaMacro const *macro);

void sova_macro_finalize(SovaMacro *macro, guint profile_index, guint keys_index);
gboolean sova_macro_write(RoccatDevice *device, guint profile_index, guint button_index, SovaMacro *macro, GError **error);
SovaMacro *sova_macro_read(RoccatDevice *device, guint profile_index, guint button_index, GError **error);

gboolean gaminggear_macro_to_sova_macro(GaminggearMacro const *gaminggear_macro, SovaMacro *sova_macro, GError **error);
GaminggearMacro *sova_macro_to_gaminggear_macro(SovaMacro const *sova_macro);

gboolean sova_macro_equal(SovaMacro const *left, SovaMacro const *right);
void sova_macro_copy(SovaMacro *destination, SovaMacro const *source);
SovaMacro *sova_macro_dup(SovaMacro const *old);

G_END_DECLS

#endif
