#ifndef __ROCCAT_SKELTR_RKP_H__
#define __ROCCAT_SKELTR_RKP_H__

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

#include "skeltr_profile_data.h"

G_BEGIN_DECLS

gpointer skeltr_profile_data_to_rkp(SkeltrProfileData const *profile_data, gsize *length);
gboolean skeltr_rkp_to_profile_data(SkeltrProfileData *profile_data, gconstpointer rkp, gsize length);

G_END_DECLS

#endif
