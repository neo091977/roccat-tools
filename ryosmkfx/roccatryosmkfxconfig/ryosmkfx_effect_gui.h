#ifndef __ROCCAT_RYOSMKFX_EFFECT_GUI_H__
#define __ROCCAT_RYOSMKFX_EFFECT_GUI_H__

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

#include "ryos_effect_client.h"

G_BEGIN_DECLS

#define RYOSMKFX_EFFECT_GUI_TYPE (ryosmkfx_effect_gui_get_type())
#define RYOSMKFX_EFFECT_GUI(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), RYOSMKFX_EFFECT_GUI_TYPE, RyosmkfxEffectGui))
#define IS_RYOSMKFX_EFFECT_GUI(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), RYOSMKFX_EFFECT_GUI_TYPE))

typedef struct _RyosmkfxEffectGui RyosmkfxEffectGui;

GType ryosmkfx_effect_gui_get_type(void);
RyosmkfxEffectGui *ryosmkfx_effect_gui_new(void);

G_END_DECLS

#endif
