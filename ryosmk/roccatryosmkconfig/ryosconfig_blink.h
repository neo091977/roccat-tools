#ifndef __ROCCAT_RYOSCONFIG_BLINK_H__
#define __ROCCAT_RYOSCONFIG_BLINK_H__

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

#define RYOSCONFIG_BLINK_TYPE (ryosconfig_blink_get_type())
#define RYOSCONFIG_BLINK(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), RYOSCONFIG_BLINK_TYPE, RyosconfigBlink))
#define IS_RYOSCONFIG_BLINK(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), RYOSCONFIG_BLINK_TYPE))
#define RYOSCONFIG_BLINK_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), RYOSCONFIG_BLINK_TYPE, RyosconfigBlinkClass))
#define IS_RYOSCONFIG_BLINK_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), RYOSCONFIG_BLINK_TYPE))

typedef struct _RyosconfigBlink RyosconfigBlink;
typedef struct _RyosconfigBlinkClass RyosconfigBlinkClass;
typedef struct _RyosconfigBlinkPrivate RyosconfigBlinkPrivate;

struct _RyosconfigBlink {
	GObject parent;
	RyosconfigBlinkPrivate *priv;
};

struct _RyosconfigBlinkClass {
	GObjectClass parent_class;
};

GType ryosconfig_blink_get_type(void);
RyosconfigBlink *ryosconfig_blink_new(void);

void ryosconfig_blink_start(RyosconfigBlink *blink);
void ryosconfig_blink_stop(RyosconfigBlink *blink);

G_END_DECLS

#endif
