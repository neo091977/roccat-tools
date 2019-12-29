#ifndef __ROCCAT_KONECONFIG_SENSITIVITY_FRAME_H__
#define __ROCCAT_KONECONFIG_SENSITIVITY_FRAME_H__

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

#include "kone.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define KONECONFIG_SENSITIVITY_FRAME_TYPE (koneconfig_sensitivity_frame_get_type())
#define KONECONFIG_SENSITIVITY_FRAME(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), KONECONFIG_SENSITIVITY_FRAME_TYPE, KoneconfigSensitivityFrame))
#define IS_KONECONFIG_SENSITIVITY_FRAME(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), KONECONFIG_SENSITIVITY_FRAME_TYPE))

typedef struct _KoneconfigSensitivityFrame KoneconfigSensitivityFrame;

GType koneconfig_sensitivity_frame_get_type(void);
GtkWidget *koneconfig_sensitivity_frame_new(void);

void koneconfig_sensitivity_frame_set_from_rmp(KoneconfigSensitivityFrame *frame, KoneRMP *rmp);
void koneconfig_sensitivity_frame_update_rmp(KoneconfigSensitivityFrame *frame, KoneRMP *rmp);

G_END_DECLS

#endif
