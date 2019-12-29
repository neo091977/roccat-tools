#ifndef __ROCCAT_NYTH_GFX_H__
#define __ROCCAT_NYTH_GFX_H__

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

#include "nyth.h"
#include "roccat_device.h"

G_BEGIN_DECLS

#define NYTH_GFX_TYPE (nyth_gfx_get_type())
#define NYTH_GFX(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), NYTH_GFX_TYPE, NythGfx))
#define IS_NYTH_GFX(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), NYTH_GFX_TYPE))
#define NYTH_GFX_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), NYTH_GFX_TYPE, NythGfxClass))
#define IS_NYTH_GFX_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), NYTH_GFX_TYPE))

typedef struct _NythGfx NythGfx;
typedef struct _NythGfxClass NythGfxClass;
typedef struct _NythGfxPrivate NythGfxPrivate;

struct _NythGfx {
	GObject parent;
	NythGfxPrivate *priv;
};

struct _NythGfxClass {
	GObjectClass parent_class;
};

GType nyth_gfx_get_type(void);
NythGfx *nyth_gfx_new(RoccatDevice * const device);

gboolean nyth_gfx_update(NythGfx *gfx, GError **error);

guint32 nyth_gfx_get_color(NythGfx *gfx, guint index);
void nyth_gfx_set_color(NythGfx *gfx, guint index, guint32 color);

G_END_DECLS

#endif
