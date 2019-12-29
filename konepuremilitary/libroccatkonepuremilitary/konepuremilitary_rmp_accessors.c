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

#include "konepuremilitary.h"
#include "koneplus_rmp.h"

static gchar const * const konepuremilitary_rmp_anglesnap_value_name = "ASnap";
static gchar const * const konepuremilitary_rmp_anglesnap_flag_name = "ASnapFlag";
static gchar const * const konepuremilitary_rmp_sensoralign_value_name = "SAlignment";
static gchar const * const konepuremilitary_rmp_sensoralign_flag_name = "SAlignmentFlag";

static gchar *konepuremilitary_rmp_create_xy_cpi_flag_name(guint index) {
	g_assert(index < KONEPLUS_PROFILE_SETTING_CPI_LEVELS_NUM);
	return g_strdup_printf("XYCpiFlag%u", index);
}

void konepuremilitary_rmp_set_anglesnap_value(KoneplusRmp *rmp, guint value) {
 	koneplus_rmp_set_value(rmp, konepuremilitary_rmp_anglesnap_value_name, value);
}

guint konepuremilitary_rmp_get_anglesnap_value(KoneplusRmp *rmp) {
	return koneplus_rmp_get_value(rmp, konepuremilitary_rmp_anglesnap_value_name);
}

void konepuremilitary_rmp_set_anglesnap_flag(KoneplusRmp *rmp, guint value) {
 	koneplus_rmp_set_value(rmp, konepuremilitary_rmp_anglesnap_flag_name, value);
}

guint konepuremilitary_rmp_get_anglesnap_flag(KoneplusRmp *rmp) {
	return koneplus_rmp_get_value(rmp, konepuremilitary_rmp_anglesnap_flag_name);
}

void konepuremilitary_rmp_set_sensoralign_value(KoneplusRmp *rmp, guint value) {
 	koneplus_rmp_set_value(rmp, konepuremilitary_rmp_sensoralign_value_name, value);
}

guint konepuremilitary_rmp_get_sensoralign_value(KoneplusRmp *rmp) {
	return koneplus_rmp_get_value(rmp, konepuremilitary_rmp_sensoralign_value_name);
}

void konepuremilitary_rmp_set_sensoralign_flag(KoneplusRmp *rmp, guint value) {
 	koneplus_rmp_set_value(rmp, konepuremilitary_rmp_sensoralign_flag_name, value);
}

guint konepuremilitary_rmp_get_sensoralign_flag(KoneplusRmp *rmp) {
	return koneplus_rmp_get_value(rmp, konepuremilitary_rmp_sensoralign_flag_name);
}

void konepuremilitary_rmp_set_xy_cpi_flag(KoneplusRmp *rmp, guint index, gboolean value) {
	gchar *key = konepuremilitary_rmp_create_xy_cpi_flag_name(index);
	koneplus_rmp_set_value(rmp, key, value == TRUE ?
			KONEPUREMILITARY_RMP_XY_CPI_FLAG_LOCKED : KONEPUREMILITARY_RMP_XY_CPI_FLAG_UNLOCKED);
	g_free(key);
}

gboolean konepuremilitary_rmp_get_xy_cpi_flag(KoneplusRmp *rmp, guint index) {
	gchar *key = konepuremilitary_rmp_create_xy_cpi_flag_name(index);
	guint retval = koneplus_rmp_get_value(rmp, key);
	g_free(key);
	return retval == KONEPUREMILITARY_RMP_XY_CPI_FLAG_LOCKED ? TRUE : FALSE;
}
