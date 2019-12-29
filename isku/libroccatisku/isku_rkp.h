#ifndef __ROCCAT_ISKU_RKP_H__
#define __ROCCAT_ISKU_RKP_H__

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

#include "isku.h"

G_BEGIN_DECLS

struct _IskuRkp {
	gboolean modified_rkp; /* means things only in rkp like profile name */
	gboolean modified_light;
	gboolean modified_key_mask;
	gboolean modified_keys[ISKU_KEY_INDEX_NUM];
	GKeyFile *key_file;
	GKeyFile *default_key_file;
};

guint isku_rkp_get_value(IskuRkp *rkp, gchar const *key);
void isku_rkp_set_value(IskuRkp *rkp, gchar const *key, guint value);

G_END_DECLS

#endif
