#ifndef __ROCCAT_LUACONFIG_SURFACE_CHECK_H__
#define __ROCCAT_LUACONFIG_SURFACE_CHECK_H__

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

#include "lua.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define LUACONFIG_SURFACE_CHECK_TYPE (luaconfig_surface_check_get_type())
#define LUACONFIG_SURFACE_CHECK(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), LUACONFIG_SURFACE_CHECK_TYPE, LuaconfigSurfaceCheck))
#define IS_LUACONFIG_SURFACE_CHECK(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), LUACONFIG_SURFACE_CHECK_TYPE))

typedef struct _LuaconfigSurfaceCheck LuaconfigSurfaceCheck;

GType luaconfig_surface_check_get_type(void);
GtkWidget *luaconfig_surface_check_new(GtkWindow *parent);

void luaconfig_surface_check_set_device(LuaconfigSurfaceCheck *surface_check, RoccatDevice *device);

void luaconfig_surface_check_set_squal(LuaconfigSurfaceCheck *surface_check, guint value);
void luaconfig_surface_check_set_average(LuaconfigSurfaceCheck *surface_check, guint value);

void luaconfig_surface_check(GtkWindow *parent, RoccatDevice *lua);

G_END_DECLS

#endif
