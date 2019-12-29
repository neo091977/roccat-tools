#ifndef __ROCCAT_ISKU_MACRO_FRAME_H__
#define __ROCCAT_ISKU_MACRO_FRAME_H__

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

#include "isku.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define ISKU_MACRO_FRAME_TYPE (isku_macro_frame_get_type())
#define ISKU_MACRO_FRAME(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ISKU_MACRO_FRAME_TYPE, IskuMacroFrame))
#define IS_ISKU_MACRO_FRAME(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ISKU_MACRO_FRAME_TYPE))

typedef struct _IskuMacroFrame IskuMacroFrame;

GType isku_macro_frame_get_type(void);
GtkWidget *isku_macro_frame_new(void);
void isku_macro_frame_set_from_rkp(IskuMacroFrame *macro_frame, IskuRkp *rkp);
void isku_macro_frame_update_rkp(IskuMacroFrame *macro_frame, IskuRkp *rkp);

void isku_macro_frame_set_key(IskuMacroFrame *macro_frame, guint key_index, IskuRkpMacroKeyInfo *key_info);

G_END_DECLS

#endif
