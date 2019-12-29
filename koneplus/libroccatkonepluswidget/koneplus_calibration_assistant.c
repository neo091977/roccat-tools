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

#include "koneplus_calibration_assistant.h"
#include "koneplus.h"
#include "g_roccat_helper.h"
#include "gdk_roccat_helper.h"
#include "roccat_warning_dialog.h"
#include <gaminggear/gaminggear_timeout_dialog.h>
#include "i18n-lib.h"

#define KONEPLUS_CALIBRATION_ASSISTANT_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KONEPLUS_CALIBRATION_ASSISTANT_TYPE, KoneplusCalibrationAssistantClass))
#define IS_KONEPLUS_CALIBRATION_ASSISTANT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KONEPLUS_CALIBRATION_ASSISTANT_TYPE))
#define KONEPLUS_CALIBRATION_ASSISTANT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KONEPLUS_CALIBRATION_ASSISTANT_TYPE, KoneplusCalibrationAssistantPrivate))

typedef struct _KoneplusCalibrationAssistantClass KoneplusCalibrationAssistantClass;
typedef struct _KoneplusCalibrationAssistantPrivate KoneplusCalibrationAssistantPrivate;

struct _KoneplusCalibrationAssistant {
	GtkAssistant parent;
	KoneplusCalibrationAssistantPrivate *priv;
};

struct _KoneplusCalibrationAssistantClass {
	GtkAssistantClass parent_class;
};

struct _KoneplusCalibrationAssistantPrivate {
	RoccatDevice *device;
	GtkWidget *test_page;
	GtkWidget *image_widget;
	GtkProgressBar *progress_bar;
	guint i;
	guint result;
	GtkWindow *parent;
	guint actual_dcu;
	guint tcu_value;
};

static const guint pixel_size = 4;

