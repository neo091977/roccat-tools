#ifndef __ROCCAT_KONEXTD_GFX_H__
#define __ROCCAT_KONEXTD_GFX_H__

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

#include "konextd.h"
#include "roccat_device.h"

G_BEGIN_DECLS

#define KONEXTD_GFX_TYPE (konextd_gfx_get_type())
#define KONEXTD_GFX(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), KONEXTD_GFX_TYPE, KonextdGfx))
#define IS_KONEXTD_GFX(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), KONEXTD_GFX_TYPE))
#define KONEXTD_GFX_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KONEXTD_GFX_TYPE, KonextdGfxClass))
#define IS_KONEXTD_GFX_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KONEXTD_GFX_TYPE))

typedef struct _KonextdGfx KonextdGfx;
typedef struct _KonextdGfxClass KonextdGfxClass;
typedef struct _KonextdGfxPrivate KonextdGfxPrivate;

struct _KonextdGfx {
	GObject parent;
	KonextdGfxPrivate *priv;
};

struct _KonextdGfxClass {
	GObjectClass parent_class;
};

GType konextd_gfx_get_type(void);
KonextdGfx *konextd_gfx_new(RoccatDevice * const device);

gboolean konextd_gfx_update(KonextdGfx *gfx, GError **error);

guint32 konextd_gfx_get_color(KonextdGfx *gfx, guint index);
void konextd_gfx_set_color(KonextdGfx *gfx, guint index, guint32 color);

G_END_DECLS

#endif
