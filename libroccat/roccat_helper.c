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

#include "roccat_helper.h"
#include "config.h"
#include "i18n-lib.h"
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <locale.h>

gchar *roccat_xdg_get_default_folder(gchar const *type, GError **error) {
	FILE *fp;
	gchar *command;
	gchar *buffer = NULL;
	long size = 0;
	int c;

	command = g_strdup_printf("xdg-user-dir %s", type);
	fp = popen(command, "r");
	if (fp == NULL) {
		g_free(command);
		g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(errno), _("Could not open pipe: %s"), g_strerror(errno));
		return NULL;
	}
	g_free(command);

	while ((c = getc(fp)) != EOF && c != '\n') {
		++size;
		buffer = (gchar *)g_realloc(buffer, size);
		buffer[size - 1] = c;
	}

	buffer = (gchar *)g_realloc(buffer, size + 1);
	buffer[size] = '\0';

	if (pclose(fp)) {
		g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(errno), _("Could not close pipe: %s"), g_strerror(errno));
		return NULL;
	}

	return buffer;
}

static gboolean roccat_create_dir_if_needed_with_mode(gchar const *dir, int mode, GError **error) {
	if (!g_file_test(dir, G_FILE_TEST_IS_DIR)) {
		if (g_mkdir_with_parents(dir, mode)) {
			g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(errno), _("Could not create directory: %s"), g_strerror(errno));
			return FALSE;
		}
	}
	return TRUE;
}

gboolean roccat_create_dir_if_needed(gchar const *dir, GError **error) {
	return roccat_create_dir_if_needed_with_mode(dir, 0700, error);
}

gchar *roccat_read_from_file(gchar const *filename, gsize length, GError **error) {
	gchar *result;
	gsize bytes;

	if (!g_file_get_contents(filename, &result, &bytes, error)) {
		return NULL;
	}

	if (length && bytes != length) {
		g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_INVAL, _("Data has wrong length, %zu instead of %zu"), bytes, length);
		g_free(result);
		return NULL;
	}

	return result;
}

gchar *roccat_profile_dir(void) {
	return g_build_path("/", "/var", "lib", "roccat", NULL);
}

gboolean roccat_profile_dir_create_if_needed(gchar const *dir, GError **error) {
	mode_t mask;
	gboolean retval;

	mask = umask(S_IROTH | S_IWOTH | S_IXOTH);
	retval = roccat_create_dir_if_needed_with_mode(dir, 0770, error);
	(void)umask(mask);

	return retval;
}

gboolean roccat_profile_write_with_path(gchar const *filename, gchar const *data, gsize length, GError **error) {
	mode_t mask;
	gboolean retval;

	mask = umask(S_IROTH | S_IWOTH | S_IXOTH);
	retval = g_file_set_contents(filename, data, length, error);
	(void)umask(mask);

	return retval;
}

gchar *roccat_profile_filename_proposition(gchar const *profile_name, guint profile_index, gchar const *extension) {
	gchar *filename;

	if (profile_name != NULL && *profile_name != 0)
		filename = g_strdup_printf("%s.%s", profile_name, extension);
	else
		filename = g_strdup_printf("Profile%u.%s", profile_index + 1, extension);
	return filename;
}

gchar *roccat_create_filename_with_extension(gchar const *filename, gchar const *extension) {
	gchar *result;
	gchar *suffix;

	suffix = g_strconcat(".", extension, NULL);

	if (g_str_has_suffix(filename, suffix))
		result = g_strdup(filename);
	else
		result = g_strconcat(filename, suffix, NULL);

	g_free(suffix);

	return result;
}

void roccat_set_bit8(guint8 *byte, guint bit, gboolean value) {
	g_assert(bit < 8);
	if (value)
		*byte |= 1 << bit;
	else
		*byte &= ~(1 << bit);
}

gboolean roccat_get_bit8(guint8 byte, guint bit) {
	g_assert(bit < 8);
	return (byte & (1 << bit)) ? TRUE : FALSE;
}

void roccat_toggle_bit8(guint8 *byte, guint bit) {
	g_assert(bit < 8);
	*byte ^= 1 << bit;
}

void roccat_set_bit16(guint16 *word, guint bit, gboolean value) {
	g_assert(bit < 16);
	if (value)
		*word |= 1 << bit;
	else
		*word &= ~(1 << bit);
}

gboolean roccat_get_bit16(guint16 word, guint bit) {
	g_assert(bit < 16);
	return (word & (1 << bit)) ? TRUE : FALSE;
}

