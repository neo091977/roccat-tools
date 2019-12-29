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

#include "koneconfig_polling_rate_frame.h"
#include "roccat_polling_rate_selector.h"
#include "kone.h"
#include "i18n.h"

#define KONECONFIG_POLLING_RATE_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KONECONFIG_POLLING_RATE_FRAME_TYPE, KoneconfigPollingRateFrameClass))
#define IS_KONECONFIG_POLLING_RATE_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KONECONFIG_POLLING_RATE_FRAME_TYPE))
#define KONECONFIG_POLLING_RATE_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KONECONFIG_POLLING_RATE_FRAME_TYPE, KoneconfigPollingRateFramePrivate))

typedef struct _KoneconfigPollingRateFrameClass KoneconfigPollingRateFrameClass;
typedef struct _KoneconfigPollingRateFramePrivate KoneconfigPollingRateFramePrivate;

struct _KoneconfigPollingRateFrame {
	GtkFrame parent;
	KoneconfigPollingRateFramePrivate *priv;
};

struct _KoneconfigPollingRateFrameClass {
	GtkFrameClass parent_class;
};

struct _KoneconfigPollingRateFramePrivate {
	RoccatPollingRateSelector *selector;
};

G_DEFINE_TYPE(KoneconfigPollingRateFrame, koneconfig_polling_rate_frame, GTK_TYPE_FRAME);

GtkWidget *koneconfig_polling_rate_frame_new(void) {
	return GTK_WIDGET(g_object_new(KONECONFIG_POLLING_RATE_FRAME_TYPE, NULL));
}

static void koneconfig_polling_rate_frame_init(KoneconfigPollingRateFrame *frame) {
	frame->priv = KONECONFIG_POLLING_RATE_FRAME_GET_PRIVATE(frame);
	frame->priv->selector = ROCCAT_POLLING_RATE_SELECTOR(roccat_polling_rate_selector_new(
			ROCCAT_POLLING_RATE_SELECTOR_125 | ROCCAT_POLLING_RATE_SELECTOR_500 | ROCCAT_POLLING_RATE_SELECTOR_1000));
	gtk_container_add(GTK_CONTAINER(frame), GTK_WIDGET(frame->priv->selector));
	gtk_frame_set_label(GTK_FRAME(frame), _("Polling rate"));
}

static void koneconfig_polling_rate_frame_class_init(KoneconfigPollingRateFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(KoneconfigPollingRateFramePrivate));
}

void koneconfig_polling_rate_frame_set_value(KoneconfigPollingRateFrame *frame, guint rmp_value) {
	guint roccat_value;

	switch (rmp_value) {
	case KONE_POLLING_RATE_125:
		roccat_value = ROCCAT_POLLING_RATE_SELECTOR_125;
		break;
	case KONE_POLLING_RATE_500:
		roccat_value = ROCCAT_POLLING_RATE_SELECTOR_500;
		break;
	case KONE_POLLING_RATE_1000:
		roccat_value = ROCCAT_POLLING_RATE_SELECTOR_1000;
		break;
	default:
		g_warn_if_reached();
		roccat_value = ROCCAT_POLLING_RATE_SELECTOR_NONE;
		break;
	}

	roccat_polling_rate_selector_set_value(frame->priv->selector, roccat_value);
}

guint koneconfig_polling_rate_frame_get_value(KoneconfigPollingRateFrame *frame) {
	guint roccat_value;
	guint rmp_value;

	roccat_value = roccat_polling_rate_selector_get_value(frame->priv->selector);

	switch (roccat_value) {
	case ROCCAT_POLLING_RATE_SELECTOR_125:
		rmp_value = KONE_POLLING_RATE_125;
		break;
	case ROCCAT_POLLING_RATE_SELECTOR_500:
		rmp_value = KONE_POLLING_RATE_500;
		break;
	case ROCCAT_POLLING_RATE_SELECTOR_1000:
		rmp_value = KONE_POLLING_RATE_1000;
		break;
	default:
		g_warn_if_reached();
		__attribute__ ((fallthrough));
	case ROCCAT_POLLING_RATE_SELECTOR_NONE:
		rmp_value = KONE_POLLING_RATE_1000;
		break;
	}

	return rmp_value;
}
