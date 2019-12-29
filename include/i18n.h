#ifndef __ROCCAT_I18N_H__
#define __ROCCAT_I18N_H__

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

#include <glib/gi18n.h>

/* That's meant to be used for the action that translates N_ marked strings.
   Do not add to xgettext keywords.
 */
#define _N(Reference) _(Reference)
#define _NC(Context,Reference) C_(Context,Reference)

#endif
