#ifndef __ROCCAT_KONEPLUS_GFX_H__
#define __ROCCAT_KONEPLUS_GFX_H__

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
#include "roccat_device.h"

G_BEGIN_DECLS

#define KONEPLUS_GFX_TYPE (koneplus_gfx_get_type())
#define KONEPLUS_GFX(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), KONEPLUS_GFX_TYPE, KoneplusGfx))
#define IS_KONEPLUS_GFX(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), KONEPLUS_GFX_TYPE))
#define KONEPLUS_GFX_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KONEPLUS_GFX_TYPE, KoneplusGfxClass))
#define IS_KONEPLUS_GFX_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KONEPLUS_GFX_TYPE))

typedef struct _KoneplusGfx KoneplusGfx;
typedef struct _KoneplusGfxClass KoneplusGfxClass;
typedef struct _KoneplusGfxPrivate KoneplusGfxPrivate;

struct _KoneplusGfx {
	GObject parent;
	KoneplusGfxPrivate *priv;
};

struct _KoneplusGfxClass {
	GObjectClass parent_class;
};

GType koneplus_gfx_get_type(void);
KoneplusGfx *koneplus_gfx_new(RoccatDevice * const device);

gboolean koneplus_gfx_update(KoneplusGfx *gfx, GError **error);

guint32 koneplus_gfx_get_color(KoneplusGfx *gfx, guint index);
void koneplus_gfx_set_color(KoneplusGfx *gfx, guint index, guint32 color);

G_END_DECLS

#endif
