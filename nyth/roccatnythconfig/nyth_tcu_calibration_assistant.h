#ifndef __ROCCAT_NYTH_TCU_CALIBRATION_ASSISTANT_H__
#define __ROCCAT_NYTH_TCU_CALIBRATION_ASSISTANT_H__

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
#include "roccat_device.h"

G_BEGIN_DECLS

#define NYTH_TCU_CALIBRATION_ASSISTANT_TYPE (nyth_tcu_calibration_assistant_get_type())
#define NYTH_TCU_CALIBRATION_ASSISTANT(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), NYTH_TCU_CALIBRATION_ASSISTANT_TYPE, NythTcuCalibrationAssistant))
#define IS_NYTH_TCU_CALIBRATION_ASSISTANT(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), NYTH_TCU_CALIBRATION_ASSISTANT_TYPE))

typedef struct _NythTcuCalibrationAssistant NythTcuCalibrationAssistant;

GType nyth_tcu_calibration_assistant_get_type(void);
GtkWidget *nyth_tcu_calibration_assistant_new(GtkWindow *parent, RoccatDevice *device);
guint nyth_tcu_calibration_assistant_get_tcu_value(NythTcuCalibrationAssistant *cal_assistant);

G_END_DECLS

#endif
