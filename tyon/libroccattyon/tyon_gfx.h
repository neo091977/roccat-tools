#ifndef __ROCCAT_TYON_GFX_H__
#define __ROCCAT_TYON_GFX_H__

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

#include "tyon.h"
#include "roccat_device.h"

G_BEGIN_DECLS

#define TYON_GFX_TYPE (tyon_gfx_get_type())
#define TYON_GFX(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), TYON_GFX_TYPE, TyonGfx))
#define IS_TYON_GFX(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), TYON_GFX_TYPE))
#define TYON_GFX_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), TYON_GFX_TYPE, TyonGfxClass))
#define IS_TYON_GFX_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), TYON_GFX_TYPE))

typedef struct _TyonGfx TyonGfx;
typedef struct _TyonGfxClass TyonGfxClass;
typedef struct _TyonGfxPrivate TyonGfxPrivate;

struct _TyonGfx {
	GObject parent;
	TyonGfxPrivate *priv;
};

struct _TyonGfxClass {
	GObjectClass parent_class;
};

GType tyon_gfx_get_type(void);
TyonGfx *tyon_gfx_new(RoccatDevice * const device);

gboolean tyon_gfx_update(TyonGfx *gfx, GError **error);

guint32 tyon_gfx_get_color(TyonGfx *gfx, guint index);
void tyon_gfx_set_color(TyonGfx *gfx, guint index, guint32 color);

G_END_DECLS

#endif
