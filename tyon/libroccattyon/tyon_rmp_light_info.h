#ifndef __ROCCAT_TYON_RMP_LIGHT_INFO_H__
#define __ROCCAT_TYON_RMP_LIGHT_INFO_H__

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

#include "tyon_profile_settings.h"

G_BEGIN_DECLS

typedef struct _TyonRmpLightInfo TyonRmpLightInfo;

enum {
	TYON_RMP_LIGHT_INFO_COLORS_NUM = 16,
};

struct _TyonRmpLightInfo {
	guint8 index;
	guint8 state;
	guint8 red;
	guint8 green;
	guint8 blue;
	guint8 null;
	guint8 checksum;
} __attribute__ ((packed));

typedef enum {
	TYON_RMP_LIGHT_CHOSE_TYPE_PALETTE = 0,
	TYON_RMP_LIGHT_CHOSE_TYPE_CUSTOM = 1,
} TyonRmpLightType;

typedef enum {
	TYON_RMP_LIGHT_INFO_STATE_ON = 1,
	TYON_RMP_LIGHT_INFO_STATE_OFF = 2,
} TyonRmpLightInfoState;

TyonRmpLightInfo const *tyon_rmp_light_info_get_standard(guint index);
guint8 tyon_rmp_light_info_calc_checksum(TyonRmpLightInfo const *rmp_light_info);
void tyon_rmp_light_info_set_checksum(TyonRmpLightInfo *rmp_light_info);
void tyon_rmp_light_info_to_light(TyonRmpLightInfo const *from, TyonLight *to);
void tyon_light_to_rmp_light_info(TyonLight const *from, TyonRmpLightInfo *to, guint state);
gboolean tyon_rmp_light_info_equal(TyonRmpLightInfo const *left, TyonRmpLightInfo const *right);

G_END_DECLS

#endif
