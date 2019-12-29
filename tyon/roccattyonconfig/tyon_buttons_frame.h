#ifndef __ROCCAT_TYON_BUTTONS_FRAME_H__
#define __ROCCAT_TYON_BUTTONS_FRAME_H__

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

#include "tyon_rmp.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define TYON_BUTTONS_FRAME_TYPE (tyon_buttons_frame_get_type())
#define TYON_BUTTONS_FRAME(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), TYON_BUTTONS_FRAME_TYPE, TyonButtonsFrame))
#define IS_TYON_BUTTONS_FRAME(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), TYON_BUTTONS_FRAME_TYPE))

typedef struct _TyonButtonsFrame TyonButtonsFrame;

GType tyon_buttons_frame_get_type(void);
GtkWidget *tyon_buttons_frame_new(void);
void tyon_buttons_frame_set_from_rmp(TyonButtonsFrame *buttons_frame, TyonRmp *rmp);
void tyon_buttons_frame_update_rmp(TyonButtonsFrame *buttons_frame, TyonRmp *rmp);

G_END_DECLS

#endif
