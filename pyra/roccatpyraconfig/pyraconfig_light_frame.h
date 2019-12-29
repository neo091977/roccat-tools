#ifndef __ROCCAT_PYRACONFIG_LIGHT_FRAME_H__
#define __ROCCAT_PYRACONFIG_LIGHT_FRAME_H__

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

#include "pyra.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define PYRACONFIG_LIGHT_FRAME_TYPE (pyraconfig_light_frame_get_type())
#define PYRACONFIG_LIGHT_FRAME(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), PYRACONFIG_LIGHT_FRAME_TYPE, PyraconfigLightFrame))
#define IS_PYRACONFIG_LIGHT_FRAME(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), PYRACONFIG_LIGHT_FRAME_TYPE))

typedef struct _PyraconfigLightFrame PyraconfigLightFrame;

GType pyraconfig_light_frame_get_type(void);
GtkWidget *pyraconfig_light_frame_new(void);

void pyraconfig_light_frame_set_from_rmp(PyraconfigLightFrame *frame, PyraRMP *rmp);
void pyraconfig_light_frame_update_rmp(PyraconfigLightFrame *frame, PyraRMP *rmp);

G_END_DECLS

#endif
