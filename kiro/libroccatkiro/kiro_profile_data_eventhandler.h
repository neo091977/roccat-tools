#ifndef __ROCCAT_KIRO_PROFILE_DATA_EVENTHANDLER_H__
#define __ROCCAT_KIRO_PROFILE_DATA_EVENTHANDLER_H__

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

#include "kiro.h"
#include "roccat_swarm_rmp.h"

G_BEGIN_DECLS

typedef struct _KiroProfileDataEventhandler KiroProfileDataEventhandler;

struct _KiroProfileDataEventhandler {
	gboolean modified;
	gchar profile_name[ROCCAT_SWARM_RMP_PROFILE_NAME_LENGTH + 1];
	guint8 sensitivity_notification_type;
	guint8 cpi_notification_type;
	gdouble notification_volume;

	RoccatSwarmOpener openers[KIRO_BUTTON_NUM];
};

gboolean kiro_profile_data_eventhandler_get_modified(KiroProfileDataEventhandler const *profile_data);
void kiro_profile_data_eventhandler_set_modified(KiroProfileDataEventhandler *profile_data);
void kiro_profile_data_eventhandler_set_unmodified(KiroProfileDataEventhandler *profile_data);

void kiro_profile_data_eventhandler_set_profile_name(KiroProfileDataEventhandler *profile_data, gchar const *new_name);
void kiro_profile_data_eventhandler_set_opener(KiroProfileDataEventhandler *profile_data, guint index, gchar const *new_opener);

void kiro_profile_data_eventhandler_update_with_default(KiroProfileDataEventhandler *profile_data);
gboolean kiro_profile_data_eventhandler_update(KiroProfileDataEventhandler *profile_data, GError **error);

gboolean kiro_profile_data_eventhandler_save(KiroProfileDataEventhandler *profile_data, GError **error);

G_END_DECLS

#endif
