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

#include "roccat_orig_timer.h"
#include "g_roccat_helper.h"
#include "roccat_helper.h"
#include "i18n-lib.h"
#include <string.h>

static gchar const * const group_name = "Timer";
static gchar const * const counter_name = "Count";

static guint8 roccat_orig_timer_calc_checksum(RoccatOrigTimer const *timer) {
	return ROCCAT_BYTESUM_PARTIALLY(timer, RoccatOrigTimer, seconds, checksum);
}

static void roccat_orig_timer_set_checksum(RoccatOrigTimer *timer) {
	timer->checksum = roccat_orig_timer_calc_checksum(timer);
}

static void roccat_orig_timer_finalize(RoccatOrigTimer *timer) {
	roccat_orig_timer_set_checksum(timer);
}

static void roccat_orig_timer_set_name(RoccatOrigTimer *timer, gchar const *new_name) {
	g_strlcpy((gchar *)timer->name, new_name, ROCCAT_ORIG_TIMER_NAME_LENGTH);
}

static RoccatOrigTimer *roccat_orig_timer_new(gchar const *name, guint32 seconds) {
	RoccatOrigTimer *timer = g_malloc0(sizeof(RoccatOrigTimer));
	roccat_orig_timer_set_name(timer, name);
	roccat_orig_timer_set_seconds(timer, seconds);
	roccat_orig_timer_finalize(timer);
	return timer;
}

static gboolean roccat_orig_timer_import_iterate_keys(GKeyFile *roccat_orig_timer_file, RoccatTimers *timers, GError **error) {
	gchar **key_names, **key_name;
	RoccatOrigTimer *roccat_orig_timer;
	RoccatTimer *timer;

	key_names = g_key_file_get_keys(roccat_orig_timer_file, group_name, NULL, error);
	for (key_name = key_names; *key_name; ++key_name) {
		if (strcmp(*key_name, counter_name)) {
			roccat_orig_timer = roccat_key_file_get_binary(roccat_orig_timer_file, group_name, *key_name, sizeof(RoccatOrigTimer), error);
			if (*error)
				goto free_keys;

			timer = roccat_timer_new();
			roccat_timer_set_seconds(timer, roccat_orig_timer_get_seconds(roccat_orig_timer));
			roccat_timer_set_name(timer, (gchar const *)roccat_orig_timer->name);
			roccat_timers_set(timers, timer);
			roccat_timer_free(timer);
			g_free(roccat_orig_timer);
		}
	}
	g_strfreev(key_names);
	return TRUE;
free_keys:
	g_strfreev(key_names);
	return FALSE;
}

RoccatTimers *roccat_original_timers_import(gchar const *filename, GError **error) {
	RoccatTimers *timers;
	GKeyFile *orig_timers;

	orig_timers = g_key_file_new();
	timers = roccat_timers_new();

	if (!g_key_file_load_from_file(orig_timers, filename, G_KEY_FILE_NONE, error))
		goto free;

	roccat_orig_timer_import_iterate_keys(orig_timers, timers, error);

	g_key_file_free(orig_timers);

	return timers;
free:
	g_key_file_free(orig_timers);
	roccat_timers_free(timers);
	return NULL;
}

gboolean roccat_original_timers_export(gchar const *filename, RoccatTimers *timers, GError **error) {
	GKeyFile *orig_timers;
	RoccatOrigTimer *roccat_orig_timer;
	gchar **timer_name;
	gchar **timer_names;
	gchar *intkey;
	RoccatTimer *timer;
	guint counter = 0;
	gchar *data;
	gsize length;

	timer_names = roccat_timers_get_timer_names(timers, NULL, error);
	if (*error)
		return FALSE;

	orig_timers = g_key_file_new();

	for (timer_name = timer_names; *timer_name; ++timer_name) {
		timer = roccat_timers_get(timers, *timer_name, error);
		if (*error) {
			g_critical(_("Could not export timer: %s"), (*error)->message);
			g_clear_error(error);
			/* continue and try to save the rest */
		}

		intkey = g_strdup_printf("%i", counter);
		roccat_orig_timer = roccat_orig_timer_new((gchar const *)timer->name, roccat_timer_get_seconds(timer));
		roccat_key_file_set_binary(orig_timers, group_name, intkey, roccat_orig_timer, sizeof(RoccatOrigTimer));
		g_free(intkey);
		roccat_timer_free(timer);
		g_free(roccat_orig_timer);
		++counter;
	}
	g_strfreev(timer_names);

	g_key_file_set_integer(orig_timers, group_name, counter_name, counter);

	data = g_key_file_to_data(orig_timers, &length, error);
	g_key_file_free(orig_timers);
	if (*error)
		return FALSE;

	g_file_set_contents(filename, data, length, error);
	g_free(data);
	if (*error)
		return FALSE;

	return TRUE;
}
