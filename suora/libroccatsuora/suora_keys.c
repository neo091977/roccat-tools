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

#include "suora_keys.h"
#include "suora_key_relations.h"
#include "suora_rkp_keys.h"
#include "suora_device.h"
#include "roccat_helper.h"
#include "i18n-lib.h"
#include <gaminggear/hid_uid.h>
#include <string.h>

static SuoraDataDeclaration const keys_declaration = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00};

static guint8 const default_keys[sizeof(SuoraKeys)] = {
	0x03, 0x00, 0x25, 0x02, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x20, 0x00,
	0x00, 0x00, 0x21, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x03, 0x00, 0x2a, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x2b, 0x00, 0x00, 0x00, 0x1d, 0x00, 0x00, 0x00, 0x1b, 0x00, 0x00, 0x00, 0x06, 0x00,
	0x00, 0x00, 0x15, 0x00, 0x00, 0x00, 0x17, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x19, 0x00,
	0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x10, 0x01, 0x0c, 0x00, 0x00, 0x00, 0x4a, 0x00, 0x00, 0x00, 0x4c, 0x00, 0x00, 0x00, 0x0b, 0x00,
	0x00, 0x00, 0x1a, 0x00, 0x00, 0x00, 0x91, 0x00, 0x00, 0x00, 0x90, 0x00, 0x00, 0x00, 0x64, 0x00,
	0x10, 0x01, 0x0d, 0x00, 0x10, 0x01, 0x0f, 0x00, 0x00, 0x00, 0x31, 0x00, 0x00, 0x00, 0x4b, 0x00,
	0x00, 0x00, 0x08, 0x00, 0x10, 0x01, 0x00, 0x00, 0x10, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x31, 0x00, 0x00, 0x00, 0x27, 0x00, 0x00, 0x00, 0x4e, 0x00,
	0x00, 0x00, 0x04, 0x00, 0x10, 0x01, 0x01, 0x00, 0x10, 0x01, 0x05, 0x00, 0x00, 0x00, 0x13, 0x00,
	0x00, 0x00, 0x53, 0x00, 0x00, 0x00, 0x5f, 0x00, 0x00, 0x00, 0x5c, 0x00, 0x00, 0x00, 0x59, 0x00,
	0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x28, 0x00, 0x10, 0x01, 0x06, 0x00, 0x00, 0x00, 0x1c, 0x00,
	0x00, 0x00, 0x4d, 0x00, 0x00, 0x00, 0x24, 0x00, 0x10, 0x01, 0x0a, 0x00, 0x00, 0x00, 0x34, 0x00,
	0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x29, 0x00, 0x10, 0x01, 0x07, 0x00, 0x00, 0x00, 0x18, 0x00,
	0x00, 0x00, 0x2f, 0x00, 0x00, 0x00, 0x25, 0x00, 0x10, 0x01, 0x0b, 0x00, 0x03, 0x00, 0x94, 0x01,
	0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x35, 0x00, 0x10, 0x01, 0x08, 0x00, 0x00, 0x00, 0x0c, 0x00,
	0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x26, 0x00, 0x10, 0x01, 0x09, 0x00, 0x10, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x2c, 0x00, 0x00, 0x00, 0x2d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00,
	0x00, 0x00, 0x52, 0x00, 0x00, 0x00, 0x2e, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x54, 0x00,
	0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x5d, 0x00, 0x00, 0x00, 0x5a, 0x00, 0x00, 0x00, 0x62, 0x00,
	0x00, 0x00, 0x51, 0x00, 0x00, 0x00, 0x2a, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x55, 0x00,
	0x00, 0x00, 0x61, 0x00, 0x00, 0x00, 0x5e, 0x00, 0x00, 0x00, 0x5b, 0x00, 0x00, 0x00, 0x63, 0x00,
	0x00, 0x00, 0x50, 0x00, 0x10, 0x01, 0x17, 0x00, 0x10, 0x01, 0x0e, 0x00, 0x00, 0x00, 0xe5, 0x00,
	0x00, 0x00, 0x10, 0x00, 0x10, 0x01, 0x02, 0x00, 0x00, 0x00, 0x33, 0x00, 0x10, 0x01, 0x03, 0x00,
	0x00, 0x00, 0x4f, 0x00, 0x10, 0x01, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00,
	0x00, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x23, 0x00,
	0x00, 0x00, 0xe1, 0x00, 0x00, 0x00, 0x65, 0x00, 0x03, 0x00, 0xb5, 0x00, 0x03, 0x00, 0x24, 0x02,
	0x03, 0x00, 0xb5, 0x00, 0x03, 0x00, 0x26, 0x02, 0x05, 0x00, 0x02, 0x00, 0x03, 0x00, 0xe2, 0x00,
	0x00, 0x00, 0xe2, 0x00, 0x03, 0x00, 0x8a, 0x01, 0x03, 0x00, 0x21, 0x02, 0x03, 0x00, 0xe2, 0x00,
	0x03, 0x00, 0xb7, 0x00, 0x03, 0x00, 0xea, 0x00, 0x03, 0x00, 0xe9, 0x00, 0x03, 0x00, 0x23, 0x02,
	0x00, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x49, 0x00, 0x00, 0x00, 0x37, 0x00, 0x00, 0x00, 0x56, 0x00,
	0x00, 0x00, 0x57, 0x00, 0x00, 0x00, 0x39, 0x00, 0x00, 0x00, 0x58, 0x00, 0x00, 0x00, 0xe7, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

SuoraKeys const *suora_keys_default(void) {
	return (SuoraKeys const *)default_keys;
}

void suora_keys_copy(SuoraKeys *destination, SuoraKeys const *source) {
	memcpy(destination, source, sizeof(SuoraKeys));
}

gboolean suora_keys_equal(SuoraKeys const *left, SuoraKeys const *right) {
	return memcmp(left, right, sizeof(SuoraKeys)) ? FALSE : TRUE;
}

SuoraKeys *suora_keys_read(RoccatDevice *device, GError **error) {
	return (SuoraKeys *)suora_read_interrupt(device, SUORA_FEATURE_REQUEST_KEYS, &keys_declaration, sizeof(SuoraKeys), error);
}

gboolean suora_keys_write(RoccatDevice *device, SuoraKeys const *keys, GError **error) {
	return suora_write_interrupt(device, SUORA_FEATURE_REQUEST_KEYS, &keys_declaration, (gchar const *)keys, sizeof(SuoraKeys), error);
}

static void suora_key_set_to_system(SuoraKey *key, guint value) {
	key->type = SUORA_KEY_TYPE_SYSTEM;
	key->modifier1 = 0;
	key->key = value;
	key->modifier2 = 0;
}

static void suora_key_set_to_consumer(SuoraKey *key, guint hid) {
	key->type = SUORA_KEY_TYPE_CONSUMER;
	key->modifier1 = 0;
	key->key = hid & 0xff;
	key->modifier2 = (hid & 0xff00) >> 8;
}

static void suora_key_set_to_shortcut(SuoraKey *key, guint hid, guint modifier1, guint modifier2) {
	key->type = SUORA_KEY_TYPE_NORMAL;
	key->modifier1 = modifier1;
	key->key = hid;
	key->modifier2 = modifier2;
}

static void suora_key_set_to_normal(SuoraKey *key, guint hid) {
	suora_key_set_to_shortcut(key, hid, 0, 0);
}

static void suora_key_set_to_shortcut_from_mask(SuoraKey *key, guint hid, guint modifier_mask) {
	guint modifier[4] = {0, 0, 0, 0};
	guint index = 0;

	if (roccat_get_bit8(modifier_mask, ROCCAT_BUTTON_MODIFIER_BIT_SHIFT))
		modifier[index++] = HID_UID_KB_LEFT_SHIFT;
	else if (roccat_get_bit8(modifier_mask, ROCCAT_BUTTON_MODIFIER_BIT_CTRL))
		modifier[index++] = HID_UID_KB_LEFT_CONTROL;
	else if (roccat_get_bit8(modifier_mask, ROCCAT_BUTTON_MODIFIER_BIT_ALT))
		modifier[index++] = HID_UID_KB_LEFT_ALT;
	else if (roccat_get_bit8(modifier_mask, ROCCAT_BUTTON_MODIFIER_BIT_WIN))
		modifier[index++] = HID_UID_KB_LEFT_GUI;

	suora_key_set_to_shortcut(key, hid, modifier[0], modifier[1]);
}

static void suora_key_set_to_macro(SuoraKey *key, guint index) {
	key->type = SUORA_KEY_TYPE_MACRO;
	key->modifier1 = 0;
	key->key = index;
	key->modifier2 = 0;
}

static guint8 hid_to_modifier(guint hid) {
	guint8 result = 0;

	roccat_set_bit8(&result, ROCCAT_BUTTON_MODIFIER_BIT_SHIFT, hid == HID_UID_KB_LEFT_SHIFT);
	roccat_set_bit8(&result, ROCCAT_BUTTON_MODIFIER_BIT_CTRL, hid == HID_UID_KB_LEFT_CONTROL);
	roccat_set_bit8(&result, ROCCAT_BUTTON_MODIFIER_BIT_ALT, hid == HID_UID_KB_LEFT_ALT);
	roccat_set_bit8(&result, ROCCAT_BUTTON_MODIFIER_BIT_WIN, hid == HID_UID_KB_LEFT_GUI);
	
	return result;
}

void suora_key_set(SuoraKey *key, guint keys_index, guint type, guint modifier_mask, guint hid) {
	switch (type) {
	case SUORA_RKP_KEY_TYPE_SYSTEM_SHUTDOWN:
		suora_key_set_to_system(key, SUORA_KEY_TYPE_SYSTEM_SHUTDOWN);
		break;
	case SUORA_RKP_KEY_TYPE_SYSTEM_SLEEP:
		suora_key_set_to_system(key, SUORA_KEY_TYPE_SYSTEM_SLEEP);
		break;
	case SUORA_RKP_KEY_TYPE_SYSTEM_WAKE:
		suora_key_set_to_system(key, SUORA_KEY_TYPE_SYSTEM_WAKE);
		break;
	case SUORA_RKP_KEY_TYPE_APP_EMAIL:
		suora_key_set_to_consumer(key, HID_UID_AL_EMAIL);
		break;
	case SUORA_RKP_KEY_TYPE_APP_CALCULATOR:
		suora_key_set_to_consumer(key, HID_UID_AL_CALCULATOR);
		break;
	case SUORA_RKP_KEY_TYPE_APP_MY_COMPUTER:
		suora_key_set_to_consumer(key, HID_UID_AL_LOCAL_MACHINE_BROWSER);
		break;
	case SUORA_RKP_KEY_TYPE_WWW_SEARCH:
		suora_key_set_to_consumer(key, HID_UID_AC_SEARCH);
		break;
	case SUORA_RKP_KEY_TYPE_WWW_HOME:
		suora_key_set_to_consumer(key, HID_UID_AC_HOME);
		break;
	case SUORA_RKP_KEY_TYPE_WWW_BACK:
		suora_key_set_to_consumer(key, HID_UID_AC_BACK);
		break;
	case SUORA_RKP_KEY_TYPE_WWW_FORWARD:
		suora_key_set_to_consumer(key, HID_UID_AC_FORWARD);
		break;
	case SUORA_RKP_KEY_TYPE_WWW_STOP:
		suora_key_set_to_consumer(key, HID_UID_AC_STOP);
		break;
	case SUORA_RKP_KEY_TYPE_WWW_REFRESH:
		suora_key_set_to_consumer(key, HID_UID_AC_REFRESH);
		break;
	case SUORA_RKP_KEY_TYPE_APP_MEDIA_PLAYER:
		suora_key_set_to_consumer(key, HID_UID_AL_CONSUMER_CONTROL_CONFIG);
		break;
	case SUORA_RKP_KEY_TYPE_MULTIMEDIA_PLAY_PAUSE:
		suora_key_set_to_consumer(key, HID_UID_CP_PLAYPAUSE);
		break;
	case SUORA_RKP_KEY_TYPE_MULTIMEDIA_NEXT_TRACK:
		suora_key_set_to_consumer(key, HID_UID_CP_NEXTSONG);
		break;
	case SUORA_RKP_KEY_TYPE_MULTIMEDIA_PREV_TRACK:
		suora_key_set_to_consumer(key, HID_UID_CP_PREVIOUSSONG);
		break;
	case SUORA_RKP_KEY_TYPE_MULTIMEDIA_STOP:
		suora_key_set_to_consumer(key, HID_UID_CP_STOP);
		break;
	case SUORA_RKP_KEY_TYPE_MULTIMEDIA_VOLUME_UP:
		suora_key_set_to_consumer(key, HID_UID_CP_VOLUMEUP);
		break;
	case SUORA_RKP_KEY_TYPE_MULTIMEDIA_VOLUME_DOWN:
		suora_key_set_to_consumer(key, HID_UID_CP_VOLUMEDOWN);
		break;
	case SUORA_RKP_KEY_TYPE_MULTIMEDIA_MUTE:
		suora_key_set_to_consumer(key, HID_UID_CP_MUTE);
		break;
	case SUORA_RKP_KEY_TYPE_SHORTCUT:
		suora_key_set_to_shortcut_from_mask(key, hid, modifier_mask);
		break;
	case SUORA_RKP_KEY_TYPE_MACRO:
		suora_key_set_to_macro(key, suora_key_relations[keys_index].macro_index);
		break;
	default:
		suora_key_set_to_normal(key, type);
		break;
	}
}

void roccat_button_to_suora_key(SuoraKey *key, RoccatButton const *button, guint keys_index) {
	suora_key_set(key, keys_index, button->type, button->modifier, button->key);
}

void suora_key_to_roccat_button(RoccatButton *rkp_key, SuoraKey const *key, guint keys_index) {
	guint16 hid;
	
	switch (key->type) {
	/* The keys with index 105 and 113 can use both, NORMAL and 10.
	 * The 10 variant is set after device reset, Windows software sets NORMAL
	 */
	case SUORA_KEY_TYPE_10:
		switch (key->key) {
		case SUORA_KEY_TYPE_10_RIGHT_ALT:
			rkp_key->type = HID_UID_KB_RIGHT_ALT;
			rkp_key->key = HID_UID_KB_RIGHT_ALT;

			break;
		case SUORA_KEY_TYPE_10_RIGHT_CONTROL:
			rkp_key->type = HID_UID_KB_RIGHT_CONTROL;
			rkp_key->key = HID_UID_KB_RIGHT_CONTROL;
			break;
		default:
			break;
		}
		break;
	case SUORA_KEY_TYPE_NORMAL:
		rkp_key->type = suora_key_relations[keys_index].value;
		rkp_key->modifier = hid_to_modifier(key->modifier1) | hid_to_modifier(key->modifier2);
		rkp_key->key = key->key;
		break;
	case SUORA_KEY_TYPE_SYSTEM:
		switch (key->key) {
		case SUORA_KEY_TYPE_SYSTEM_SHUTDOWN:
			rkp_key->type = SUORA_RKP_KEY_TYPE_SYSTEM_SHUTDOWN;
			break;
		case SUORA_KEY_TYPE_SYSTEM_SLEEP:
			rkp_key->type = SUORA_RKP_KEY_TYPE_SYSTEM_SLEEP;
			break;
		case SUORA_KEY_TYPE_SYSTEM_WAKE:
			rkp_key->type = SUORA_RKP_KEY_TYPE_SYSTEM_WAKE;
			break;
		default:
			g_warning(_("Unknown SuoraKey %02x %02x %02x %02x at %u"), key->type, key->modifier1, key->key, key->modifier2, keys_index);
			break;
		}
		break;
	case SUORA_KEY_TYPE_CONSUMER:
		hid = (guint16)key->key | ((guint16)key->modifier2 << 8);
		switch (hid) {
		case HID_UID_AL_EMAIL:
			rkp_key->type = SUORA_RKP_KEY_TYPE_APP_EMAIL;
			break;
		case HID_UID_AL_CALCULATOR:
			rkp_key->type = SUORA_RKP_KEY_TYPE_APP_CALCULATOR;
			break;
		case HID_UID_AL_LOCAL_MACHINE_BROWSER:
			rkp_key->type = SUORA_RKP_KEY_TYPE_APP_MY_COMPUTER;
			break;
		case HID_UID_AC_SEARCH:
			rkp_key->type = SUORA_RKP_KEY_TYPE_WWW_SEARCH;
			break;
		case HID_UID_AC_HOME:
			rkp_key->type = SUORA_RKP_KEY_TYPE_WWW_HOME;
			break;
		case HID_UID_AC_BACK:
			rkp_key->type = SUORA_RKP_KEY_TYPE_WWW_BACK;
			break;
		case HID_UID_AC_FORWARD:
			rkp_key->type = SUORA_RKP_KEY_TYPE_WWW_FORWARD;
			break;
		case HID_UID_AC_STOP:
			rkp_key->type = SUORA_RKP_KEY_TYPE_WWW_STOP;
			break;
		case HID_UID_AC_REFRESH:
			rkp_key->type = SUORA_RKP_KEY_TYPE_WWW_REFRESH;
			break;
		case HID_UID_AL_CONSUMER_CONTROL_CONFIG:
			rkp_key->type = SUORA_RKP_KEY_TYPE_APP_MEDIA_PLAYER;
			break;
		case HID_UID_CP_PLAYPAUSE:
			rkp_key->type = SUORA_RKP_KEY_TYPE_MULTIMEDIA_PLAY_PAUSE;
			break;
		case HID_UID_CP_NEXTSONG:
			rkp_key->type = SUORA_RKP_KEY_TYPE_MULTIMEDIA_NEXT_TRACK;
			break;
		case HID_UID_CP_PREVIOUSSONG:
			rkp_key->type = SUORA_RKP_KEY_TYPE_MULTIMEDIA_PREV_TRACK;
			break;
		case HID_UID_CP_STOP:
			rkp_key->type = SUORA_RKP_KEY_TYPE_MULTIMEDIA_STOP;
			break;
		case HID_UID_CP_VOLUMEUP:
			rkp_key->type = SUORA_RKP_KEY_TYPE_MULTIMEDIA_VOLUME_UP;
			break;
		case HID_UID_CP_VOLUMEDOWN:
			rkp_key->type = SUORA_RKP_KEY_TYPE_MULTIMEDIA_VOLUME_DOWN;
			break;
		case HID_UID_CP_MUTE:
			rkp_key->type = SUORA_RKP_KEY_TYPE_MULTIMEDIA_MUTE;
			break;
		default:
			g_warning(_("Unknown SuoraKey %02x %02x %02x %02x at %u"), key->type, key->modifier1, key->key, key->modifier2, keys_index);
			break;
		}
		break;
	case SUORA_KEY_TYPE_MACRO:
		rkp_key->type = SUORA_RKP_KEY_TYPE_MACRO;
		break;
	default:
		g_warning(_("Unknown SuoraKey %02x %02x %02x %02x at %u"), key->type, key->modifier1, key->key, key->modifier2, keys_index);
		break;
	}
}
