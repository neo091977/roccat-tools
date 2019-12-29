#ifndef __ROCCAT_ISKUFX_GFX_H__
#define __ROCCAT_ISKUFX_GFX_H__

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

#include "iskufx.h"
#include "roccat_device.h"

G_BEGIN_DECLS

#define ISKUFX_GFX_TYPE (iskufx_gfx_get_type())
#define ISKUFX_GFX(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ISKUFX_GFX_TYPE, IskufxGfx))
#define IS_ISKUFX_GFX(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ISKUFX_GFX_TYPE))
#define ISKUFX_GFX_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ISKUFX_GFX_TYPE, IskufxGfxClass))
#define IS_ISKUFX_GFX_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ISKUFX_GFX_TYPE))

typedef struct _IskufxGfx IskufxGfx;
typedef struct _IskufxGfxClass IskufxGfxClass;
typedef struct _IskufxGfxPrivate IskufxGfxPrivate;

struct _IskufxGfx {
	GObject parent;
	IskufxGfxPrivate *priv;
};

struct _IskufxGfxClass {
	GObjectClass parent_class;
};

GType iskufx_gfx_get_type(void);
IskufxGfx *iskufx_gfx_new(RoccatDevice * const device);

gboolean iskufx_gfx_update(IskufxGfx *gfx, GError **error);

guint32 iskufx_gfx_get_color(IskufxGfx *gfx, guint index);
void iskufx_gfx_set_color(IskufxGfx *gfx, guint index, guint32 color);

G_END_DECLS

#endif
