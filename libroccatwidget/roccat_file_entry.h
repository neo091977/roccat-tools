#ifndef __ROCCAT_FILE_ENTRY_H__
#define __ROCCAT_FILE_ENTRY_H__

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

/*
 * roccat_file_entry is a entry coupled with a button on the right side opening a file_chooser_dialog
 */

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define ROCCAT_FILE_ENTRY_TYPE (roccat_file_entry_get_type())
#define ROCCAT_FILE_ENTRY(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ROCCAT_FILE_ENTRY_TYPE, RoccatFileEntry))
#define IS_ROCCAT_FILE_ENTRY(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ROCCAT_FILE_ENTRY_TYPE))

typedef struct _RoccatFileEntry RoccatFileEntry;

GType roccat_file_entry_get_type(void);
GtkWidget *roccat_file_entry_new(void);

gchar const *roccat_file_entry_get_text(RoccatFileEntry *file_entry);
void roccat_file_entry_set_text(RoccatFileEntry *file_entry, gchar const *new_value);

G_END_DECLS

#endif
