#ifndef __ROCCAT_LUACONFIG_PROFILE_PAGE_H__
#define __ROCCAT_LUACONFIG_PROFILE_PAGE_H__

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

#include "roccat_profile_page.h"
#include "lua.h"

G_BEGIN_DECLS

#define LUACONFIG_PROFILE_PAGE_TYPE (luaconfig_profile_page_get_type())
#define LUACONFIG_PROFILE_PAGE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), LUACONFIG_PROFILE_PAGE_TYPE, LuaconfigProfilePage))
#define IS_LUACONFIG_PROFILE_PAGE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), LUACONFIG_PROFILE_PAGE_TYPE))

typedef struct _LuaconfigProfilePage LuaconfigProfilePage;

GType luaconfig_profile_page_get_type(void);
GtkWidget *luaconfig_profile_page_new(void);
void luaconfig_profile_page_set_rmp(LuaconfigProfilePage *profile_page, LuaRmp *rmp);
LuaRmp *luaconfig_profile_page_get_rmp(LuaconfigProfilePage *profile_page);

G_END_DECLS

#endif
