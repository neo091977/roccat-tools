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

#include "koneconfig_color_frame.h"
#include "roccat_color_selection_button.h"
#include "gdk_roccat_helper.h"
#include "i18n.h"

#define KONECONFIG_COLOR_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KONECONFIG_COLOR_FRAME_TYPE, KoneconfigColorFrameClass))
#define IS_KONECONFIG_COLOR_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KONECONFIG_COLOR_FRAME_TYPE))
#define KONECONFIG_COLOR_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KONECONFIG_COLOR_FRAME_TYPE, KoneconfigColorFramePrivate))

typedef struct _KoneconfigColorFrameClass KoneconfigColorFrameClass;
typedef struct _KoneconfigColorFramePrivate KoneconfigColorFramePrivate;

struct _KoneconfigColorFrame {
	GtkFrame parent;
	KoneconfigColorFramePrivate *priv;
};

struct _KoneconfigColorFrameClass {
	GtkFrameClass parent_class;
};

struct _KoneconfigColorFramePrivate {
	RoccatColorSelectionButton *color_buttons[KONE_LIGHT_INFO_NUM];
	GtkToggleButton *check_buttons[KONE_LIGHT_INFO_NUM];
};

G_DEFINE_TYPE(KoneconfigColorFrame, koneconfig_color_frame, GTK_TYPE_FRAME);

static void light_info_to_color(KoneRMPLightInfo const *light_info, GdkColor *color) {
	color->pixel = 0;
	color->red = light_info->red * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR;
	color->green = light_info->green * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR;
	color->blue = light_info->blue * GDK_ROCCAT_BYTE_TO_COLOR_FACTOR;
}

static void color_to_light_info(GdkColor const *color, KoneRMPLightInfo *light_info) {
	light_info->red = color->red / GDK_ROCCAT_BYTE_TO_COLOR_FACTOR;
	light_info->green = color->green / GDK_ROCCAT_BYTE_TO_COLOR_FACTOR;
	light_info->blue = color->blue / GDK_ROCCAT_BYTE_TO_COLOR_FACTOR;
}

void koneconfig_color_frame_set_from_rmp(KoneconfigColorFrame *frame, KoneRMP *rmp) {
	KoneconfigColorFramePrivate *priv = frame->priv;
	KoneRMPLightInfo *light_info;
	GdkColor color;
	guint i;

	for (i = 0; i < KONE_LIGHT_INFO_NUM; ++i) {
		light_info = kone_rmp_get_rmp_light_info(rmp, i);
		light_info_to_color(light_info, &color);
		roccat_color_selection_button_set_custom_color(priv->color_buttons[i], &color);
		gtk_toggle_button_set_active(priv->check_buttons[i], light_info->mod == KONE_LIGHT_INFO_MOD_ON);
		g_free(light_info);
	}
}

void koneconfig_color_frame_update_rmp(KoneconfigColorFrame *frame, KoneRMP *rmp) {
	KoneconfigColorFramePrivate *priv = frame->priv;
	guint i;
	KoneRMPLightInfo light_info;
	GdkColor color;

	for (i = 0; i < KONE_LIGHT_INFO_NUM; ++i) {
		roccat_color_selection_button_get_custom_color(priv->color_buttons[i], &color);
		color_to_light_info(&color, &light_info);

		light_info.mod = gtk_toggle_button_get_active(priv->check_buttons[i]) ? KONE_LIGHT_INFO_MOD_ON : KONE_LIGHT_INFO_MOD_OFF;
		kone_rmp_set_rmp_light_info(rmp, i, &light_info);
	}
}

GtkWidget *koneconfig_color_frame_new(void) {
	KoneconfigColorFrame *koneconfig_color_frame;

	koneconfig_color_frame = KONECONFIG_COLOR_FRAME(g_object_new(KONECONFIG_COLOR_FRAME_TYPE,
			NULL));

	return GTK_WIDGET(koneconfig_color_frame);
}

static void koneconfig_color_frame_init(KoneconfigColorFrame *frame) {
	KoneconfigColorFramePrivate *priv = KONECONFIG_COLOR_FRAME_GET_PRIVATE(frame);
	GtkWidget *table;
	GtkWidget *box;
	guint i;

	/* order is in KoneLightInfoIndex */
	struct {
		guint left_attach, right_attach, top_attach, bottom_attach;
	} colors[KONE_LIGHT_INFO_NUM] = {
			{0, 1, 0, 1},
			{0, 1, 1, 2},
			{1, 2, 0, 1},
			{1, 2, 1, 2},
			{0, 2, 2, 3}
	};

	frame->priv = priv;

	table = gtk_table_new(3, 2, TRUE);
	gtk_container_add(GTK_CONTAINER(frame), table);

	for (i = 0; i < KONE_LIGHT_INFO_NUM; ++i) {
		box = gtk_hbox_new(FALSE, 0);

		priv->color_buttons[i] = ROCCAT_COLOR_SELECTION_BUTTON(roccat_color_selection_button_new());
		roccat_color_selection_button_set_view_type(priv->color_buttons[i], ROCCAT_COLOR_SELECTION_DIALOG_TYPE_CUSTOM);
		gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(priv->color_buttons[i]), FALSE, FALSE, 0);

		priv->check_buttons[i] = GTK_TOGGLE_BUTTON(gtk_check_button_new());
		gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(priv->check_buttons[i]), FALSE, FALSE, 0);

		gtk_table_attach(GTK_TABLE(table), box,
				colors[i].left_attach, colors[i].right_attach,
				colors[i].top_attach, colors[i].bottom_attach,
				GTK_EXPAND, GTK_EXPAND, 0, 0);
	}

	gtk_frame_set_label(GTK_FRAME(frame), _("Colors"));
}

static void koneconfig_color_frame_class_init(KoneconfigColorFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(KoneconfigColorFramePrivate));
}
