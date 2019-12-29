#ifndef __ROCCAT_G_HELPER_H__
#define __ROCCAT_G_HELPER_H__

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

G_BEGIN_DECLS

#define G_ROCCAT_MSEC_PER_SEC 1000
#define G_ROCCAT_USEC_PER_MSEC 1000
#define G_ROCCAT_NSEC_PER_MSEC 1000000
#define G_ROCCAT_NSEC_PER_SEC (G_ROCCAT_MSEC_PER_SEC * G_ROCCAT_NSEC_PER_MSEC)

#define G_ROCCAT_STRUCT_CONTAINER_OF(ptr, type, member) \
	(type*)((guint8*)(ptr) - G_STRUCT_OFFSET(type,member))

static inline GSList *g_roccat_data_to_slist(gpointer data) {
	return (GSList *)( (char *)data - ((size_t) &((GSList *)0)->data) );
}

#if !(GLIB_CHECK_VERSION(2, 28, 0))

void g_slist_free_full(GSList *list, GDestroyNotify freefunc);
void g_list_free_full(GList *list, GDestroyNotify freefunc);
#define g_clear_object(object_ptr) g_clear_pointer(object_ptr, g_object_unref);

#endif

#if !(GLIB_CHECK_VERSION(2, 34, 0))

#define g_clear_pointer(pp, destroy)				\
	G_STMT_START {						\
		gpointer *_pp = (gpointer *)(pp);		\
		GDestroyNotify _d = (GDestroyNotify)(destroy);	\
		*_pp = NULL;					\
		if (*_pp)					\
			_d(*_pp);				\
	} G_STMT_END

#endif

GList *g_roccat_list_find_custom_backwards(GList *list, gconstpointer data, GCompareFunc func);

/*
 * Assuming that g_list_sort uses as stable sorting algorithm means relying
 * on undocumented behavior.
 * This stable sorting implementation is not meant for large datasets though.
 */
GList *g_roccat_list_stable_sort(GList *list, GCompareFunc compare_func);

/*!
 * \brief converts hex string from key_file to binary data
 * \retval If successful returns pointer to data, that has to be freed with g_free().
 *         On error returns +NULL+ with error set.
 */
gpointer roccat_key_file_get_binary(GKeyFile *key_file, gchar const *group_name, gchar const *key, gsize size, GError **error);

/*!
 * \brief converts binary data to hex string and puts it in key_file
 */
void roccat_key_file_set_binary(GKeyFile *key_file, gchar const *group_name, gchar const *key, gconstpointer data, gsize size);

/*!
 * \retval copy of \e src
 */
GKeyFile *roccat_key_file_dup(GKeyFile *src);

static inline void g_roccat_info(gchar const *format, ...) {
	va_list args;
	va_start(args, format);
	g_logv(G_LOG_DOMAIN, G_LOG_LEVEL_INFO, format, args);
	va_end(args);
}

gunichar2 *roccat_utf8_to_be_utf16(gchar const *str, glong len, glong *items_read, glong *items_written, GError **error);
gchar *roccat_be_utf16_to_utf8(const gunichar2 *str, glong len, glong *items_read, glong *items_written, GError **error);

/*!
 * \brief Increases memory in @mem of @size by @increment.
 * 
 * New memory region is initialized with 0.
 * @mem and @size get updated.
 * \param mem Start of existing memory region, gets updated.
 * \param size Actual size of memory region, gets updated.
 * \param increment Amount of bytes the memory region should be increased.
 * \retval pointer to start of new region.
 */
gpointer roccat_realloc(gpointer *mem, gsize *size, gsize increment);

G_END_DECLS

#endif
