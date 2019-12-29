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

#include "skeltr_gui_key_relations.h"
#include <string.h>

SkeltrGuiKeyPosition const skeltr_gui_key_positions_general[] = {
	{  0,  0, 3,  5,  4}, // HID_UID_KB_ESCAPE
	{  1,  0, 3, 13,  4}, // HID_UID_KB_F1
	{  2,  0, 3, 17,  4},
	{  3,  0, 3, 21,  4},
	{  4,  0, 3, 25,  4},
	{  5,  0, 3, 31,  4},
	{  6,  0, 3, 35,  4},
	{  7,  0, 3, 39,  4},
	{  8,  0, 3, 43,  4},
	{  9,  0, 3, 49,  4},
	{ 10,  0, 3, 53,  4},
	{ 11,  0, 3, 57,  4},
	{ 12,  0, 3, 61,  4},
	{ 13,  0, 3, 66,  4},
	{ 14,  0, 3, 70,  4},
	{ 15,  0, 3, 74,  4},
	{ 16,  4, 4,  0,  4}, // m1
	{ 17,  8, 4,  0,  4},
	{ 18, 12, 4,  0,  4},
	{ 19, 16, 4,  0,  4},
	{ 20, 20, 4,  0,  4},
	{ 21, 26, 3, 26,  4}, // t1
	{ 22, 26, 3, 30,  4},
	{ 23, 26, 3, 34,  4},
	{ 24,  4, 4,  5,  4}, // row 1 0x35
	{ 25,  4, 4,  9,  4},
	{ 26,  4, 4, 13,  4},
	{ 27,  4, 4, 17,  4},
	{ 28,  4, 4, 21,  4},
	{ 29,  4, 4, 25,  4},
	{ 30,  4, 4, 29,  4},
	{ 31,  4, 4, 33,  4},
	{ 32,  4, 4, 37,  4},
	{ 33,  4, 4, 41,  4},
	{ 34,  4, 4, 45,  4},
	{ 35,  4, 4, 49,  4},
	{ 36,  4, 4, 53,  4},
	{ 37,  4, 4, 57,  8},
	{ 38,  8, 4,  5,  6}, // row 2 HID_UID_KB_TAB
	{ 39,  8, 4, 11,  4},
	{ 40,  8, 4, 15,  4},
	{ 41,  8, 4, 19,  4},
	{ 42,  8, 4, 23,  4},
	{ 43,  8, 4, 27,  4},
	{ 44,  8, 4, 31,  4},
	{ 45,  8, 4, 35,  4},
	{ 46,  8, 4, 39,  4},
	{ 47,  8, 4, 43,  4},
	{ 48,  8, 4, 47,  4},
	{ 49,  8, 4, 51,  4},
	{ 50,  8, 4, 55,  4},
	{ 51,  8, 8, 60,  5},
	{ 52, 12, 4,  5,  7}, // row 3 HID_UID_KB_CAPS_LOCK
	{ 53, 12, 4, 12,  4},
	{ 54, 12, 4, 16,  4},
	{ 55, 12, 4, 20,  4},
	{ 56, 12, 4, 24,  4},
	{ 57, 12, 4, 28,  4},
	{ 58, 12, 4, 32,  4},
	{ 59, 12, 4, 36,  4},
	{ 60, 12, 4, 40,  4},
	{ 61, 12, 4, 44,  4},
	{ 62, 12, 4, 48,  4},
	{ 63, 12, 4, 52,  4},
	{ 64, 12, 4, 56,  4},
	{ 65, 16, 4, 5,  5}, // row 4 HID_UID_KB_LEFT_SHIFT
	{ 66, 16, 4, 10,  4},
	{ 67, 16, 4, 14,  4},
	{ 68, 16, 4, 18,  4},
	{ 69, 16, 4, 22,  4},
	{ 70, 16, 4, 26,  4},
	{ 71, 16, 4, 30,  4},
	{ 72, 16, 4, 34,  4},
	{ 73, 16, 4, 38,  4},
	{ 74, 16, 4, 42,  4},
	{ 75, 16, 4, 46,  4},
	{ 76, 16, 4, 50,  4},
	{ 77, 16, 4, 54, 11},
	{ 78, 20, 4,  5,  5}, // row 5 HID_UID_KB_LEFT_CONTROL
	{ 79, 20, 4, 10,  5},
	{ 80, 20, 4, 15,  5},
	{ 81, 20, 4, 20, 25},
	{ 82, 20, 4, 45,  5},
	{ 83, 20, 4, 50,  5},
	{ 84, 20, 4, 55,  5},
	{ 85, 20, 4, 60,  5},
	{ 86,  4, 4, 66,  4}, // block HID_UID_KB_INSERT
	{ 87,  4, 4, 70,  4},
	{ 88,  4, 4, 74,  4},
	{ 89,  8, 4, 66,  4},
	{ 90,  8, 4, 70,  4},
	{ 91,  8, 4, 74,  4},
	{ 92, 16, 4, 70,  4}, // arrows HID_UID_KB_UP_ARROW
	{ 93, 20, 4, 66,  4},
	{ 94, 20, 4, 70,  4},
	{ 95, 20, 4, 74,  4},
	{ 96,  4, 4, 79,  4}, // numpad HID_UID_KP_NUM_LOCK
	{ 97,  4, 4, 83,  4},
	{ 98,  4, 4, 87,  4},
	{ 99,  4, 4, 91,  4},
	{100,  8, 8, 91,  4},
	{101, 16, 8, 91,  4},
	{102, 16, 4, 79,  4},
	{103, 16, 4, 83,  4},
	{104, 16, 4, 87,  4},
	{105, 12, 4, 79,  4},
	{106, 12, 4, 83,  4},
	{107, 12, 4, 87,  4},
	{108,  8, 4, 79,  4},
	{109,  8, 4, 83,  4},
	{110,  8, 4, 87,  4},
	{111, 20, 4, 79,  8},
	{112, 20, 4, 87,  4},
	{SKELTR_GUI_KEY_POSITION_INDEX_END, 0, 0, 0}
};

