#ifndef __ROCCAT_TYON_CPI_SELECTOR_H__
#define __ROCCAT_TYON_CPI_SELECTOR_H__

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

#include "tyon_rmp.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define TYON_CPI_SELECTOR_TYPE (tyon_cpi_selector_get_type())
#define TYON_CPI_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), TYON_CPI_SELECTOR_TYPE, TyonCpiSelector))
#define IS_TYON_CPI_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), TYON_CPI_SELECTOR_TYPE))

typedef struct _TyonCpiSelector TyonCpiSelector;

GType tyon_cpi_selector_get_type(void);
GtkWidget *tyon_cpi_selector_new(void);

void tyon_cpi_selector_set_from_rmp(TyonCpiSelector *selector, TyonRmp *rmp);
void tyon_cpi_selector_update_rmp(TyonCpiSelector *selector, TyonRmp *rmp);

G_END_DECLS

#endif
