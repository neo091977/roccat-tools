#ifndef __SUORA_PROFILE_PAGE_H__
#define __SUORA_PROFILE_PAGE_H__

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

#include "suora_profile_data.h"
#include "suora_config.h"
#include <gaminggear/gaminggear_profile_page.h>

G_BEGIN_DECLS

#define SUORA_PROFILE_PAGE_TYPE (suora_profile_page_get_type())
#define SUORA_PROFILE_PAGE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), SUORA_PROFILE_PAGE_TYPE, SuoraProfilePage))
#define IS_SUORA_PROFILE_PAGE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), SUORA_PROFILE_PAGE_TYPE))

typedef struct _SuoraProfilePage SuoraProfilePage;

GType suora_profile_page_get_type(void);
GtkWidget *suora_profile_page_new(void);

void suora_profile_page_set_keyboard_layout(SuoraProfilePage *profile_page, gchar const *layout);

G_END_DECLS

#endif
