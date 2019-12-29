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

#include "konepure.h"
#include "koneplus_rmp.h"
#include "roccat_helper.h"
#include "roccat.h"
#include "i18n-lib.h"

KonepureProfileSettings *konepure_rmp_to_profile_settings(KoneplusRmp *rmp) {
	KonepureProfileSettings *settings;
	guint i;
	KoneplusRmpLightInfo *rmp_light_info;

	settings = g_malloc0(sizeof(KonepureProfileSettings));
	settings->report_id = KONEPLUS_REPORT_ID_PROFILE_SETTINGS;
	settings->size = sizeof(KonepureProfileSettings);

	settings->xy_sync = koneplus_rmp_get_xy_synchronous(rmp);
	if (settings->xy_sync == ROCCAT_SENSITIVITY_ADVANCED_ON) {
		settings->sensitivity_x = koneplus_rmp_get_sensitivity_x(rmp);
		settings->sensitivity_y = koneplus_rmp_get_sensitivity_y(rmp);
	} else {
		settings->sensitivity_x = koneplus_rmp_get_sensitivity(rmp);
		settings->sensitivity_y = settings->sensitivity_x;
	}

	for (i = 0; i < KONEPLUS_PROFILE_SETTING_CPI_LEVELS_NUM; ++i) {
		settings->cpi_levels_x[i] = konextd_rmp_cpi_to_bin(koneplus_rmp_get_cpi_level_x(rmp, i));
		settings->cpi_levels_y[i] = konextd_rmp_cpi_to_bin(koneplus_rmp_get_cpi_level_y(rmp, i));
	}
	settings->cpi_levels_enabled = koneplus_rmp_get_cpi_all(rmp);
	settings->cpi_startup_level = koneplus_rmp_get_cpi_x(rmp);

	settings->polling_rate = koneplus_rmp_get_polling_rate(rmp);

	settings->color_flow_effect = koneplus_rmp_get_light_color_flow(rmp);
	settings->light_effect_type = koneplus_rmp_get_light_effect_type(rmp);
	settings->light_effect_speed = koneplus_rmp_get_light_effect_speed(rmp);
	settings->light_chose_type = konextd_rmp_get_light_chose_type(rmp);

	rmp_light_info = koneplus_rmp_get_rmp_light_info(rmp, 0);
	roccat_set_bit8(&settings->light_enabled, 0, rmp_light_info->state == KONEPLUS_RMP_LIGHT_INFO_STATE_ON);
	if (settings->light_chose_type == KONEXTD_RMP_LIGHT_CHOSE_TYPE_TABLE) {
		koneplus_rmp_light_info_to_light_info(rmp_light_info, &settings->light, FALSE);
	} else {
		g_free(rmp_light_info);
		rmp_light_info = konextd_rmp_get_custom_light_info(rmp, 0);
		koneplus_rmp_light_info_to_light_info(rmp_light_info, &settings->light, TRUE);
	}
	g_free(rmp_light_info);

	return settings;
}

void konepure_rmp_update_with_profile_settings(KoneplusRmp *rmp, KonepureProfileSettings const *profile_settings) {
	KoneplusRmpLightInfo rmp_light_info;
	KoneplusRmpLightInfo* rmp_light_info_pointer;
	guint i;

	koneplus_rmp_set_xy_synchronous(rmp, profile_settings->xy_sync);
	if (profile_settings->xy_sync == ROCCAT_SENSITIVITY_ADVANCED_ON) {
		koneplus_rmp_set_sensitivity_x(rmp, profile_settings->sensitivity_x);
		koneplus_rmp_set_sensitivity_y(rmp, profile_settings->sensitivity_y);
	} else {
		koneplus_rmp_set_sensitivity(rmp, profile_settings->sensitivity_x);
	}

	for (i = 0; i < KONEPLUS_PROFILE_SETTING_CPI_LEVELS_NUM; ++i) {
		koneplus_rmp_set_cpi_level_x(rmp, i, konextd_bin_cpi_to_rmp(profile_settings->cpi_levels_x[i]));
		koneplus_rmp_set_cpi_level_y(rmp, i, konextd_bin_cpi_to_rmp(profile_settings->cpi_levels_y[i]));
	}
	koneplus_rmp_set_cpi_all(rmp, profile_settings->cpi_levels_enabled);
	koneplus_rmp_set_cpi_x(rmp, profile_settings->cpi_startup_level);
	koneplus_rmp_set_cpi_y(rmp, profile_settings->cpi_startup_level);

	if (profile_settings->light_chose_type == KONEXTD_RMP_LIGHT_CHOSE_TYPE_CUSTOM) {
		koneplus_light_info_to_custom_rmp_light_info(&profile_settings->light, &rmp_light_info);
		konextd_rmp_set_custom_light_info(rmp, 0, &rmp_light_info);

		rmp_light_info_pointer = koneplus_rmp_get_rmp_light_info(rmp, 0);
		rmp_light_info_pointer->state = roccat_get_bit8(profile_settings->light_enabled, 0) ? KONEPLUS_RMP_LIGHT_INFO_STATE_ON : KONEPLUS_RMP_LIGHT_INFO_STATE_OFF;
		koneplus_rmp_set_rmp_light_info(rmp, 0, rmp_light_info_pointer);
		g_free(rmp_light_info_pointer);
	} else {
		koneplus_light_info_to_rmp_light_info(&profile_settings->light, &rmp_light_info, roccat_get_bit8(profile_settings->light_enabled, 0));
		koneplus_rmp_set_rmp_light_info(rmp, 0, &rmp_light_info);
	}

	koneplus_rmp_set_light_effect_type(rmp, profile_settings->light_effect_type);
	koneplus_rmp_set_light_effect_speed(rmp, profile_settings->light_effect_speed);
	koneplus_rmp_set_light_color_flow(rmp, profile_settings->color_flow_effect);
	konextd_rmp_set_light_chose_type(rmp, profile_settings->light_chose_type);

	koneplus_rmp_set_polling_rate(rmp, profile_settings->polling_rate);
}

