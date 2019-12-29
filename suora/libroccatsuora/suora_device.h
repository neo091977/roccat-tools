#ifndef __ROCCAT_SUORA_DEVICE_H__
#define __ROCCAT_SUORA_DEVICE_H__

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

#include "roccat_device_scanner.h"

G_BEGIN_DECLS

typedef struct _SuoraDataDeclaration SuoraDataDeclaration;
typedef struct _SuoraFeaturePayload SuoraFeaturePayload;
typedef struct _SuoraFeatureBuffer SuoraFeatureBuffer;

struct _SuoraDataDeclaration {
	guint8 unknown1;
	guint8 macro_index;
	guint8 packet_count;
	guint8 unknown3;
	guint8 unknown4;
	guint8 unknown5;
} __attribute__ ((packed));

/* can be cast as
 * SuoraDataDeclaration
 * SuoraIllumination
 * SuoraInfo
 */
struct _SuoraFeaturePayload {
	guint8 data1;
	guint8 data2;
	guint8 data3;
	guint8 data4;
	guint8 data5;
	guint8 data6;
} __attribute__ ((packed));

struct _SuoraFeatureBuffer {
    guint8 action;
    SuoraFeaturePayload payload;
    guint8 checksum;
} __attribute__ ((packed));

typedef enum {
	SUORA_FEATURE_REQUEST_INFO = 0x00, /* R */
	SUORA_FEATURE_REQUEST_1 = 0x01, /* R */
	SUORA_FEATURE_REQUEST_GAME_MODE = 0x02, /* RW */
	SUORA_FEATURE_REQUEST_5 = 0x05, /* R */
	SUORA_FEATURE_REQUEST_ILLUMINATION = 0x08, /* RW */
	SUORA_FEATURE_REQUEST_KEYS = 0x0d, /* RW 10*64 bytes interrupt data */
	SUORA_FEATURE_REQUEST_MACRO = 0x11, /* RW 2*64 bytes interrupt data */
	SUORA_FEATURE_REQUEST_RESET = 0x12, /* W */
} SuoraFeatureRequest;

typedef enum {
	SUORA_INTERFACE_KEYBOARD = 0,
	SUORA_INTERFACE_MOUSE = 1,
	SUORA_INTERFACE_VENDOR = 2,
} SuoraInterface;

RoccatDevice *suora_device_first(void);
RoccatDeviceScanner *suora_device_scanner_new(void);

void suora_feature_buffer_set_checksum(SuoraFeatureBuffer *buffer);

SuoraFeaturePayload *suora_read(RoccatDevice *device, guint request, SuoraFeaturePayload const *payload, GError **error);
gchar *suora_read_interrupt(RoccatDevice *device, guint request, SuoraDataDeclaration const *declaration, guint length, GError **error);

gboolean suora_write(RoccatDevice *device, guint request, SuoraFeaturePayload const *payload, GError **error);
gboolean suora_write_interrupt(RoccatDevice *device, guint request, SuoraDataDeclaration const *declaration, gchar const *data, guint length, GError **error);

G_END_DECLS

#endif
