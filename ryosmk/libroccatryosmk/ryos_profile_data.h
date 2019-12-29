#ifndef __ROCCAT_RYOS_PROFILE_DATA_H__
#define __ROCCAT_RYOS_PROFILE_DATA_H__

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

#include "ryos_profile_data_eventhandler.h"
#include "ryos_profile_data_hardware.h"

G_BEGIN_DECLS

typedef struct _RyosProfileData RyosProfileData;

struct _RyosProfileData {
	RyosProfileDataEventhandler eventhandler;
	RyosProfileDataHardware hardware;
};

RyosProfileData *ryos_profile_data_new(void);
RyosProfileData *ryos_profile_data_dup(RyosProfileData const *other);

void ryos_profile_data_set_modified(RyosProfileData *profile_data);
gboolean ryos_profile_data_get_modified(RyosProfileData const *profile_data);

gboolean ryos_profile_data_update_filesystem(RyosProfileData *profile_data, guint profile_index, GError **error);
gboolean ryos_profile_data_update_hardware(RyosProfileData *profile_data, RoccatDevice *device, guint profile_index, GError **error);

gboolean ryos_profile_data_save(RoccatDevice *device, RyosProfileData *profile_data, guint profile_index, GError **error);

G_END_DECLS

#endif
