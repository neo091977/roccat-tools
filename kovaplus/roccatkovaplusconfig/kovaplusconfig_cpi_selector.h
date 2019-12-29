#ifndef __ROCCAT_KOVAPLUSCONFIG_CPI_SELECTOR_H__
#define __ROCCAT_KOVAPLUSCONFIG_CPI_SELECTOR_H__

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

#include "kovaplus.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define KOVAPLUSCONFIG_CPI_SELECTOR_TYPE (kovaplusconfig_cpi_selector_get_type())
#define KOVAPLUSCONFIG_CPI_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), KOVAPLUSCONFIG_CPI_SELECTOR_TYPE, KovaplusconfigCpiSelector))
#define IS_KOVAPLUSCONFIG_CPI_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), KOVAPLUSCONFIG_CPI_SELECTOR_TYPE))

typedef struct _KovaplusconfigCpiSelector KovaplusconfigCpiSelector;

GType kovaplusconfig_cpi_selector_get_type(void);
GtkWidget *kovaplusconfig_cpi_selector_new(void);

void kovaplusconfig_cpi_selector_set_from_rmp(KovaplusconfigCpiSelector *selector, KovaplusRmp *rmp);
void kovaplusconfig_cpi_selector_update_rmp(KovaplusconfigCpiSelector *selector, KovaplusRmp *rmp);

G_END_DECLS

#endif
