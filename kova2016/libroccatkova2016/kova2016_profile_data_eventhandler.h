#ifndef __ROCCAT_KOVA2016_PROFILE_DATA_EVENTHANDLER_H__
#define __ROCCAT_KOVA2016_PROFILE_DATA_EVENTHANDLER_H__

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

#include "kova2016.h"
#include "roccat_timer.h"
#include "roccat_swarm_rmp.h"

G_BEGIN_DECLS

typedef struct _Kova2016ProfileDataEventhandler Kova2016ProfileDataEventhandler;

struct _Kova2016ProfileDataEventhandler {
	gboolean modified;
	gchar profile_name[ROCCAT_SWARM_RMP_PROFILE_NAME_LENGTH + 1];
	guint8 timer_notification_type;
	guint8 profile_notification_type;
	guint8 cpi_notification_type;
	gdouble notification_volume;

	RoccatSwarmGamefile gamefile_names[ROCCAT_SWARM_RMP_GAMEFILE_NUM];
	RoccatTimer timers[KOVA2016_PROFILE_BUTTON_NUM];
	RoccatSwarmOpener openers[KOVA2016_PROFILE_BUTTON_NUM];
	guint16 talk_targets[KOVA2016_PROFILE_BUTTON_NUM];
};

gboolean kova2016_profile_data_eventhandler_get_modified(Kova2016ProfileDataEventhandler const *profile_data);
void kova2016_profile_data_eventhandler_set_modified(Kova2016ProfileDataEventhandler *profile_data);
void kova2016_profile_data_eventhandler_set_unmodified(Kova2016ProfileDataEventhandler *profile_data);

void kova2016_profile_data_eventhandler_set_profile_name(Kova2016ProfileDataEventhandler *profile_data, gchar const *new_name);
void kova2016_profile_data_eventhandler_set_gamefile_name(Kova2016ProfileDataEventhandler *profile_data, guint index, gchar const *new_name);
void kova2016_profile_data_eventhandler_set_timer(Kova2016ProfileDataEventhandler *profile_data, guint index, RoccatTimer const *timer);
void kova2016_profile_data_eventhandler_set_opener(Kova2016ProfileDataEventhandler *profile_data, guint index, gchar const *new_opener);
void kova2016_profile_data_eventhandler_set_talk_target(Kova2016ProfileDataEventhandler *profile_data, guint index, guint new_target);

void kova2016_profile_data_eventhandler_update_with_default(Kova2016ProfileDataEventhandler *profile_data);
gboolean kova2016_profile_data_eventhandler_update(Kova2016ProfileDataEventhandler *profile_data, guint profile_index, GError **error);

gboolean kova2016_profile_data_eventhandler_save(Kova2016ProfileDataEventhandler *profile_data, guint profile_index, GError **error);

G_END_DECLS

#endif
