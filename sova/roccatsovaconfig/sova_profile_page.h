#ifndef __SOVA_PROFILE_PAGE_H__
#define __SOVA_PROFILE_PAGE_H__

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

#include "sova_profile_data.h"
#include "sova_config.h"
#include "roccat_profile_page.h"

G_BEGIN_DECLS

#define SOVA_PROFILE_PAGE_TYPE (sova_profile_page_get_type())
#define SOVA_PROFILE_PAGE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), SOVA_PROFILE_PAGE_TYPE, SovaProfilePage))
#define IS_SOVA_PROFILE_PAGE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), SOVA_PROFILE_PAGE_TYPE))

typedef struct _SovaProfilePage SovaProfilePage;

GType sova_profile_page_get_type(void);
GtkWidget *sova_profile_page_new(void);

/*
 * ProfilePage does not change eventhandler specific settings in provided config
 */
void sova_profile_page_set_profile_data(SovaProfilePage *profile_page, SovaProfileData *profile_data, RoccatKeyFile *config);
SovaProfileData *sova_profile_page_get_profile_data(SovaProfilePage *profile_page, RoccatKeyFile *config);

void sova_profile_page_update_brightness(SovaProfilePage *profile_page, guint new_brightness);
void sova_profile_page_update_key(SovaProfilePage *profile_page, guint macro_index, SovaMacro *macro);

void sova_profile_page_set_keyboard_layout(SovaProfilePage *profile_page, gchar const *layout);

G_END_DECLS

#endif
