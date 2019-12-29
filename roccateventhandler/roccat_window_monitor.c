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

#include "roccat_window_monitor.h"
#include "g_roccat_helper.h"
#include "i18n.h"
#include <X11/Xlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#define ROCCAT_WINDOW_MONITOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ROCCAT_WINDOW_MONITOR_TYPE, RoccatWindowMonitorClass))
#define IS_ROCCAT_WINDOW_MONITOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ROCCAT_WINDOW_MONITOR_TYPE))
#define ROCCAT_WINDOW_MONITOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ROCCAT_WINDOW_MONITOR_TYPE, RoccatWindowMonitorPrivate))

typedef struct _RoccatWindowMonitorClass RoccatWindowMonitorClass;
typedef struct _RoccatWindowMonitorPrivate RoccatWindowMonitorPrivate;
typedef struct _PluginListData PluginListData;

struct _RoccatWindowMonitor {
	GObject parent;
	RoccatWindowMonitorPrivate *priv;
};

struct _RoccatWindowMonitorClass {
	GObjectClass parent_class;
};

struct _RoccatWindowMonitorPrivate {
	guint timeout;

	Atom net_active_window;
	Atom net_wm_name;
	Atom net_wm_pid;

	Atom wm_name;
	Atom wm_client_machine;
	Atom wm_state;

	Atom utf8_string;
	Atom string;

	Display *display;
	gchar *last_string;
};

G_DEFINE_TYPE(RoccatWindowMonitor, roccat_window_monitor, G_TYPE_OBJECT);

enum {
	ACTIVE_WINDOW_CHANGED,
	LAST_SIGNAL,
};

static guint signals[LAST_SIGNAL] = { 0 };

static int (*old_error_handler)(Display *, XErrorEvent *);

static Window get_current_window_ewmh(RoccatWindowMonitor *monitor) {
	RoccatWindowMonitorPrivate *priv = monitor->priv;
	Atom type;
	Window root;
	Window window;
	int status;
	unsigned long nitems;
	unsigned long bytes_after;
	unsigned char *prop;
	int format;

	if (priv->net_active_window == None)
		return None;

	root = XDefaultRootWindow(priv->display);
	if (root == None)
		return None;

	status = XGetWindowProperty(priv->display, root, priv->net_active_window, 0L, ~0L, False,
			AnyPropertyType, &type, &format, &nitems, &bytes_after, &prop);
	if (status == Success && type != None && prop) {
		window = *(Window *)prop;
		XFree(prop);
		return window;
	} else
		return None;
}

/*
 * QUOTE get_current_window_icccm and window_is_toplevel_icccm implementations
 * inspired by KeePassX
 * src/autotype/x11/AutoTypeX11.cpp:AutoTypePlatformX11::activeWindow()
 * https://www.keepassx.org
 * Copyright (c) 2012 Felix Geyer
 * Copyright (c) 2000-2008 Tom Sato
 */
static gboolean window_is_toplevel_icccm(RoccatWindowMonitor *monitor, Window window) {
	RoccatWindowMonitorPrivate *priv = monitor->priv;
	Atom type;
	int format;
	unsigned long n_items;
	unsigned long after;
	unsigned char *data;
	int status;

	if (window == None)
		return False;

	status = XGetWindowProperty(priv->display, window, priv->wm_state, 0, 0, False, AnyPropertyType, &type, &format, &n_items, &after, &data);
	if (data) {
		XFree(data);
	}
	return (status == Success) && type;
}

static Window get_current_window_icccm(RoccatWindowMonitor *monitor) {
	RoccatWindowMonitorPrivate *priv = monitor->priv;
	Window window;
	int revert_to_return;
	int tree;
	Window root;
	Window parent;
	Window *children;
	unsigned int n_children;

	XGetInputFocus(priv->display, &window, &revert_to_return);

	if (window == None)
		return None;

	do {
		if (window_is_toplevel_icccm(monitor, window)) {
			break;
		}
		tree = XQueryTree(priv->display, window, &root, &parent, &children, &n_children);
		if (!tree)
			return None;

		window = parent;
		if (children) {
			XFree(children);
		}
	} while (tree && window);

	return window;
}

static Window get_current_window(RoccatWindowMonitor *monitor) {
	Window window;

	window = get_current_window_ewmh(monitor);

	if (window == None)
		window = get_current_window_icccm(monitor);

	return window;
}

