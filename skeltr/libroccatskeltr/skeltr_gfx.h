#ifndef __ROCCAT_SKELTR_GFX_H__
#define __ROCCAT_SKELTR_GFX_H__

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

#include "roccat_device.h"

G_BEGIN_DECLS

#define SKELTR_GFX_TYPE (skeltr_gfx_get_type())
#define SKELTR_GFX(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), SKELTR_GFX_TYPE, SkeltrGfx))
#define IS_SKELTR_GFX(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), SKELTR_GFX_TYPE))
#define SKELTR_GFX_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), SKELTR_GFX_TYPE, SkeltrGfxClass))
#define IS_SKELTR_GFX_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), SKELTR_GFX_TYPE))

typedef struct _SkeltrGfx SkeltrGfx;
typedef struct _SkeltrGfxClass SkeltrGfxClass;
typedef struct _SkeltrGfxPrivate SkeltrGfxPrivate;

struct _SkeltrGfx {
	GObject parent;
	SkeltrGfxPrivate *priv;
};

struct _SkeltrGfxClass {
	GObjectClass parent_class;
};

GType skeltr_gfx_get_type(void);
SkeltrGfx *skeltr_gfx_new(RoccatDevice * const device);

gboolean skeltr_gfx_update(SkeltrGfx *gfx, GError **error);

guint32 skeltr_gfx_get_color(SkeltrGfx *gfx, guint index);
void skeltr_gfx_set_color(SkeltrGfx *gfx, guint index, guint32 color);

G_END_DECLS

#endif
