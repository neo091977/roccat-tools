#ifndef __ROCCAT_WARNING_DIALOG_H__
#define __ROCCAT_WARNING_DIALOG_H__

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

#include <gaminggear/macro.h>
#include "roccat_device.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

void roccat_warning_dialog(GtkWindow *parent, gchar const *first_text, gchar const *second_text);

/* returns TRUE if no error */
gboolean roccat_handle_error_dialog(GtkWindow *parent, gchar const *text, GError **error);

void roccat_multiple_device_warning_dialog(GtkWindow *parent, gchar const *device_name);
gboolean roccat_no_device_warning_dialog(GtkWindow *parent, RoccatDevice const *device, gchar const *device_name);

/* returns TRUE if firmware is not too old */
gboolean roccat_too_old_firmware_version_dialog(GtkWindow *parent, guint has, guint needs);

/* returns TRUE if macro has loop value of 1 */
gboolean gaminggear_macro_loop_not_supported_dialog(GtkWindow *parent, GaminggearMacro *macro);

G_END_DECLS

#endif
