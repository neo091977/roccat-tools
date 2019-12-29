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

#include "g_roccat_helper.h"
#include "i18n.h"
#include <string.h>
#include <stdlib.h>

static void G_GNUC_UNUSED g_roccat_call_destroy_notify(gpointer data, gpointer user_data) {
	((GDestroyNotify)user_data)(data);
}

#if !(GLIB_CHECK_VERSION(2, 28, 0))

void g_slist_free_full(GSList *list, GDestroyNotify freefunc) {
	g_slist_foreach(list, g_roccat_call_destroy_notify, freefunc);
	g_slist_free(list);
}

void g_list_free_full(GList *list, GDestroyNotify freefunc) {
	g_list_foreach(list, g_roccat_call_destroy_notify, freefunc);
	g_list_free(list);
}

#endif

GList *g_roccat_list_find_custom_backwards(GList *list, gconstpointer data, GCompareFunc func) {
	GList *element;
	element = g_list_last(list);
	while (element) {
		if (func(element->data, data) == 0)
			return element;
		element = g_list_previous(element);
	}

	return NULL;
}

/*
 * For this implementation to be a stable sort its important that:
 * - pivot is the last element of the list
 * - all elements smaller AND EQUAL are put in lesser list
 */
GList *g_roccat_list_stable_sort(GList *list, GCompareFunc compare_func) {
	GList *lesser = NULL;
	GList *greater = NULL;
	GList *pivot;
	GList *element;
	guint length;

	length = g_list_length(list);

	if (length <= 1)
		return list;

	pivot = g_list_last(list);
	list = g_list_remove_link(list, pivot);

	while (list) {
		element = list;
		list = g_list_remove_link(list, element);

		if (compare_func(element->data, pivot->data) <= 0)
			lesser = g_list_concat(lesser, element);
		else
			greater = g_list_concat(greater, element);
	}

	list = g_list_concat(g_roccat_list_stable_sort(lesser, compare_func), pivot);
	list = g_list_concat(list, g_roccat_list_stable_sort(greater, compare_func));
	return list;
}

gpointer roccat_key_file_get_binary(GKeyFile *key_file, gchar const *group_name, gchar const *key, gsize size, GError **error) {
	gchar *string, *source;
	gpointer result;
	guint8 *target;
	char hex[3] = { 0, 0, 0 };
	guint i;

	string = g_key_file_get_value(key_file, group_name, key, error);

	if (!string)
		return NULL;

	if (strlen(string) != size * 2) {
		g_set_error(error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_INVALID_VALUE, _("Data has wrong length, %zu instead of %zu"), (gsize)strlen(string), (gsize)(size * 2));
		g_free(string);
		return NULL;
	}

	source = string;

	result = g_malloc(size);
	target = result;

	for (i = 0; i < size; ++i, ++target) {
		hex[0] = *source++;
		hex[1] = *source++;
		*target = strtoul(hex, NULL, 16);
	}

	g_free(string);
	return result;
}

void roccat_key_file_set_binary(GKeyFile *key_file, gchar const *group_name, gchar const *key, gconstpointer data, gsize size) {
	gchar *string, *new_string, *element;
	guint8 const *source = data;
	guint i;

	string = g_malloc(1);
	*string = 0;

	for (i = 0; i < size; ++i, ++source) {
		element = g_strdup_printf("%02X", *source);
		new_string = g_strconcat(string, element, NULL);
		g_free(string);
		g_free(element);
		string = new_string;
	}

	g_key_file_set_value(key_file, group_name, key, string);
	g_free(string);
}

GKeyFile *roccat_key_file_dup(GKeyFile *src) {
	GKeyFile *dest;
	gchar *data;
	gsize length;

	dest = g_key_file_new();
	data = g_key_file_to_data(src, &length, NULL);

	if (!g_key_file_load_from_data(dest, data, length, G_KEY_FILE_NONE, NULL))
		g_clear_pointer(&dest, g_key_file_free);

	g_free(data);
	return dest;
}

gunichar2 *roccat_utf8_to_be_utf16(gchar const *str, glong len, glong *items_read, glong *items_written, GError **error) {
	gunichar2 *utf16;
	glong i;
	glong local_items_written;
	
	utf16 = g_utf8_to_utf16(str, len, items_read, &local_items_written, error);
	*items_written = local_items_written;
	for (i = 0; i < local_items_written; ++i)
		utf16[i] = GUINT16_TO_BE(utf16[i]);
	return utf16;
}

gchar *roccat_be_utf16_to_utf8(const gunichar2 *str, glong len, glong *items_read, glong *items_written, GError **error) {
	gunichar2 *utf16;
	gchar *utf8;
	glong i;
	
	utf16 = (gunichar2 *)g_malloc0(len * sizeof(gunichar2));
	for (i = 0; i < len; ++i)
		utf16[i] = GUINT16_FROM_BE(str[i]);
	utf8 = g_utf16_to_utf8(utf16, len, items_read, items_written, error);
	g_free(utf16);
	return utf8;
}

gpointer roccat_realloc(gpointer *mem, gsize *size, gsize increment) {
	gpointer new_mem = g_realloc(*mem, *size + increment);
	gpointer start = new_mem + *size;
	memset(start, 0, increment);
	*mem = new_mem;
	*size += increment;
	return start;
}
