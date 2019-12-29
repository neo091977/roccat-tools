#ifndef __ROCCAT_SINGLE_CPI_SELECTOR_H__
#define __ROCCAT_SINGLE_CPI_SELECTOR_H__

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

#define ROCCAT_SINGLE_CPI_SELECTOR_TYPE (roccat_single_cpi_selector_get_type())
#define ROCCAT_SINGLE_CPI_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ROCCAT_SINGLE_CPI_SELECTOR_TYPE, RoccatSingleCpiSelector))
#define IS_ROCCAT_SINGLE_CPI_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ROCCAT_SINGLE_CPI_SELECTOR_TYPE))

typedef struct _RoccatSingleCpiSelector RoccatSingleCpiSelector;
typedef struct _RoccatSingleCpiSelectorPrivate RoccatSingleCpiSelectorPrivate;

struct _RoccatSingleCpiSelector {
	GtkFrame frame;
	RoccatSingleCpiSelectorPrivate *priv;
};

GType roccat_single_cpi_selector_get_type(void);
GtkWidget *roccat_single_cpi_selector_new(gchar const *label, GSList *group, gdouble min, gdouble max, gdouble step);

GSList *roccat_single_cpi_selector_get_group(RoccatSingleCpiSelector *selector);
void roccat_single_cpi_selector_set_group(RoccatSingleCpiSelector *selector, GSList *group);

gdouble roccat_single_cpi_selector_get_value(RoccatSingleCpiSelector *selector);
void roccat_single_cpi_selector_set_value(RoccatSingleCpiSelector *selector, gdouble value);

void roccat_single_cpi_selector_set_range(RoccatSingleCpiSelector *selector, gdouble min, gdouble max);
void roccat_single_cpi_selector_set_increment(RoccatSingleCpiSelector *selector, gdouble step);

gboolean roccat_single_cpi_selector_get_active(RoccatSingleCpiSelector *selector);
void roccat_single_cpi_selector_set_active(RoccatSingleCpiSelector *selector, gboolean active);

gboolean roccat_single_cpi_selector_get_selected(RoccatSingleCpiSelector *selector);
void roccat_single_cpi_selector_set_selected(RoccatSingleCpiSelector *selector, gboolean active);

G_END_DECLS

#endif
