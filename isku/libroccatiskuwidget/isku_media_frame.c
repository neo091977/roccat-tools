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

#include "isku_media_frame.h"
#include "isku_key_combo_box.h"
#include "i18n-lib.h"

#define ISKU_MEDIA_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ISKU_MEDIA_FRAME_TYPE, IskuMediaFrameClass))
#define IS_ISKU_MEDIA_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ISKU_MEDIA_FRAME_TYPE))
#define ISKU_MEDIA_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ISKU_MEDIA_FRAME_TYPE, IskuMediaFramePrivate))

typedef struct _IskuMediaFrameClass IskuMediaFrameClass;
typedef struct _IskuMediaFramePrivate IskuMediaFramePrivate;

struct _IskuMediaFrame {
	GtkFrame parent;
	IskuMediaFramePrivate *priv;
};

struct _IskuMediaFrameClass {
	GtkFrameClass parent_class;
};

struct _IskuMediaFramePrivate {
	IskuKeyComboBox *buttons[ISKU_KEYS_MEDIA_NUM];
	guint button_rkp_index[ISKU_KEYS_MEDIA_NUM];
};

G_DEFINE_TYPE(IskuMediaFrame, isku_media_frame, GTK_TYPE_FRAME);

void isku_media_frame_set_key(IskuMediaFrame *media_frame, guint key_index, IskuRkpMacroKeyInfo *key_info) {
	IskuMediaFramePrivate *priv = media_frame->priv;
	guint i;
	for (i = 0; i < ISKU_KEYS_MEDIA_NUM; ++i)
		if (priv->button_rkp_index[i] == key_index)
			isku_key_combo_box_set_value_blocked(priv->buttons[i], key_info);
}

typedef struct {
	gchar const * const name;
	guint index;
	guint standard_type;
} ButtonList;

static ButtonList button_list[ISKU_KEYS_MEDIA_NUM] = {
	{N_("Mute"), ISKU_KEY_INDEX_MEDIA_MUTE, ISKU_KEY_TYPE_STANDARD_MULTIMEDIA_MUTE},
	{N_("Down"), ISKU_KEY_INDEX_MEDIA_DOWN, ISKU_KEY_TYPE_STANDARD_MULTIMEDIA_VOLUME_DOWN},
	{N_("Up"), ISKU_KEY_INDEX_MEDIA_UP, ISKU_KEY_TYPE_STANDARD_MULTIMEDIA_VOLUME_UP},
	{N_("Play"), ISKU_KEY_INDEX_MEDIA_PLAY, ISKU_KEY_TYPE_STANDARD_MULTIMEDIA_PLAY_PAUSE},
	{N_("Prev"), ISKU_KEY_INDEX_MEDIA_PREV, ISKU_KEY_TYPE_STANDARD_MULTIMEDIA_PREV_TRACK},
	{N_("Next"), ISKU_KEY_INDEX_MEDIA_NEXT, ISKU_KEY_TYPE_STANDARD_MULTIMEDIA_NEXT_TRACK},
	{N_("WWW"), ISKU_KEY_INDEX_MEDIA_WWW, ISKU_KEY_TYPE_STANDARD_APP_IE_BROWSER},
	{N_("Computer"), ISKU_KEY_INDEX_MEDIA_COMPUTER, ISKU_KEY_TYPE_STANDARD_APP_MY_COMPUTER},
};

GtkWidget *isku_media_frame_new(void) {
	IskuMediaFrame *frame;

	frame = ISKU_MEDIA_FRAME(g_object_new(ISKU_MEDIA_FRAME_TYPE,
			"label", _("Media keys"),
			NULL));

	return GTK_WIDGET(frame);
}

static void isku_media_frame_init(IskuMediaFrame *frame) {
	IskuMediaFramePrivate *priv = ISKU_MEDIA_FRAME_GET_PRIVATE(frame);
	GtkWidget *table;
	guint i;

	frame->priv = priv;

	table = gtk_table_new(ISKU_KEYS_MEDIA_NUM, 2, FALSE);
	gtk_container_add(GTK_CONTAINER(frame), table);

	for (i = 0; i < ISKU_KEYS_MEDIA_NUM; ++i) {
		priv->button_rkp_index[i] = button_list[i].index;
		priv->buttons[i] = ISKU_KEY_COMBO_BOX(
				isku_key_combo_box_new(ROCCAT_KEY_COMBO_BOX_GROUP_MACRO, button_list[i].standard_type));
		gtk_table_attach(GTK_TABLE(table), gtk_label_new(_N(button_list[i].name)), 0, 1, i, i + 1, GTK_EXPAND, GTK_EXPAND, 0, 0);
		gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(priv->buttons[i]), 1, 2, i, i + 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);
	}
}

static void isku_media_frame_class_init(IskuMediaFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(IskuMediaFramePrivate));
}

void isku_media_frame_set_from_rkp(IskuMediaFrame *buttons_frame, IskuRkp *rkp) {
	IskuMediaFramePrivate *priv = buttons_frame->priv;
	guint i;
	IskuRkpMacroKeyInfo *key_info;

	for (i = 0; i < ISKU_KEYS_MEDIA_NUM; ++i) {
		key_info = isku_rkp_get_macro_key_info(rkp, priv->button_rkp_index[i]);
		isku_key_combo_box_set_value_blocked(priv->buttons[i], key_info);
	}
}

void isku_media_frame_update_rkp(IskuMediaFrame *buttons_frame, IskuRkp *rkp) {
	IskuMediaFramePrivate *priv = buttons_frame->priv;
	guint i;
	IskuRkpMacroKeyInfo *key_info;

	for (i = 0; i < ISKU_KEYS_MEDIA_NUM; ++i) {
		key_info = isku_key_combo_box_get_value(priv->buttons[i]);
		isku_rkp_set_macro_key_info(rkp, priv->button_rkp_index[i], key_info);
		isku_rkp_macro_key_info_free(key_info);
	}
}
