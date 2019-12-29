#ifndef __ROCCAT_TIMER_DIALOG_H__
#define __ROCCAT_TIMER_DIALOG_H__

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

#define ROCCAT_TIMER_DIALOG_TYPE (roccat_timer_dialog_get_type())
#define ROCCAT_TIMER_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ROCCAT_TIMER_DIALOG_TYPE, RoccatTimerDialog))
#define IS_ROCCAT_TIMER_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ROCCAT_TIMER_DIALOG_TYPE))

typedef struct _RoccatTimerDialog RoccatTimerDialog;
typedef struct _RoccatTimerDialogPrivate RoccatTimerDialogPrivate;

struct _RoccatTimerDialog {
	GtkDialog dialog;
	RoccatTimerDialogPrivate *priv;
};

RoccatTimer *roccat_timer_dialog(GtkWindow *parent);
RoccatTimer *roccat_timer_dialog_run(RoccatTimerDialog *timer_dialog);
RoccatTimer *roccat_timer_dialog_get_value(RoccatTimerDialog *timer_dialog);
void roccat_timer_dialog_set_timers(RoccatTimerDialog *timer_dialog, RoccatTimers *timers);
GtkWidget *roccat_timer_dialog_new(GtkWindow *parent);

G_END_DECLS

#endif
