#ifndef __ROCCAT_PYRACONFIG_PROFILE_PAGE_H__
#define __ROCCAT_PYRACONFIG_PROFILE_PAGE_H__

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
#include "pyra.h"

G_BEGIN_DECLS

#define PYRACONFIG_PROFILE_PAGE_TYPE (pyraconfig_profile_page_get_type())
#define PYRACONFIG_PROFILE_PAGE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), PYRACONFIG_PROFILE_PAGE_TYPE, PyraconfigProfilePage))
#define IS_PYRACONFIG_PROFILE_PAGE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), PYRACONFIG_PROFILE_PAGE_TYPE))

typedef struct _PyraconfigProfilePage PyraconfigProfilePage;

GType pyraconfig_profile_page_get_type(void);
GtkWidget *pyraconfig_profile_page_new(void);

void pyraconfig_profile_page_set_rmp(PyraconfigProfilePage *profile_page, PyraRMP *rmp);
PyraRMP *pyraconfig_profile_page_get_rmp(PyraconfigProfilePage *profile_page);

G_END_DECLS

#endif
