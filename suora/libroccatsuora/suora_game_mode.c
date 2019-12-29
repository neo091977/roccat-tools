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

#include "suora_game_mode.h"
#include "suora_device.h"

gint suora_game_mode_read(RoccatDevice *device, GError **error) {
	SuoraFeaturePayload out = {0};
	SuoraGameMode *in;
	gint state;

	in = (SuoraGameMode *)suora_read(device, SUORA_FEATURE_REQUEST_GAME_MODE, &out, error);
	if (!in)
		return -1;

	state = in->state;
	g_free(in);
	return state;
}

gboolean suora_game_mode_write(RoccatDevice *device, guint state, GError **error) {
	SuoraGameMode out = {0};
	out.state = state;
	return suora_write(device, SUORA_FEATURE_REQUEST_GAME_MODE, (SuoraFeaturePayload *)&out, error);
}
