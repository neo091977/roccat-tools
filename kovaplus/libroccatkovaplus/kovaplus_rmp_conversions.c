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

#include "kovaplus_rmp.h"
#include "i18n-lib.h"

KovaplusProfileSettings *kovaplus_rmp_to_profile_settings(KovaplusRmp *rmp) {
	KovaplusProfileSettings *settings;

	settings = g_malloc0(sizeof(KovaplusProfileSettings));
	settings->report_id = KOVAPLUS_REPORT_ID_PROFILE_SETTINGS;
	settings->size = sizeof(KovaplusProfileSettings);
	settings->sensitivity_x = kovaplus_rmp_get_sensitivity_x(rmp);
	settings->sensitivity_y = kovaplus_rmp_get_sensitivity_y(rmp);
	settings->cpi_levels_enabled = kovaplus_rmp_get_cpi_all(rmp);
	settings->cpi_startup_level = kovaplus_rmp_get_cpi_x(rmp);
	settings->polling_rate = kovaplus_rmp_get_polling_rate(rmp);
	settings->light_switch = kovaplus_rmp_get_light_switch(rmp);
	settings->light_type = kovaplus_rmp_get_light_type(rmp);
	settings->orientation = kovaplus_rmp_get_orientation(rmp);

	switch(kovaplus_rmp_get_color_flag(rmp)) {
	case KOVAPLUS_RMP_COLOR_FLAG_SINGLE_COLOR:
		settings->color_option = kovaplus_rmp_get_color_options(rmp) + 1;
		settings->color_change_mode = KOVAPLUS_PROFILE_SETTINGS_COLOR_CHANGE_MODE_OFF;
		break;
	case KOVAPLUS_RMP_COLOR_FLAG_MULTICOLOR_CYCLING:
		settings->color_option = KOVAPLUS_PROFILE_SETTINGS_COLOR_OPTION_OFF;
		settings->color_change_mode = kovaplus_rmp_get_color_change_mode(rmp);
		break;
	default:
		g_error(_("Got unknown color flag %i"), kovaplus_rmp_get_color_flag(rmp));
	}

	return settings;
}

void kovaplus_rmp_update_with_profile_settings(KovaplusRmp *rmp, KovaplusProfileSettings const *profile_settings) {
	kovaplus_rmp_set_sensitivity_x(rmp, profile_settings->sensitivity_x);
	kovaplus_rmp_set_sensitivity_y(rmp, profile_settings->sensitivity_y);
	kovaplus_rmp_set_cpi_all(rmp, profile_settings->cpi_levels_enabled);
	kovaplus_rmp_set_cpi_x(rmp, profile_settings->cpi_startup_level);
	kovaplus_rmp_set_polling_rate(rmp, profile_settings->polling_rate);
	kovaplus_rmp_set_light_switch(rmp, profile_settings->light_switch);
	kovaplus_rmp_set_light_type(rmp, profile_settings->light_type);
	kovaplus_rmp_set_orientation(rmp, profile_settings->orientation);

	if (profile_settings->color_change_mode == KOVAPLUS_PROFILE_SETTINGS_COLOR_CHANGE_MODE_OFF) {
		kovaplus_rmp_set_color_flag(rmp, KOVAPLUS_RMP_COLOR_FLAG_SINGLE_COLOR);
		kovaplus_rmp_set_color_options(rmp, profile_settings->color_option - 1);
	} else {
		kovaplus_rmp_set_color_flag(rmp, KOVAPLUS_RMP_COLOR_FLAG_MULTICOLOR_CYCLING);
		kovaplus_rmp_set_color_change_mode(rmp, profile_settings->color_change_mode);
	}
}

KovaplusProfileButtons *kovaplus_rmp_to_profile_buttons(KovaplusRmp *rmp) {
	KovaplusProfileButtons *buttons;
	KovaplusRmpMacroKeyInfo *macro_key_info;
	guint i;

	buttons = g_malloc0(sizeof(KovaplusProfileButtons));
	buttons->report_id = KOVAPLUS_REPORT_ID_PROFILE_BUTTONS;
	buttons->size = sizeof(KovaplusProfileButtons);
	for (i = 0; i < KOVAPLUS_BUTTON_NUM; ++i) {
		macro_key_info = kovaplus_rmp_get_macro_key_info(rmp, i);
		buttons->button_type[i] = macro_key_info->type;
		g_free(macro_key_info);
	}
	return buttons;
}

void kovaplus_rmp_update_with_profile_buttons(KovaplusRmp *rmp, KovaplusProfileButtons const *profile_buttons) {
	KovaplusRmpMacroKeyInfo macro_key_info;
	KovaplusRmpMacroKeyInfo *actual_key_info;
	guint i;

	for (i = 0; i < KOVAPLUS_BUTTON_NUM; ++i) {
		switch (profile_buttons->button_type[i]) {
		case KOVAPLUS_PROFILE_BUTTON_TYPE_SHORTCUT:
		case KOVAPLUS_PROFILE_BUTTON_TYPE_QUICKLAUNCH:
		case KOVAPLUS_PROFILE_BUTTON_TYPE_MACRO:
		case KOVAPLUS_PROFILE_BUTTON_TYPE_TIMER:
			actual_key_info = kovaplus_rmp_get_macro_key_info(rmp, i);
			if (profile_buttons->button_type[i] != actual_key_info->type) {
				g_warning(_("Profile button %i differs from rmp with missing data, so rmp takes precedence"), i);
				rmp->modified_profile_buttons = TRUE; /* Hardware should be updated */
			}
			g_free(actual_key_info);
			break;
		default:
			macro_key_info.type = profile_buttons->button_type[i];
			kovaplus_rmp_set_macro_key_info(rmp, i, &macro_key_info);
			break;
		}
	}
}
