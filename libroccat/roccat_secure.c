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

#include "roccat_secure.h"
#include <sys/param.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <grp.h>
#include <paths.h>

static void drop_privileges_permanent(void) {
	gid_t newgid = getgid();
	gid_t oldgid = getegid();
	uid_t newuid = getuid();
	uid_t olduid = geteuid();

	if (!olduid) setgroups(1, &newgid);

	if (newgid != oldgid) {
		if (setregid(newgid, newgid) == -1)
			exit(EXIT_FAILURE);
	}

	if (newuid != olduid) {
		if (setreuid(newuid, newuid) == -1)
			exit(EXIT_FAILURE);
	}

	if (newgid != oldgid && (setegid(oldgid) != -1 || getegid() != newgid))
		exit(EXIT_FAILURE);
	if (newuid != olduid && (seteuid(olduid) != -1 || geteuid() != newuid))
		exit(EXIT_FAILURE);
}

static void disable_core_dumps(void) {
#ifdef NDEBUG
	struct rlimit r = { 0, 0};
	if (setrlimit(RLIMIT_CORE, &r) != 0)
		exit(EXIT_FAILURE);
#endif
}

static void close_descriptors(void) {
	int nd;
	if ((nd = sysconf(_SC_OPEN_MAX)) < 0)
		exit(EXIT_FAILURE);
	while (--nd > 2)
		close(nd);
}

static void open_stdfiles(void) {
	struct stat buf;
	FILE *f[3];
	char *m[3] = { "rb", "wb", "wb" };
	int i;

	f[0] = stdin;
	f[1] = stdout;
	f[2] = stderr;
	for (i = 0; i < 3; ++i) {
		if (fstat(i, &buf) == 0)
			continue;
		if (errno != EBADF)
			exit(EXIT_FAILURE);
		if (freopen(_PATH_DEVNULL, m[i], f[i]) != f[i])
			exit(EXIT_FAILURE);
	}
}

void roccat_secure(void) {
	disable_core_dumps();
	drop_privileges_permanent();
	close_descriptors();
	open_stdfiles();
}
