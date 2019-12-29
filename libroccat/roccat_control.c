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

#include "roccat_control.h"
#include "roccat_device_hidraw.h"
#include "g_roccat_helper.h"
#include "i18n-lib.h"

typedef struct _RoccatControl RoccatControl;

struct _RoccatControl {
	guint8 report_id;
	guint8 value;
	guint8 request;
} __attribute__ ((packed));

typedef enum {
	ROCCAT_CONTROL_VALUE_STATUS_CRITICAL_1 = 0,
	ROCCAT_CONTROL_VALUE_STATUS_OK = 1,
	ROCCAT_CONTROL_VALUE_STATUS_INVALID = 2,
	ROCCAT_CONTROL_VALUE_STATUS_BUSY = 3,
	ROCCAT_CONTROL_VALUE_STATUS_CRITICAL_2 = 4, /* used by Ryos MK */
} RoccatControlValue;

gboolean roccat_select(RoccatDevice *device, guint endpoint, guint report_id, guint profile_index, guint request, GError **error) {
	RoccatControl control;
	gboolean retval;

	control.report_id = report_id;
	control.value = profile_index;
	control.request = request;

	retval = roccat_device_hidraw_write(device, endpoint, (gchar const *)&control, sizeof(RoccatControl), error);

	return retval;
}

gboolean roccat_check_write_immediate(RoccatDevice *device, guint endpoint, guint report_id, guint busy_wait, GError **error) {
	RoccatControl *control;
	guint8 value;
	guint increasing_wait = busy_wait;

	while (TRUE) {
		control = (RoccatControl *)roccat_device_hidraw_read(device, endpoint, report_id, sizeof(RoccatControl), error);
		if (!control)
			return FALSE;
		value = control->value;
		g_free(control);
		switch (value) {
		case ROCCAT_CONTROL_VALUE_STATUS_OK:
			return TRUE;
			break;
		case ROCCAT_CONTROL_VALUE_STATUS_BUSY:
			g_usleep(increasing_wait * G_ROCCAT_USEC_PER_MSEC);
			increasing_wait += busy_wait;
			break;
		case ROCCAT_CONTROL_VALUE_STATUS_CRITICAL_1:
		case ROCCAT_CONTROL_VALUE_STATUS_CRITICAL_2:
			g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_INVAL, _("Got critical status"));
			return FALSE;
			break;
		case ROCCAT_CONTROL_VALUE_STATUS_INVALID:
			g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_INVAL, _("Got invalid status"));
			return FALSE;
			break;
		default:
			g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_FAILED, _("Got unknown error"));
			return FALSE;
			break;
		}
	};
}

gboolean roccat_check_write(RoccatDevice *device, guint endpoint, guint report_id, guint init_wait, guint busy_wait, GError **error) {
	g_usleep(init_wait * G_ROCCAT_USEC_PER_MSEC);
	return roccat_check_write_immediate(device, endpoint, report_id, busy_wait, error);
}
