#ifndef __ROCCAT_KIRO_PROFILE_DATA_H__
#define __ROCCAT_KIRO_PROFILE_DATA_H__

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

#include "kiro_profile_data_eventhandler.h"
#include "kiro_profile_data_hardware.h"

G_BEGIN_DECLS

typedef struct _KiroProfileData KiroProfileData;

struct _KiroProfileData {
	KiroProfileDataEventhandler eventhandler;
	KiroProfileDataHardware hardware;
};

KiroProfileData *kiro_profile_data_new(void);
KiroProfileData *kiro_profile_data_dup(KiroProfileData const *other);

void kiro_profile_data_set_modified(KiroProfileData *profile_data);
gboolean kiro_profile_data_get_modified(KiroProfileData const *profile_data);

gboolean kiro_profile_data_update_filesystem(KiroProfileData *profile_data, GError **error);
gboolean kiro_profile_data_update_hardware(KiroProfileData *profile_data, RoccatDevice *device, GError **error);

gboolean kiro_profile_data_save(RoccatDevice *device, KiroProfileData *profile_data, GError **error);

G_END_DECLS

#endif
