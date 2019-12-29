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

#include "savu.h"
#include "roccat_process_helper.h"
#include <unistd.h>
#include <errno.h>

gboolean savu_play_quicklaunch_forked(SavuRmp *rmp, guint index) {
	RoccatButton *button;
	guint type;
	gchar *launchpath;
	gboolean retval;

	button = savu_rmp_get_button(rmp, index);
	type = button->type;
	g_free(button);
	if (type != SAVU_BUTTON_TYPE_QUICKLAUNCH)
		return FALSE;

	launchpath = savu_rmp_get_launchpath(rmp, index);
	retval = roccat_double_fork(launchpath);
	g_free(launchpath);
	return retval;
}
