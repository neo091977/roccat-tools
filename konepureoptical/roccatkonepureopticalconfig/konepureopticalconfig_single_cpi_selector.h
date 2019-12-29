#ifndef __ROCCAT_KONEPUREOPTICALCONFIG_SINGLE_CPI_SELECTOR_H__
#define __ROCCAT_KONEPUREOPTICALCONFIG_SINGLE_CPI_SELECTOR_H__

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

#define KONEPUREOPTICALCONFIG_SINGLE_CPI_SELECTOR_TYPE (konepureopticalconfig_single_cpi_selector_get_type())
#define KONEPUREOPTICALCONFIG_SINGLE_CPI_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), KONEPUREOPTICALCONFIG_SINGLE_CPI_SELECTOR_TYPE, KonepureopticalconfigSingleCpiSelector))
#define IS_KONEPUREOPTICALCONFIG_SINGLE_CPI_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), KONEPUREOPTICALCONFIG_SINGLE_CPI_SELECTOR_TYPE))

typedef struct _KonepureopticalconfigSingleCpiSelector KonepureopticalconfigSingleCpiSelector;
typedef struct _KonepureopticalconfigSingleCpiSelectorPrivate KonepureopticalconfigSingleCpiSelectorPrivate;

struct _KonepureopticalconfigSingleCpiSelector {
	GtkFrame frame;
	KonepureopticalconfigSingleCpiSelectorPrivate *priv;
};

GType konepureopticalconfig_single_cpi_selector_get_type(void);
GtkWidget *konepureopticalconfig_single_cpi_selector_new(gchar const *label, GSList *group);

GSList *konepureopticalconfig_single_cpi_selector_get_group(KonepureopticalconfigSingleCpiSelector *selector);
void konepureopticalconfig_single_cpi_selector_set_group(KonepureopticalconfigSingleCpiSelector *selector, GSList *group);

guint konepureopticalconfig_single_cpi_selector_get_value(KonepureopticalconfigSingleCpiSelector *selector);
void konepureopticalconfig_single_cpi_selector_set_value_blocked(KonepureopticalconfigSingleCpiSelector *selector, guint value);

gboolean konepureopticalconfig_single_cpi_selector_get_active(KonepureopticalconfigSingleCpiSelector *selector);
void konepureopticalconfig_single_cpi_selector_set_active(KonepureopticalconfigSingleCpiSelector *selector, gboolean active);

gboolean konepureopticalconfig_single_cpi_selector_get_selected(KonepureopticalconfigSingleCpiSelector *selector);
void konepureopticalconfig_single_cpi_selector_set_selected(KonepureopticalconfigSingleCpiSelector *selector, gboolean active);

G_END_DECLS

#endif
