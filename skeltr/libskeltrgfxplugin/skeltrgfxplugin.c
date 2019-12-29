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

#include "skeltr_light.h"
#include "skeltr_dbus_services.h"
#include "skeltr.h"
#include "roccat_gfx.h"
#include "roccat.h"

typedef struct {
	DBusConnection *bus;
} SkeltrGaminggearFxPluginPrivate;

static struct {
	gchar const *description;
	GfxPosition position;
} const values[SKELTR_LIGHTS_NUM] = {
		{"Keys", {25, 2, 10}},
		{"Top left", {2, 1, 7}},
		{"Bottom left", {2, 1, 11}},
		{"Top right", {23, 1, 7}},
		{"Bottom right", {23, 1, 11}},
};

GfxResult skeltr_get_device_description(GaminggearFxPlugin *self, char * const device_description,
		unsigned int const device_description_size, GfxDevtype * const device_type) {
	if (device_description)
		g_strlcpy(device_description, ROCCAT_VENDOR_NAME " " SKELTR_DEVICE_NAME, device_description_size);
	if (device_type)
		*device_type = GFX_DEVTYPE_KEYBOARD;
	return GFX_SUCCESS;
}

GfxResult skeltr_get_num_lights(GaminggearFxPlugin *self, unsigned int * const num_lights) {
	if (num_lights)
		*num_lights = SKELTR_LIGHTS_NUM;
	return GFX_SUCCESS;
}

GfxResult skeltr_get_light_description(GaminggearFxPlugin *self, unsigned int const light_index,
		char * const light_description, unsigned int const light_description_size) {
	if (!light_description) return GFX_SUCCESS;
	if (light_index > SKELTR_LIGHTS_NUM) return GFX_ERROR_NOLIGHTS;

	g_strlcpy(light_description, values[light_index].description, light_description_size);
	return GFX_SUCCESS;
}

GfxResult skeltr_get_light_position(GaminggearFxPlugin *self, unsigned int const light_index,
		GfxPosition * const light_position) {
	if (!light_position) return GFX_SUCCESS;
	if (light_index > SKELTR_LIGHTS_NUM) return GFX_ERROR_NOLIGHTS;

	*light_position = values[light_index].position;
	return GFX_SUCCESS;
}

GfxResult skeltr_gfx_get_light_color(GaminggearFxPlugin *self, unsigned int const light_index, uint32_t * const color) {
	SkeltrGaminggearFxPluginPrivate *private = (SkeltrGaminggearFxPluginPrivate *)self->private;
	if (light_index >= SKELTR_LIGHTS_NUM) return GFX_ERROR_NOLIGHTS;
	if (!color) return GFX_SUCCESS;
	return roccat_gfx_get_light_color(private->bus, SKELTR_DBUS_SERVER_PATH, SKELTR_DBUS_SERVER_INTERFACE, light_index, color);
}

GfxResult skeltr_gfx_set_light_color(GaminggearFxPlugin *self, unsigned int const light_index, uint32_t const color) {
	SkeltrGaminggearFxPluginPrivate *private = (SkeltrGaminggearFxPluginPrivate *)self->private;
	if (light_index >= SKELTR_LIGHTS_NUM) return GFX_ERROR_NOLIGHTS;
	return roccat_gfx_set_light_color(private->bus, SKELTR_DBUS_SERVER_PATH, SKELTR_DBUS_SERVER_INTERFACE, light_index, color);
}

GfxResult skeltr_gfx_update(GaminggearFxPlugin *self) {
	SkeltrGaminggearFxPluginPrivate *private = (SkeltrGaminggearFxPluginPrivate *)self->private;
	return roccat_gfx_update(private->bus, SKELTR_DBUS_SERVER_PATH, SKELTR_DBUS_SERVER_INTERFACE);
}

static void skeltr_gfx_finalize(GaminggearFxPlugin *self) {
	SkeltrGaminggearFxPluginPrivate *private = (SkeltrGaminggearFxPluginPrivate *)self->private;

	dbus_connection_unref(private->bus);
	g_free(private);
	g_free(self);
}

GAMINGGEAR_FX_PLUGIN_API GaminggearFxPlugin *gaminggear_fx_plugin_new(void) {
	GaminggearFxPlugin *plugin;
	SkeltrGaminggearFxPluginPrivate *private;
	DBusConnection *bus;

	bus = dbus_bus_get(DBUS_BUS_SESSION, NULL);

	if (!bus)
		return NULL;

	if (!dbus_check_interface(bus, ROCCAT_DBUS_SERVER_NAME, SKELTR_DBUS_SERVER_PATH, SKELTR_DBUS_SERVER_INTERFACE)) {
		dbus_connection_unref(bus);
		return NULL;
	}

	plugin = (GaminggearFxPlugin *)g_malloc0(sizeof(GaminggearFxPlugin));
	private = (SkeltrGaminggearFxPluginPrivate *)g_malloc(sizeof(SkeltrGaminggearFxPluginPrivate));

	private->bus = bus;

	plugin->private = private;
	plugin->get_device_description = skeltr_get_device_description;
	plugin->get_num_lights = skeltr_get_num_lights;
	plugin->get_light_description = skeltr_get_light_description;
	plugin->get_light_position = skeltr_get_light_position;
	plugin->get_light_color = skeltr_gfx_get_light_color;
	plugin->set_light_color = skeltr_gfx_set_light_color;
	plugin->update = skeltr_gfx_update;
	plugin->finalize = skeltr_gfx_finalize;

	return plugin;
}
