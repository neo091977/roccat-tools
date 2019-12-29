#ifndef __ROCCAT_RYOSTKLCONFIG_PROFILE_PAGE_H__
#define __ROCCAT_RYOSTKLCONFIG_PROFILE_PAGE_H__

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
#include "ryostkl_profile_data.h"
#include "ryostklconfig_blink.h"
#include "ryos_config.h"

G_BEGIN_DECLS

#define RYOSTKLCONFIG_PROFILE_PAGE_TYPE (ryostklconfig_profile_page_get_type())
#define RYOSTKLCONFIG_PROFILE_PAGE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), RYOSTKLCONFIG_PROFILE_PAGE_TYPE, RyostklconfigProfilePage))
#define IS_RYOSTKLCONFIG_PROFILE_PAGE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), RYOSTKLCONFIG_PROFILE_PAGE_TYPE))

typedef struct _RyostklconfigProfilePage RyostklconfigProfilePage;

GType ryostklconfig_profile_page_get_type(void);
GtkWidget *ryostklconfig_profile_page_new(void);

void ryostklconfig_profile_page_set_profile_data(RyostklconfigProfilePage *profile_page, RyostklProfileData const *profile_data, RoccatKeyFile *config);
RyostklProfileData *ryostklconfig_profile_page_get_profile_data(RyostklconfigProfilePage *profile_page, RoccatKeyFile *config);

void ryostklconfig_profile_page_update_brightness(RyostklconfigProfilePage *profile_page, guint new_brightness);
void ryostklconfig_profile_page_update_key(RyostklconfigProfilePage *profile_page, guint macro_index, RyosMacro *macro);
void ryostklconfig_profile_page_update_keyboard_illumination(RyostklconfigProfilePage *profile_page, RyostklconfigBlink const *blink);

void ryostklconfig_profile_page_set_device(RyostklconfigProfilePage *profile_page, RoccatDevice const *device);
void ryostklconfig_profile_page_set_keyboard_layout(RyostklconfigProfilePage *profile_page, gchar const *layout);

G_END_DECLS

#endif
