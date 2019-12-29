#ifndef __ROCCAT_KONEPUREOPTICALCONFIG_CPI_SELECTOR_H__
#define __ROCCAT_KONEPUREOPTICALCONFIG_CPI_SELECTOR_H__

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

#include "konepureoptical.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define KONEPUREOPTICALCONFIG_CPI_SELECTOR_TYPE (konepureopticalconfig_cpi_selector_get_type())
#define KONEPUREOPTICALCONFIG_CPI_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), KONEPUREOPTICALCONFIG_CPI_SELECTOR_TYPE, KonepureopticalconfigCpiSelector))
#define IS_KONEPUREOPTICALCONFIG_CPI_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), KONEPUREOPTICALCONFIG_CPI_SELECTOR_TYPE))

typedef struct _KonepureopticalconfigCpiSelectorClass KonepureopticalconfigCpiSelectorClass;
typedef struct _KonepureopticalconfigCpiSelector KonepureopticalconfigCpiSelector;
typedef struct _KonepureopticalconfigCpiSelectorPrivate KonepureopticalconfigCpiSelectorPrivate;

struct _KonepureopticalconfigCpiSelectorClass {
	GtkFrameClass parent_class;
};

struct _KonepureopticalconfigCpiSelector {
	GtkFrame frame;
	KonepureopticalconfigCpiSelectorPrivate *priv;
};

GType konepureopticalconfig_cpi_selector_get_type(void);
GtkWidget *konepureopticalconfig_cpi_selector_new(void);

void konepureopticalconfig_cpi_selector_set_from_rmp(KonepureopticalconfigCpiSelector *selector, KoneplusRmp *rmp);
void konepureopticalconfig_cpi_selector_update_rmp(KonepureopticalconfigCpiSelector *selector, KoneplusRmp *rmp);

G_END_DECLS

#endif
