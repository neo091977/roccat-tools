#ifndef __ROCCAT_KOVA2016_PROFILE_DATA_HARDWARE_H__
#define __ROCCAT_KOVA2016_PROFILE_DATA_HARDWARE_H__

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

#include "kova2016_profile_buttons.h"
#include "kova2016_profile_settings.h"
#include "kova2016_macro.h"

G_BEGIN_DECLS

typedef struct _Kova2016ProfileDataHardware Kova2016ProfileDataHardware;

struct _Kova2016ProfileDataHardware {
	gboolean modified_profile_buttons;
	gboolean modified_profile_settings;
	gboolean modified_macro[KOVA2016_PROFILE_BUTTON_NUM];

	Kova2016ProfileSettings profile_settings;
	Kova2016ProfileButtons profile_buttons;
	Kova2016Macro macros[KOVA2016_PROFILE_BUTTON_NUM];
};

gboolean kova2016_profile_data_hardware_get_modified(Kova2016ProfileDataHardware const *profile_data);
void kova2016_profile_data_hardware_set_modified(Kova2016ProfileDataHardware *profile_data);
void kova2016_profile_data_hardware_set_unmodified(Kova2016ProfileDataHardware *profile_data);

void kova2016_profile_data_hardware_set_profile_settings(Kova2016ProfileDataHardware *profile_data, Kova2016ProfileSettings const *profile_settings);
void kova2016_profile_data_hardware_set_profile_buttons(Kova2016ProfileDataHardware *profile_data, Kova2016ProfileButtons const *profile_buttons);
void kova2016_profile_data_hardware_set_macro(Kova2016ProfileDataHardware *profile_data, guint index, Kova2016Macro const *macro);

void kova2016_profile_data_hardware_update_with_default(Kova2016ProfileDataHardware *profile_data);
gboolean kova2016_profile_data_hardware_update(Kova2016ProfileDataHardware *profile_data, RoccatDevice *device, guint profile_index, GError **error);

gboolean kova2016_profile_data_hardware_save(RoccatDevice *device, Kova2016ProfileDataHardware *profile_data, guint profile_index, GError **error);

G_END_DECLS

#endif
