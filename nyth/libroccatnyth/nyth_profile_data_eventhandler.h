#ifndef __ROCCAT_NYTH_PROFILE_DATA_EVENTHANDLER_H__
#define __ROCCAT_NYTH_PROFILE_DATA_EVENTHANDLER_H__

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

#include "roccat_timer.h"
#include "nyth.h"
#include "roccat_swarm_rmp.h"

G_BEGIN_DECLS

typedef struct _NythProfileDataEventhandler NythProfileDataEventhandler;

struct _NythProfileDataEventhandler {
	gboolean modified;
	gchar profile_name[ROCCAT_SWARM_RMP_PROFILE_NAME_LENGTH + 1];
	guint8 timer_notification_type;
	guint8 profile_notification_type;
	guint8 sensitivity_notification_type;
	guint8 cpi_notification_type;
	gdouble notification_volume;

	RoccatSwarmGamefile gamefile_names[ROCCAT_SWARM_RMP_GAMEFILE_NUM];
	RoccatTimer timers[NYTH_PROFILE_BUTTON_NUM];
	RoccatSwarmOpener openers[NYTH_PROFILE_BUTTON_NUM];
	guint16 talk_targets[NYTH_PROFILE_BUTTON_NUM];
};

gboolean nyth_profile_data_eventhandler_get_modified(NythProfileDataEventhandler const *profile_data);
void nyth_profile_data_eventhandler_set_modified(NythProfileDataEventhandler *profile_data);
void nyth_profile_data_eventhandler_set_unmodified(NythProfileDataEventhandler *profile_data);

void nyth_profile_data_eventhandler_set_profile_name(NythProfileDataEventhandler *profile_data, gchar const *new_name);
void nyth_profile_data_eventhandler_set_gamefile_name(NythProfileDataEventhandler *profile_data, guint index, gchar const *new_name);
void nyth_profile_data_eventhandler_set_timer(NythProfileDataEventhandler *profile_data, guint index, RoccatTimer const *timer);
void nyth_profile_data_eventhandler_set_opener(NythProfileDataEventhandler *profile_data, guint index, gchar const *new_opener);
void nyth_profile_data_eventhandler_set_talk_target(NythProfileDataEventhandler *profile_data, guint index, guint new_target);

void nyth_profile_data_eventhandler_update_with_default(NythProfileDataEventhandler *profile_data);
gboolean nyth_profile_data_eventhandler_update(NythProfileDataEventhandler *profile_data, guint profile_index, GError **error);

gboolean nyth_profile_data_eventhandler_save(NythProfileDataEventhandler *profile_data, guint profile_index, GError **error);

G_END_DECLS

#endif
