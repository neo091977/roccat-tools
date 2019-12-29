#ifndef __ROCCAT_KONEPUREMILITARY_ANGLESNAP_FRAME_H__
#define __ROCCAT_KONEPUREMILITARY_ANGLESNAP_FRAME_H__

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

#define KONEPUREMILITARY_ANGLESNAP_FRAME_TYPE (konepuremilitary_anglesnap_frame_get_type())
#define KONEPUREMILITARY_ANGLESNAP_FRAME(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), KONEPUREMILITARY_ANGLESNAP_FRAME_TYPE, KonepuremilitaryAnglesnapFrame))
#define IS_KONEPUREMILITARY_ANGLESNAP_FRAME(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), KONEPUREMILITARY_ANGLESNAP_FRAME_TYPE))

typedef struct _KonepuremilitaryAnglesnapFrame KonepuremilitaryAnglesnapFrame;

GType konepuremilitary_anglesnap_frame_get_type(void);
GtkWidget *konepuremilitary_anglesnap_frame_new(void);

void konepuremilitary_anglesnap_frame_set_from_rmp(KonepuremilitaryAnglesnapFrame *frame, KoneplusRmp *rmp);
void konepuremilitary_anglesnap_frame_update_rmp(KonepuremilitaryAnglesnapFrame *frame, KoneplusRmp *rmp);

G_END_DECLS

#endif
