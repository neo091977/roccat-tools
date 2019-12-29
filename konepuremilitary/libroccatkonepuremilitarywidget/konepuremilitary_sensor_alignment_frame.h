#ifndef __ROCCAT_KONEPUREMILITARY_SENSOR_ALIGNMENT_FRAME_H__
#define __ROCCAT_KONEPUREMILITARY_SENSOR_ALIGNMENT_FRAME_H__

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

#define KONEPUREMILITARY_SENSOR_ALIGNMENT_FRAME_TYPE (konepuremilitary_sensor_alignment_frame_get_type())
#define KONEPUREMILITARY_SENSOR_ALIGNMENT_FRAME(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), KONEPUREMILITARY_SENSOR_ALIGNMENT_FRAME_TYPE, KonepuremilitarySensorAlignmentFrame))
#define IS_KONEPUREMILITARY_SENSOR_ALIGNMENT_FRAME(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), KONEPUREMILITARY_SENSOR_ALIGNMENT_FRAME_TYPE))

typedef struct _KonepuremilitarySensorAlignmentFrame KonepuremilitarySensorAlignmentFrame;

GType konepuremilitary_sensor_alignment_frame_get_type(void);
GtkWidget *konepuremilitary_sensor_alignment_frame_new(void);

void konepuremilitary_sensor_alignment_frame_set_from_rmp(KonepuremilitarySensorAlignmentFrame *frame, KoneplusRmp *rmp);
void konepuremilitary_sensor_alignment_frame_update_rmp(KonepuremilitarySensorAlignmentFrame *frame, KoneplusRmp *rmp);

G_END_DECLS

#endif
