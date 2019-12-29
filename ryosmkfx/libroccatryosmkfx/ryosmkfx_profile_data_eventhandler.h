#ifndef __ROCCAT_RYOSMKFX_PROFILE_DATA_EVENTHANDLER_H__
#define __ROCCAT_RYOSMKFX_PROFILE_DATA_EVENTHANDLER_H__

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

#include "ryos.h"
#include "ryos_led_macro.h"
#include "roccat_timer.h"
#include "roccat_swarm_rmp.h"

G_BEGIN_DECLS

typedef struct _RyosmkfxProfileDataEventhandler RyosmkfxProfileDataEventhandler;

struct _RyosmkfxProfileDataEventhandler {
	gboolean modified;
	gchar profile_name[ROCCAT_SWARM_RMP_PROFILE_NAME_LENGTH + 1];
	guint8 timer_notification_type;
	guint8 profile_notification_type;
	guint8 live_recording_notification_type;
	gdouble notification_volume;

	RoccatSwarmGamefile gamefile_names[ROCCAT_SWARM_RMP_GAMEFILE_NUM];
	RoccatTimer timers[RYOS_RKP_KEYS_NUM];
	RoccatSwarmOpener openers[RYOS_RKP_KEYS_NUM];
	guint16 talk_targets[RYOS_RKP_KEYS_NUM];
	RyosLedMacro led_macros[RYOS_RKP_KEYS_NUM];
	gchar effect_script[RYOS_EFFECT_SCRIPT_NAME_LENGTH];
};

gboolean ryosmkfx_profile_data_eventhandler_get_modified(RyosmkfxProfileDataEventhandler const *profile_data);
void ryosmkfx_profile_data_eventhandler_set_modified(RyosmkfxProfileDataEventhandler *profile_data);
void ryosmkfx_profile_data_eventhandler_set_unmodified(RyosmkfxProfileDataEventhandler *profile_data);

void ryosmkfx_profile_data_eventhandler_set_profile_name(RyosmkfxProfileDataEventhandler *profile_data, gchar const *new_name);
void ryosmkfx_profile_data_eventhandler_set_gamefile_name(RyosmkfxProfileDataEventhandler *profile_data, guint index, gchar const *new_name);
void ryosmkfx_profile_data_eventhandler_set_timer(RyosmkfxProfileDataEventhandler *profile_data, guint index, RoccatTimer const *timer);
void ryosmkfx_profile_data_eventhandler_set_opener(RyosmkfxProfileDataEventhandler *profile_data, guint index, gchar const *new_opener);
void ryosmkfx_profile_data_eventhandler_set_talk_target(RyosmkfxProfileDataEventhandler *profile_data, guint index, guint16 new_target);
void ryosmkfx_profile_data_eventhandler_set_led_macro(RyosmkfxProfileDataEventhandler *profile_data, guint index, RyosLedMacro *led_macro);
void ryosmkfx_profile_data_eventhandler_set_effect_script_name(RyosmkfxProfileDataEventhandler *profile_data, gchar const *new_name);

void ryosmkfx_profile_data_eventhandler_update_with_default(RyosmkfxProfileDataEventhandler *profile_data);
gboolean ryosmkfx_profile_data_eventhandler_update(RyosmkfxProfileDataEventhandler *profile_data, guint profile_index, GError **error);

gboolean ryosmkfx_profile_data_eventhandler_save(RyosmkfxProfileDataEventhandler *profile_data, guint profile_index, GError **error);

G_END_DECLS

#endif
