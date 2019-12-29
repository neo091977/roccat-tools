#ifndef __ROCCAT_RYOSTKL_PROFILE_DATA_EVENTHANDLER_H__
#define __ROCCAT_RYOSTKL_PROFILE_DATA_EVENTHANDLER_H__

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

#include "ryos_rkp_talk.h"
#include "ryos_rkp_timer.h"
#include "ryos_rkp_quicklaunch.h"
#include "ryos_led_macro.h"
#include "roccat_swarm_rmp.h"

G_BEGIN_DECLS

typedef struct _RyostklProfileDataEventhandler RyostklProfileDataEventhandler;

/* Data used by eventhandler not stored in device.
 * One indicator for all information. Eventhandler has to be notified only if
 * this information changes.
 */
struct _RyostklProfileDataEventhandler {
	/* Has to be set manually. No difference checks needed. */
	guint8 modified;

	gchar profile_name[ROCCAT_SWARM_RMP_PROFILE_NAME_LENGTH + 1];
	guint8 timer_notification_type;
	guint8 profile_notification_type;
	guint8 live_recording_notification_type;
	gdouble notification_volume;
	gboolean led_volume_indicator;

	RyosRkpTalk talks[RYOS_RKP_TALK_NUM];
	RoccatSwarmGamefile gamefile_names[ROCCAT_SWARM_RMP_GAMEFILE_NUM];
	RyosRkpTimer timers[RYOS_RKP_KEYS_NUM];
	RyosRkpQuicklaunch launchers[RYOS_RKP_KEYS_NUM];
	RyosLedMacro led_macros[RYOS_RKP_KEYS_NUM];
	gchar effect_script[RYOS_EFFECT_SCRIPT_NAME_LENGTH];
} __attribute__ ((packed));

void ryostkl_profile_data_eventhandler_set_modified(RyostklProfileDataEventhandler *profile_data);
void ryostkl_profile_data_eventhandler_set_unmodified(RyostklProfileDataEventhandler *profile_data);

void ryostkl_profile_data_eventhandler_set_profile_name(RyostklProfileDataEventhandler *profile_data, gchar const *new_name);
void ryostkl_profile_data_eventhandler_set_gamefile_name(RyostklProfileDataEventhandler *profile_data, guint index, gchar const *new_name);
void ryostkl_profile_data_eventhandler_set_timer(RyostklProfileDataEventhandler *profile_data, guint index, RyosRkpTimer const *timer);
void ryostkl_profile_data_eventhandler_set_quicklaunch(RyostklProfileDataEventhandler *profile_data, guint index, RyosRkpQuicklaunch const *launcher);
void ryostkl_profile_data_eventhandler_set_talk(RyostklProfileDataEventhandler *profile_data, guint index, RyosRkpTalk const *talk);
void ryostkl_profile_data_eventhandler_set_led_macro(RyostklProfileDataEventhandler *profile_data, guint index, RyosLedMacro const *led_macro);
void ryostkl_profile_data_eventhandler_set_effect_script_name(RyostklProfileDataEventhandler *profile_data, gchar const *new_name);

void ryostkl_profile_data_eventhandler_update_with_default(RyostklProfileDataEventhandler *profile_data);
gboolean ryostkl_profile_data_eventhandler_update(RyostklProfileDataEventhandler *profile_data, guint profile_index, GError **error);

gboolean ryostkl_profile_data_eventhandler_save(RyostklProfileDataEventhandler *profile_data, guint profile_index, GError **error);

G_END_DECLS

#endif
