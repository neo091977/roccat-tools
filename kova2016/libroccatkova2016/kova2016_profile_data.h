#ifndef __ROCCAT_KOVA2016_PROFILE_DATA_H__
#define __ROCCAT_KOVA2016_PROFILE_DATA_H__

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

#include "kova2016_profile_data_eventhandler.h"
#include "kova2016_profile_data_hardware.h"

G_BEGIN_DECLS

typedef struct _Kova2016ProfileData Kova2016ProfileData;

struct _Kova2016ProfileData {
	Kova2016ProfileDataEventhandler eventhandler;
	Kova2016ProfileDataHardware hardware;
};

Kova2016ProfileData *kova2016_profile_data_new(void);
Kova2016ProfileData *kova2016_profile_data_dup(Kova2016ProfileData const *other);

void kova2016_profile_data_set_modified(Kova2016ProfileData *profile_data);
gboolean kova2016_profile_data_get_modified(Kova2016ProfileData const *profile_data);

gboolean kova2016_profile_data_update_filesystem(Kova2016ProfileData *profile_data, guint profile_index, GError **error);
gboolean kova2016_profile_data_update_hardware(Kova2016ProfileData *profile_data, RoccatDevice *device, guint profile_index, GError **error);

gboolean kova2016_profile_data_save(RoccatDevice *device, Kova2016ProfileData *profile_data, guint profile_index, GError **error);

G_END_DECLS

#endif
