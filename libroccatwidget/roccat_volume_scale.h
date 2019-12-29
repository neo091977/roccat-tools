#ifndef __ROCCAT_VOLUME_SCALE_H__
#define __ROCCAT_VOLUME_SCALE_H__

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

#define ROCCAT_VOLUME_SCALE_TYPE (roccat_volume_scale_get_type())
#define ROCCAT_VOLUME_SCALE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ROCCAT_VOLUME_SCALE_TYPE, RoccatVolumeScale))
#define IS_ROCCAT_VOLUME_SCALE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ROCCAT_VOLUME_SCALE_TYPE))

typedef struct _RoccatVolumeScale RoccatVolumeScale;

GType roccat_volume_scale_get_type(void);
GtkWidget *roccat_volume_scale_new(void);

gdouble roccat_volume_scale_get_value(RoccatVolumeScale *volume_scale);
void roccat_volume_scale_set_value(RoccatVolumeScale *volume_scale, gdouble value);

G_END_DECLS

#endif