SkeltrGuiKeyPosition const skeltr_gui_key_positions_ch[] = {
	{SKELTR_GUI_KEY_POSITION_INDEX_END, 0, 0, 0}
};

SkeltrGuiKeyPosition const skeltr_gui_key_positions_eu[] = {
	{SKELTR_GUI_KEY_POSITION_INDEX_END, 0, 0, 0}
};

SkeltrGuiKeyPosition const skeltr_gui_key_positions_fr[] = {
	{SKELTR_GUI_KEY_POSITION_INDEX_END, 0, 0, 0}
};

SkeltrGuiKeyPosition const skeltr_gui_key_positions_no[] = {
	{SKELTR_GUI_KEY_POSITION_INDEX_END, 0, 0, 0}
};

SkeltrGuiKeyPosition const skeltr_gui_key_positions_ru[] = {
	{SKELTR_GUI_KEY_POSITION_INDEX_END, 0, 0, 0}
};

SkeltrGuiKeyPosition const skeltr_gui_key_positions_uk[] = {
	{SKELTR_GUI_KEY_POSITION_INDEX_END, 0, 0, 0}
};

SkeltrGuiKeyPosition const skeltr_gui_key_positions_us[] = {
	{SKELTR_GUI_KEY_POSITION_INDEX_END, 0, 0, 0}
};

SkeltrGuiKeyPosition const skeltr_gui_key_positions_empty[] = {
	{SKELTR_GUI_KEY_POSITION_INDEX_END, 0, 0, 0}
};

SkeltrGuiKeyPosition const *skeltr_gui_key_positions_get(gchar const *layout) {
	if (!strcmp(layout, "CH"))
		return skeltr_gui_key_positions_ch;
	else if (!strcmp(layout, "EU"))
		return skeltr_gui_key_positions_eu;
	else if (!strcmp(layout, "FR"))
		return skeltr_gui_key_positions_fr;
	else if (!strcmp(layout, "NO"))
		return skeltr_gui_key_positions_no;
	else if (!strcmp(layout, "RU"))
		return skeltr_gui_key_positions_ru;
	else if (!strcmp(layout, "UK"))
		return skeltr_gui_key_positions_uk;
	else if (!strcmp(layout, "US"))
		return skeltr_gui_key_positions_us;
	else
		return skeltr_gui_key_positions_empty;
}
