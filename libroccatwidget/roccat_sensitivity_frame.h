#ifndef __ROCCAT_SENSITIVITY_FRAME_H__
#define __ROCCAT_SENSITIVITY_FRAME_H__

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

#define ROCCAT_SENSITIVITY_FRAME_TYPE (roccat_sensitivity_frame_get_type())
#define ROCCAT_SENSITIVITY_FRAME(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ROCCAT_SENSITIVITY_FRAME_TYPE, RoccatSensitivityFrame))
#define IS_ROCCAT_SENSITIVITY_FRAME(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ROCCAT_SENSITIVITY_FRAME_TYPE))

typedef struct _RoccatSensitivityFrame RoccatSensitivityFrame;

GType roccat_sensitivity_frame_get_type(void);
GtkWidget *roccat_sensitivity_frame_new(void);

RoccatSensitivity roccat_sensitivity_frame_get_x_value(RoccatSensitivityFrame *frame);
void roccat_sensitivity_frame_set_x_value(RoccatSensitivityFrame *frame, RoccatSensitivity value);

RoccatSensitivity roccat_sensitivity_frame_get_y_value(RoccatSensitivityFrame *frame);
void roccat_sensitivity_frame_set_y_value(RoccatSensitivityFrame *frame, RoccatSensitivity value);

RoccatSensitivityAdvanced roccat_sensitivity_frame_get_advanced(RoccatSensitivityFrame *frame);
void roccat_sensitivity_frame_set_advanced(RoccatSensitivityFrame *frame, RoccatSensitivityAdvanced value);

G_END_DECLS

#endif
