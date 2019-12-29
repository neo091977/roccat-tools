#ifndef __ROCCAT_RYOSMKFX_GFX_H__
#define __ROCCAT_RYOSMKFX_GFX_H__

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

#include "ryosmkfx.h"
#include "roccat_device.h"

G_BEGIN_DECLS

#define RYOSMKFX_GFX_TYPE (ryosmkfx_gfx_get_type())
#define RYOSMKFX_GFX(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), RYOSMKFX_GFX_TYPE, RyosmkfxGfx))
#define IS_RYOSMKFX_GFX(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), RYOSMKFX_GFX_TYPE))
#define RYOSMKFX_GFX_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), RYOSMKFX_GFX_TYPE, RyosmkfxGfxClass))
#define IS_RYOSMKFX_GFX_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), RYOSMKFX_GFX_TYPE))

typedef struct _RyosmkfxGfx RyosmkfxGfx;
typedef struct _RyosmkfxGfxClass RyosmkfxGfxClass;
typedef struct _RyosmkfxGfxPrivate RyosmkfxGfxPrivate;

struct _RyosmkfxGfx {
	GObject parent;
	RyosmkfxGfxPrivate *priv;
};

struct _RyosmkfxGfxClass {
	GObjectClass parent_class;
};

GType ryosmkfx_gfx_get_type(void);
RyosmkfxGfx *ryosmkfx_gfx_new(RoccatDevice * const device);

gboolean ryosmkfx_gfx_update(RyosmkfxGfx *gfx, GError **error);

guint32 ryosmkfx_gfx_get_color(RyosmkfxGfx *gfx, guint index);
void ryosmkfx_gfx_set_color(RyosmkfxGfx *gfx, guint index, guint32 color);

G_END_DECLS

#endif
