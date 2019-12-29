#ifndef __ROCCAT_KONECONFIG_POLLING_RATE_FRAME_H__
#define __ROCCAT_KONECONFIG_POLLING_RATE_FRAME_H__

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

#define KONECONFIG_POLLING_RATE_FRAME_TYPE (koneconfig_polling_rate_frame_get_type())
#define KONECONFIG_POLLING_RATE_FRAME(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), KONECONFIG_POLLING_RATE_FRAME_TYPE, KoneconfigPollingRateFrame))
#define IS_KONECONFIG_POLLING_RATE_FRAME(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), KONECONFIG_POLLING_RATE_FRAME_TYPE))

typedef struct _KoneconfigPollingRateFrame KoneconfigPollingRateFrame;

GType koneconfig_polling_rate_frame_get_type(void);
GtkWidget *koneconfig_polling_rate_frame_new(void);
void koneconfig_polling_rate_frame_set_value(KoneconfigPollingRateFrame *frame, guint rmp_value);
guint koneconfig_polling_rate_frame_get_value(KoneconfigPollingRateFrame *frame);

G_END_DECLS

#endif
