#ifndef __ROCCAT_ISKUFXCONFIG_COLOR_FRAME_H__
#define __ROCCAT_ISKUFXCONFIG_COLOR_FRAME_H__

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

#include "isku.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define ISKUFXCONFIG_COLOR_FRAME_TYPE (iskufxconfig_color_frame_get_type())
#define ISKUFXCONFIG_COLOR_FRAME(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ISKUFXCONFIG_COLOR_FRAME_TYPE, IskufxconfigColorFrame))
#define IS_ISKUFXCONFIG_COLOR_FRAME(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ISKUFXCONFIG_COLOR_FRAME_TYPE))

typedef struct _IskufxconfigColorFrame IskufxconfigColorFrame;

GType iskufxconfig_color_frame_get_type(void);
GtkWidget *iskufxconfig_color_frame_new(void);

void iskufxconfig_color_frame_set_from_rkp(IskufxconfigColorFrame *frame, IskuRkp *rkp);
void iskufxconfig_color_frame_update_rkp(IskufxconfigColorFrame *frame, IskuRkp *rkp);

G_END_DECLS

#endif
