#ifndef __ROCCAT_RYOSTKLCONFIG_BLINK_H__
#define __ROCCAT_RYOSTKLCONFIG_BLINK_H__

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

#include "ryostkl_light_layer.h"
#include "ryos_custom_lights.h"

G_BEGIN_DECLS

#define RYOSTKLCONFIG_BLINK_TYPE (ryostklconfig_blink_get_type())
#define RYOSTKLCONFIG_BLINK(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), RYOSTKLCONFIG_BLINK_TYPE, RyostklconfigBlink))
#define IS_RYOSTKLCONFIG_BLINK(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), RYOSTKLCONFIG_BLINK_TYPE))
#define RYOSTKLCONFIG_BLINK_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), RYOSTKLCONFIG_BLINK_TYPE, RyostklconfigBlinkClass))
#define IS_RYOSTKLCONFIG_BLINK_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), RYOSTKLCONFIG_BLINK_TYPE))

typedef struct _RyostklconfigBlink RyostklconfigBlink;
typedef struct _RyostklconfigBlinkClass RyostklconfigBlinkClass;
typedef struct _RyostklconfigBlinkPrivate RyostklconfigBlinkPrivate;

struct _RyostklconfigBlink {
	GObject parent;
	RyostklconfigBlinkPrivate *priv;
};

struct _RyostklconfigBlinkClass {
	GObjectClass parent_class;
};

GType ryostklconfig_blink_get_type(void);
RyostklconfigBlink *ryostklconfig_blink_new(void);

void ryostklconfig_blink_start(RyostklconfigBlink *blink);
void ryostklconfig_blink_stop(RyostklconfigBlink *blink);

gboolean ryostklconfig_blink_get_light_layer_key_state(RyostklconfigBlink const *blink, guint16 const *key);
gfloat ryostklconfig_blink_get_light_layer_key_intensity(RyostklconfigBlink const *blink, guint16 const *key);

void ryostklconfig_blink_light_layer_to_custom_lights(RyostklconfigBlink const *blink, RyostklLightLayer const *light_layer, RyosCustomLights *custom_lights);

G_END_DECLS

#endif
