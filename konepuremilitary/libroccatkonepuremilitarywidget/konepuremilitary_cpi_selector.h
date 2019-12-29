#ifndef __ROCCAT_KONEPUREMILITARY_CPI_SELECTOR_H__
#define __ROCCAT_KONEPUREMILITARY_CPI_SELECTOR_H__

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

#include "konepuremilitary.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define KONEPUREMILITARY_CPI_SELECTOR_TYPE (konepuremilitary_cpi_selector_get_type())
#define KONEPUREMILITARY_CPI_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), KONEPUREMILITARY_CPI_SELECTOR_TYPE, KonepuremilitaryCpiSelector))
#define IS_KONEPUREMILITARY_CPI_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), KONEPUREMILITARY_CPI_SELECTOR_TYPE))

typedef struct _KonepuremilitaryCpiSelectorClass KonepuremilitaryCpiSelectorClass;
typedef struct _KonepuremilitaryCpiSelector KonepuremilitaryCpiSelector;
typedef struct _KonepuremilitaryCpiSelectorPrivate KonepuremilitaryCpiSelectorPrivate;

struct _KonepuremilitaryCpiSelectorClass {
	GtkFrameClass parent_class;
};

struct _KonepuremilitaryCpiSelector {
	GtkFrame frame;
	KonepuremilitaryCpiSelectorPrivate *priv;
};

GType konepuremilitary_cpi_selector_get_type(void);
GtkWidget *konepuremilitary_cpi_selector_new(gdouble min, gdouble max, gdouble step);

void konepuremilitary_cpi_selector_set_from_rmp(KonepuremilitaryCpiSelector *selector, KoneplusRmp *rmp);
void konepuremilitary_cpi_selector_update_rmp(KonepuremilitaryCpiSelector *selector, KoneplusRmp *rmp);

G_END_DECLS

#endif
