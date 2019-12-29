#ifndef __ROCCAT_RYOSTKL_RKP_H__
#define __ROCCAT_RYOSTKL_RKP_H__

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

#include "ryostkl_profile_data.h"

G_BEGIN_DECLS

typedef struct _RyostklRkp RyostklRkp;

struct _RyostklRkp {
	RyosKeysPrimary keys_primary;
	guint8 unused1;
	RyosKeysFunction keys_function;
	guint8 unused2;
	RyosKeysThumbster keys_thumbster;
	guint8 unused3;
	RyosKeysExtra keys_extra;
	guint8 unused4;
	RyosKeysEasyzone keys_easyzone;
	guint8 unused5;
	RyosKeyMask key_mask;
	guint8 unused6;
	RyosLight light;
	guint8 unused7;
	Ryos14 _14;
	guint8 unused8;
	RyosMacro macros[RYOS_RKP_KEYS_NUM];
	guint8 unused9;
	guint16 profile_name[RYOS_RKP_PROFILE_NAME_LENGTH];
	guint32 character_repeat_delay;
	guint32 character_repeat_rate;
	guint8 unknown1;
	guint8 led_volume_indicator;
	guint8 sound_feedback_mask;
	guint8 sound_feedback_volume;
	guint8 unknown2[5];
	RyosRkpTalk talks[RYOS_RKP_TALK_NUM];
	guint8 unused10;
	guint16 gamefile_names[RYOS_RKP_PROFILE_GAMEFILE_NUM][RYOS_RKP_PROFILE_GAMEFILE_LENGTH];
	guint8 unknown3[5];
	RyosRkpTimer timers[RYOS_RKP_KEYS_NUM];
	guint8 unknown4[1070];
	RyosRkpQuicklaunch launchers[RYOS_RKP_KEYS_NUM];
	guint8 unknown5[0x5c15];
	RyostklLightLayer light_layer_automatic_0;
	guint8 unused11;
	RyostklLightLayer light_layer_manual_0;
	guint8 unused12;
	RyosLedMacro led_macros[RYOS_RKP_KEYS_NUM];
	guint8 unknown6[0xc9];
} __attribute__ ((packed)); // size 0x84a03

RyostklRkp *ryostkl_profile_data_to_rkp(RyostklProfileData const *profile_data);
RyostklProfileData *ryostkl_rkp_to_profile_data(RyostklRkp const *rkp);

RyostklRkp *ryostkl_rkp_read_with_path(gchar const *path, GError **error);
gboolean ryostkl_rkp_write_with_path(gchar const *path, RyostklRkp const *rkp, GError **error);

G_END_DECLS

#endif
