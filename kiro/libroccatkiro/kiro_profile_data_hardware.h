#ifndef __ROCCAT_KIRO_PROFILE_DATA_HARDWARE_H__
#define __ROCCAT_KIRO_PROFILE_DATA_HARDWARE_H__

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

#include "kiro_profile.h"

G_BEGIN_DECLS

typedef struct _KiroProfileDataHardware KiroProfileDataHardware;

struct _KiroProfileDataHardware {
	gboolean modified_profile;
	KiroProfile profile;
	gboolean modified_dcu;
	guint8 dcu;
};

gboolean kiro_profile_data_hardware_get_modified(KiroProfileDataHardware const *profile_data);
void kiro_profile_data_hardware_set_modified(KiroProfileDataHardware *profile_data);
void kiro_profile_data_hardware_set_unmodified(KiroProfileDataHardware *profile_data);

void kiro_profile_data_hardware_set_profile(KiroProfileDataHardware *profile_data, KiroProfile const *profile);
void kiro_profile_data_hardware_set_dcu(KiroProfileDataHardware *profile_data, guint8 new_dcu);

void kiro_profile_data_hardware_update_with_default(KiroProfileDataHardware *profile_data);
gboolean kiro_profile_data_hardware_update(KiroProfileDataHardware *profile_data, RoccatDevice *device, GError **error);

gboolean kiro_profile_data_hardware_save(RoccatDevice *device, KiroProfileDataHardware *profile_data, GError **error);

G_END_DECLS

#endif