void roccat_set_nibble8(guint8 *byte, guint nibble, guint8 value) {
	if (nibble == 1)
		*byte = (*byte & 0x0f) | ((value << 4) & 0xf0);
	else
		*byte = (*byte & 0xf0) | (value & 0x0f);
}

guint8 roccat_get_nibble8(guint8 byte, guint nibble) {
	if (nibble == 1)
		return (byte & 0xf0) >> 4;
	else
		return byte & 0x0f;
}

void roccat_set_nibble16(guint16 *word, guint nibble, guint16 value) {
	guint shift = nibble << 2; /* *4 */
	guint16 mask = 0x000f << shift;
	value = value << shift;
	*word = (*word & ~mask) | (value & mask);
}

guint16 roccat_get_nibble16(guint16 word, guint nibble) {
	guint shift = nibble << 2; /* *4 */
	return (word >> shift) & 0x000f;
}

void roccat_nibblefield_set_nibble(guint8 *array, gsize nibble, guint8 value) {
	gsize byte_offset = nibble >> 1;
	guint nibble_in_byte = nibble & 1;

	roccat_set_nibble8(&array[byte_offset], nibble_in_byte, value);
}

guint8 roccat_nibblefield_get_nibble(guint8 const *array, gsize nibble) {
	gsize byte_offset = nibble >> 1;
	guint nibble_in_byte = nibble & 1;

	return roccat_get_nibble8(array[byte_offset], nibble_in_byte);
}

void roccat_bitfield_toggle_bit(guint8 *array, gsize bit) {
	gsize byte_offset;
	guint bit_in_byte;

	byte_offset = bit >> 3; /* /8 */
	bit_in_byte = bit & 7; /* %8 */

	roccat_toggle_bit8(array + byte_offset, bit_in_byte);
}

void roccat_bitfield_set_bit(guint8 *array, gsize bit, gboolean value) {
	gsize byte_offset;
	guint bit_in_byte;

	byte_offset = bit >> 3; /* /8 */
	bit_in_byte = bit & 7; /* %8 */

	roccat_set_bit8(array + byte_offset, bit_in_byte, value);
}

gboolean roccat_bitfield_get_bit(guint8 const *array, gsize bit) {
	gsize byte_offset;
	guint bit_in_byte;

	byte_offset = bit >> 3; /* /8 */
	bit_in_byte = bit & 7; /* %8 */

	return roccat_get_bit8(*(array + byte_offset), bit_in_byte);
}

gulong roccat_calc_bytesum(gconstpointer data, gsize size) {
	guchar const *array = (guchar const *)data;
	gsize i;
	gulong result = 0;

	for (i = 0; i < size; ++i)
		result += array[i];

	return result;
}

#if (GLIB_CHECK_VERSION(2, 34, 0))
G_DEFINE_QUARK(roccat-error-quark, roccat_error);
G_DEFINE_QUARK(roccat-errno-error-quark, roccat_errno_error);
#else
GQuark roccat_error_quark(void) {
	static GQuark q;
	if G_UNLIKELY (q == 0)
		q = g_quark_from_static_string("roccat-error-quark");
	return q;
}
GQuark roccat_errno_error_quark(void) {
	static GQuark q;
	if G_UNLIKELY (q == 0)
		q = g_quark_from_static_string("roccat-errno-error-quark");
	return q;
}
#endif

gchar *roccat_data8_to_string(guchar const *data, gsize count) {
	gchar *buffer;
	gsize i;
	guint n;

	if (count == 0)
		return g_strdup("");

	buffer = (gchar *)g_malloc(count * 3);

	n = sprintf(buffer, "%02x", data[0]);
	for (i = 1; i < count; ++i)
		n += sprintf(buffer + n, " %02x", data[i]);

	return buffer;
}

gchar *roccat_data16_to_string(guint16 const *data, gsize count) {
	gchar *buffer;
	gsize i;
	guint n;

	if (count == 0)
		return g_strdup("");

	buffer = (gchar *)g_malloc(count * 5);

	n = sprintf(buffer, "%04x", data[0]);
	for (i = 1; i < count; ++i)
		n += sprintf(buffer + n, " %04x", data[i]);

	return buffer;
}

void roccat_textdomain(void) {
	setlocale(LC_ALL, "");
	bindtextdomain(PROJECT_NAME, LOCALEDIR);
	bind_textdomain_codeset(PROJECT_NAME, "UTF-8");
	textdomain(PROJECT_NAME);
}

gboolean roccat_user_decision_console(gchar const *text) {
	gchar decision;
	int retval;

	g_print("%s", text);
	g_print(" (y|n)");
	retval = scanf("%c", &decision);
	if (retval != 1)
		return FALSE;
	fflush(stdin);
	return g_ascii_tolower(decision) == 'y';
}
