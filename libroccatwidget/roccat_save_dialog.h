#ifndef __ROCCAT_SAVE_DIALOG_H__
#define __ROCCAT_SAVE_DIALOG_H__

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

#include <gtk/gtk.h>

G_BEGIN_DECLS

/*!
 * \brief asks the user if he want's to save data and exit
 *
 * The dialog is modal.
 *
 * If cancel is \c TRUE, an additional cancel button is added
 *
 * \retval response of dialog can be one of
 *         GTK_RESPONSE_ACCEPT - save data and exit
 *         GTK_RESPONSE_REJECT - exit without saving
 *         GTK_RESPONSE_CANCEL - don't save and don't exit
 */
gint roccat_save_dialog(GtkWindow *parent, gchar const *text, gboolean cancel);

/*!
 * \brief asks the user if he want's to save data and exit
 *
 * The dialog is modal.
 *
 * If cancel is \c TRUE, an additional cancel button is added
 *
 * \retval response of dialog can be one of
 *         GTK_RESPONSE_ACCEPT - save data and exit
 *         GTK_RESPONSE_REJECT - exit without saving
 *         GTK_RESPONSE_CANCEL - don't save and don't exit
 */
gint roccat_save_unsaved_dialog(GtkWindow *parent, gboolean cancel);

G_END_DECLS

#endif
