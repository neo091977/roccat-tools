#ifndef __ROCCAT_RYOSCONFIG_PROFILE_PAGE_H__
#define __ROCCAT_RYOSCONFIG_PROFILE_PAGE_H__

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
#include "ryos_profile_data.h"
#include "ryos_config.h"

G_BEGIN_DECLS

#define RYOSCONFIG_PROFILE_PAGE_TYPE (ryosconfig_profile_page_get_type())
#define RYOSCONFIG_PROFILE_PAGE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), RYOSCONFIG_PROFILE_PAGE_TYPE, RyosconfigProfilePage))
#define IS_RYOSCONFIG_PROFILE_PAGE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), RYOSCONFIG_PROFILE_PAGE_TYPE))

typedef struct _RyosconfigProfilePage RyosconfigProfilePage;

GType ryosconfig_profile_page_get_type(void);
GtkWidget *ryosconfig_profile_page_new(void);

void ryosconfig_profile_page_set_profile_data(RyosconfigProfilePage *profile_page, RyosProfileData const *profile_data, RoccatKeyFile *config);
RyosProfileData *ryosconfig_profile_page_get_profile_data(RyosconfigProfilePage *profile_page, RoccatKeyFile *config);

void ryosconfig_profile_page_update_brightness(RyosconfigProfilePage *profile_page, guint new_brightness);
void ryosconfig_profile_page_update_key(RyosconfigProfilePage *profile_page, guint macro_index, RyosMacro *macro);
void ryosconfig_profile_page_update_keyboard_illumination(RyosconfigProfilePage *profile_page, gboolean blink_state);

void ryosconfig_profile_page_set_device(RyosconfigProfilePage *profile_page, RoccatDevice const *device);
void ryosconfig_profile_page_set_keyboard_layout(RyosconfigProfilePage *profile_page, gchar const *layout);

G_END_DECLS

#endif
