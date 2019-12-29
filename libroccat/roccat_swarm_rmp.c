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

#include "roccat_swarm_rmp.h"
#include "roccat_helper.h"
#include "g_roccat_helper.h"
#include "i18n-lib.h"

void roccat_swarm_rmp_header_set_profile_name(RoccatSwarmRmpHeader *header, gchar const *name) {
	gunichar2 *string;
	glong items;
	string = g_utf8_to_utf16(name, -1L, NULL, &items, NULL);
	items = MIN(ROCCAT_SWARM_RMP_PROFILE_NAME_LENGTH, items);
	memcpy(&header->profile_name, string, items * sizeof(gunichar2));
	g_free(string);
}

gchar *roccat_swarm_rmp_header_get_profile_name(RoccatSwarmRmpHeader const *header) {
	return g_utf16_to_utf8((gunichar2 const *)(&header->profile_name), ROCCAT_SWARM_RMP_PROFILE_NAME_LENGTH, NULL, NULL, NULL);
}

guint32 roccat_swarm_rmp_read_bigendian32(gconstpointer *data) {
	guint32 result = GUINT32_FROM_BE(*(guint32 const *)*data);
	*data += sizeof(guint32);
	return result;
}

gboolean roccat_swarm_rmp_read_unknown(gconstpointer *data) {
	gchar *string;

	/* keep this in two lines to prevent pointer change problem */
	guint32 size = roccat_swarm_rmp_read_bigendian32(data);
	string = roccat_data8_to_string(*data, size);
	g_debug(_("Rmp unknown chunk size 0x%08x: %s"), size, string);
	g_free(string);
	*data += size;
	return TRUE;
}

gboolean roccat_swarm_rmp_read_unknown_with_count(gconstpointer *data) {
	guint32 count;
	guint32 i;

	count = roccat_swarm_rmp_read_bigendian32(data);
	g_debug(_("Rmp unknown count 0x%08x {"), count);

	for (i = 0; i < count; ++i)
		if (!roccat_swarm_rmp_read_unknown(data))
			return FALSE;

	g_debug(_("}"));

	return TRUE;
}

static gboolean read_png(gconstpointer *data) {
	guint32 length;
	guint32 type;

	*data += 8; //signature

	do {
		length = roccat_swarm_rmp_read_bigendian32(data);
		type = roccat_swarm_rmp_read_bigendian32(data);
		*data += length;
		(void)roccat_swarm_rmp_read_bigendian32(data); // crc
	} while(type != 0x49454e44);

	return TRUE;
}

gboolean roccat_swarm_rmp_read_pictures(gconstpointer *data) {
	guint32 count;
	guint32 i;

	count = roccat_swarm_rmp_read_bigendian32(data);
	for (i = 0; i < count; ++i)
		read_png(data);

	return TRUE;
}

gboolean roccat_swarm_rmp_read_timers(gconstpointer *data) {
	guint32 count;
	guint32 size;
	guint i;

	count = roccat_swarm_rmp_read_bigendian32(data);
	for (i = 0; i < count; ++i) {
		(void)roccat_swarm_rmp_read_bigendian32(data); // button_index
		size = roccat_swarm_rmp_read_bigendian32(data);
		if (size != sizeof(RoccatSwarmRmpTimer)) {
			g_warning("%u != sizeof(RoccatSwarmRmpTimer)", size);
			return FALSE;
		}

		*data += size;
	}

	return TRUE;
}

static gpointer roccat_swarm_rmp_write(gpointer *data, gsize *length, guint32 size) {
	return roccat_realloc(data, length, size);
}

void roccat_swarm_rmp_write_bigendian32(gpointer *data, gsize *length, guint32 value) {
	guint32 *start = (guint32 *)roccat_swarm_rmp_write(data, length, sizeof(guint32));
	*start = GUINT32_TO_BE(value);
}

gpointer roccat_swarm_rmp_write_with_size(gpointer *data, gsize *length, guint32 size) {
	roccat_swarm_rmp_write_bigendian32(data, length, size);
	return roccat_swarm_rmp_write(data, length, size);
}

gpointer roccat_swarm_rmp_write_unknown(gpointer *data, gsize *length, gsize size) {
	return roccat_swarm_rmp_write_with_size(data, length, size);
}

void roccat_swarm_rmp_write_unknown_with_count(gpointer *data, gsize *length, gsize count, gsize size) {
	guint i;

	roccat_swarm_rmp_write_bigendian32(data, length, count);

	for (i = 0; i < count; ++i)
		(void)roccat_swarm_rmp_write_unknown(data, length, size);
}

void roccat_swarm_rmp_write_timers(gpointer *data, gsize *length) {
	roccat_swarm_rmp_write_unknown_with_count(data, length, 0, 0);
}

gchar *roccat_swarm_rmp_read_bigendian_string(gconstpointer *data, guint32 chars) {
	gchar *utf8;
	utf8 = roccat_be_utf16_to_utf8((gunichar2 const *)*data, chars, NULL, NULL, NULL);
	*data += chars * sizeof(gunichar2);
	return utf8;
}

gchar *roccat_swarm_rmp_read_bigendian_string_with_size(gconstpointer *data) {
	guint32 size;

	size = roccat_swarm_rmp_read_bigendian32(data);
	if (size % sizeof(gunichar2)) {
		g_warning(_("Rmp wrong divider %u %% %lu"), size, (gulong)sizeof(gunichar2));
		return NULL;
	}

	return roccat_swarm_rmp_read_bigendian_string(data, size / sizeof(gunichar2));
}

void roccat_swarm_rmp_write_bigendian_string(gpointer *data, gsize *length, gchar const *string, guint32 chars) {
	gunichar2 *be_utf16;
	glong items_written;
	gpointer pointer;

	be_utf16 = roccat_utf8_to_be_utf16(string, chars, NULL, &items_written, NULL);
	pointer = roccat_swarm_rmp_write(data, length, chars * sizeof(gunichar2));
	memcpy(pointer, be_utf16, items_written * sizeof(gunichar2));
	g_free(be_utf16);
}

void roccat_swarm_rmp_write_bigendian_string_with_size(gpointer *data, gsize *length, gchar const *string, guint32 max_chars) {
	gunichar2 *be_utf16;
	glong items_written;
	gpointer pointer;

	be_utf16 = roccat_utf8_to_be_utf16(string, max_chars, NULL, &items_written, NULL);
	pointer = roccat_swarm_rmp_write_with_size(data, length, items_written * sizeof(gunichar2));
	memcpy(pointer, be_utf16, items_written * sizeof(gunichar2));
	g_free(be_utf16);
}

gpointer roccat_swarm_rmp_read_with_path(gchar const *path, gsize *length, GError **error) {
	guint8 *rmp;
	gboolean result;
	result = g_file_get_contents(path, (gchar **)&rmp, length, error);
	if (!result)
		return NULL;
	return rmp;
}

gboolean roccat_swarm_rmp_write_with_path(gchar const *path, gconstpointer rmp, gsize length, GError **error) {
	return roccat_profile_write_with_path(path, (gchar const *)rmp, length, error);
}
