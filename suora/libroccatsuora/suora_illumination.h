#ifndef __ROCCAT_SUORA_ILLUMINATION_H__
#define __ROCCAT_SUORA_ILLUMINATION_H__

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

typedef struct _SuoraIllumination SuoraIllumination;

struct _SuoraIllumination {
	guint8 unknown1;
	guint8 effect;
	guint8 unknown2;
	guint8 brightness;
	guint8 unknown3;
	guint8 unknown4;
} __attribute__ ((packed));

enum {
	SUORA_ILLUMINATION_BRIGHTNESS_MIN = 0,
	SUORA_ILLUMINATION_BRIGHTNESS_MAX = 0xa,
	SUORA_ILLUMINATION_BRIGHTNESS_BREATHING = 0xb,
};

typedef enum {
	SUORA_ILLUMINATION_EFFECT_OFF = 0,
	SUORA_ILLUMINATION_EFFECT_FULLY_LIT = 1,
	SUORA_ILLUMINATION_EFFECT_BREATHING = 2,
} SuoraIlluminationEffect;

SuoraIllumination const *suora_illumination_default(void);
void suora_illumination_copy(SuoraIllumination *destination, SuoraIllumination const *source);
SuoraIllumination *suora_illumination_dup(SuoraIllumination const *old);
gboolean suora_illumination_equal(SuoraIllumination const *left, SuoraIllumination const *right);

SuoraIllumination *suora_illumination_read(RoccatDevice *device, GError **error);
gboolean suora_illumination_write(RoccatDevice *device, SuoraIllumination const *illumination, GError **error);

G_END_DECLS

#endif
