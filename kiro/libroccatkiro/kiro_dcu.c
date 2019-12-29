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

#include "kiro_dcu.h"
#include "kiro_device.h"

KiroDcu *kiro_dcu_read(RoccatDevice *kiro, GError **error) {
	return (KiroDcu *)kiro_device_read(kiro, KIRO_REPORT_ID_DCU, sizeof(KiroDcu), error);
}

static gboolean kiro_dcu_write(RoccatDevice *kiro, KiroDcu const *data, GError **error) {
	return kiro_device_write(kiro, (char const *)data, sizeof(KiroDcu), error);
}

guint kiro_dcu_get(RoccatDevice *kiro, GError **error) {
	KiroDcu *dcu;
	guint retval;

	dcu = kiro_dcu_read(kiro, error);
	if (!dcu)
		return 0;

	retval = dcu->dcu;
	g_free(dcu);
	return retval;
}

gboolean kiro_dcu_set(RoccatDevice *kiro, guint new_dcu, GError **error) {
	KiroDcu dcu;

	dcu.report_id = KIRO_REPORT_ID_DCU;
	dcu.size = 3;
	dcu.dcu = new_dcu;
	dcu.unused = 0;

	return kiro_dcu_write(kiro, &dcu, error);
}
