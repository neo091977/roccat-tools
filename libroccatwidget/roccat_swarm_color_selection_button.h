#ifndef __ROCCAT_SWARM_COLOR_SELECTION_BUTTON_H__
#define __ROCCAT_SWARM_COLOR_SELECTION_BUTTON_H__

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
 * RoccatSwarmColorSelectionButton works like GtkColorButton but provides same look and
 * feel as RoccatColorPaletteButton
 */

#include "roccat_color_selection_button.h"

G_BEGIN_DECLS

enum {
	ROCCAT_SWARM_COLORS_NUM = 30,
};

#define ROCCAT_SWARM_COLOR_SELECTION_BUTTON_TYPE (roccat_swarm_color_selection_button_get_type())
#define ROCCAT_SWARM_COLOR_SELECTION_BUTTON_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ROCCAT_SWARM_COLOR_SELECTION_BUTTON_TYPE, RoccatSwarmColorSelectionButtonClass))
#define IS_ROCCAT_SWARM_COLOR_SELECTION_BUTTON_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ROCCAT_SWARM_COLOR_SELECTION_BUTTON_TYPE))
#define ROCCAT_SWARM_COLOR_SELECTION_BUTTON(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ROCCAT_SWARM_COLOR_SELECTION_BUTTON_TYPE, RoccatSwarmColorSelectionButton))
#define IS_ROCCAT_SWARM_COLOR_SELECTION_BUTTON(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ROCCAT_SWARM_COLOR_SELECTION_BUTTON_TYPE))

typedef struct _RoccatSwarmColorSelectionButtonClass RoccatSwarmColorSelectionButtonClass;
typedef struct _RoccatSwarmColorSelectionButton RoccatSwarmColorSelectionButton;

struct _RoccatSwarmColorSelectionButtonClass {
	RoccatColorSelectionButtonClass parent_class;
};

struct _RoccatSwarmColorSelectionButton {
	RoccatColorSelectionButton parent;
};

GType roccat_swarm_color_selection_button_get_type(void);
GtkWidget *roccat_swarm_color_selection_button_new(void);

G_END_DECLS

#endif