static gchar *window_get_name_ewmh(RoccatWindowMonitor *monitor, Window window) {
	RoccatWindowMonitorPrivate *priv = monitor->priv;
	int format;
	Atom type;
	unsigned long nitems;
	unsigned long bytes_after;
	unsigned char *uname;
	int status;
	gchar *result;

	if (priv->net_wm_name == None || priv->utf8_string == None)
		return NULL;

	if (window == None)
		return NULL;

	status = XGetWindowProperty(priv->display, window, priv->net_wm_name, 0, 2048, False,
			priv->utf8_string, &type, &format, &nitems, &bytes_after, &uname);
	if (status == Success && uname) {
		result = g_strdup((char *)uname);
		XFree(uname);
		return result;
	}

	return NULL;
}

static gchar *window_get_name_icccm(RoccatWindowMonitor *monitor, Window window) {
	RoccatWindowMonitorPrivate *priv = monitor->priv;
	int format;
	Atom type;
	unsigned long nitems;
	unsigned long bytes_after;
	unsigned char *uname;
	int status;
	gchar *result;

	if (priv->wm_name == None || priv->string == None)
		return NULL;

	if (window == None)
		return NULL;

	status = XGetWindowProperty(priv->display, window, priv->wm_name, 0, 2048, False,
			priv->string, &type, &format, &nitems, &bytes_after, &uname);
	if (status == Success && uname) {
		result = g_strdup((char *)uname);
		XFree(uname);
		return result;
	}

	return NULL;
}

static gchar *window_get_name_fallback(RoccatWindowMonitor *monitor, Window window) {
	RoccatWindowMonitorPrivate *priv = monitor->priv;
	char *name;
	gchar *result;

	if (window == None)
		return NULL;

	if (XFetchName(priv->display, window, &name) && name) {
		result = g_strdup(name);
		XFree(name);
		return result;
	}
	return NULL;
}

static gchar *window_get_name(RoccatWindowMonitor *monitor, Window window) {
	gchar *name;

	name = window_get_name_ewmh(monitor, window);

	if (name == NULL)
		name = window_get_name_icccm(monitor, window);

	if (name == NULL)
		name = window_get_name_fallback(monitor, window);

	return name;
}

static gchar *window_get_client_machine_icccm(RoccatWindowMonitor *monitor, Window window) {
	RoccatWindowMonitorPrivate *priv = monitor->priv;
	int format;
	Atom type;
	unsigned long nitems;
	unsigned long bytes_after;
	unsigned char *client_prop;
	gchar *client = NULL;
	int status;

	if (window == None)
		goto exit;

	status = XGetWindowProperty(priv->display, window, priv->wm_client_machine, 0L, ~0L, False,
			AnyPropertyType, &type, &format, &nitems, &bytes_after, &client_prop);
	if (status != Success || !client_prop)
		goto exit;

	client = g_strdup((char *)client_prop);

	XFree(client_prop);
exit:
	return client;
}

static pid_t window_get_pid_ewmh(RoccatWindowMonitor *monitor, Window window) {
	RoccatWindowMonitorPrivate *priv = monitor->priv;
	int format;
	Atom type;
	unsigned long nitems;
	unsigned long bytes_after;
	unsigned char *pid_prop;
	int status;
	pid_t pid = 0;

	if (window == None)
		goto exit;

	status = XGetWindowProperty(priv->display, window, priv->net_wm_pid, 0L, ~0L, False,
			AnyPropertyType, &type, &format, &nitems, &bytes_after, &pid_prop);
	if (status != Success || !pid_prop)
		goto exit;

	pid = *((pid_t *)pid_prop);

	XFree(pid_prop);
exit:
	return pid;
}

/*
 * This is not to be used with root rights.
 * returns 0 on error or if uid is indeed from root
 */
static uid_t pid_get_uid(pid_t pid) {
	gchar *dirname;
	struct stat status;
	gint retval;

	dirname = g_strdup_printf("/proc/%li", (gulong)pid);
	retval = stat(dirname, &status);
	g_free(dirname);

	if (retval == -1)
		return 0;

	return status.st_uid;
}

// FIXME put in g_roccat_helper
static gchar *g_roccat_file_read_link_utf8(gchar const *filename, GError **error) {
	gchar *link;
	gchar *utf8_link;

	link = g_file_read_link(filename, error);
	if (*error)
		return NULL;

	utf8_link = g_filename_to_utf8(link, -1, NULL, NULL, error);
	g_free(link);
	if (*error)
		return NULL;

	return utf8_link;
}

