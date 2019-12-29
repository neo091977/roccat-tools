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

#include "luaconfig_profile_page.h"
#include "luaconfig_buttons_frame.h"
#include "luaconfig_light_frame.h"
#include "luaconfig_polling_rate_frame.h"
#include "luaconfig_handedness_selector.h"
#include "luaconfig_cpi_selector.h"

#define LUACONFIG_PROFILE_PAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), LUACONFIG_PROFILE_PAGE_TYPE, LuaconfigProfilePageClass))
#define IS_LUACONFIG_PROFILE_PAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), LUACONFIG_PROFILE_PAGE_TYPE))
#define LUACONFIG_PROFILE_PAGE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), LUACONFIG_PROFILE_PAGE_TYPE, LuaconfigProfilePagePrivate))

typedef struct _LuaconfigProfilePageClass LuaconfigProfilePageClass;
typedef struct _LuaconfigProfilePagePrivate LuaconfigProfilePagePrivate;

struct _LuaconfigProfilePage {
	RoccatProfilePage parent;
	LuaconfigProfilePagePrivate *priv;
};

struct _LuaconfigProfilePageClass {
	RoccatProfilePageClass parent_class;
};

struct _LuaconfigProfilePagePrivate {
	LuaRmp *rmp;
	LuaconfigCpiSelector *cpi;
	LuaconfigPollingRateFrame *polling_rate;
	LuaconfigHandednessSelector *handedness;
	LuaconfigLightFrame *light;
	LuaconfigButtonsFrame *buttons;
};

G_DEFINE_TYPE(LuaconfigProfilePage, luaconfig_profile_page, ROCCAT_PROFILE_PAGE_TYPE);

GtkWidget *luaconfig_profile_page_new(void) {
	LuaconfigProfilePage *profile_page;

	profile_page = LUACONFIG_PROFILE_PAGE(g_object_new(LUACONFIG_PROFILE_PAGE_TYPE,
			"renameable", FALSE,
			NULL));

	return GTK_WIDGET(profile_page);
}

static void set_from_rmp(LuaconfigProfilePage *profile_page) {
	LuaconfigProfilePagePrivate *priv = profile_page->priv;

	luaconfig_buttons_frame_set_from_rmp(priv->buttons, priv->rmp);
	luaconfig_light_frame_set_from_rmp(priv->light, priv->rmp);
	luaconfig_cpi_selector_set_from_rmp(priv->cpi, priv->rmp);
	luaconfig_polling_rate_frame_set_value(priv->polling_rate, lua_rmp_get_polling_rate(priv->rmp));
	luaconfig_handedness_selector_set_value(priv->handedness, lua_rmp_get_handed_mode(priv->rmp));
}

static void update_rmp(LuaconfigProfilePage *profile_page, LuaRmp *rmp) {
	LuaconfigProfilePagePrivate *priv = profile_page->priv;

	luaconfig_buttons_frame_update_rmp(priv->buttons, rmp);
	luaconfig_light_frame_update_rmp(priv->light, rmp);
	luaconfig_cpi_selector_update_rmp(priv->cpi, rmp);
	lua_rmp_set_polling_rate(rmp, luaconfig_polling_rate_frame_get_value(priv->polling_rate));
	lua_rmp_set_handed_mode(rmp, luaconfig_handedness_selector_get_value(priv->handedness));
}

static void reset_cb(RoccatProfilePage *profile_page, gpointer user_data) {
	set_from_rmp(LUACONFIG_PROFILE_PAGE(profile_page));
}

static void luaconfig_profile_page_init(LuaconfigProfilePage *profile_page) {
	LuaconfigProfilePagePrivate *priv = LUACONFIG_PROFILE_PAGE_GET_PRIVATE(profile_page);
	GtkWidget *content, *left, *right;

	profile_page->priv = priv;

	content = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(profile_page), content, TRUE, TRUE, 0);

	left = gtk_vbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(content), left, TRUE, TRUE, 0);

	right = gtk_vbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(content), right, TRUE, TRUE, 0);

	priv->light = LUACONFIG_LIGHT_FRAME(luaconfig_light_frame_new());
	gtk_box_pack_start(GTK_BOX(left), GTK_WIDGET(priv->light), TRUE, TRUE, 0);

	priv->cpi = LUACONFIG_CPI_SELECTOR(luaconfig_cpi_selector_new());
	gtk_box_pack_start(GTK_BOX(left), GTK_WIDGET(priv->cpi), TRUE, TRUE, 0);

	priv->polling_rate = LUACONFIG_POLLING_RATE_FRAME(luaconfig_polling_rate_frame_new());
	gtk_box_pack_start(GTK_BOX(left), GTK_WIDGET(priv->polling_rate), TRUE, TRUE, 0);

	priv->buttons = LUACONFIG_BUTTONS_FRAME(luaconfig_buttons_frame_new());
	gtk_box_pack_start(GTK_BOX(right), GTK_WIDGET(priv->buttons), TRUE, TRUE, 0);

	priv->handedness = LUACONFIG_HANDEDNESS_SELECTOR(luaconfig_handedness_selector_new());
	gtk_box_pack_start(GTK_BOX(right), GTK_WIDGET(priv->handedness), TRUE, TRUE, 0);

	gtk_widget_show_all(content);
	g_signal_connect(G_OBJECT(profile_page), "reset", G_CALLBACK(reset_cb), NULL);
}

static void luaconfig_profile_page_class_init(LuaconfigProfilePageClass *klass) {
	g_type_class_add_private(klass, sizeof(LuaconfigProfilePagePrivate));
}

void luaconfig_profile_page_set_rmp(LuaconfigProfilePage *profile_page, LuaRmp *rmp) {
	LuaconfigProfilePagePrivate *priv = profile_page->priv;
	if (priv->rmp)
		lua_rmp_free(priv->rmp);
	priv->rmp = lua_rmp_dup(rmp);
	set_from_rmp(profile_page);
}

LuaRmp *luaconfig_profile_page_get_rmp(LuaconfigProfilePage *profile_page) {
	LuaconfigProfilePagePrivate *priv = profile_page->priv;
	LuaRmp *rmp;
	rmp = lua_rmp_dup(priv->rmp);
	update_rmp(profile_page, rmp);
	return rmp;
}
