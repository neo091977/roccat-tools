#ifndef __ROCCAT_KONEPUREOPTICALCONFIG_CPI_SCALE_H__
#define __ROCCAT_KONEPUREOPTICALCONFIG_CPI_SCALE_H__

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
 * konepureopticalconfig_cpi_scale is a hscale coupled with a spinbutton on the right side
 */

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define KONEPUREOPTICALCONFIG_CPI_SCALE_TYPE (konepureopticalconfig_cpi_scale_get_type())
#define KONEPUREOPTICALCONFIG_CPI_SCALE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), KONEPUREOPTICALCONFIG_CPI_SCALE_TYPE, KonepureopticalconfigCpiScale))
#define IS_KONEPUREOPTICALCONFIG_CPI_SCALE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), KONEPUREOPTICALCONFIG_CPI_SCALE_TYPE))

typedef struct _KonepureopticalconfigCpiScale KonepureopticalconfigCpiScale;
typedef struct _KonepureopticalconfigCpiScalePrivate KonepureopticalconfigCpiScalePrivate;

struct _KonepureopticalconfigCpiScale {
	GtkHBox hbox;
	KonepureopticalconfigCpiScalePrivate *priv;
};

GType konepureopticalconfig_cpi_scale_get_type(void);
GtkWidget *konepureopticalconfig_cpi_scale_new(void);

guint konepureopticalconfig_cpi_scale_get_value(KonepureopticalconfigCpiScale *hscale);
void konepureopticalconfig_cpi_scale_set_value_blocked(KonepureopticalconfigCpiScale *hscale, guint rmp_value);

G_END_DECLS

#endif
