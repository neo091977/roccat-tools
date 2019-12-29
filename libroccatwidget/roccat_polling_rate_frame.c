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

#include "roccat_polling_rate_frame.h"
#include "roccat_polling_rate_selector.h"
#include "i18n-lib.h"

#define ROCCAT_POLLING_RATE_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ROCCAT_POLLING_RATE_FRAME_TYPE, RoccatPollingRateFrameClass))
#define IS_ROCCAT_POLLING_RATE_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ROCCAT_POLLING_RATE_FRAME_TYPE))
#define ROCCAT_POLLING_RATE_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ROCCAT_POLLING_RATE_FRAME_TYPE, RoccatPollingRateFramePrivate))

struct _RoccatPollingRateFramePrivate {
	RoccatPollingRateSelector *selector;
};

G_DEFINE_TYPE(RoccatPollingRateFrame, roccat_polling_rate_frame, GTK_TYPE_FRAME);

RoccatPollingRate roccat_polling_rate_frame_get_value(RoccatPollingRateFrame *frame) {
	guint roccat_value;
	guint value;

	roccat_value = roccat_polling_rate_selector_get_value(frame->priv->selector);

	switch (roccat_value) {
	case ROCCAT_POLLING_RATE_SELECTOR_125:
		value = ROCCAT_POLLING_RATE_125;
		break;
	case ROCCAT_POLLING_RATE_SELECTOR_250:
		value = ROCCAT_POLLING_RATE_250;
		break;
	case ROCCAT_POLLING_RATE_SELECTOR_500:
		value = ROCCAT_POLLING_RATE_500;
		break;
	case ROCCAT_POLLING_RATE_SELECTOR_1000:
		value = ROCCAT_POLLING_RATE_1000;
		break;
	case ROCCAT_POLLING_RATE_SELECTOR_NONE:
		value = ROCCAT_POLLING_RATE_1000;
		break;
	default:
		g_warn_if_reached();
		value = ROCCAT_POLLING_RATE_1000;
		break;
	}

	return value;
}

void roccat_polling_rate_frame_set_value(RoccatPollingRateFrame *frame, RoccatPollingRate new_value) {
	guint roccat_value;

	switch (new_value) {
	case ROCCAT_POLLING_RATE_125:
		roccat_value = ROCCAT_POLLING_RATE_SELECTOR_125;
		break;
	case ROCCAT_POLLING_RATE_250:
		roccat_value = ROCCAT_POLLING_RATE_SELECTOR_250;
		break;
	case ROCCAT_POLLING_RATE_500:
		roccat_value = ROCCAT_POLLING_RATE_SELECTOR_500;
		break;
	case ROCCAT_POLLING_RATE_1000:
		roccat_value = ROCCAT_POLLING_RATE_SELECTOR_1000;
		break;
	default:
		g_warn_if_reached();
		roccat_value = ROCCAT_POLLING_RATE_SELECTOR_NONE;
		break;
	}

	roccat_polling_rate_selector_set_value(frame->priv->selector, roccat_value);
}

GtkWidget *roccat_polling_rate_frame_new(void) {
	return GTK_WIDGET(g_object_new(ROCCAT_POLLING_RATE_FRAME_TYPE, NULL));
}

static void roccat_polling_rate_frame_init(RoccatPollingRateFrame *frame) {
	frame->priv = ROCCAT_POLLING_RATE_FRAME_GET_PRIVATE(frame);
	frame->priv->selector = ROCCAT_POLLING_RATE_SELECTOR(roccat_polling_rate_selector_new(ROCCAT_POLLING_RATE_SELECTOR_ALL));
	gtk_container_add(GTK_CONTAINER(frame), GTK_WIDGET(frame->priv->selector));
	gtk_frame_set_label(GTK_FRAME(frame), _("Polling rate"));
}

static void roccat_polling_rate_frame_class_init(RoccatPollingRateFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(RoccatPollingRateFramePrivate));
}
