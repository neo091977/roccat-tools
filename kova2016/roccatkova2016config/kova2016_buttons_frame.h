#ifndef __ROCCAT_KOVA2016_BUTTONS_FRAME_H__
#define __ROCCAT_KOVA2016_BUTTONS_FRAME_H__

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

#include "kova2016_profile_data.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define KOVA2016_BUTTONS_FRAME_TYPE (kova2016_buttons_frame_get_type())
#define KOVA2016_BUTTONS_FRAME(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), KOVA2016_BUTTONS_FRAME_TYPE, Kova2016ButtonsFrame))
#define IS_KOVA2016_BUTTONS_FRAME(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), KOVA2016_BUTTONS_FRAME_TYPE))

typedef struct _Kova2016ButtonsFrame Kova2016ButtonsFrame;

GType kova2016_buttons_frame_get_type(void);
GtkWidget *kova2016_buttons_frame_new(void);

void kova2016_buttons_frame_set_from_profile_data(Kova2016ButtonsFrame *buttons_frame, Kova2016ProfileData const *profile_data);
void kova2016_buttons_frame_update_profile_data(Kova2016ButtonsFrame *buttons_frame, Kova2016ProfileData *profile_data);

void kova2016_buttons_frame_change_handedness(Kova2016ButtonsFrame *buttons_frame);

G_END_DECLS

#endif
