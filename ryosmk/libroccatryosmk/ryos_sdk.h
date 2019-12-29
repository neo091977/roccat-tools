#ifndef __ROCCAT_RYOS_SDK_H__
#define __ROCCAT_RYOS_SDK_H__

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

#include "ryos.h"

G_BEGIN_DECLS

gboolean ryos_sdk_set_kb_sdk_mode(RoccatDevice *ryos, gboolean state, GError **error);
gboolean ryos_sdk_set_all_leds(RoccatDevice *ryos, GArray const *data, GError **error);
gboolean ryos_sdk_turn_on_all_leds(RoccatDevice *ryos, GError **error);
gboolean ryos_sdk_turn_off_all_leds(RoccatDevice *ryos, GError **error);
gboolean ryos_sdk_set_led_on(RoccatDevice *ryos, guint8 index, GError **error);
gboolean ryos_sdk_set_led_off(RoccatDevice *ryos, guint8 index, GError **error);
gboolean ryos_sdk_all_key_blinking(RoccatDevice *ryos, guint interval, guint count, GError **error);

G_END_DECLS

#endif
