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

#include "isku.h"
#include "roccat_process_helper.h"
#include <unistd.h>
#include <errno.h>

gboolean isku_play_quicklaunch_forked(IskuRkpMacroKeyInfo const *macro_key_info) {
	if (macro_key_info->type != ISKU_KEY_TYPE_QUICKLAUNCH)
		return FALSE;
	return roccat_double_fork((gchar const *)macro_key_info->filename);
}
