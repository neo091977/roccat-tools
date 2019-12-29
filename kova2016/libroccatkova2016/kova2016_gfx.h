#ifndef __ROCCAT_KOVA2016_GFX_H__
#define __ROCCAT_KOVA2016_GFX_H__

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

#include "kova2016.h"
#include "roccat_device.h"

G_BEGIN_DECLS

#define KOVA2016_GFX_TYPE (kova2016_gfx_get_type())
#define KOVA2016_GFX(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), KOVA2016_GFX_TYPE, Kova2016Gfx))
#define IS_KOVA2016_GFX(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), KOVA2016_GFX_TYPE))
#define KOVA2016_GFX_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KOVA2016_GFX_TYPE, Kova2016GfxClass))
#define IS_KOVA2016_GFX_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KOVA2016_GFX_TYPE))

typedef struct _Kova2016Gfx Kova2016Gfx;
typedef struct _Kova2016GfxClass Kova2016GfxClass;
typedef struct _Kova2016GfxPrivate Kova2016GfxPrivate;

struct _Kova2016Gfx {
	GObject parent;
	Kova2016GfxPrivate *priv;
};

struct _Kova2016GfxClass {
	GObjectClass parent_class;
};

GType kova2016_gfx_get_type(void);
Kova2016Gfx *kova2016_gfx_new(RoccatDevice * const device);

gboolean kova2016_gfx_update(Kova2016Gfx *gfx, GError **error);

guint32 kova2016_gfx_get_color(Kova2016Gfx *gfx, guint index);
void kova2016_gfx_set_color(Kova2016Gfx *gfx, guint index, guint32 color);

G_END_DECLS

#endif
