#ifndef __ROCCAT_SKELTR_MACRO_H__
#define __ROCCAT_SKELTR_MACRO_H__

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

#include "skeltr_device.h"
#include "roccat_keystroke.h"
#include <gaminggear/macro.h>

G_BEGIN_DECLS

enum {
	SKELTR_MACRO_EDITOR_MACROSET_NAME_LENGTH = 40,
	SKELTR_MACRO_EDITOR_MACRO_NAME_LENGTH = 32,
	SKELTR_MACRO_RECORD_DEVICE_LENGTH = 24,
	SKELTR_MACRO_RECORD_MACROSET_NAME_LENGTH = 24,
	SKELTR_MACRO_RECORD_MACRO_NAME_LENGTH = 24,
	SKELTR_MACRO_KEYSTROKES_NUM = 480,
	SKELTR_MACRO_NUM = 20 + 6 + 10, /* SKELTR_KEYS_EASYZONE_NUM + SKELTR_KEYS_THUMBSTER_NUM * 2 + SKELTR_KEYS_MACRO_NUM * 2 */
};

typedef struct _SkeltrMacro SkeltrMacro;
typedef struct _SkeltrMacroRecordNames SkeltrMacroRecordNames;
typedef struct _SkeltrMacroEditorNames SkeltrMacroEditorNames;

struct _SkeltrMacroEditorNames {
	guint8 macroset_name[SKELTR_MACRO_EDITOR_MACROSET_NAME_LENGTH];
	guint8 macro_name[SKELTR_MACRO_EDITOR_MACRO_NAME_LENGTH];
} __attribute__ ((packed));

struct _SkeltrMacroRecordNames {
	guint8 device[SKELTR_MACRO_RECORD_DEVICE_LENGTH];
	guint8 macroset_name[SKELTR_MACRO_RECORD_MACROSET_NAME_LENGTH];
	guint8 macro_name[SKELTR_MACRO_RECORD_MACRO_NAME_LENGTH];
} __attribute__ ((packed));

struct _SkeltrMacro {
	guint8 report_id; /* RYOS_REPORT_ID_MACRO */
	guint16 size; /* 2002 */
	guint8 profile_index;
	guint8 button_index;
	guint8 loop;
	union {
		SkeltrMacroEditorNames editor;
		SkeltrMacroRecordNames record;
	};
	guint16 count;
	RoccatKeystroke keystrokes[SKELTR_MACRO_KEYSTROKES_NUM];
	guint16 checksum; // FIXME used in rkp, check also used in transfer?
} __attribute__ ((packed));

enum {
	SKELTR_MACRO_CONST_SIZE = sizeof(SkeltrMacro) - SKELTR_MACRO_KEYSTROKES_NUM * sizeof(RoccatKeystroke),
};

static inline guint16 skeltr_macro_get_size(SkeltrMacro const *macro) {
	return GUINT16_FROM_LE(macro->size);
}

static inline void skeltr_macro_set_size(SkeltrMacro *macro, guint16 new_value) {
	macro->size = GUINT16_TO_LE(new_value);
}

static inline guint16 skeltr_macro_get_count(SkeltrMacro const *macro) {
	return GUINT16_FROM_LE(macro->count);
}

static inline void skeltr_macro_set_count(SkeltrMacro *macro, guint16 new_value) {
	macro->count = GUINT16_TO_LE(new_value);
}

static inline guint16 skeltr_macro_get_checksum(SkeltrMacro const *macro) {
	return GUINT16_FROM_LE(macro->checksum);
}

static inline void skeltr_macro_set_checksum(SkeltrMacro *macro, guint16 new_value) {
	macro->checksum = GUINT16_TO_LE(new_value);
}

gchar const *skeltr_macro_get_macroset_name(SkeltrMacro const *macro);
gchar const *skeltr_macro_get_macro_name(SkeltrMacro const *macro);

void skeltr_macro_finalize(SkeltrMacro *macro, guint profile_index, guint keys_index);
gboolean skeltr_macro_write(RoccatDevice *device, guint profile_index, guint button_index, SkeltrMacro *macro, GError **error);
SkeltrMacro *skeltr_macro_read(RoccatDevice *device, guint profile_index, guint button_index, GError **error);

gboolean gaminggear_macro_to_skeltr_macro(GaminggearMacro const *gaminggear_macro, SkeltrMacro *skeltr_macro, GError **error);
GaminggearMacro *skeltr_macro_to_gaminggear_macro(SkeltrMacro const *skeltr_macro);

gboolean skeltr_macro_equal(SkeltrMacro const *left, SkeltrMacro const *right);
void skeltr_macro_copy(SkeltrMacro *destination, SkeltrMacro const *source);
SkeltrMacro *skeltr_macro_dup(SkeltrMacro const *old);

G_END_DECLS

#endif
