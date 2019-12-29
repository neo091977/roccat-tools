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

#include "tyon_device.h"
#include "tyon_xcelerator.h"
#include "tyon_xcelerator_calibration_assistant.h"
#include "tyon_2d_positional.h"
#include "g_roccat_helper.h"
#include "roccat_device_hidraw.h"
#include "roccat_helper.h"
#include "roccat_warning_dialog.h"
#include <gaminggear/gaminggear_timeout_dialog.h>
#include "i18n.h"
#include <time.h>
#include "../libroccattyon/tyon_special.h"

#define TYON_XCELERATOR_CALIBRATION_ASSISTANT_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), TYON_XCELERATOR_CALIBRATION_ASSISTANT_TYPE, TyonXceleratorCalibrationAssistantClass))
#define IS_TYON_XCELERATOR_CALIBRATION_ASSISTANT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), TYON_XCELERATOR_CALIBRATION_ASSISTANT_TYPE))
#define TYON_XCELERATOR_CALIBRATION_ASSISTANT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), TYON_XCELERATOR_CALIBRATION_ASSISTANT_TYPE, TyonXceleratorCalibrationAssistantPrivate))

typedef struct _TyonXceleratorCalibrationAssistantClass TyonXceleratorCalibrationAssistantClass;
typedef struct _TyonXceleratorCalibrationAssistantPrivate TyonXceleratorCalibrationAssistantPrivate;

struct _TyonXceleratorCalibrationAssistant {
	GtkAssistant parent;
	TyonXceleratorCalibrationAssistantPrivate *priv;
};

struct _TyonXceleratorCalibrationAssistantClass {
	GtkAssistantClass parent_class;
};

struct _TyonXceleratorCalibrationAssistantPrivate {
	RoccatDevice *device;

	GtkWidget *progress_page;
	GtkProgressBar *progress_bar;
	GtkLabel *progress_label;
	Tyon2DPositional *positional;

	gint confirm_page_index;
	GtkWidget *confirm_page;

	gint summary_page_index;
	GtkWidget *summary_page;
	GtkLabel *summary_label;

	guint phase;
	time_t phase_start; /* clock() seems not to work in gtk apps */

	guint32 min, mid, max;

	guint last_value;
	guint count;
};

#define AVERAGE_SHIFT 7;
static guint const valid_count = 1 << AVERAGE_SHIFT;

enum {
	PHASE_MID = 0,
	PHASE_WAIT_CHANGE_MAX,
	PHASE_WAIT_TIME_MAX,
	PHASE_MAX,
	PHASE_WAIT_CHANGE_MIN,
	PHASE_WAIT_TIME_MIN,
	PHASE_MIN,
	PHASE_END,
};

static guint const next_phases[] = {
	PHASE_WAIT_CHANGE_MAX,
	PHASE_WAIT_TIME_MAX,
	PHASE_MAX,
	PHASE_WAIT_CHANGE_MIN,
	PHASE_WAIT_TIME_MIN,
	PHASE_MIN,
	PHASE_END,
	PHASE_END,
};

static gchar const * const phase_strings[4] = {
	N_("Please don't touch the paddle for center calibration."),
	N_("Please push the paddle all the way upwards."),
	N_("Please push the paddle all the way downwards."),
	""
};

static guint const phase_string_indices[] = { 0, 1, 1, 1, 2, 2, 2, 3 };

