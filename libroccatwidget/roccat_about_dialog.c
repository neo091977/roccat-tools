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

#include "roccat_about_dialog.h"
#include "config.h"

void roccat_about_dialog(GtkWindow *parent, gchar const *program_name, gchar const *comments) {
	gchar const *authors[] = {
			"Stefan Achatz <erazor_de@users.sourceforge.net>",
			NULL
	};
	gtk_show_about_dialog(parent,
			"authors", authors,
			"comments", comments,
			"copyright", "Copyright \302\251 2009-2019 Stefan Achatz",
			"license", "GPL 2 (GNU General Public License)\n\nsee http://www.gnu.org/licenses/old-licenses/gpl-2.0.html",
			"logo-icon-name", "roccat",
			"modal", TRUE,
			"program-name", program_name,
			"version", VERSION_STRING,
			"website", "http://roccat.sourceforge.net",
			"website-label", "roccat.sourceforge.net",
			NULL);
}
