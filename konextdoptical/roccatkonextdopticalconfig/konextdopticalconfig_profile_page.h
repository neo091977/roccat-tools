#ifndef __ROCCAT_KONEXTDOPTICALCONFIG_PROFILE_PAGE_H__
#define __ROCCAT_KONEXTDOPTICALCONFIG_PROFILE_PAGE_H__

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
#include "konextdoptical.h"

G_BEGIN_DECLS

#define KONEXTDOPTICALCONFIG_PROFILE_PAGE_TYPE (konextdopticalconfig_profile_page_get_type())
#define KONEXTDOPTICALCONFIG_PROFILE_PAGE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), KONEXTDOPTICALCONFIG_PROFILE_PAGE_TYPE, KonextdopticalconfigProfilePage))
#define IS_KONEXTDOPTICALCONFIG_PROFILE_PAGE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), KONEXTDOPTICALCONFIG_PROFILE_PAGE_TYPE))

typedef struct _KonextdopticalconfigProfilePage KonextdopticalconfigProfilePage;

GType konextdopticalconfig_profile_page_get_type(void);
GtkWidget *konextdopticalconfig_profile_page_new(void);

void konextdopticalconfig_profile_page_set_rmp(KonextdopticalconfigProfilePage *profile_page, KoneplusRmp *rmp);
KoneplusRmp *konextdopticalconfig_profile_page_get_rmp(KonextdopticalconfigProfilePage *profile_page);

G_END_DECLS

#endif
