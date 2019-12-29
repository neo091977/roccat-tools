#ifndef __ROCCAT_NYTH_PROFILE_DATA_HARDWARE_H__
#define __ROCCAT_NYTH_PROFILE_DATA_HARDWARE_H__

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

#include "nyth_profile_buttons.h"
#include "nyth_profile_settings.h"
#include "nyth_button_set.h"
#include "nyth_macro.h"

G_BEGIN_DECLS

typedef struct _NythProfileDataHardware NythProfileDataHardware;

struct _NythProfileDataHardware {
	gboolean modified_profile_buttons;
	gboolean modified_profile_settings;
	gboolean modified_button_set;
	gboolean modified_macro[NYTH_PROFILE_BUTTON_NUM];

	NythProfileSettings profile_settings;
	NythProfileButtons profile_buttons;
	NythButtonSet button_set;
	NythMacro macros[NYTH_PROFILE_BUTTON_NUM];
};

gboolean nyth_profile_data_hardware_get_modified(NythProfileDataHardware const *profile_data);
void nyth_profile_data_hardware_set_modified(NythProfileDataHardware *profile_data);
void nyth_profile_data_hardware_set_unmodified(NythProfileDataHardware *profile_data);

void nyth_profile_data_hardware_set_profile_settings(NythProfileDataHardware *profile_data, NythProfileSettings const *profile_settings);
void nyth_profile_data_hardware_set_profile_buttons(NythProfileDataHardware *profile_data, NythProfileButtons const *profile_buttons);
void nyth_profile_data_hardware_set_macro(NythProfileDataHardware *profile_data, guint index, NythMacro const *macro);
void nyth_profile_data_hardware_set_button_set(NythProfileDataHardware *profile_data, NythButtonSet const *button_set);

void nyth_profile_data_hardware_update_with_default(NythProfileDataHardware *profile_data);
gboolean nyth_profile_data_hardware_update(NythProfileDataHardware *profile_data, RoccatDevice *device, guint profile_index, GError **error);

gboolean nyth_profile_data_hardware_save(RoccatDevice *device, NythProfileDataHardware *profile_data, guint profile_index, GError **error);

G_END_DECLS

#endif
