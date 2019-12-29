#ifndef __ROCCAT_ISKUCONFIG_PROFILE_PAGE_H__
#define __ROCCAT_ISKUCONFIG_PROFILE_PAGE_H__

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
#include "isku.h"

G_BEGIN_DECLS

#define ISKUCONFIG_PROFILE_PAGE_TYPE (iskuconfig_profile_page_get_type())
#define ISKUCONFIG_PROFILE_PAGE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ISKUCONFIG_PROFILE_PAGE_TYPE, IskuconfigProfilePage))
#define IS_ISKUCONFIG_PROFILE_PAGE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ISKUCONFIG_PROFILE_PAGE_TYPE))

typedef struct _IskuconfigProfilePage IskuconfigProfilePage;

GType iskuconfig_profile_page_get_type(void);
GtkWidget *iskuconfig_profile_page_new(void);

void iskuconfig_profile_page_set_rkp(IskuconfigProfilePage *profile_page, IskuRkp *rkp);
IskuRkp *iskuconfig_profile_page_get_rkp(IskuconfigProfilePage *profile_page);

void iskuconfig_profile_page_update_brightness(IskuconfigProfilePage *profile_page, guint new_brightness);
void iskuconfig_profile_page_update_key(IskuconfigProfilePage *profile_page, guint key_index, IskuRkpMacroKeyInfo *key_info);

G_END_DECLS

#endif
