/*
 * This file is part of roccat-tools.
 *
 * roccat-tools is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Buttons Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * roccat-tools is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Buttons Public License for more details.
 *
 * You should have received a copy of the GNU Buttons Public License
 * along with roccat-tools. If not, see <http://www.gnu.org/licenses/>.
 */

#include "savu.h"
#include "roccat_helper.h"
#include <string.h>
#include <stdlib.h>

guint16 savu_buttons_calc_checksum(SavuButtons const *buttons) {
	return ROCCAT_BYTESUM_PARTIALLY(buttons, SavuButtons, report_id, checksum);
}

static void savu_buttons_finalize(SavuButtons *buttons, guint profile_index) {
	buttons->report_id = SAVU_REPORT_ID_BUTTONS;
	buttons->size = sizeof(SavuButtons);
	buttons->profile_index = profile_index;
	savu_buttons_set_checksum(buttons, savu_buttons_calc_checksum(buttons));
}

SavuButtons *savu_buttons_read(RoccatDevice *savu, guint profile_index, GError **error) {
	SavuButtons *buttons;

	g_assert(profile_index < SAVU_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(savu));

	if (!savu_select(savu, profile_index, SAVU_CONTROL_REQUEST_BUTTONS, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(savu));
		return NULL;
	}

	buttons = (SavuButtons *)savu_device_read(savu, SAVU_REPORT_ID_BUTTONS, sizeof(SavuButtons), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(savu));

	return buttons;
}

gboolean savu_buttons_write(RoccatDevice *savu, guint profile_index, SavuButtons *buttons, GError **error) {
	g_assert(profile_index < SAVU_PROFILE_NUM);
	savu_buttons_finalize(buttons, profile_index);
	return savu_device_write(savu, (gchar const *)buttons, sizeof(SavuButtons), error);
}

SavuButtons *savu_rmp_to_buttons(SavuRmp *rmp) {
	SavuButtons *buttons;
	RoccatButton *button;
	guint i;

	buttons = (SavuButtons *)g_malloc0(sizeof(SavuButtons));

	for (i = 0; i < SAVU_BUTTON_NUM; ++i) {
		button = savu_rmp_get_button(rmp, i);
		buttons->buttons[i] = *button;
		g_free(button);
	}

	return buttons;
}

void savu_rmp_update_with_buttons(SavuRmp *rmp, SavuButtons *buttons) {
	guint i;

	for (i = 0; i < SAVU_BUTTON_NUM; ++i)
		savu_rmp_set_button(rmp, i, &buttons->buttons[i]);
}
