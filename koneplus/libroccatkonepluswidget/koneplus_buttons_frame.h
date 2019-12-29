#ifndef __ROCCAT_KONEPLUS_BUTTONS_FRAME_H__
#define __ROCCAT_KONEPLUS_BUTTONS_FRAME_H__

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

#include "koneplus.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define KONEPLUS_BUTTONS_FRAME_TYPE (koneplus_buttons_frame_get_type())
#define KONEPLUS_BUTTONS_FRAME(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), KONEPLUS_BUTTONS_FRAME_TYPE, KoneplusButtonsFrame))
#define IS_KONEPLUS_BUTTONS_FRAME(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), KONEPLUS_BUTTONS_FRAME_TYPE))

typedef struct _KoneplusButtonsFrame KoneplusButtonsFrame;

GType koneplus_buttons_frame_get_type(void);
GtkWidget *koneplus_buttons_frame_new(void);
void koneplus_buttons_frame_set_from_rmp(KoneplusButtonsFrame *buttons_frame, KoneplusRmp *rmp);
void koneplus_buttons_frame_update_rmp(KoneplusButtonsFrame *buttons_frame, KoneplusRmp *rmp);

G_END_DECLS

#endif
