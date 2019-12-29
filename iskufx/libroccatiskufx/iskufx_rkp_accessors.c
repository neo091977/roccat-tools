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

#include "isku_rkp.h"
#include "iskufx.h"
#include "g_roccat_helper.h"
#include "i18n-lib.h"

static gchar const * const iskufx_rkp_group_name = "Setting";

static gchar const * const iskufx_rkp_light_switch_name = "LtSwitch";
static gchar const * const iskufx_rkp_light_breath_switch_name = "LtBreathSwitch";
static gchar const * const iskufx_rkp_light_color_type_name = "LtColorType";
static gchar const * const iskufx_rkp_light_info_name = "FixedLtInfo";
static gchar const * const iskufx_rkp_talkfx_name = "HDDKeyboard";
/* TODO What is CCLtInfo? */

void iskufx_rkp_set_light_switch(IskuRkp *rkp, guint value) {
	g_assert(value >= ISKUFX_LIGHT_STATE_OFF && value <= ISKUFX_LIGHT_STATE_ON);
	if (iskufx_rkp_get_light_switch(rkp) != value) {
		isku_rkp_set_value(rkp, iskufx_rkp_light_switch_name, value);
		rkp->modified_light = TRUE;
	}
}

guint iskufx_rkp_get_light_switch(IskuRkp *rkp) {
	return isku_rkp_get_value(rkp, iskufx_rkp_light_switch_name);
}

void iskufx_rkp_set_light_breath_switch(IskuRkp *rkp, guint value) {
	g_assert(value >= ISKUFX_LIGHT_BREATHING_OFF && value <= ISKUFX_LIGHT_BREATHING_ON);
	if (iskufx_rkp_get_light_breath_switch(rkp) != value) {
		isku_rkp_set_value(rkp, iskufx_rkp_light_breath_switch_name, value);
		rkp->modified_light = TRUE;
	}
}

guint iskufx_rkp_get_light_breath_switch(IskuRkp *rkp) {
	return isku_rkp_get_value(rkp, iskufx_rkp_light_breath_switch_name);
}

void iskufx_rkp_set_light_color_type(IskuRkp *rkp, guint value) {
	g_assert(value >= ISKUFX_LIGHT_COLOR_TYPE_SINGLE && value <= ISKUFX_LIGHT_COLOR_TYPE_FLOW);
	if (iskufx_rkp_get_light_color_type(rkp) != value) {
		isku_rkp_set_value(rkp, iskufx_rkp_light_color_type_name, value);
		rkp->modified_light = TRUE;
	}
}

guint iskufx_rkp_get_light_color_type(IskuRkp *rkp) {
	return isku_rkp_get_value(rkp, iskufx_rkp_light_color_type_name);
}

IskufxRkpLightInfo *iskufx_rkp_get_rkp_light_info(IskuRkp *rkp) {
	IskuRkp const *default_rkp;
	GError *error = NULL;
	IskufxRkpLightInfo *rkp_light_info;

	rkp_light_info = roccat_key_file_get_binary(rkp->key_file, iskufx_rkp_group_name, iskufx_rkp_light_info_name, sizeof(IskufxRkpLightInfo), &error);
	if (error) {
		g_clear_error(&error);
		default_rkp = iskufx_default_rkp();
		rkp_light_info = roccat_key_file_get_binary(default_rkp->key_file, iskufx_rkp_group_name, iskufx_rkp_light_info_name, sizeof(IskufxRkpLightInfo), &error);
		if (error)
			g_error(_("Could not get default value for key '%s': %s"), iskufx_rkp_light_info_name, error->message);
	}
	return rkp_light_info;
}

void iskufx_rkp_set_rkp_light_info(IskuRkp *rkp, IskufxRkpLightInfo *rkp_light_info) {
	IskufxRkpLightInfo *actual_light_info;

	actual_light_info = iskufx_rkp_get_rkp_light_info(rkp);
	if (!iskufx_rkp_light_info_equal(rkp_light_info, actual_light_info)) {
		iskufx_rkp_light_info_set_checksum(rkp_light_info);
		roccat_key_file_set_binary(rkp->key_file, iskufx_rkp_group_name, iskufx_rkp_light_info_name, (gconstpointer)rkp_light_info, sizeof(IskufxRkpLightInfo));
		rkp->modified_light = TRUE;
	}

	g_free(actual_light_info);
}

guint iskufx_rkp_get_talkfx(IskuRkp *rkp) {
	return isku_rkp_get_value(rkp, iskufx_rkp_talkfx_name);
}

void iskufx_rkp_set_talkfx(IskuRkp *rkp, guint value) {
	if (iskufx_rkp_get_talkfx(rkp) != value) {
		isku_rkp_set_value(rkp, iskufx_rkp_talkfx_name, value);
		rkp->modified_light = TRUE;
	}
}
