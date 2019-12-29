#ifndef __KOVA2016_PROFILE_PAGE_H__
#define __KOVA2016_PROFILE_PAGE_H__

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
#include "kova2016_profile_data.h"

G_BEGIN_DECLS

#define KOVA2016_PROFILE_PAGE_TYPE (kova2016_profile_page_get_type())
#define KOVA2016_PROFILE_PAGE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), KOVA2016_PROFILE_PAGE_TYPE, Kova2016ProfilePage))
#define IS_KOVA2016_PROFILE_PAGE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), KOVA2016_PROFILE_PAGE_TYPE))

typedef struct _Kova2016ProfilePage Kova2016ProfilePage;

GType kova2016_profile_page_get_type(void);
GtkWidget *kova2016_profile_page_new(void);

void kova2016_profile_page_set_profile_data(Kova2016ProfilePage *profile_page, Kova2016ProfileData const *profile_data);
Kova2016ProfileData *kova2016_profile_page_get_profile_data(Kova2016ProfilePage *profile_page);

G_END_DECLS

#endif
