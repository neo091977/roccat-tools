#ifndef __ROCCAT_ARVOCONFIG_PROFILE_PAGE_H__
#define __ROCCAT_ARVOCONFIG_PROFILE_PAGE_H__

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
#include "arvo.h"

G_BEGIN_DECLS

#define ARVOCONFIG_PROFILE_PAGE_TYPE (arvoconfig_profile_page_get_type())
#define ARVOCONFIG_PROFILE_PAGE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ARVOCONFIG_PROFILE_PAGE_TYPE, ArvoconfigProfilePage))
#define IS_ARVOCONFIG_PROFILE_PAGE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ARVOCONFIG_PROFILE_PAGE_TYPE))

typedef struct _ArvoconfigProfilePage ArvoconfigProfilePage;

GType arvoconfig_profile_page_get_type(void);
GtkWidget *arvoconfig_profile_page_new(void);

void arvoconfig_profile_page_set_rkp(ArvoconfigProfilePage *profile_page, ArvoRkp *rkp);
ArvoRkp *arvoconfig_profile_page_get_rkp(ArvoconfigProfilePage *profile_page);
ArvoRkp *arvoconfig_profile_page_get_rkp_orig(ArvoconfigProfilePage *profile_page);

G_END_DECLS

#endif
