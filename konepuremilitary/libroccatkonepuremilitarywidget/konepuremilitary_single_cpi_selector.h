#ifndef __ROCCAT_KONEPUREMILITARY_SINGLE_CPI_SELECTOR_H__
#define __ROCCAT_KONEPUREMILITARY_SINGLE_CPI_SELECTOR_H__

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

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define KONEPUREMILITARY_SINGLE_CPI_SELECTOR_TYPE (konepuremilitary_single_cpi_selector_get_type())
#define KONEPUREMILITARY_SINGLE_CPI_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), KONEPUREMILITARY_SINGLE_CPI_SELECTOR_TYPE, KonepuremilitarySingleCpiSelector))
#define IS_KONEPUREMILITARY_SINGLE_CPI_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), KONEPUREMILITARY_SINGLE_CPI_SELECTOR_TYPE))

typedef struct _KonepuremilitarySingleCpiSelector KonepuremilitarySingleCpiSelector;
typedef struct _KonepuremilitarySingleCpiSelectorPrivate KonepuremilitarySingleCpiSelectorPrivate;

struct _KonepuremilitarySingleCpiSelector {
	GtkHBox parent;
	KonepuremilitarySingleCpiSelectorPrivate *priv;
};

GType konepuremilitary_single_cpi_selector_get_type(void);
GtkWidget *konepuremilitary_single_cpi_selector_new(GSList *group, gdouble min, gdouble max, gdouble step);

GSList *konepuremilitary_single_cpi_selector_get_group(KonepuremilitarySingleCpiSelector *selector);
void konepuremilitary_single_cpi_selector_set_group(KonepuremilitarySingleCpiSelector *selector, GSList *group);

guint konepuremilitary_single_cpi_selector_get_x_value(KonepuremilitarySingleCpiSelector *selector);
void konepuremilitary_single_cpi_selector_set_x_value(KonepuremilitarySingleCpiSelector *selector, guint value);

guint konepuremilitary_single_cpi_selector_get_y_value(KonepuremilitarySingleCpiSelector *selector);
void konepuremilitary_single_cpi_selector_set_y_value(KonepuremilitarySingleCpiSelector *selector, guint value);

gboolean konepuremilitary_single_cpi_selector_get_linked(KonepuremilitarySingleCpiSelector *selector);
void konepuremilitary_single_cpi_selector_set_linked(KonepuremilitarySingleCpiSelector *selector, gboolean value);

void konepuremilitary_single_cpi_selector_set_range(KonepuremilitarySingleCpiSelector *selector, gdouble min, gdouble max);
void konepuremilitary_single_cpi_selector_set_increment(KonepuremilitarySingleCpiSelector *selector, gdouble step);

gboolean konepuremilitary_single_cpi_selector_get_active(KonepuremilitarySingleCpiSelector *selector);
void konepuremilitary_single_cpi_selector_set_active(KonepuremilitarySingleCpiSelector *selector, gboolean active);

gboolean konepuremilitary_single_cpi_selector_get_selected(KonepuremilitarySingleCpiSelector *selector);
void konepuremilitary_single_cpi_selector_set_selected(KonepuremilitarySingleCpiSelector *selector, gboolean active);

G_END_DECLS

#endif
