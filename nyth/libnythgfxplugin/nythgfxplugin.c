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

#include "nyth.h"
#include "nyth_dbus_services.h"
#include "roccat_gfx.h"
#include "roccat.h"

typedef struct {
	DBusConnection *bus;
} NythGaminggearFxPluginPrivate;

GfxResult nyth_get_device_description(GaminggearFxPlugin *self, char * const device_description,
		unsigned int const device_description_size, GfxDevtype * const device_type) {
	if (device_description)
		g_strlcpy(device_description, ROCCAT_VENDOR_NAME " " NYTH_DEVICE_NAME, device_description_size);
	if (device_type)
		*device_type = GFX_DEVTYPE_MOUSE;
	return GFX_SUCCESS;
}

GfxResult nyth_get_num_lights(GaminggearFxPlugin *self, unsigned int * const num_lights) {
	if (num_lights)
		*num_lights = NYTH_LIGHTS_NUM;
	return GFX_SUCCESS;
}

GfxResult nyth_get_light_description(GaminggearFxPlugin *self, unsigned int const light_index,
		char * const light_description, unsigned int const light_description_size) {
	char const * string;

	if (!light_description) return GFX_SUCCESS;

	switch (light_index) {
	case 0:
		string = "Logo";
		break;
	case 1:
		string = "Pipe";
		break;
	default:
		return GFX_ERROR_NOLIGHTS;
		break;
	}

	g_strlcpy(light_description, string, light_description_size);
	return GFX_SUCCESS;
}

GfxResult nyth_get_light_position(GaminggearFxPlugin *self, unsigned int const light_index,
		GfxPosition * const light_position) {
	if (!light_position) return GFX_SUCCESS;

	switch (light_index) {
	case 0:
		light_position->x = 3;
		light_position->y = 3;
		light_position->z = 9;
		break;
	case 1:
		light_position->x = 3;
		light_position->y = 0;
		light_position->z = 10;
		break;
	default:
		return GFX_ERROR_NOLIGHTS;
		break;
	}
	return GFX_SUCCESS;
}

GfxResult nyth_gfx_get_light_color(GaminggearFxPlugin *self, unsigned int const light_index, uint32_t * const color) {
	NythGaminggearFxPluginPrivate *private = (NythGaminggearFxPluginPrivate *)self->private;
	if (light_index >= NYTH_LIGHTS_NUM) return GFX_ERROR_NOLIGHTS;
	if (!color) return GFX_SUCCESS;
	return roccat_gfx_get_light_color(private->bus, NYTH_DBUS_SERVER_PATH, NYTH_DBUS_SERVER_INTERFACE, light_index, color);
}

GfxResult nyth_gfx_set_light_color(GaminggearFxPlugin *self, unsigned int const light_index, uint32_t const color) {
	NythGaminggearFxPluginPrivate *private = (NythGaminggearFxPluginPrivate *)self->private;
	if (light_index >= NYTH_LIGHTS_NUM) return GFX_ERROR_NOLIGHTS;
	return roccat_gfx_set_light_color(private->bus, NYTH_DBUS_SERVER_PATH, NYTH_DBUS_SERVER_INTERFACE, light_index, color);
}

GfxResult nyth_gfx_update(GaminggearFxPlugin *self) {
	NythGaminggearFxPluginPrivate *private = (NythGaminggearFxPluginPrivate *)self->private;
	return roccat_gfx_update(private->bus, NYTH_DBUS_SERVER_PATH, NYTH_DBUS_SERVER_INTERFACE);
}

static void nyth_gfx_finalize(GaminggearFxPlugin *self) {
	NythGaminggearFxPluginPrivate *private = (NythGaminggearFxPluginPrivate *)self->private;

	dbus_connection_unref(private->bus);
	g_free(private);
	g_free(self);
}

GAMINGGEAR_FX_PLUGIN_API GaminggearFxPlugin *gaminggear_fx_plugin_new(void) {
	GaminggearFxPlugin *plugin;
	NythGaminggearFxPluginPrivate *private;
	DBusConnection *bus;

	bus = dbus_bus_get(DBUS_BUS_SESSION, NULL);

	if (!bus)
		return NULL;

	if (!dbus_check_interface(bus, ROCCAT_DBUS_SERVER_NAME, NYTH_DBUS_SERVER_PATH, NYTH_DBUS_SERVER_INTERFACE)) {
		dbus_connection_unref(bus);
		return NULL;
	}

	plugin = (GaminggearFxPlugin *)g_malloc0(sizeof(GaminggearFxPlugin));
	private = (NythGaminggearFxPluginPrivate *)g_malloc(sizeof(NythGaminggearFxPluginPrivate));

	private->bus = bus;

	plugin->private = private;
	plugin->get_device_description = nyth_get_device_description;
	plugin->get_num_lights = nyth_get_num_lights;
	plugin->get_light_description = nyth_get_light_description;
	plugin->get_light_position = nyth_get_light_position;
	plugin->get_light_color = nyth_gfx_get_light_color;
	plugin->set_light_color = nyth_gfx_set_light_color;
	plugin->update = nyth_gfx_update;
	plugin->finalize = nyth_gfx_finalize;

	return plugin;
}
