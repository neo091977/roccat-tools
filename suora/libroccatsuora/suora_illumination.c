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

#include "suora_illumination.h"
#include "suora_device.h"
#include <string.h>

static SuoraIllumination const default_illumination = {
	0x00, 0x02, 0x00, 0x0b, 0x00, 0x00,
};

SuoraIllumination const *suora_illumination_default(void) {
	return &default_illumination;
}

void suora_illumination_copy(SuoraIllumination *destination, SuoraIllumination const *source) {
	memcpy(destination, source, sizeof(SuoraIllumination));
}

SuoraIllumination *suora_illumination_dup(SuoraIllumination const *old) {
	return g_memdup(old, sizeof(SuoraIllumination));
}

gboolean suora_illumination_equal(SuoraIllumination const *left, SuoraIllumination const *right) {
	return memcmp(left, right, sizeof(SuoraIllumination)) ? FALSE : TRUE;
}

SuoraIllumination *suora_illumination_read(RoccatDevice *device, GError **error) {
	SuoraFeaturePayload const out = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	return (SuoraIllumination *)suora_read(device, SUORA_FEATURE_REQUEST_ILLUMINATION, &out, error);
}

gboolean suora_illumination_write(RoccatDevice *device, SuoraIllumination const *illumination, GError **error) {
	return suora_write(device, SUORA_FEATURE_REQUEST_ILLUMINATION, (SuoraFeaturePayload const *)illumination, error);
}
