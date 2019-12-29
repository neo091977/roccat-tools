#ifndef __ROCCAT_DEVICE_SYSFS_H__
#define __ROCCAT_DEVICE_SYSFS_H__

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

gchar *roccat_device_sysfs_read(RoccatDevice *roccat_device, gchar const *attribute, gsize length, GError **error);
gboolean roccat_device_sysfs_write(RoccatDevice *roccat_device, gchar const *attribute, gchar const *data, gsize length, GError **error);

G_END_DECLS

#endif
