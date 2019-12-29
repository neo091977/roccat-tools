#ifndef __ROCCAT_RYOSTKL_PROFILE_DATA_H__
#define __ROCCAT_RYOSTKL_PROFILE_DATA_H__

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

#include "ryostkl_profile_data_eventhandler.h"
#include "ryostkl_profile_data_hardware.h"

G_BEGIN_DECLS

typedef struct _RyostklProfileData RyostklProfileData;

struct _RyostklProfileData {
	RyostklProfileDataEventhandler eventhandler;
	RyostklProfileDataHardware hardware;
};

RyostklProfileData *ryostkl_profile_data_new(void);
RyostklProfileData *ryostkl_profile_data_dup(RyostklProfileData const *other);

void ryostkl_profile_data_set_modified(RyostklProfileData *profile_data);
gboolean ryostkl_profile_data_get_modified(RyostklProfileData const *profile_data);

gboolean ryostkl_profile_data_update_filesystem(RyostklProfileData *profile_data, guint profile_index, GError **error);
gboolean ryostkl_profile_data_update_hardware(RyostklProfileData *profile_data, RoccatDevice *device, guint profile_index, GError **error);

gboolean ryostkl_profile_data_save(RoccatDevice *device, RyostklProfileData *profile_data, guint profile_index, GError **error);

G_END_DECLS

#endif
