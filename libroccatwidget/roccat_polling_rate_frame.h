#ifndef __ROCCAT_POLLING_RATE_FRAME_H__
#define __ROCCAT_POLLING_RATE_FRAME_H__

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

#include "roccat.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define ROCCAT_POLLING_RATE_FRAME_TYPE (roccat_polling_rate_frame_get_type())
#define ROCCAT_POLLING_RATE_FRAME(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ROCCAT_POLLING_RATE_FRAME_TYPE, RoccatPollingRateFrame))
#define IS_ROCCAT_POLLING_RATE_FRAME(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ROCCAT_POLLING_RATE_FRAME_TYPE))

typedef struct _RoccatPollingRateFrameClass RoccatPollingRateFrameClass;
typedef struct _RoccatPollingRateFrame RoccatPollingRateFrame;
typedef struct _RoccatPollingRateFramePrivate RoccatPollingRateFramePrivate;

struct _RoccatPollingRateFrameClass {
	GtkFrameClass parent_class;
};

struct _RoccatPollingRateFrame {
	GtkFrame parent;
	RoccatPollingRateFramePrivate *priv;
};

GType roccat_polling_rate_frame_get_type(void);
GtkWidget *roccat_polling_rate_frame_new(void);

RoccatPollingRate roccat_polling_rate_frame_get_value(RoccatPollingRateFrame *frame);
void roccat_polling_rate_frame_set_value(RoccatPollingRateFrame *frame, RoccatPollingRate new_value);

G_END_DECLS

#endif
