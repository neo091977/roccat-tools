#ifndef __ROCCAT_TIMER_COMBO_BOX_H__
#define __ROCCAT_TIMER_COMBO_BOX_H__

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

#include "roccat_timers.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

GtkWidget *roccat_timer_combo_box_new(void);
void roccat_timer_combo_box_set_timers(GtkComboBox *combo_box, RoccatTimers *timers);

RoccatTimer *roccat_timer_combo_box_get_value(GtkComboBox *combo);

G_END_DECLS

#endif
