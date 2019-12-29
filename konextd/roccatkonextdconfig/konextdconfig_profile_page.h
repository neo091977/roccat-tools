#ifndef __ROCCAT_KONEXTDCONFIG_PROFILE_PAGE_H__
#define __ROCCAT_KONEXTDCONFIG_PROFILE_PAGE_H__

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
#include "konextd.h"

G_BEGIN_DECLS

#define KONEXTDCONFIG_PROFILE_PAGE_TYPE (konextdconfig_profile_page_get_type())
#define KONEXTDCONFIG_PROFILE_PAGE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), KONEXTDCONFIG_PROFILE_PAGE_TYPE, KonextdconfigProfilePage))
#define IS_KONEXTDCONFIG_PROFILE_PAGE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), KONEXTDCONFIG_PROFILE_PAGE_TYPE))

typedef struct _KonextdconfigProfilePage KonextdconfigProfilePage;

GType konextdconfig_profile_page_get_type(void);
GtkWidget *konextdconfig_profile_page_new(void);

void konextdconfig_profile_page_set_rmp(KonextdconfigProfilePage *profile_page, KoneplusRmp *rmp);
KoneplusRmp *konextdconfig_profile_page_get_rmp(KonextdconfigProfilePage *profile_page);

G_END_DECLS

#endif
