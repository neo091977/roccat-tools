#ifndef __ROCCAT_SENSOR_REGISTER_DIALOG_H__
#define __ROCCAT_SENSOR_REGISTER_DIALOG_H__

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

#define ROCCAT_SENSOR_REGISTER_DIALOG_TYPE (roccat_sensor_register_dialog_get_type())
#define ROCCAT_SENSOR_REGISTER_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ROCCAT_SENSOR_REGISTER_DIALOG_TYPE, RoccatSensorRegisterDialog))
#define IS_ROCCAT_SENSOR_REGISTER_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ROCCAT_SENSOR_REGISTER_DIALOG_TYPE))

typedef struct _RoccatSensorRegisterDialog RoccatSensorRegisterDialog;
typedef struct _RoccatSensorRegisterDialogPrivate RoccatSensorRegisterDialogPrivate;

struct _RoccatSensorRegisterDialog {
	GtkDialog dialog;
	RoccatSensorRegisterDialogPrivate *priv;
};

GType roccat_sensor_register_dialog_get_type(void);
GtkWidget *roccat_sensor_register_dialog_new(GtkWindow *parent);

guint roccat_sensor_register_dialog_get_value(RoccatSensorRegisterDialog *sensor_register_dialog);
void roccat_sensor_register_dialog_set_value(RoccatSensorRegisterDialog *sensor_register_dialog, guint value);

guint roccat_sensor_register_dialog_get_register(RoccatSensorRegisterDialog *sensor_register_dialog);

G_END_DECLS

#endif
