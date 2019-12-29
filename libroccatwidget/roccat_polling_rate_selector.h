#ifndef __ROCCAT_POLLING_RATE_SELECTOR_H__
#define __ROCCAT_POLLING_RATE_SELECTOR_H__

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

#define ROCCAT_POLLING_RATE_SELECTOR_TYPE (roccat_polling_rate_selector_get_type())
#define ROCCAT_POLLING_RATE_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ROCCAT_POLLING_RATE_SELECTOR_TYPE, RoccatPollingRateSelector))
#define IS_ROCCAT_POLLING_RATE_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ROCCAT_POLLING_RATE_SELECTOR_TYPE))

typedef struct _RoccatPollingRateSelectorClass RoccatPollingRateSelectorClass;
typedef struct _RoccatPollingRateSelector RoccatPollingRateSelector;
typedef struct _RoccatPollingRateSelectorPrivate RoccatPollingRateSelectorPrivate;

struct _RoccatPollingRateSelectorClass {
	GtkTableClass parent_class;
};

struct _RoccatPollingRateSelector {
	GtkTable parent;
	RoccatPollingRateSelectorPrivate *priv;
};

typedef enum {
	ROCCAT_POLLING_RATE_SELECTOR_NONE = 0,
	ROCCAT_POLLING_RATE_SELECTOR_125 = 1 << 0,
	ROCCAT_POLLING_RATE_SELECTOR_250 = 1 << 1,
	ROCCAT_POLLING_RATE_SELECTOR_500 = 1 << 2,
	ROCCAT_POLLING_RATE_SELECTOR_1000 = 1 << 3,
	ROCCAT_POLLING_RATE_SELECTOR_ALL = ROCCAT_POLLING_RATE_SELECTOR_125 |
		ROCCAT_POLLING_RATE_SELECTOR_250 |
		ROCCAT_POLLING_RATE_SELECTOR_500 |
		ROCCAT_POLLING_RATE_SELECTOR_1000,

} RoccatPollingRateSelectorValue;

GType roccat_polling_rate_selector_get_type(void);
GtkWidget *roccat_polling_rate_selector_new(guint mask);

guint roccat_polling_rate_selector_get_value(RoccatPollingRateSelector *selector);
void roccat_polling_rate_selector_set_value(RoccatPollingRateSelector *selector, guint new_value);

guint roccat_polling_rate_selector_get_mask(RoccatPollingRateSelector *selector);

G_END_DECLS

#endif
