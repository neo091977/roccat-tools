#ifndef __ROCCAT_GFX_H__
#define __ROCCAT_GFX_H__

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

#include "dbus_roccat_helper.h"
#include <gaminggear/gaminggear_fx_plugin.h>

G_BEGIN_DECLS

GfxResult roccat_gfx_update(DBusConnection *bus, gchar const * const server_path, gchar const * const server_interface);
GfxResult roccat_gfx_set_light_color(DBusConnection *bus, gchar const * const server_path, gchar const * const server_interface,
		unsigned int const light_index, uint32_t const color);
GfxResult roccat_gfx_get_light_color(DBusConnection *bus, gchar const * const server_path, gchar const * const server_interface,
		unsigned int const light_index, uint32_t * const color);

G_END_DECLS

#endif
