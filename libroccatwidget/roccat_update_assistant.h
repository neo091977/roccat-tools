#ifndef __ROCCAT_UPDATE_ASSISTANT_H__
#define __ROCCAT_UPDATE_ASSISTANT_H__

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

#include "roccat_device.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define ROCCAT_UPDATE_ASSISTANT_TYPE (roccat_update_assistant_get_type())
#define ROCCAT_UPDATE_ASSISTANT(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ROCCAT_UPDATE_ASSISTANT_TYPE, RoccatUpdateAssistant))
#define IS_ROCCAT_UPDATE_ASSISTANT(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ROCCAT_UPDATE_ASSISTANT_TYPE))
#define ROCCAT_UPDATE_ASSISTANT_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ROCCAT_UPDATE_ASSISTANT_TYPE, RoccatUpdateAssistantClass))
#define IS_ROCCAT_UPDATE_ASSISTANT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ROCCAT_UPDATE_ASSISTANT_TYPE))

typedef struct _RoccatUpdateAssistant RoccatUpdateAssistant;
typedef struct _RoccatUpdateAssistantClass RoccatUpdateAssistantClass;
typedef struct _RoccatUpdateAssistantPrivate RoccatUpdateAssistantPrivate;

struct _RoccatUpdateAssistant {
	GtkAssistant parent;
	RoccatUpdateAssistantPrivate *priv;
};

struct _RoccatUpdateAssistantClass {
	GtkAssistantClass parent_class;
};

GType roccat_update_assistant_get_type(void);
GtkWidget *roccat_update_assistant_new(GtkWindow *parent, RoccatDevice *device, guint endpoint);

void roccat_update_assistant_add_firmware(RoccatUpdateAssistant *assistant, gchar const *name, gchar const *actual_version, guint size, guint8 number);

G_END_DECLS

#endif
