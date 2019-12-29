#ifndef __ROCCAT_KIRO_GFX_H__
#define __ROCCAT_KIRO_GFX_H__

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

#include "kiro.h"
#include "roccat_device.h"

G_BEGIN_DECLS

#define KIRO_GFX_TYPE (kiro_gfx_get_type())
#define KIRO_GFX(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), KIRO_GFX_TYPE, KiroGfx))
#define IS_KIRO_GFX(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), KIRO_GFX_TYPE))
#define KIRO_GFX_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KIRO_GFX_TYPE, KiroGfxClass))
#define IS_KIRO_GFX_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KIRO_GFX_TYPE))

typedef struct _KiroGfx KiroGfx;
typedef struct _KiroGfxClass KiroGfxClass;
typedef struct _KiroGfxPrivate KiroGfxPrivate;

struct _KiroGfx {
	GObject parent;
	KiroGfxPrivate *priv;
};

struct _KiroGfxClass {
	GObjectClass parent_class;
};

GType kiro_gfx_get_type(void);
KiroGfx *kiro_gfx_new(RoccatDevice * const device);

gboolean kiro_gfx_update(KiroGfx *gfx, GError **error);

guint32 kiro_gfx_get_color(KiroGfx *gfx, guint index);
void kiro_gfx_set_color(KiroGfx *gfx, guint index, guint32 color);

G_END_DECLS

#endif