gint konepure_profile_button_index_to_rmp_index(guint profile_button_index) {
	gint data[KONEPURE_PROFILE_BUTTON_NUM] = {
		KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_LEFT,
		KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_RIGHT,
		KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_MIDDLE,
		KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_FORWARD,
		KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_BACKWARD,
		KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_WHEEL_UP,
		KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_WHEEL_DOWN,
		KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_PLUS,
		KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_MINUS,
		KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_LEFT,
		KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_RIGHT,
		KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_MIDDLE,
		KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_FORWARD,
		KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_BACKWARD,
		KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_WHEEL_UP,
		KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_WHEEL_DOWN,
		KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_PLUS,
		KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_MINUS,
	};

	if (profile_button_index >= KONEPURE_PROFILE_BUTTON_NUM)
		return -1;
	else
		return data[profile_button_index];
}

static void konepure_rmp_update_with_profile_button(KoneplusRmp *rmp,
		guint profile_index, KonepureProfileButtons const *profile_buttons) {
	KoneplusRmpMacroKeyInfo *macro_key_info;
	KoneplusRmpMacroKeyInfo *actual_key_info;
	RoccatButton const *button;
	gint rmp_index;

	rmp_index = konepure_profile_button_index_to_rmp_index(profile_index);
	if (rmp_index < 0)
		return;

	button = &profile_buttons->buttons[profile_index];

	switch (button->type) {
	case KONEPLUS_PROFILE_BUTTON_TYPE_MACRO: /* doing nothing, macro is updated extra */
		macro_key_info = NULL;
		break;
	case KONEPLUS_PROFILE_BUTTON_TYPE_QUICKLAUNCH:
	case KONEPLUS_PROFILE_BUTTON_TYPE_TIMER:
	case KONEPLUS_PROFILE_BUTTON_TYPE_TALK_OTHER_EASYSHIFT:
	case KONEPLUS_PROFILE_BUTTON_TYPE_TALK_OTHER_EASYSHIFT_LOCK:
		actual_key_info = koneplus_rmp_get_macro_key_info(rmp, rmp_index);
		if (button->type != actual_key_info->type) {
			g_warning(_("Profile button %i differs from rmp with missing data, so rmp takes precedence"), rmp_index);
			rmp->modified_macros[rmp_index] = TRUE; /* Hardware should be updated */
		}
		koneplus_rmp_macro_key_info_free(actual_key_info);
		macro_key_info = NULL;
		break;
	case KONEPLUS_PROFILE_BUTTON_TYPE_SHORTCUT:
		macro_key_info = koneplus_rmp_macro_key_info_new_shortcut(button->key, button->modifier);
		break;
	default:
		macro_key_info = koneplus_rmp_macro_key_info_new();
		macro_key_info->type = button->type;
	}

	if (macro_key_info)
		koneplus_rmp_set_macro_key_info(rmp, rmp_index, macro_key_info);
}

void konepure_rmp_update_with_profile_buttons(KoneplusRmp *rmp, KonepureProfileButtons const *profile_buttons) {
	guint i;

	for (i = 0; i < KONEPURE_PROFILE_BUTTON_NUM; ++i)
		konepure_rmp_update_with_profile_button(rmp, i, profile_buttons);
}

static void konepure_profile_buttons_set_button(KoneplusRmp *rmp,
		guint profile_index, KonepureProfileButtons *profile_buttons) {
	KoneplusRmpMacroKeyInfo *macro_key_info;
	RoccatButton *button;
	gint rmp_index;

	rmp_index = konepure_profile_button_index_to_rmp_index(profile_index);
	if (rmp_index < 0)
		return;

	macro_key_info = koneplus_rmp_get_macro_key_info(rmp, rmp_index);
	button = &profile_buttons->buttons[profile_index];

	button->type = macro_key_info->type;
	switch (button->type) {
	case KONEPLUS_PROFILE_BUTTON_TYPE_SHORTCUT:
		button->key = macro_key_info->keystrokes[0].key;
		button->modifier = macro_key_info->keystrokes[0].action;
		break;
	default:
		button->modifier = 0;
		button->key = 0;
	}

	g_free(macro_key_info);
}

KonepureProfileButtons *konepure_rmp_to_profile_buttons(KoneplusRmp *rmp) {
	KonepureProfileButtons *buttons;
	guint i;

	buttons = g_malloc0(sizeof(KonepureProfileButtons));
	buttons->report_id = KONEPLUS_REPORT_ID_PROFILE_BUTTONS;
	buttons->size = sizeof(KonepureProfileButtons);

	for (i = 0; i < KONEPURE_PROFILE_BUTTON_NUM; ++i)
		konepure_profile_buttons_set_button(rmp, i, buttons);

	return buttons;
}
