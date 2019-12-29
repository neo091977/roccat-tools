#ifndef __ROCCAT_DEVICE_HIDRAW_H__
#define __ROCCAT_DEVICE_HIDRAW_H__

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

#include "roccat_device.h"

G_BEGIN_DECLS

gchar *roccat_device_hidraw_read(RoccatDevice *roccat_device, guint endpoint, guint report_id, gssize length, GError **error);
gboolean roccat_device_hidraw_write(RoccatDevice *roccat_device, guint endpoint, gchar const *buffer, gssize length, GError **error);

gchar *roccat_device_hidraw_read_0(RoccatDevice *roccat_device, guint endpoint, gssize length, GError **error);
gboolean roccat_device_hidraw_write_0(RoccatDevice *roccat_device, guint endpoint, gchar const *buffer, gssize length, GError **error);

guint roccat_device_hidraw_add_event_watch(RoccatDevice *roccat_device, guint endpoint, GIOFunc callback, gpointer user_data, GError **error);

G_END_DECLS

#endif
