#ifndef __ROCCAT_COLOR_SELECTION_DIALOG_H__
#define __ROCCAT_COLOR_SELECTION_DIALOG_H__

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

#include "roccat_color_palette_selector.h"

G_BEGIN_DECLS

#define ROCCAT_COLOR_SELECTION_DIALOG_TYPE (roccat_color_selection_dialog_get_type())
#define ROCCAT_COLOR_SELECTION_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ROCCAT_COLOR_SELECTION_DIALOG_TYPE, RoccatColorSelectionDialog))
#define IS_ROCCAT_COLOR_SELECTION_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ROCCAT_COLOR_SELECTION_DIALOG_TYPE))

typedef struct _RoccatColorSelectionDialog RoccatColorSelectionDialog;
typedef struct _RoccatColorSelectionDialogPrivate RoccatColorSelectionDialogPrivate;

struct _RoccatColorSelectionDialog {
	GtkDialog dialog;
	RoccatColorSelectionDialogPrivate *priv;
};

typedef enum {
	ROCCAT_COLOR_SELECTION_DIALOG_TYPE_PALETTE,
	ROCCAT_COLOR_SELECTION_DIALOG_TYPE_CUSTOM,
	ROCCAT_COLOR_SELECTION_DIALOG_TYPE_BOTH,
} RoccatColorSelectionDialogType;

GType roccat_color_selection_dialog_get_type(void);

GtkWidget *roccat_color_selection_dialog_new(GtkWindow *parent);

void roccat_color_selection_dialog_set_view_type(RoccatColorSelectionDialog *color_selection_dialog, RoccatColorSelectionDialogType type);

RoccatColorPaletteSelector *roccat_color_selection_dialog_get_palette_selector(RoccatColorSelectionDialog *color_selection_dialog);
GtkColorSelection *roccat_color_selection_dialog_get_custom_selector(RoccatColorSelectionDialog *color_selection_dialog);
RoccatColorSelectionDialogType roccat_color_selection_dialog_get_active_type(RoccatColorSelectionDialog *color_selection_dialog);

G_END_DECLS

#endif
