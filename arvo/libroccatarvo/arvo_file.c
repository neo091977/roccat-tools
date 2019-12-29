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

#include "arvo.h"
//#include <stdlib.h>

guint arvo_actual_profile_read(RoccatDevice *arvo, GError **error) {
	ArvoActualProfile *profile;
	guint number;

	profile = (ArvoActualProfile *)arvo_device_read(arvo, ARVO_REPORT_ID_STARTUP_PROFILE, sizeof(ArvoActualProfile), error);
	if (*error) return 0;
	number = profile->profile_number;
	g_free(profile);
	return number;
}

gboolean arvo_actual_profile_write(RoccatDevice *arvo, guint profile_number, GError **error) {
	ArvoActualProfile profile;

	g_assert(profile_number >= 1 && profile_number <= ARVO_PROFILE_NUM);
	profile.report_id = ARVO_REPORT_ID_STARTUP_PROFILE;
	profile.profile_number = profile_number;
	return arvo_device_write(arvo, (gchar const *)&profile, sizeof(ArvoActualProfile), error);
}

guint arvo_mode_key_read(RoccatDevice *arvo, GError **error) {
	ArvoModeKey *mode_key;
	guint state;

	mode_key = (ArvoModeKey *)arvo_device_read(arvo, ARVO_REPORT_ID_MODE_KEY, sizeof(ArvoModeKey), error);
	if (*error) return 0;
	state = mode_key->state;
	g_free(mode_key);
	return state;
}

gboolean arvo_mode_key_write(RoccatDevice *arvo, guint state, GError **error) {
	ArvoModeKey mode_key;

	g_assert(state == ARVO_MODE_KEY_STATE_ON || state == ARVO_MODE_KEY_STATE_OFF);
	mode_key.report_id = ARVO_REPORT_ID_MODE_KEY;
	mode_key.state = state;
	return arvo_device_write(arvo, (gchar const *)&mode_key, sizeof(ArvoModeKey), error);
}

guint arvo_key_mask_read(RoccatDevice *arvo, GError **error) {
	ArvoKeyMask *key_mask;
	guint mask;

	key_mask = (ArvoKeyMask *)arvo_device_read(arvo, ARVO_REPORT_ID_KEY_MASK, sizeof(ArvoKeyMask), error);
	if (*error) return 0;
	mask = key_mask->key_mask;
	g_free(key_mask);
	return mask;
}

gboolean arvo_key_mask_write(RoccatDevice *arvo, guint mask, GError **error) {
	ArvoKeyMask key_mask;

	g_assert((mask & 0xe0) == 0);
	key_mask.report_id = ARVO_REPORT_ID_KEY_MASK;
	key_mask.key_mask = mask;
	return arvo_device_write(arvo, (gchar const *)&key_mask, sizeof(ArvoKeyMask), error);
}

gboolean arvo_profile_activate(RoccatDevice *device, guint profile_number, ArvoRkp *rkp, GError **error) {
	guint mode_key, key_mask;

	mode_key = arvo_rkp_get_mode_key(rkp);
	key_mask = arvo_rkp_get_key_mask(rkp);

	arvo_actual_profile_write(device, profile_number, error);
	if (*error)
		return FALSE;
	arvo_mode_key_write(device, mode_key, error);
	if (*error)
		return FALSE;
	arvo_key_mask_write(device, key_mask, error);
	if (*error)
		return FALSE;
	return TRUE;
}
