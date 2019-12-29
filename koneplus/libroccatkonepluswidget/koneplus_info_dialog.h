#ifndef __ROCCAT_KONEPLUS_INFO_DIALOG_H__
#define __ROCCAT_KONEPLUS_INFO_DIALOG_H__

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

#include "roccat_info_dialog.h"
#include "roccat_device.h"

G_BEGIN_DECLS

#define KONEPLUS_INFO_DIALOG_TYPE (koneplus_info_dialog_get_type())
#define KONEPLUS_INFO_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), KONEPLUS_INFO_DIALOG_TYPE, KoneplusInfoDialog))
#define IS_KONEPLUS_INFO_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), KONEPLUS_INFO_DIALOG_TYPE))

typedef struct _KoneplusInfoDialog KoneplusInfoDialog;
typedef struct _KoneplusInfoDialogPrivate KoneplusInfoDialogPrivate;

struct _KoneplusInfoDialog {
	RoccatInfoDialog parent;
	KoneplusInfoDialogPrivate *priv;
};

GType koneplus_info_dialog_get_type(void);
GtkWidget *koneplus_info_dialog_new(GtkWindow *parent, RoccatDevice *device);

G_END_DECLS

#endif
