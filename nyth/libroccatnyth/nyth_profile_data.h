#ifndef __ROCCAT_NYTH_PROFILE_DATA_H__
#define __ROCCAT_NYTH_PROFILE_DATA_H__

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

#include "nyth_profile_data_eventhandler.h"
#include "nyth_profile_data_hardware.h"

G_BEGIN_DECLS

typedef struct _NythProfileData NythProfileData;

struct _NythProfileData {
	NythProfileDataEventhandler eventhandler;
	NythProfileDataHardware hardware;
};

NythProfileData *nyth_profile_data_new(void);
NythProfileData *nyth_profile_data_dup(NythProfileData const *other);

void nyth_profile_data_set_modified(NythProfileData *profile_data);
gboolean nyth_profile_data_get_modified(NythProfileData const *profile_data);

gboolean nyth_profile_data_update_filesystem(NythProfileData *profile_data, guint profile_index, GError **error);
gboolean nyth_profile_data_update_hardware(NythProfileData *profile_data, RoccatDevice *device, guint profile_index, GError **error);

gboolean nyth_profile_data_save(RoccatDevice *device, NythProfileData *profile_data, guint profile_index, GError **error);

G_END_DECLS

#endif
