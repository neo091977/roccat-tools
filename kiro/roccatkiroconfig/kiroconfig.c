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

#include "kiroconfig_window.h"
#include "roccat_helper.h"
#include "roccat_secure.h"
#include "roccat_warning_dialog.h"
#include "config.h"
#include "i18n.h"
#include <gaminggear/gaminggear.h>

int main(int argc, char **argv) {
	roccat_secure();
	roccat_textdomain();

#if !(GLIB_CHECK_VERSION(2, 36, 0))
	g_type_init();
#endif
	gtk_init(&argc, &argv);
	gaminggearwidget_init();

	g_debug(_("Version: %s"), VERSION_STRING);

	(void)kiroconfig_window_new();

	gtk_main();

	return 0;
}
