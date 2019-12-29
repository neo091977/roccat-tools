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

#include "roccat_process_helper.h"
#include "roccat_secure.h"
#include "roccat_helper.h"
#include "i18n-lib.h"
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>

/*
 * QUOTE function for double forking copied from Unix Programming FAQ
 * Examples/Daemon utility functions:fork2()
 * http://www.mercymachines.net/unix/faq_toc.html
 * Copyright (c) 1997-2000 Andrew Gierth
 */
static int fork2(void) {
	pid_t pid;
	int status;

	if (!(pid = fork())) {

		roccat_secure();

		switch (fork()) {
		case 0:
			return 0;
			break;
		case -1:
			_exit(errno); /* assumes all errnos are <256 */
			break;
		default:
			_exit(0);
			break;
		}
	}

	if (pid < 0 || waitpid(pid, &status, 0) < 0)
		return -1;

	if (WIFEXITED(status))
		if (WEXITSTATUS(status) == 0)
			return 1;
		else
			errno = WEXITSTATUS(status);
	else
		errno = EINTR; /* well, sort of :-) */

	return -1;
}

gboolean roccat_double_fork_with_arg(gchar const *command, gchar const *arg) {
	g_debug(_("Forking %s"), command);

	switch (fork2()) {
	case 0:
		execlp(command, command, arg, (char const *)NULL);
		/* if code is executed past this point, something went wrong */
		g_critical(_("Could not execute %s %s: %s"), command, arg, g_strerror(errno));
		break;
	case -1:
		g_critical(_("Could not fork %s"), command);
		return FALSE;
	}

	return TRUE;
}

gboolean roccat_double_fork(gchar const *command) {
	return roccat_double_fork_with_arg(command, (gchar const *)NULL);
}

gboolean roccat_open_url(gchar const *url) {
	if (!roccat_double_fork_with_arg("xdg-open", url)) {
		g_warning(_("Could not open url %s"), url);
		return FALSE;
	} else
		return TRUE;
}

gboolean roccat_open_default_folder(gchar const *type) {
	GError *local_error = NULL;
	gchar *path;
	gboolean retval;

	path = roccat_xdg_get_default_folder(type, &local_error);
	if (path == NULL) {
		g_warning(_("Could not get default folder %s: %s"), type, local_error->message);
		g_clear_error(&local_error);
		return FALSE;
	}

	retval = roccat_open_url(path);
	g_free(path);
	return retval;
}
