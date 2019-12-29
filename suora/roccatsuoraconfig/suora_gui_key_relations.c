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

#include "suora_gui_key_relations.h"
#include "suora_key_relations.h"
#include "suora.h"
#include "roccat_key_combo_box.h"
#include <gaminggear/hid_uid.h>

SuoraGuiKeyPosition const suora_gui_key_positions_general[] = {
	{ 65,  0, 4,  0,  4},
	{ 41,  0, 4,  8,  4},
	{ 49,  0, 4, 12,  4},
	{109,  0, 4, 16,  4},
	{111,  0, 4, 20,  4},
	{ 42,  0, 4, 26,  4},
	{ 50,  0, 4, 30,  4},
	{ 58,  0, 4, 34,  4},
	{ 66,  0, 4, 38,  4},
	{ 74,  0, 4, 44,  4},
	{ 78,  0, 4, 48,  4},
	{ 62,  0, 4, 52,  4},
	{ 70,  0, 4, 56,  4},
	{ 28,  0, 4, 61,  4},
	{ 36,  0, 4, 65,  4},
	{ 37,  0, 4, 69,  4},
	{127,  0, 4, 74,  4},
	{133,  0, 4, 78,  4},
	{134,  0, 4, 82,  4},
	{126,  0, 4, 86,  4},
	{ 73,  5, 4,  0,  4},
	{  1,  5, 4,  4,  4},
	{  2,  5, 4,  8,  4},
	{  3,  5, 4, 12,  4},
	{  4,  5, 4, 16,  4},
	{  5,  5, 4, 20,  4},
	{119,  5, 4, 24,  4},
	{ 61,  5, 4, 28,  4},
	{ 69,  5, 4, 32,  4},
	{ 77,  5, 4, 36,  4},
	{ 46,  5, 4, 40,  4},
	{ 81,  5, 4, 44,  4},
	{ 89,  5, 4, 48,  4},
	{ 97,  5, 4, 52,  8},
	{137,  5, 4, 61,  4},
	{ 29,  5, 4, 65,  4},
	{ 39,  5, 4, 69,  4},
	{ 52,  5, 4, 74,  4},
	{ 91,  5, 4, 78,  4},
	{ 99,  5, 4, 82,  4},
	{139,  5, 4, 86,  4},
	{ 16,  9, 4,  0,  6},
	{ 24,  9, 4,  6,  4},
	{ 32,  9, 4, 10,  4},
	{ 40,  9, 4, 14,  4},
	{ 20,  9, 4, 18,  4},
	{ 21,  9, 4, 22,  4},
	{ 59,  9, 4, 26,  4},
	{ 67,  9, 4, 30,  4},
	{ 75,  9, 4, 34,  4},
	{ 83,  9, 4, 38,  4},
	{ 51,  9, 4, 42,  4},
	{ 68,  9, 4, 46,  4},
	{ 76,  9, 4, 50,  4},
	{ 30,  9, 4, 61,  4},
	{ 60,  9, 4, 65,  4},
	{ 47,  9, 4, 69,  4},
	{ 53,  9, 4, 74,  4},
	{ 92,  9, 4, 78,  4},
	{100,  9, 4, 82,  4},
	{140,  9, 8, 86,  4},
	{141, 13, 4,  0,  7},
	{ 48, 13, 4,  7,  4},
	{ 56, 13, 4, 11,  4},
	{ 64, 13, 4, 15,  4},
	{ 72, 13, 4, 19,  4},
	{ 22, 13, 4, 23,  4},
	{ 31, 13, 4, 27,  4},
	{116, 13, 4, 31,  4},
	{117, 13, 4, 35,  4},
	{118, 13, 4, 39,  4},
	{110, 13, 4, 43,  4},
	{ 63, 13, 4, 47,  4},
	{ 54, 13, 4, 74,  4},
	{ 93, 13, 4, 78,  4},
	{101, 13, 4, 82,  4},
	{ 17, 17, 4,  9,  4},
	{ 18, 17, 4, 13,  4},
	{ 19, 17, 4, 17,  4},
	{ 23, 17, 4, 21,  4},
	{115, 17, 4, 25,  4},
	{ 87, 17, 4, 29,  4},
	{108, 17, 4, 33,  4},
	{ 98, 17, 4, 37,  4},
	{138, 17, 4, 41,  4},
	{ 90, 17, 4, 45,  4},
	{107, 17, 4, 49, 11},
	{ 88, 17, 4, 65,  4},
	{ 55, 17, 4, 74,  4},
	{ 94, 17, 4, 78,  4},
	{102, 17, 4, 82,  4},
	{142, 17, 8, 86,  4},
	{136, 21, 4,  0,  5},
	{106, 21, 4,  5,  5},
	{128, 21, 4, 10,  5},
	{ 80, 21, 4, 15, 25},
	{105, 21, 4, 40,  5},
	{ 79, 21, 4, 45,  5},
	{121, 21, 4, 50,  5},
	{113, 21, 4, 55,  5},
	{104, 21, 4, 61,  4},
	{ 96, 21, 4, 65,  4},
	{112, 21, 4, 69,  4},
	{ 95, 21, 4, 74,  8},
	{103, 21, 4, 82,  4},
	{SUORA_GUI_KEY_POSITION_INDEX_END, 0, 0, 0, 0}
};

SuoraGuiKeyPosition const suora_gui_key_positions_de[] = {
	{ 57,  9, 8, 55,  5},
	{ 38, 13, 4, 51,  4},
	{120, 17, 4,  0,  5},
	{ 35, 17, 4,  5,  4},
	{SUORA_GUI_KEY_POSITION_INDEX_END, 0, 0, 0, 0}
};

gchar *suora_hid_to_special_keyname(guint8 hid) {
	gchar *text = NULL;

	switch(hid) {
	case SUORA_KEY_RELATION_VALUE_GAME_MODE:
		text = "GAME\nMODE";
		break;
	case SUORA_KEY_RELATION_VALUE_FN:
		text = "FN";
		break;
	case SUORA_KEY_RELATION_VALUE_MUTE:
		text = "\xf0\x9f\x94\x87";
		break;
	case SUORA_KEY_RELATION_VALUE_VOLUMEDOWN:
		text = "\xf0\x9f\x94\x89";
		break;
	case SUORA_KEY_RELATION_VALUE_VOLUMEUP:
		text = "\xf0\x9f\x94\x8a";
		break;
	default:
		break;
	}

	if (text)
		return g_strdup(text);
	else
		return NULL;
}
