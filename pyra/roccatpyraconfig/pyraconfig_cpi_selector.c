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

#include "pyraconfig_cpi_selector.h"
#include "roccat_cpi_fixed_selector.h"

#define PYRACONFIG_CPI_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), PYRACONFIG_CPI_SELECTOR_TYPE, PyraconfigCpiSelectorClass))
#define IS_PYRACONFIG_CPI_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), PYRACONFIG_CPI_SELECTOR_TYPE))
#define PYRACONFIG_CPI_SELECTOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), PYRACONFIG_CPI_SELECTOR_TYPE, PyraconfigCpiSelectorPrivate))

typedef struct _PyraconfigCpiSelectorClass PyraconfigCpiSelectorClass;

struct _PyraconfigCpiSelector {
	RoccatCpiFixedSelector parent;
};

struct _PyraconfigCpiSelectorClass {
	RoccatCpiFixedSelectorClass parent_class;
};

G_DEFINE_TYPE(PyraconfigCpiSelector, pyraconfig_cpi_selector, ROCCAT_CPI_FIXED_SELECTOR_TYPE);

static guint const cpi_values[PYRA_CPI_NUM] = { 400, 800, 1600 };

void pyraconfig_cpi_selector_set_from_rmp(PyraconfigCpiSelector *selector, PyraRMP *rmp) {
	guint roccat_value;

	switch (pyra_rmp_get_cpi(rmp)) {
	case PYRA_CPI_400:
		roccat_value = 0;
		break;
	case PYRA_CPI_800:
		roccat_value = 1;
		break;
	case PYRA_CPI_1600:
	default:
		roccat_value = 2;
		break;
	}
	roccat_cpi_fixed_selector_set_selected(ROCCAT_CPI_FIXED_SELECTOR(selector), roccat_value);
}

void pyraconfig_cpi_selector_update_rmp(PyraconfigCpiSelector *selector, PyraRMP *rmp) {
	guint const pyra_values[PYRA_CPI_NUM] = { PYRA_CPI_400, PYRA_CPI_800, PYRA_CPI_1600 };
	pyra_rmp_set_cpi(rmp, pyra_values[roccat_cpi_fixed_selector_get_selected(ROCCAT_CPI_FIXED_SELECTOR(selector))]);
}

GtkWidget *pyraconfig_cpi_selector_new(void) {
	PyraconfigCpiSelector *cpi_selector;

	cpi_selector = PYRACONFIG_CPI_SELECTOR(g_object_new(PYRACONFIG_CPI_SELECTOR_TYPE,
			"count", PYRA_CPI_NUM,
			"values", cpi_values,
			"with-buttons", FALSE,
			NULL));

	return GTK_WIDGET(cpi_selector);
}

static void pyraconfig_cpi_selector_init(PyraconfigCpiSelector *cpi_selector) {}

static void pyraconfig_cpi_selector_class_init(PyraconfigCpiSelectorClass *klass) {}
