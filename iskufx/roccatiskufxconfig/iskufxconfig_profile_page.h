#ifndef __ROCCAT_ISKUFXCONFIG_PROFILE_PAGE_H__
#define __ROCCAT_ISKUFXCONFIG_PROFILE_PAGE_H__

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
#include "iskufx.h"

G_BEGIN_DECLS

#define ISKUFXCONFIG_PROFILE_PAGE_TYPE (iskufxconfig_profile_page_get_type())
#define ISKUFXCONFIG_PROFILE_PAGE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ISKUFXCONFIG_PROFILE_PAGE_TYPE, IskufxconfigProfilePage))
#define IS_ISKUFXCONFIG_PROFILE_PAGE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ISKUFXCONFIG_PROFILE_PAGE_TYPE))

typedef struct _IskufxconfigProfilePage IskufxconfigProfilePage;

GType iskufxconfig_profile_page_get_type(void);
GtkWidget *iskufxconfig_profile_page_new(void);

void iskufxconfig_profile_page_set_rkp(IskufxconfigProfilePage *profile_page, IskuRkp *rkp);
IskuRkp *iskufxconfig_profile_page_get_rkp(IskufxconfigProfilePage *profile_page);

void iskufxconfig_profile_page_update_brightness(IskufxconfigProfilePage *profile_page, guint new_brightness);
void iskufxconfig_profile_page_update_key(IskufxconfigProfilePage *profile_page, guint key_index, IskuRkpMacroKeyInfo *key_info);

G_END_DECLS

#endif
