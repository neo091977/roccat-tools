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

#include "savuconfig_cpi_selector.h"
#include "roccat_cpi_fixed_selector.h"

#define SAVUCONFIG_CPI_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), SAVUCONFIG_CPI_SELECTOR_TYPE, SavuconfigCpiSelectorClass))
#define IS_SAVUCONFIG_CPI_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), SAVUCONFIG_CPI_SELECTOR_TYPE))
#define SAVUCONFIG_CPI_SELECTOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), SAVUCONFIG_CPI_SELECTOR_TYPE, SavuconfigCpiSelectorPrivate))

typedef struct _SavuconfigCpiSelectorClass SavuconfigCpiSelectorClass;

struct _SavuconfigCpiSelector {
	RoccatCpiFixedSelector parent;
};

struct _SavuconfigCpiSelectorClass {
	RoccatCpiFixedSelectorClass parent_class;
};

G_DEFINE_TYPE(SavuconfigCpiSelector, savuconfig_cpi_selector, ROCCAT_CPI_FIXED_SELECTOR_TYPE);

static guint const cpi_values[SAVU_CPI_NUM] = { 400, 800, 1600, 4000 };

void savuconfig_cpi_selector_set_from_rmp(SavuconfigCpiSelector *selector, SavuRmp *rmp) {
	RoccatCpiFixedSelector *roccat_selector = ROCCAT_CPI_FIXED_SELECTOR(selector);
	guint i;

	roccat_cpi_fixed_selector_set_selected(roccat_selector, savu_rmp_get_cpi_step(rmp));
	for (i = 0; i < SAVU_CPI_NUM; ++i)
		roccat_cpi_fixed_selector_set_active(roccat_selector, i, savu_rmp_get_cpi(rmp, i));
}

void savuconfig_cpi_selector_update_rmp(SavuconfigCpiSelector *selector, SavuRmp *rmp) {
	RoccatCpiFixedSelector *roccat_selector = ROCCAT_CPI_FIXED_SELECTOR(selector);
	guint i;

	savu_rmp_set_cpi_step(rmp, roccat_cpi_fixed_selector_get_selected(roccat_selector));
	for (i = 0; i < SAVU_CPI_NUM; ++i)
		savu_rmp_set_cpi(rmp, i, roccat_cpi_fixed_selector_get_active(roccat_selector, i));

}

GtkWidget *savuconfig_cpi_selector_new(void) {
	SavuconfigCpiSelector *cpi_selector;

	cpi_selector = SAVUCONFIG_CPI_SELECTOR(g_object_new(SAVUCONFIG_CPI_SELECTOR_TYPE,
			"count", SAVU_CPI_NUM,
			"values", cpi_values,
			"with-buttons", TRUE,
			NULL));

	return GTK_WIDGET(cpi_selector);
}

static void savuconfig_cpi_selector_init(SavuconfigCpiSelector *cpi_selector) {}

static void savuconfig_cpi_selector_class_init(SavuconfigCpiSelectorClass *klass) {}
