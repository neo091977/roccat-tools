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

#include "arvo.h"
#include "roccat_process_helper.h"
#include "g_roccat_helper.h"
#include <unistd.h>
#include <errno.h>

gboolean arvo_play_quicklaunch_forked_ns(ArvoRkpButtonInfo const *button_info) {
	if (button_info->type != ARVO_RKP_BUTTON_INFO_TYPE_QUICKLAUNCH_NS)
		return FALSE;
	return roccat_double_fork((gchar const *)button_info->macroset_name);
}