enum {
	COMPLETED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

G_DEFINE_TYPE(KoneplusCalibrationAssistant, koneplus_calibration_assistant, GTK_TYPE_ASSISTANT);

static void image_paint(GtkWidget *widget, KoneplusSensorImage const *image) {
	gint x, y;
	GdkColor color;
	gulong color_value;
	cairo_t *cr;
	GdkWindow *window;

	window = gtk_widget_get_window(widget);

	if (!GDK_IS_DRAWABLE(window))
		return;

	cr = gdk_cairo_create(window);
	cairo_set_line_width(cr, 1.0);
	cairo_set_line_cap(cr, CAIRO_LINE_CAP_SQUARE);

	for (x = 0; x < KONEPLUS_TCU_IMAGE_SIZE; ++x) {
		for (y = 0; y < KONEPLUS_TCU_IMAGE_SIZE; ++y) {

			/*
			 * The orientation is taken from the avago9500 data sheet
			 * page 20, Roccat driver flips x axis.
			 */
			color_value = image->data[30 * x + 29 - y];

			/* Color transformation is from roccat driver */
			color_value = (color_value * 4 + 24) * 256;

			/* cap */
			color_value = MIN(G_MAXUINT16, color_value);

			color.red = color_value;
			color.green = color_value;
			color.blue = color_value;
			gdk_cairo_set_source_color(cr, &color);
			cairo_rectangle(cr, x * pixel_size, y * pixel_size, pixel_size, pixel_size);
			cairo_fill(cr);
		}
	}
	cairo_destroy(cr);
}

static void append_finish_page(KoneplusCalibrationAssistant *assistant) {
	GtkWidget *label;

	label = gtk_label_new(_("Finished!"));

	gtk_assistant_append_page(GTK_ASSISTANT(assistant), label);
	gtk_assistant_set_page_title(GTK_ASSISTANT(assistant), label, _("Finish"));
	gtk_assistant_set_page_type(GTK_ASSISTANT(assistant), label, GTK_ASSISTANT_PAGE_CONFIRM);
	gtk_assistant_set_page_complete(GTK_ASSISTANT(assistant), label, TRUE);
}

static GtkWidget *image_widget_new(void) {
	GtkWidget *widget;

	widget = gtk_drawing_area_new();
	gtk_widget_set_size_request(widget, pixel_size * KONEPLUS_TCU_IMAGE_SIZE, pixel_size * KONEPLUS_TCU_IMAGE_SIZE);
	return widget;
}

static void append_test_page(KoneplusCalibrationAssistant *assistant) {
	KoneplusCalibrationAssistantPrivate *priv = assistant->priv;
	GtkWidget *vbox;

	priv->test_page = gtk_hbox_new(FALSE, 0);
	priv->image_widget = image_widget_new();
	vbox = gtk_vbox_new(FALSE, 0);
	priv->progress_bar = GTK_PROGRESS_BAR(gtk_progress_bar_new());

	gtk_box_pack_start(GTK_BOX(priv->test_page), priv->image_widget, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(priv->test_page), vbox, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(priv->progress_bar), TRUE, FALSE, 0);

	gtk_assistant_append_page(GTK_ASSISTANT(assistant), priv->test_page);
	gtk_assistant_set_page_title(GTK_ASSISTANT(assistant), priv->test_page, _("Surface test"));
	gtk_assistant_set_page_type(GTK_ASSISTANT(assistant), priv->test_page, GTK_ASSISTANT_PAGE_PROGRESS);
	gtk_assistant_set_page_complete(GTK_ASSISTANT(assistant), priv->test_page, FALSE);
}

static void append_intro_page(KoneplusCalibrationAssistant *assistant) {
	GtkWidget *label;

	label = gtk_label_new(_("Please don't move your mouse after pressing next"));

	gtk_assistant_append_page(GTK_ASSISTANT(assistant), label);
	gtk_assistant_set_page_title(GTK_ASSISTANT(assistant), label, _("Start calibration"));
	gtk_assistant_set_page_type(GTK_ASSISTANT(assistant), label, GTK_ASSISTANT_PAGE_INTRO);
	gtk_assistant_set_page_complete(GTK_ASSISTANT(assistant), label, TRUE);
}

static gboolean timeout_cb(gpointer user_data) {
	KoneplusCalibrationAssistant *assistant = (KoneplusCalibrationAssistant *)user_data;
	KoneplusCalibrationAssistantPrivate *priv = assistant->priv;
	KoneplusSensorImage *image;
	GError *error = NULL;
	guint median;

	// TODO no fixed count? sometimes 00 are returned.

	if (!koneplus_sensor_calibrate_step(priv->device, &error)) {
		g_warning(_("Could not calibrate TCU: %s"), error->message);
		return FALSE;
	}

	gtk_progress_bar_set_fraction(priv->progress_bar, priv->i * 0.2 + 0.1);

	/*
	 * Sometimes all zeroes are returned.
	 * Might be that Pixel_Burst register is read before data is ready.
	 */
	image = koneplus_sensor_image_read(priv->device, &error);
	if (!image) {
		g_message(_("Could not calibrate TCU: %s"), error->message);
		return FALSE;
	}

	median = koneplus_sensor_image_get_median(image);
	g_debug(_("Frame capture nr. %i median = %i"), priv->i, median);
	image_paint(priv->image_widget, image);
	g_free(image);

	gtk_progress_bar_set_fraction(priv->progress_bar, priv->i * 0.2 + 0.2);

	++priv->i;

	if (priv->i == 5) {
		priv->result = median;
		gtk_assistant_set_page_complete(GTK_ASSISTANT(assistant), priv->test_page, TRUE);
		return FALSE;
	}

	return TRUE;
}

static void prepare_cb(GtkAssistant *assistant, GtkWidget *page, gpointer user_data) {
	KoneplusCalibrationAssistant *cal_assistant = KONEPLUS_CALIBRATION_ASSISTANT(assistant);
	KoneplusCalibrationAssistantPrivate *priv = cal_assistant->priv;

	if (page == priv->test_page) {
		priv->i = 0;
		g_timeout_add(0.5 * G_ROCCAT_MSEC_PER_SEC, timeout_cb, cal_assistant);
	}
}

static void close_cb(GtkAssistant *assistant, gpointer user_data) {
	KoneplusCalibrationAssistant *cal_assistant = KONEPLUS_CALIBRATION_ASSISTANT(assistant);

	g_debug(_("TCU calibration closed"));

	g_signal_emit((gpointer)cal_assistant, signals[COMPLETED], 0);
}

static void cancel_cb(GtkAssistant *assistant, gpointer user_data) {
	KoneplusCalibrationAssistant *cal_assistant = KONEPLUS_CALIBRATION_ASSISTANT(assistant);
	KoneplusCalibrationAssistantPrivate *priv = cal_assistant->priv;
	GError *local_error = NULL;

	g_debug(_("TCU calibration cancelled"));

	koneplus_tracking_control_unit_cancel(priv->device, priv->actual_dcu, &local_error);
	roccat_handle_error_dialog(priv->parent, _("Could not set TCU data"), &local_error);

	priv->tcu_value = KONEPLUS_TRACKING_CONTROL_UNIT_OFF;

	/* close is not issued on cancel */
	g_signal_emit((gpointer)assistant, signals[COMPLETED], 0);
}

static void apply_cb(GtkAssistant *assistant, gpointer user_data) {
	KoneplusCalibrationAssistant *cal_assistant = KONEPLUS_CALIBRATION_ASSISTANT(assistant);
	KoneplusCalibrationAssistantPrivate *priv = cal_assistant->priv;
	GError *local_error = NULL;
	gboolean answer;

	koneplus_tracking_control_unit_test(priv->device, priv->actual_dcu, priv->result, &local_error);
	if (!roccat_handle_error_dialog(priv->parent, _("Could not set TCU data"), &local_error))
		return;

	answer = gaminggear_timeout_dialog(priv->parent,
			_("Keep settings?"),
			_("Do you want to keep this settings?"), 20);

	if (answer) {
		g_debug(_("TCU calibrated to %i"), priv->result);
		koneplus_tracking_control_unit_accept(priv->device, priv->actual_dcu, priv->result, &local_error);
		priv->tcu_value = KONEPLUS_TRACKING_CONTROL_UNIT_ON;
	} else {
		koneplus_tracking_control_unit_cancel(priv->device, priv->actual_dcu, &local_error);
		priv->tcu_value = KONEPLUS_TRACKING_CONTROL_UNIT_OFF;
	}
	if (!roccat_handle_error_dialog(priv->parent, _("Could not set TCU data"), &local_error))
		return;

	/* close gets issued after this */
}

guint koneplus_calibration_assistant_get_tcu_value(KoneplusCalibrationAssistant *cal_assistant) {
	return cal_assistant->priv->tcu_value;
}

GtkWidget *koneplus_calibration_assistant_new(GtkWindow *parent, RoccatDevice *device, guint actual_dcu) {
	KoneplusCalibrationAssistant *assistant;
	KoneplusCalibrationAssistantPrivate *priv;

	assistant = KONEPLUS_CALIBRATION_ASSISTANT(g_object_new(KONEPLUS_CALIBRATION_ASSISTANT_TYPE,
			NULL));

	priv = assistant->priv;
	priv->device = device;
	priv->parent = parent;
	priv->actual_dcu = actual_dcu;

	gtk_window_set_transient_for(GTK_WINDOW(assistant), parent);
	gtk_window_set_modal(GTK_WINDOW(assistant), TRUE);
	gtk_window_set_title(GTK_WINDOW(assistant), _("TCU calibration"));

	return GTK_WIDGET(assistant);
}

static void koneplus_calibration_assistant_init(KoneplusCalibrationAssistant *assistant) {
	KoneplusCalibrationAssistantPrivate *priv = KONEPLUS_CALIBRATION_ASSISTANT_GET_PRIVATE(assistant);
	assistant->priv = priv;

	append_intro_page(assistant);
	append_test_page(assistant);
	append_finish_page(assistant);

	g_signal_connect(G_OBJECT(assistant), "prepare", G_CALLBACK(prepare_cb), NULL);
	g_signal_connect(G_OBJECT(assistant), "close", G_CALLBACK(close_cb), NULL);
	g_signal_connect(G_OBJECT(assistant), "cancel", G_CALLBACK(cancel_cb), NULL);
	g_signal_connect(G_OBJECT(assistant), "apply", G_CALLBACK(apply_cb), NULL);
}

static void koneplus_calibration_assistant_class_init(KoneplusCalibrationAssistantClass *klass) {
	g_type_class_add_private(klass, sizeof(KoneplusCalibrationAssistantPrivate));

	signals[COMPLETED] = g_signal_new("completed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION, 0,
			NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}
