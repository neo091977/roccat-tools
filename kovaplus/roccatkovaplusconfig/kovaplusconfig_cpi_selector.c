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

#include "kovaplusconfig_cpi_selector.h"
#include "roccat_cpi_fixed_selector.h"

#define KOVAPLUSCONFIG_CPI_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KOVAPLUSCONFIG_CPI_SELECTOR_TYPE, KovaplusconfigCpiSelectorClass))
#define IS_KOVAPLUSCONFIG_CPI_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KOVAPLUSCONFIG_CPI_SELECTOR_TYPE))
#define KOVAPLUSCONFIG_CPI_SELECTOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KOVAPLUSCONFIG_CPI_SELECTOR_TYPE, KovaplusconfigCpiSelectorPrivate))

typedef struct _KovaplusconfigCpiSelectorClass KovaplusconfigCpiSelectorClass;

struct _KovaplusconfigCpiSelector {
	RoccatCpiFixedSelector parent;
};

struct _KovaplusconfigCpiSelectorClass {
	RoccatCpiFixedSelectorClass parent_class;
};

G_DEFINE_TYPE(KovaplusconfigCpiSelector, kovaplusconfig_cpi_selector, ROCCAT_CPI_FIXED_SELECTOR_TYPE);

static guint const cpi_values[KOVAPLUS_CPI_LEVELS_NUM] = { 400, 800, 1600, 3200 };

void kovaplusconfig_cpi_selector_set_from_rmp(KovaplusconfigCpiSelector *selector, KovaplusRmp *rmp) {
	RoccatCpiFixedSelector *roccat_selector = ROCCAT_CPI_FIXED_SELECTOR(selector);
	guint i;
	guint roccat_value;

	switch (kovaplus_rmp_get_cpi_x(rmp)) {
	case 1:
		roccat_value = 0;
		break;
	case 2:
		roccat_value = 1;
		break;
	case 4:
		roccat_value = 2;
		break;
	case 7:
		roccat_value = 3;
		break;
	default:
		roccat_value = 2;
		break;
	};

	roccat_cpi_fixed_selector_set_selected(roccat_selector, roccat_value);
	for (i = 0; i < KOVAPLUS_CPI_LEVELS_NUM; ++i)
		roccat_cpi_fixed_selector_set_active(roccat_selector, i, kovaplus_rmp_get_cpi(rmp, i));
}

void kovaplusconfig_cpi_selector_update_rmp(KovaplusconfigCpiSelector *selector, KovaplusRmp *rmp) {
	RoccatCpiFixedSelector *roccat_selector = ROCCAT_CPI_FIXED_SELECTOR(selector);
	guint i;
	guint kovaplus_values[KOVAPLUS_CPI_LEVELS_NUM] = {
		KOVAPLUS_PROFILE_SETTINGS_CPI_LEVEL_400,
		KOVAPLUS_PROFILE_SETTINGS_CPI_LEVEL_800,
		KOVAPLUS_PROFILE_SETTINGS_CPI_LEVEL_1600,
		KOVAPLUS_PROFILE_SETTINGS_CPI_LEVEL_3200,
	};
	guint roccat_value;

	roccat_value = roccat_cpi_fixed_selector_get_selected(roccat_selector);
	kovaplus_rmp_set_cpi_x(rmp, kovaplus_values[roccat_value]);
	kovaplus_rmp_set_cpi_y(rmp, kovaplus_values[roccat_value]);
	for (i = 0; i < KOVAPLUS_CPI_LEVELS_NUM; ++i)
		kovaplus_rmp_set_cpi(rmp, i, roccat_cpi_fixed_selector_get_active(roccat_selector, i));
}

GtkWidget *kovaplusconfig_cpi_selector_new(void) {
	KovaplusconfigCpiSelector *cpi_selector;

	cpi_selector = KOVAPLUSCONFIG_CPI_SELECTOR(g_object_new(KOVAPLUSCONFIG_CPI_SELECTOR_TYPE,
			"count", KOVAPLUS_CPI_LEVELS_NUM,
			"values", cpi_values,
			"with-buttons", TRUE,
			NULL));

	return GTK_WIDGET(cpi_selector);
}

static void kovaplusconfig_cpi_selector_init(KovaplusconfigCpiSelector *cpi_selector) {}

static void kovaplusconfig_cpi_selector_class_init(KovaplusconfigCpiSelectorClass *klass) {}