enum {
	COMPLETED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

G_DEFINE_TYPE(TyonXceleratorCalibrationAssistant, tyon_xcelerator_calibration_assistant, GTK_TYPE_ASSISTANT);

static void append_confirm_page(TyonXceleratorCalibrationAssistant *assistant) {
	TyonXceleratorCalibrationAssistantPrivate *priv = assistant->priv;

	priv->confirm_page = gtk_label_new(_("Finished!"));

	priv->confirm_page_index = gtk_assistant_append_page(GTK_ASSISTANT(assistant), priv->confirm_page);
	gtk_assistant_set_page_title(GTK_ASSISTANT(assistant), priv->confirm_page, _("Finish"));
	gtk_assistant_set_page_type(GTK_ASSISTANT(assistant), priv->confirm_page, GTK_ASSISTANT_PAGE_CONFIRM);
	gtk_assistant_set_page_complete(GTK_ASSISTANT(assistant), priv->confirm_page, TRUE);
}

static void append_progress_page(TyonXceleratorCalibrationAssistant *assistant) {
	TyonXceleratorCalibrationAssistantPrivate *priv = assistant->priv;
	GtkWidget *vbox;

	priv->progress_label = GTK_LABEL(gtk_label_new(NULL));
	priv->progress_page = gtk_hbox_new(FALSE, 0);
	priv->positional = TYON_2D_POSITIONAL(tyon_2d_positional_new());
	vbox = gtk_vbox_new(FALSE, 0);
	priv->progress_bar = GTK_PROGRESS_BAR(gtk_progress_bar_new());

	gtk_box_pack_start(GTK_BOX(priv->progress_page), GTK_WIDGET(priv->positional), FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(priv->progress_page), vbox, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(priv->progress_label), TRUE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(priv->progress_bar), TRUE, FALSE, 0);

	gtk_assistant_append_page(GTK_ASSISTANT(assistant), priv->progress_page);
	gtk_assistant_set_page_title(GTK_ASSISTANT(assistant), priv->progress_page, _("Calibration"));
	gtk_assistant_set_page_type(GTK_ASSISTANT(assistant), priv->progress_page, GTK_ASSISTANT_PAGE_PROGRESS);
	gtk_assistant_set_page_complete(GTK_ASSISTANT(assistant), priv->progress_page, FALSE);
}

static void append_intro_page(TyonXceleratorCalibrationAssistant *assistant) {
	GtkWidget *label;

	label = gtk_label_new(_("You can calibrate the X-Celerator analog thumb paddle after pressing next"));

	gtk_assistant_append_page(GTK_ASSISTANT(assistant), label);
	gtk_assistant_set_page_title(GTK_ASSISTANT(assistant), label, _("Start calibration"));
	gtk_assistant_set_page_type(GTK_ASSISTANT(assistant), label, GTK_ASSISTANT_PAGE_INTRO);
	gtk_assistant_set_page_complete(GTK_ASSISTANT(assistant), label, TRUE);
}

static void append_summary_page(TyonXceleratorCalibrationAssistant *assistant) {
	TyonXceleratorCalibrationAssistantPrivate *priv = assistant->priv;

	priv->summary_page = gtk_label_new(NULL);
	priv->summary_label = GTK_LABEL(priv->summary_page);

	priv->summary_page_index = gtk_assistant_append_page(GTK_ASSISTANT(assistant), priv->summary_page);
	gtk_assistant_set_page_title(GTK_ASSISTANT(assistant), priv->summary_page, _("Summary"));
	gtk_assistant_set_page_type(GTK_ASSISTANT(assistant), priv->summary_page, GTK_ASSISTANT_PAGE_SUMMARY);
	gtk_assistant_set_page_complete(GTK_ASSISTANT(assistant), priv->summary_page, TRUE);
}

static void set_phase(TyonXceleratorCalibrationAssistant *cal_assistant, guint phase) {
	TyonXceleratorCalibrationAssistantPrivate *priv = cal_assistant->priv;
	priv->phase = phase;
	priv->count = 0;
	priv->phase_start = time(NULL);
	gtk_label_set_text(priv->progress_label, _N(phase_strings[phase_string_indices[phase]]));
}

static void next_phase(TyonXceleratorCalibrationAssistant *cal_assistant) {
	TyonXceleratorCalibrationAssistantPrivate *priv = cal_assistant->priv;
	set_phase(cal_assistant, next_phases[priv->phase]);
}

static gboolean in_range(guint32 a, guint32 b, guint32 range) {
	return ((a - b) < range || (b - a) < range) ? TRUE : FALSE;
}

static void show_error_summary(TyonXceleratorCalibrationAssistant *assistant, gchar const * text) {
	TyonXceleratorCalibrationAssistantPrivate *priv = assistant->priv;
	gtk_label_set_text(priv->summary_label, text);
	gtk_assistant_set_current_page(GTK_ASSISTANT(assistant), priv->summary_page_index);
#if (GTK_CHECK_VERSION(2, 22, 0))
	gtk_assistant_commit(GTK_ASSISTANT(assistant));
#endif
}

static gboolean io_cb(GIOChannel *channel, GIOCondition condition, gpointer user_data) {
	TyonXceleratorCalibrationAssistant *assistant = (TyonXceleratorCalibrationAssistant *)user_data;
	TyonXceleratorCalibrationAssistantPrivate *priv = assistant->priv;
	GError *local_error = NULL;
	guint value;
	GIOStatus status;
	gsize length;
	TyonSpecial event;

	if (condition & G_IO_HUP)
		return FALSE;

	status = g_io_channel_read_chars(channel, (gchar *)&event, sizeof(TyonSpecial), &length, &local_error);

	if (status == G_IO_STATUS_AGAIN)
		return TRUE;

	if (status == G_IO_STATUS_ERROR) {
		show_error_summary(assistant, _("Could not read io_channel"));
		g_error_free(local_error);
		return FALSE;
	}

	if (status == G_IO_STATUS_EOF) {
		show_error_summary(assistant, _("Could not read io_channel"));
		return FALSE;
	}

	if (length < sizeof(TyonSpecial) ||
			event.report_id != TYON_REPORT_ID_SPECIAL ||
			event.type != TYON_SPECIAL_TYPE_XCELERATOR_CALIBRATION)
		return TRUE;

	value = event.action;
	tyon_2d_positional_set(priv->positional, 0.0, ((gdouble)value - 127.0) / 127.0);

	switch(priv->phase) {
	case PHASE_MID:
		if (in_range(value, priv->last_value, 10)) {
			priv->mid += value;
			++priv->count;
			gtk_progress_bar_set_fraction(priv->progress_bar, 1.0 / valid_count * priv->count);
			if (priv->count == valid_count) {
				priv->mid = priv->mid >> AVERAGE_SHIFT;
				next_phase(assistant);
			}
		} else {
			priv->mid = 0;
			priv->count = 0;
		}
		break;
	case PHASE_MAX:
		if (in_range(value, priv->last_value, 10)) {
			priv->max += value;
			++priv->count;
			gtk_progress_bar_set_fraction(priv->progress_bar, 1.0 / valid_count * priv->count);
			if (priv->count == valid_count) {
				priv->max = priv->max >> AVERAGE_SHIFT;
				next_phase(assistant);
			}
		} else {
			priv->max = 0;
			priv->count = 0;
		}
		break;
	case PHASE_MIN:
		if (in_range(value, priv->last_value, 10)) {
			priv->min += value;
			++priv->count;
			gtk_progress_bar_set_fraction(priv->progress_bar, 1.0 / valid_count * priv->count);
			if (priv->count == valid_count) {
				priv->min = priv->min >> AVERAGE_SHIFT;
				next_phase(assistant);
			}
		} else {
			priv->min = 0;
			priv->count = 0;
		}
		break;
	case PHASE_WAIT_TIME_MAX:
	case PHASE_WAIT_TIME_MIN:
		/* 1 sec grace period */
		if (difftime(time(NULL), priv->phase_start) >= 1.0)
			next_phase(assistant);
		break;
	case PHASE_WAIT_CHANGE_MAX: /* values < mid */
		if ((value + 20) < priv->mid)
			next_phase(assistant);
		break;
	case PHASE_WAIT_CHANGE_MIN: /* values > mid */
		if (value > (priv->mid + 20))
			next_phase(assistant);
		break;
	case PHASE_END:
		gtk_assistant_set_page_complete(GTK_ASSISTANT(assistant), priv->progress_page, TRUE);
		gtk_assistant_set_current_page(GTK_ASSISTANT(assistant), priv->confirm_page_index);
		gtk_label_set_text(priv->summary_label, _("All OK"));
		return FALSE;
		break;
	default:
		// FIXME error
		break;
	}

	priv->last_value = value;

	return TRUE;
}

static void prepare_cb(GtkAssistant *assistant, GtkWidget *page, gpointer user_data) {
	TyonXceleratorCalibrationAssistant *cal_assistant = TYON_XCELERATOR_CALIBRATION_ASSISTANT(assistant);
	TyonXceleratorCalibrationAssistantPrivate *priv = cal_assistant->priv;
	GError *local_error = NULL;

	if (page == priv->progress_page) {
		tyon_xcelerator_calibration_start(priv->device, &local_error);
		// FIXME error handling
		priv->max = 0;
		priv->min = 0;
		priv->mid = 0;
		priv->last_value = 0;
		set_phase(cal_assistant, PHASE_MID);
		(void)roccat_device_hidraw_add_event_watch(priv->device, TYON_INTERFACE_MISC, io_cb, cal_assistant, &local_error);
		// FIXME error handling
	}
}

static void close_cb(GtkAssistant *assistant, gpointer user_data) {
	TyonXceleratorCalibrationAssistant *cal_assistant = TYON_XCELERATOR_CALIBRATION_ASSISTANT(assistant);
	TyonXceleratorCalibrationAssistantPrivate *priv = cal_assistant->priv;
	GError *local_error = NULL;

	g_signal_emit((gpointer)cal_assistant, signals[COMPLETED], 0);
	tyon_xcelerator_calibration_end(priv->device, &local_error);
	// FIXME error handling
}

static void cancel_cb(GtkAssistant *assistant, gpointer user_data) {
	TyonXceleratorCalibrationAssistant *cal_assistant = TYON_XCELERATOR_CALIBRATION_ASSISTANT(assistant);
	TyonXceleratorCalibrationAssistantPrivate *priv = cal_assistant->priv;
	GError *local_error = NULL;

	/* close_cb is not called on cancel */
	g_signal_emit((gpointer)assistant, signals[COMPLETED], 0);
	tyon_xcelerator_calibration_end(priv->device, &local_error);
	// FIXME error handling
}

static void apply_cb(GtkAssistant *assistant, gpointer user_data) {
	TyonXceleratorCalibrationAssistant *cal_assistant = TYON_XCELERATOR_CALIBRATION_ASSISTANT(assistant);
	TyonXceleratorCalibrationAssistantPrivate *priv = cal_assistant->priv;
	GError *local_error = NULL;

	tyon_xcelerator_calibration_data(priv->device, priv->min, priv->mid, priv->max, &local_error);
	// FIXME error handling

	/* close_cb gets called after this */
}

GtkWidget *tyon_xcelerator_calibration_assistant_new(GtkWindow *parent, RoccatDevice *device) {
	TyonXceleratorCalibrationAssistant *assistant;
	TyonXceleratorCalibrationAssistantPrivate *priv;

	assistant = TYON_XCELERATOR_CALIBRATION_ASSISTANT(g_object_new(TYON_XCELERATOR_CALIBRATION_ASSISTANT_TYPE,
			NULL));

	priv = assistant->priv;
	priv->device = device;

	gtk_window_set_transient_for(GTK_WINDOW(assistant), parent);
	gtk_window_set_modal(GTK_WINDOW(assistant), TRUE);
	gtk_window_set_title(GTK_WINDOW(assistant), _("X-Celerator calibration"));

	return GTK_WIDGET(assistant);
}

static void tyon_xcelerator_calibration_assistant_init(TyonXceleratorCalibrationAssistant *assistant) {
	TyonXceleratorCalibrationAssistantPrivate *priv = TYON_XCELERATOR_CALIBRATION_ASSISTANT_GET_PRIVATE(assistant);
	assistant->priv = priv;

	append_intro_page(assistant);
	append_progress_page(assistant);
	append_confirm_page(assistant);
	append_summary_page(assistant);

	g_signal_connect(G_OBJECT(assistant), "prepare", G_CALLBACK(prepare_cb), NULL);
	g_signal_connect(G_OBJECT(assistant), "close", G_CALLBACK(close_cb), NULL);
	g_signal_connect(G_OBJECT(assistant), "cancel", G_CALLBACK(cancel_cb), NULL);
	g_signal_connect(G_OBJECT(assistant), "apply", G_CALLBACK(apply_cb), NULL);
}

static void tyon_xcelerator_calibration_assistant_class_init(TyonXceleratorCalibrationAssistantClass *klass) {
	g_type_class_add_private(klass, sizeof(TyonXceleratorCalibrationAssistantPrivate));

	signals[COMPLETED] = g_signal_new("completed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION, 0,
			NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}
