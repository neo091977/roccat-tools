#ifndef __ROCCAT_RYOSMKFX_LIGHT_EFFECTS_EMULATOR_H__
#define __ROCCAT_RYOSMKFX_LIGHT_EFFECTS_EMULATOR_H__

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

#include "ryosmkfx_light_layer.h"
#include "ryosmkfx_keyboard_selector.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define RYOSMKFX_LIGHT_EFFECTS_EMULATOR_TYPE (ryosmkfx_light_effects_emulator_get_type())
#define RYOSMKFX_LIGHT_EFFECTS_EMULATOR(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), RYOSMKFX_LIGHT_EFFECTS_EMULATOR_TYPE, RyosmkfxLightEffectsEmulator))
#define IS_RYOSMKFX_LIGHT_EFFECTS_EMULATOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), RYOSMKFX_LIGHT_EFFECTS_EMULATOR_TYPE))
#define RYOSMKFX_LIGHT_EFFECTS_EMULATOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), RYOSMKFX_LIGHT_EFFECTS_EMULATOR_TYPE, RyosmkfxLightEffectsEmulatorClass))
#define IS_RYOSMKFX_LIGHT_EFFECTS_EMULATOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), RYOSMKFX_LIGHT_EFFECTS_EMULATOR_TYPE))

typedef struct _RyosmkfxLightEffectsEmulator RyosmkfxLightEffectsEmulator;
typedef struct _RyosmkfxLightEffectsEmulatorClass RyosmkfxLightEffectsEmulatorClass;
typedef struct _RyosmkfxLightEffectsEmulatorPrivate RyosmkfxLightEffectsEmulatorPrivate;

struct _RyosmkfxLightEffectsEmulator {
	GObject parent;
	RyosmkfxLightEffectsEmulatorPrivate *priv;
};

struct _RyosmkfxLightEffectsEmulatorClass {
	GObjectClass parent_class;
};

GType ryosmkfx_light_effects_emulator_get_type(void);
RyosmkfxLightEffectsEmulator *ryosmkfx_light_effects_emulator_new(RyosmkfxKeyboardSelector *keyboard_selector);

void ryosmkfx_light_effects_emulator_set_state(RyosmkfxLightEffectsEmulator *emulator, gboolean run);

void ryosmkfx_light_effects_emulator_set_script(RyosmkfxLightEffectsEmulator *emulator, gchar const *script);
void ryosmkfx_light_effects_emulator_set_effect(RyosmkfxLightEffectsEmulator *emulator, guint effect);
void ryosmkfx_light_effects_emulator_set_speed(RyosmkfxLightEffectsEmulator *emulator, guint speed);

void ryosmkfx_light_effects_emulator_key_event(RyosmkfxLightEffectsEmulator *emulator, guint sdk_index, gboolean action);
void ryosmkfx_light_effects_emulator_set_layer_data_temp(RyosmkfxLightEffectsEmulator *emulator, RyosmkfxLightLayerData const *data);

gboolean ryosmkfx_light_effects_emulator_uses_temp_layer_data(RyosmkfxLightEffectsEmulator *emulator);

G_END_DECLS

#endif
