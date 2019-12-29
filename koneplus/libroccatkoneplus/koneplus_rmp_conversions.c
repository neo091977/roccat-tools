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

#include "koneplus_rmp.h"
#include "roccat_helper.h"
#include "roccat.h"
#include "i18n-lib.h"

static void koneplus_profile_buttons_set_button(KoneplusRmp *rmp, guint index,
		KoneplusProfileButtons *profile_buttons) {
	KoneplusRmpMacroKeyInfo *macro_key_info;
	RoccatButton *button;

	macro_key_info = koneplus_rmp_get_macro_key_info(rmp, index);
	button = &profile_buttons->buttons[index];

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

KoneplusProfileButtons *koneplus_rmp_to_profile_buttons(KoneplusRmp *rmp) {
	KoneplusProfileButtons *buttons;
	guint i;

	buttons = g_malloc0(sizeof(KoneplusProfileButtons));
	buttons->report_id = KONEPLUS_REPORT_ID_PROFILE_BUTTONS;
	buttons->size = sizeof(KoneplusProfileButtons);
	for (i = 0; i < KONEPLUS_PROFILE_BUTTON_NUM; ++i) {
		koneplus_profile_buttons_set_button(rmp, i, buttons);
	}
	return buttons;
}

KoneplusProfileSettings *koneplus_rmp_to_profile_settings(KoneplusRmp *rmp) {
	KoneplusProfileSettings *settings;
	guint i;
	KoneplusRmpLightInfo *rmp_light_info;

	settings = g_malloc0(sizeof(KoneplusProfileSettings));
	settings->report_id = KONEPLUS_REPORT_ID_PROFILE_SETTINGS;
	settings->size = sizeof(KoneplusProfileSettings);

	settings->xy_sync = koneplus_rmp_get_xy_synchronous(rmp);
	if (settings->xy_sync == ROCCAT_SENSITIVITY_ADVANCED_ON) {
		settings->sensitivity_x = koneplus_rmp_get_sensitivity_x(rmp);
		settings->sensitivity_y = koneplus_rmp_get_sensitivity_y(rmp);
	} else {
		settings->sensitivity_x = koneplus_rmp_get_sensitivity(rmp);
		settings->sensitivity_y = settings->sensitivity_x;
	}

	for (i = 0; i < KONEPLUS_PROFILE_SETTING_CPI_LEVELS_NUM; ++i) {
		settings->cpi_levels_x[i] = koneplus_rmp_get_cpi_level_x(rmp, i);
		settings->cpi_levels_y[i] = koneplus_rmp_get_cpi_level_y(rmp, i);
	}
	settings->cpi_levels_enabled = koneplus_rmp_get_cpi_all(rmp);
	settings->cpi_startup_level = koneplus_rmp_get_cpi_x(rmp);

	settings->polling_rate = koneplus_rmp_get_polling_rate(rmp);

	settings->color_flow_effect = koneplus_rmp_get_light_color_flow(rmp);
	settings->light_effect_type = koneplus_rmp_get_light_effect_type(rmp);
	settings->light_effect_speed = koneplus_rmp_get_light_effect_speed(rmp);
	settings->light_effect_mode = koneplus_rmp_get_light_effect_mode(rmp);

	for (i = 0; i < KONEPLUS_PROFILE_SETTING_LIGHTS_NUM; ++i) {
		rmp_light_info = koneplus_rmp_get_rmp_light_info(rmp, i);
		koneplus_rmp_light_info_to_light_info(rmp_light_info, &settings->lights[i], FALSE);
		roccat_set_bit8(&settings->lights_enabled, i, rmp_light_info->state == KONEPLUS_RMP_LIGHT_INFO_STATE_ON);
		g_free(rmp_light_info);
	}

	return settings;
}

static void koneplus_rmp_update_with_profile_button(KoneplusRmp *rmp,
		RoccatButton const *button,
		guint rmp_index) {
	KoneplusRmpMacroKeyInfo *macro_key_info;
	KoneplusRmpMacroKeyInfo *actual_key_info;

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
		break;
	}

	if (macro_key_info)
		koneplus_rmp_set_macro_key_info(rmp, rmp_index, macro_key_info);
}

void koneplus_rmp_update_with_profile_buttons(KoneplusRmp *rmp, KoneplusProfileButtons const *profile_buttons) {
	guint i;

	for (i = 0; i < KONEPLUS_PROFILE_BUTTON_NUM; ++i)
		koneplus_rmp_update_with_profile_button(rmp, &profile_buttons->buttons[i], i);
}

void koneplus_rmp_update_with_profile_settings(KoneplusRmp *rmp, KoneplusProfileSettings const *profile_settings) {
	KoneplusRmpLightInfo rmp_light_info;
	guint i;

	koneplus_rmp_set_xy_synchronous(rmp, profile_settings->xy_sync);
	if (profile_settings->xy_sync == ROCCAT_SENSITIVITY_ADVANCED_ON) {
		koneplus_rmp_set_sensitivity_x(rmp, profile_settings->sensitivity_x);
		koneplus_rmp_set_sensitivity_y(rmp, profile_settings->sensitivity_y);
	} else {
		koneplus_rmp_set_sensitivity(rmp, profile_settings->sensitivity_x);
	}

	for (i = 0; i < KONEPLUS_PROFILE_SETTING_CPI_LEVELS_NUM; ++i) {
		koneplus_rmp_set_cpi_level_x(rmp, i, profile_settings->cpi_levels_x[i]);
		koneplus_rmp_set_cpi_level_y(rmp, i, profile_settings->cpi_levels_y[i]);
	}
	koneplus_rmp_set_cpi_all(rmp, profile_settings->cpi_levels_enabled);
	koneplus_rmp_set_cpi_x(rmp, profile_settings->cpi_startup_level);
	koneplus_rmp_set_cpi_y(rmp, profile_settings->cpi_startup_level);

	for (i = 0; i < KONEPLUS_PROFILE_SETTING_LIGHTS_NUM; ++i) {
		koneplus_light_info_to_rmp_light_info(&profile_settings->lights[i], &rmp_light_info, roccat_get_bit8(profile_settings->lights_enabled, i));
		koneplus_rmp_set_rmp_light_info(rmp, i, &rmp_light_info);
	}
	koneplus_rmp_set_light_effect_type(rmp, profile_settings->light_effect_type);
	koneplus_rmp_set_light_effect_speed(rmp, profile_settings->light_effect_speed);
	koneplus_rmp_set_light_color_flow(rmp, profile_settings->color_flow_effect);
	koneplus_rmp_set_light_effect_mode(rmp, profile_settings->light_effect_mode);

	koneplus_rmp_set_polling_rate(rmp, profile_settings->polling_rate);
}

void koneplus_rmp_update_with_macro(KoneplusRmp *rmp, guint key_index, KoneplusMacro const *macro) {
	KoneplusRmpMacroKeyInfo *key_info;
	if (!macro)
		return;
	key_info = koneplus_macro_to_rmp_macro_key_info(macro);
	koneplus_rmp_set_macro_key_info(rmp, key_index, key_info);
	g_free(key_info);
}
