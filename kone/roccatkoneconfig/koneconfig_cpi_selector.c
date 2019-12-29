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

#include "koneconfig_cpi_selector.h"
#include "roccat_cpi_fixed_selector.h"

#define KONECONFIG_CPI_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KONECONFIG_CPI_SELECTOR_TYPE, KoneconfigCpiSelectorClass))
#define IS_KONECONFIG_CPI_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KONECONFIG_CPI_SELECTOR_TYPE))
#define KONECONFIG_CPI_SELECTOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KONECONFIG_CPI_SELECTOR_TYPE, KoneconfigCpiSelectorPrivate))

typedef struct _KoneconfigCpiSelectorClass KoneconfigCpiSelectorClass;

struct _KoneconfigCpiSelector {
	RoccatCpiFixedSelector parent;
};

struct _KoneconfigCpiSelectorClass {
	RoccatCpiFixedSelectorClass parent_class;
};

G_DEFINE_TYPE(KoneconfigCpiSelector, koneconfig_cpi_selector, ROCCAT_CPI_FIXED_SELECTOR_TYPE);

static guint const cpi_values[KONE_DPI_NUM] = { 800, 1200, 1600, 2000, 2400, 3200 };

void koneconfig_cpi_selector_set_from_rmp(KoneconfigCpiSelector *selector, KoneRMP *rmp) {
	RoccatCpiFixedSelector *roccat_selector = ROCCAT_CPI_FIXED_SELECTOR(selector);
	guint i;
	roccat_cpi_fixed_selector_set_selected(roccat_selector, kone_rmp_get_startup_dpi(rmp) - 1);
	for (i = 0; i < KONE_DPI_NUM; ++i)
		roccat_cpi_fixed_selector_set_active(roccat_selector, i, kone_rmp_get_dpi(rmp, i));
}

void koneconfig_cpi_selector_update_rmp(KoneconfigCpiSelector *selector, KoneRMP *rmp) {
	RoccatCpiFixedSelector *roccat_selector = ROCCAT_CPI_FIXED_SELECTOR(selector);
	guint i;
	kone_rmp_set_startup_dpi(rmp, roccat_cpi_fixed_selector_get_selected(roccat_selector) + 1);
	for (i = 0; i < KONE_DPI_NUM; ++i)
		kone_rmp_set_dpi(rmp, i, roccat_cpi_fixed_selector_get_active(roccat_selector, i));
}

GtkWidget *koneconfig_cpi_selector_new(void) {
	KoneconfigCpiSelector *cpi_selector;

	cpi_selector = KONECONFIG_CPI_SELECTOR(g_object_new(KONECONFIG_CPI_SELECTOR_TYPE,
			"count", KONE_DPI_NUM,
			"values", cpi_values,
			"with-buttons", TRUE,
			NULL));

	return GTK_WIDGET(cpi_selector);
}

static void koneconfig_cpi_selector_init(KoneconfigCpiSelector *cpi_selector) {}

static void koneconfig_cpi_selector_class_init(KoneconfigCpiSelectorClass *klass) {}