static gchar *pid_get_link(pid_t pid, GError **error) {
	gchar *filename;
	gchar *link;

	filename = g_strdup_printf("/proc/%li/exe", (gulong)pid);
	link = g_roccat_file_read_link_utf8(filename, error);
	g_free(filename);
	return link;
}

static gboolean pid_get_cmdline(pid_t pid, gchar **buffer, gsize *length, GError **error) {
	gchar *filename;
	gboolean retval;

	filename = g_strdup_printf("/proc/%li/cmdline", (gulong)pid);
	retval = g_file_get_contents(filename, buffer, length, error);
	g_free(filename);
	return retval;
}

/* read cmdline and expand executable with absolute path from exe-link */
static gchar *pid_get_abs_cmdline_if_user(pid_t pid, uid_t uid, GError **error) {
	gchar *cmdline;
	gchar *link;
	gsize length;
	gchar *abs;
	gchar *parameter_start = NULL;
	gsize iterator;
	GError *local_error = NULL;

	if (pid_get_uid(pid) != uid) {
		g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_ACCES, _("Mismatching user ids"));
		return NULL;
	}

	link = pid_get_link(pid, &local_error);
	if (local_error) {
		g_propagate_error(error, local_error);
		return NULL;
	}

	pid_get_cmdline(pid, &cmdline, &length, &local_error);
	if (local_error) {
		g_propagate_error(error, local_error);
		g_free(link);
		return NULL;
	}

	if (length == 0) {
		g_debug(_("(%s:%li): cmdline has 0 length"), link, (gulong)pid);
		abs = g_strdup(link);
		g_free(cmdline);
		g_free(link);
		return abs;
	}

	/* make length string length, not buffer length */
	--length;

	/* set parameter_start to first char after first 0 in cmdline */
	for (iterator = 0; iterator < length; ++iterator) {
		if (cmdline[iterator] == 0) {
			parameter_start = cmdline + iterator + 1;
			break;
		}
	}

	/* happens when no args */
	if (parameter_start == NULL)
		abs = g_strdup(link);
	else {
		/* substitute 0 delimiter with spaces except eos */
		for (iterator = 0; iterator < length; ++iterator) {
			if (cmdline[iterator] == 0)
				cmdline[iterator] = ' ';
		}

		abs = g_strdup_printf("%s %s", link, parameter_start);
	}

	g_free(cmdline);
	g_free(link);

	return abs;
}

static gboolean is_local(gchar const *client) {
	gchar *host;
	gboolean retval;

	host = g_malloc0(strlen(client) + 1);

	/* if we get a ENAMETOOLONG the strings are different anyway */
	if (gethostname(host, strlen(client) + 1) == -1)
		return FALSE;

	if (strcmp(host, client))
		retval = FALSE;
	else
		retval = TRUE;

	g_free(host);

	return retval;
}

static gchar *window_get_process_name(RoccatWindowMonitor *monitor, Window window) {
	RoccatWindowMonitorPrivate *priv = monitor->priv;
	pid_t pid;
	gchar *client;

	if (window == None)
		return NULL;

	if (priv->net_wm_pid == None || priv->wm_client_machine == None)
		return NULL;

	pid = window_get_pid_ewmh(monitor, window);
	if (pid == 0)
		return NULL;

	client = window_get_client_machine_icccm(monitor, window);
	if (!client)
		return NULL;

	if (!is_local(client)) {
		g_free(client);
		return NULL;
	}
	g_free(client);

	return pid_get_abs_cmdline_if_user(pid, getuid(), NULL);
}

/*
 * Race condition with window always possible, but especially needed to survive
 * if autostarted early
 */
static int error_handler(Display *display, XErrorEvent *event) {
	if (event->error_code == BadWindow)
		return 0;
	return old_error_handler(display, event);
}

