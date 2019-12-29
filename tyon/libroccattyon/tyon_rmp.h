#ifndef __ROCCAT_TYON_RMP_H__
#define __ROCCAT_TYON_RMP_H__

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

#include "tyon_rmp_macro_key_info.h"
#include "tyon_rmp_light_info.h"

G_BEGIN_DECLS

enum {
	TYON_GAMEFILE_NUM = 3,
};

typedef struct _TyonRmp TyonRmp;

struct _TyonRmp {
	gboolean modified_rmp; /* means things only in rmp like profile name */
	gboolean modified_settings;
	gboolean modified_macros[TYON_PROFILE_BUTTON_NUM];
	GKeyFile *key_file;
	GKeyFile *default_key_file;
};

void tyon_rmp_free(TyonRmp *rmp);
TyonRmp *tyon_rmp_dup(TyonRmp const *src);
TyonRmp *tyon_rmp_read_with_path(gchar const *path, GKeyFile *default_key_file, GError **error);
gboolean tyon_rmp_write_with_path(gchar const *path, TyonRmp *rmp, GError **error);
GKeyFile *tyon_rmp_defaults(void);
TyonRmp *tyon_default_rmp(void);

TyonRmpMacroKeyInfo *tyon_rmp_get_macro_key_info(TyonRmp *rmp, guint index);
void tyon_rmp_set_macro_key_info(TyonRmp *rmp, guint index, TyonRmpMacroKeyInfo *rmp_macro_key_info);
gchar *tyon_rmp_get_profile_name(TyonRmp *rmp);
void tyon_rmp_set_profile_name(TyonRmp *rmp, gchar const *string);
guint tyon_rmp_get_xy_synchronous(TyonRmp *rmp);
void tyon_rmp_set_xy_synchronous(TyonRmp *rmp, guint value);
guint tyon_rmp_get_sensitivity_x(TyonRmp *rmp);
void tyon_rmp_set_sensitivity_x(TyonRmp *rmp, guint value);
guint tyon_rmp_get_sensitivity_y(TyonRmp *rmp);
void tyon_rmp_set_sensitivity_y(TyonRmp *rmp, guint value);
guint tyon_rmp_get_sensitivity(TyonRmp *rmp);
void tyon_rmp_set_sensitivity(TyonRmp *rmp, guint value);
guint tyon_rmp_get_cpi(TyonRmp *rmp, guint bit);
void tyon_rmp_set_cpi(TyonRmp *rmp, guint bit, guint value);
guint8 tyon_rmp_get_cpi_all(TyonRmp *rmp);
void tyon_rmp_set_cpi_all(TyonRmp *rmp, guint value);
guint tyon_rmp_get_light_chose_type(TyonRmp *rmp);
void tyon_rmp_set_light_chose_type(TyonRmp *rmp, guint value);
TyonRmpLightInfo *tyon_rmp_get_custom_light_info(TyonRmp *rmp, guint index);
void tyon_rmp_set_custom_light_info(TyonRmp *rmp, guint index, TyonRmpLightInfo *rmp_light_info);
TyonRmpLightInfo *tyon_rmp_get_rmp_light_info(TyonRmp *rmp, guint index);
void tyon_rmp_set_rmp_light_info(TyonRmp *rmp, guint index, TyonRmpLightInfo *rmp_light_info);
guint tyon_rmp_get_polling_rate(TyonRmp *rmp);
void tyon_rmp_set_polling_rate(TyonRmp *rmp, guint value);
guint tyon_rmp_get_light_effect_type(TyonRmp *rmp);
void tyon_rmp_set_light_effect_type(TyonRmp *rmp, guint value);
guint tyon_rmp_get_light_effect_speed(TyonRmp *rmp);
void tyon_rmp_set_light_effect_speed(TyonRmp *rmp, guint value);
//guint tyon_rmp_get_light_effect_mode(TyonRmp *rmp);
//void tyon_rmp_set_light_effect_mode(TyonRmp *rmp, guint value);
guint tyon_rmp_get_light_color_flow(TyonRmp *rmp);
void tyon_rmp_set_light_color_flow(TyonRmp *rmp, guint value);
guint tyon_rmp_get_cpi_level_x(TyonRmp *rmp, guint index);
void tyon_rmp_set_cpi_level_x(TyonRmp *rmp, guint index, guint value);
//guint tyon_rmp_get_cpi_level_y(TyonRmp *rmp, guint index);
//void tyon_rmp_set_cpi_level_y(TyonRmp *rmp, guint index, guint value);
guint tyon_rmp_get_cpi_x(TyonRmp *rmp);
void tyon_rmp_set_cpi_x(TyonRmp *rmp, guint value);
//guint tyon_rmp_get_cpi_y(TyonRmp *rmp);
//void tyon_rmp_set_cpi_y(TyonRmp *rmp, guint value);
gchar *tyon_rmp_get_game_file_name(TyonRmp *rmp, guint index);
void tyon_rmp_set_game_file_name(TyonRmp *rmp, guint index, gchar const *string);
//guint tyon_rmp_get_light_switch(TyonRmp *rmp);
//void tyon_rmp_set_light_switch(TyonRmp *rmp, guint value);
//guint tyon_rmp_get_light_type(TyonRmp *rmp);
//void tyon_rmp_set_light_type(TyonRmp *rmp, guint value);
//guint tyon_rmp_get_color_flag(TyonRmp *rmp);
//void tyon_rmp_set_color_flag(TyonRmp *rmp, guint value);
//guint tyon_rmp_get_color_options(TyonRmp *rmp);
//void tyon_rmp_set_color_options(TyonRmp *rmp, guint value);
void tyon_rmp_set_use_color_for_all(TyonRmp *rmp, guint value);
guint tyon_rmp_get_use_color_for_all(TyonRmp *rmp);
void tyon_rmp_set_talkfx(TyonRmp *rmp, guint value);
guint tyon_rmp_get_talkfx(TyonRmp *rmp);

guint tyon_rmp_cpi_to_bin(guint rmp_value);
guint tyon_bin_cpi_to_rmp(guint bin_value);

TyonProfileSettings *tyon_rmp_to_profile_settings(TyonRmp *rmp);
TyonProfileButtons *tyon_rmp_to_profile_buttons(TyonRmp *rmp);

void tyon_rmp_update_with_profile_settings(TyonRmp *rmp, TyonProfileSettings const *profile_settings);
void tyon_rmp_update_with_profile_buttons(TyonRmp *rmp, TyonProfileButtons const *profile_buttons);
void tyon_rmp_update_with_macro(TyonRmp *rmp, guint key_index, TyonMacro const *macro);
void tyon_rmp_update_from_device(TyonRmp *rmp, RoccatDevice *tyon, guint profile_index);

gboolean tyon_rmp_get_modified(TyonRmp const *rmp);
void tyon_rmp_set_modified(TyonRmp *rmp);
void tyon_rmp_set_unmodified(TyonRmp *rmp);

TyonRmp *tyon_rmp_load_actual(guint profile_index);
TyonRmp *tyon_rmp_load(RoccatDevice *tyon, guint profile_index, GError **error);
gboolean tyon_rmp_save_actual(TyonRmp *rmp, guint profile_index, GError **error);
gboolean tyon_rmp_save(RoccatDevice *tyon, TyonRmp *rmp, guint profile_index, GError **error);
TyonRmp *tyon_rmp_load_save_after_reset(RoccatDevice *tyon, guint profile_index, GError **error);

G_END_DECLS

#endif
