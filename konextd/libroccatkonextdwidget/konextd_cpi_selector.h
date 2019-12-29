#ifndef __ROCCAT_KONEXTD_CPI_SELECTOR_H__
#define __ROCCAT_KONEXTD_CPI_SELECTOR_H__

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

#include "koneplus.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define KONEXTD_CPI_SELECTOR_TYPE (konextd_cpi_selector_get_type())
#define KONEXTD_CPI_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), KONEXTD_CPI_SELECTOR_TYPE, KonextdCpiSelector))
#define IS_KONEXTD_CPI_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), KONEXTD_CPI_SELECTOR_TYPE))

typedef struct _KonextdCpiSelector KonextdCpiSelector;

GType konextd_cpi_selector_get_type(void);
GtkWidget *konextd_cpi_selector_new(void);

void konextd_cpi_selector_set_from_rmp(KonextdCpiSelector *selector, KoneplusRmp *rmp);
void konextd_cpi_selector_update_rmp(KonextdCpiSelector *selector, KoneplusRmp *rmp);

G_END_DECLS

#endif