static gboolean timeout_cb(gpointer user_data) {
	RoccatWindowMonitor *monitor = ROCCAT_WINDOW_MONITOR(user_data);
	RoccatWindowMonitorPrivate *priv = monitor->priv;
	Window window;
	gchar *process_name;
	gchar *window_name;
	gchar *both_names;

	old_error_handler = XSetErrorHandler(error_handler);

	window = get_current_window(monitor);
	if (window == None) {
		if (priv->last_string != NULL) { // Change to desktop
			g_signal_emit((gpointer)monitor, signals[ACTIVE_WINDOW_CHANGED], 0, "");
			g_free(priv->last_string);
			priv->last_string = NULL;
		}
		return TRUE;
	}

	process_name = window_get_process_name(monitor, window);
	window_name = window_get_name(monitor, window);

	if (process_name) {
		if (window_name)
			both_names = g_strdup_printf("%s %s", window_name, process_name);
		else
			both_names = g_strdup_printf("%s", process_name);
	} else {
		if (window_name)
			both_names = g_strdup_printf("%s", window_name);
		else
			both_names = NULL;
	}

	if (!both_names)
		return TRUE;

	if (g_strcmp0(priv->last_string, both_names)) {
		g_roccat_info(_("Window monitor emits '%s'"), both_names);
		g_signal_emit((gpointer)monitor, signals[ACTIVE_WINDOW_CHANGED], 0, both_names);

		if (priv->last_string)
			g_free(priv->last_string);
		priv->last_string = both_names;
	}

	(void)XSetErrorHandler(old_error_handler);

	return TRUE;
}

void roccat_window_monitor_set_interval(RoccatWindowMonitor *monitor, gint interval) {
	RoccatWindowMonitorPrivate *priv = monitor->priv;

	if (interval <= 0) {
		g_message(_("Did not start window monitor: Scan interval not > 0"));
		return;
	}

	if (priv->display == NULL) {
		g_warning(_("Could not start window monitor: No display"));
		return;
	}

	if (priv->timeout != 0)
		g_source_remove(priv->timeout);
	priv->timeout = g_timeout_add_seconds(interval, timeout_cb, monitor);
}

RoccatWindowMonitor *roccat_window_monitor_new(void) {
	return ROCCAT_WINDOW_MONITOR(g_object_new(ROCCAT_WINDOW_MONITOR_TYPE, NULL));
}

static void roccat_window_monitor_init(RoccatWindowMonitor *monitor) {
	RoccatWindowMonitorPrivate *priv = ROCCAT_WINDOW_MONITOR_GET_PRIVATE(monitor);
	monitor->priv = priv;
	gchar const * const message = _("Atom %s is not available on this display");
	priv->display = XOpenDisplay(NULL);
	priv->last_string = NULL;

	priv->net_active_window = XInternAtom(priv->display, "_NET_ACTIVE_WINDOW", True);
	if (priv->net_active_window == None)
		g_message(message, "_NET_ACTIVE_WINDOW");

	priv->net_wm_name = XInternAtom(priv->display, "_NET_WM_NAME", True);
	if (priv->net_wm_name == None)
		g_message(message, "_NET_WM_NAME");

	priv->net_wm_pid = XInternAtom(priv->display, "_NET_WM_PID", True);
	if (priv->net_wm_pid == None)
		g_message(message, "_NET_WM_PID");

	priv->wm_name = XInternAtom(priv->display, "WM_NAME", True);
	if (priv->wm_name == None)
		g_message(message, "WM_NAME");

	priv->wm_client_machine = XInternAtom(priv->display, "WM_CLIENT_MACHINE", True);
	if (priv->wm_client_machine == None)
		g_message(message, "WM_CLIENT_MACHINE");

	priv->wm_state = XInternAtom(priv->display, "WM_STATE", True);
	if (priv->wm_state == None)
		g_message(message, "WM_STATE");

	priv->utf8_string = XInternAtom(priv->display, "UTF8_STRING", True);
	if (priv->utf8_string == None)
		g_message(message, "UTF8_STRING");

	priv->string = XInternAtom(priv->display, "STRING", True);
	if (priv->string == None)
		g_message(message, "STRING");
}

static void finalize(GObject *object) {
	RoccatWindowMonitorPrivate *priv = ROCCAT_WINDOW_MONITOR(object)->priv;

	g_source_remove(priv->timeout);

	G_OBJECT_CLASS(roccat_window_monitor_parent_class)->finalize(object);
}

static void roccat_window_monitor_class_init(RoccatWindowMonitorClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(RoccatWindowMonitorPrivate));

	signals[ACTIVE_WINDOW_CHANGED] = g_signal_new("active-window-changed", ROCCAT_WINDOW_MONITOR_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__STRING, G_TYPE_NONE,
			1, G_TYPE_STRING);
}
