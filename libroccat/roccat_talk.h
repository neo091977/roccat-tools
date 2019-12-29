#ifndef __ROCCAT_TALK_H__
#define __ROCCAT_TALK_H__

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

#include <glib.h>

G_BEGIN_DECLS

typedef enum {
	ROCCAT_TALK_DEVICE_NONE = 0,
	/* Devices use USB_DEVICE_ID_ROCCAT_* */
	ROCCAT_TALK_DEVICE_KEYBOARD = 0xfffd, /* np */
	ROCCAT_TALK_DEVICE_MOUSE = 0xfffe, /* np */
	ROCCAT_TALK_DEVICE_ALL = 0xffff,
} RoccatTalkDevice;

typedef enum {
	ROCCAT_TALK_EASYAIM_OFF = 0,
	ROCCAT_TALK_EASYAIM_1 = 1,
	ROCCAT_TALK_EASYAIM_2 = 2,
	ROCCAT_TALK_EASYAIM_3 = 3,
	ROCCAT_TALK_EASYAIM_4 = 4,
	ROCCAT_TALK_EASYAIM_5 = 5,
} RoccatTalkEasyaim;

guint roccat_talk_device_get_type(guint talk_device);
gchar const *roccat_talk_device_get_text_static(guint talk_device);

G_END_DECLS

#endif
