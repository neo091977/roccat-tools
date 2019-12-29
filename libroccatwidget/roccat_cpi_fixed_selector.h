#ifndef __ROCCAT_CPI_FIXED_SELECTOR_H__
#define __ROCCAT_CPI_FIXED_SELECTOR_H__

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

#define ROCCAT_CPI_FIXED_SELECTOR_TYPE (roccat_cpi_fixed_selector_get_type())
#define ROCCAT_CPI_FIXED_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ROCCAT_CPI_FIXED_SELECTOR_TYPE, RoccatCpiFixedSelector))
#define IS_ROCCAT_CPI_FIXED_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ROCCAT_CPI_FIXED_SELECTOR_TYPE))

typedef struct _RoccatCpiFixedSelectorClass RoccatCpiFixedSelectorClass;
typedef struct _RoccatCpiFixedSelector RoccatCpiFixedSelector;
typedef struct _RoccatCpiFixedSelectorPrivate RoccatCpiFixedSelectorPrivate;

struct _RoccatCpiFixedSelectorClass {
	GtkFrameClass parent_class;
};

struct _RoccatCpiFixedSelector {
	GtkFrame frame;
	RoccatCpiFixedSelectorPrivate *priv;
};

GType roccat_cpi_fixed_selector_get_type(void);
GtkWidget *roccat_cpi_fixed_selector_new(guint count, guint const * const values, gboolean with_buttons);

gboolean roccat_cpi_fixed_selector_get_active(RoccatCpiFixedSelector *selector, guint index);
void roccat_cpi_fixed_selector_set_active(RoccatCpiFixedSelector *selector, guint index, gboolean state);

guint roccat_cpi_fixed_selector_get_all_active(RoccatCpiFixedSelector *selector);
void roccat_cpi_fixed_selector_set_all_active(RoccatCpiFixedSelector *selector, guint mask);

guint roccat_cpi_fixed_selector_get_selected(RoccatCpiFixedSelector *selector);
void roccat_cpi_fixed_selector_set_selected(RoccatCpiFixedSelector *selector, guint index);

G_END_DECLS

#endif
