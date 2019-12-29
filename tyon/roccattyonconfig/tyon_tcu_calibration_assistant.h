#ifndef __ROCCAT_TYON_TCU_CALIBRATION_ASSISTANT_H__
#define __ROCCAT_TYON_TCU_CALIBRATION_ASSISTANT_H__

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

#define TYON_TCU_CALIBRATION_ASSISTANT_TYPE (tyon_tcu_calibration_assistant_get_type())
#define TYON_TCU_CALIBRATION_ASSISTANT(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), TYON_TCU_CALIBRATION_ASSISTANT_TYPE, TyonTcuCalibrationAssistant))
#define IS_TYON_TCU_CALIBRATION_ASSISTANT(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), TYON_TCU_CALIBRATION_ASSISTANT_TYPE))

typedef struct _TyonTcuCalibrationAssistant TyonTcuCalibrationAssistant;

GType tyon_tcu_calibration_assistant_get_type(void);
GtkWidget *tyon_tcu_calibration_assistant_new(GtkWindow *parent, RoccatDevice *device, guint actual_dcu);
guint tyon_tcu_calibration_assistant_get_tcu_value(TyonTcuCalibrationAssistant *cal_assistant);

G_END_DECLS

#endif
