#ifndef __ROCCAT_HELPER_H__
#define __ROCCAT_HELPER_H__

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

#include <glib.h>
#include <string.h>

G_BEGIN_DECLS

/*!
 * \brief sets or clears bit of byte according to value
 *
 * \e bit can be in range 0 to 7
 */
void roccat_set_bit8(guint8 *byte, guint bit, gboolean value);

/*!
 * \brief returns state of bit in byte
 *
 * \e bit can be in range 0 to 7
 *
 * \retval bit
 */
gboolean roccat_get_bit8(guint8 byte, guint bit);

void roccat_set_bit16(guint16 *word, guint bit, gboolean value);
gboolean roccat_get_bit16(guint16 word, guint bit);

void roccat_toggle_bit8(guint8 *byte, guint bit);

void roccat_bitfield_set_bit(guint8 *array, gsize bit, gboolean value);
gboolean roccat_bitfield_get_bit(guint8 const *array, gsize bit);
void roccat_bitfield_toggle_bit(guint8 *array, gsize bit);

typedef enum {
	ROCCAT_NIBBLE_HIGH = 1,
	ROCCAT_NIBBLE_LOW = 0,
} RoccatNibble;

void roccat_set_nibble8(guint8 *byte, guint nibble, guint8 value);
guint8 roccat_get_nibble8(guint8 byte, guint nibble);

void roccat_set_nibble16(guint16 *word, guint nibble, guint16 value);
guint16 roccat_get_nibble16(guint16 word, guint nibble);

void roccat_nibblefield_set_nibble(guint8 *array, gsize nibble, guint8 value);
guint8 roccat_nibblefield_get_nibble(guint8 const *array, gsize nibble);

/*!
 * \brief creates directory with user access with parents if not existent.
 * \retval TRUE if successful or directory already exists.
 * 	   On Error returns \c FALSE with \e error set.
 */
gboolean roccat_create_dir_if_needed(gchar const *dir, GError **error);

/*!
 * \brief reads data from file
 *
 * If \e length is != 0 read bytes are checked and \e error is set to G_FILE_ERROR_INVAL if
 * wrong amount of data is read.
 *
 * Result is NULL terminated.
 *
 * \retval If successful returns pointer to data, that has to be freed with g_free().
 *         On error returns +NULL+ with error set.
 */
gchar *roccat_read_from_file(gchar const *filename, gsize length, GError **error);

/*!
 * \brief returns the systemwide configuration directory
 * \retval Directory that has to be freed with g_free().
 */
gchar *roccat_profile_dir(void);

/*!
 * \brief writes data with user and group access
 *
 * Works like g_file_set_contents(), but with mode 660.
 *
 * \retval \c TRUE if successful. On Error returns \c FALSE with \e error set.
 */
gboolean roccat_profile_write_with_path(gchar const *filename, gchar const *data, gsize length, GError **error);

/*!
 * \brief creates directory with user and group access with parents if not existent.
 *
 * Works like roccat_create_dir_if_needed(), but with mode 770
 *
 * \retval \c TRUE if successful. On Error returns \c FALSE with \e error set.
 */
gboolean roccat_profile_dir_create_if_needed(gchar const *dir, GError **error);

/*!
 * \brief creates a proposition for a filename.
 * \retval pointer to string that has to be freed with g_free().
 */
gchar *roccat_profile_filename_proposition(gchar const *profile_name, guint profile_index, gchar const *extension);

/*!
 * \brief adds extension to filename or returns unaltered copy.
 * \retval pointer to string that has to be freed with g_free().
 */
gchar *roccat_create_filename_with_extension(gchar const *filename, gchar const *extension);

gulong roccat_calc_bytesum(gconstpointer data, gsize size);

#define ROCCAT_BYTESUM_PARTIALLY(ptr, struct_type, first_member, last_member) \
	roccat_calc_bytesum((gchar const *)ptr + G_STRUCT_OFFSET(struct_type, first_member), \
		G_STRUCT_OFFSET(struct_type, last_member) - G_STRUCT_OFFSET(struct_type, first_member))

#define ROCCAT_MEMCMP_PARTIALLY(ptr1, ptr2, struct_type, first_member, last_member) \
	memcmp((gchar const *)ptr1 + G_STRUCT_OFFSET(struct_type, first_member), \
		(gchar const *)ptr2 + G_STRUCT_OFFSET(struct_type, first_member), \
		G_STRUCT_OFFSET(struct_type, last_member) - G_STRUCT_OFFSET(struct_type, first_member))

#define ROCCAT_MEMCMP_PARTIALLY_TO_END(ptr1, ptr2, struct_type, first_member) \
	memcmp((gchar const *)ptr1 + G_STRUCT_OFFSET(struct_type, first_member), \
		(gchar const *)ptr2 + G_STRUCT_OFFSET(struct_type, first_member), \
		sizeof(struct_type) - G_STRUCT_OFFSET(struct_type, first_member))

gchar *roccat_data8_to_string(guchar const *data, gsize count);
gchar *roccat_data16_to_string(guint16 const *data, gsize count);

static inline guint roccat_sanitize_index(guint max, guint value, guint sane) {
	if (value >= max) {
		g_warning("Index %u outside of range [0,%u) corrected to %u", value, max, sane);
		return sane;
	}
	return value;
}

static inline guint roccat_sanitize_index_min(guint max, guint value) {
	return roccat_sanitize_index(max, value, 0);
}

static inline guint roccat_sanitize_index_max(guint max, guint value) {
	return roccat_sanitize_index(max, value, max - 1);
}

void roccat_textdomain(void);

gchar *roccat_xdg_get_default_folder(gchar const *type, GError **error);

gboolean roccat_user_decision_console(gchar const *text);

G_END_DECLS

#endif
