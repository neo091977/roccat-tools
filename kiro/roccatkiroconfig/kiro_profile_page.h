#ifndef __ROCCAT_KIRO_PROFILE_PAGE_H__
#define __ROCCAT_KIRO_PROFILE_PAGE_H__

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
#include "kiro_profile_data.h"

G_BEGIN_DECLS

#define KIRO_PROFILE_PAGE_TYPE (kiro_profile_page_get_type())
#define KIRO_PROFILE_PAGE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), KIRO_PROFILE_PAGE_TYPE, KiroProfilePage))
#define IS_KIRO_PROFILE_PAGE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), KIRO_PROFILE_PAGE_TYPE))

typedef struct _KiroProfilePage KiroProfilePage;

GType kiro_profile_page_get_type(void);
GtkWidget *kiro_profile_page_new(void);

void kiro_profile_page_set_profile_data(KiroProfilePage *profile_page, KiroProfileData const *profile_data);
KiroProfileData *kiro_profile_page_get_profile_data(KiroProfilePage *profile_page);

G_END_DECLS

#endif
