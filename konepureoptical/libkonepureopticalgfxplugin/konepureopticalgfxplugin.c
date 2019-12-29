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

#include "konepureoptical.h"
#include "roccat_gfx.h"
#include "roccat.h"

typedef struct {
	DBusConnection *bus;
} KonepureopticalGaminggearFxPluginPrivate;

GfxResult konepureoptical_get_device_description(GaminggearFxPlugin *self, char * const device_description,
		unsigned int const device_description_size, GfxDevtype * const device_type) {
	if (device_description)
		g_strlcpy(device_description, ROCCAT_VENDOR_NAME " " KONEPUREOPTICAL_DEVICE_NAME, device_description_size);
	if (device_type)
		*device_type = GFX_DEVTYPE_MOUSE;
	return GFX_SUCCESS;
}

GfxResult konepureoptical_get_num_lights(GaminggearFxPlugin *self, unsigned int * const num_lights) {
	if (num_lights)
		*num_lights = 1;
	return GFX_SUCCESS;
}

GfxResult konepureoptical_get_light_description(GaminggearFxPlugin *self, unsigned int const light_index,
		char * const light_description, unsigned int const light_description_size) {
	if (light_index >= 1) return GFX_ERROR_NOLIGHTS;
	if (!light_description) return GFX_SUCCESS;
	g_strlcpy(light_description, "All", light_description_size);
	return GFX_SUCCESS;
}

GfxResult konepureoptical_get_light_position(GaminggearFxPlugin *self, unsigned int const light_index,
		GfxPosition * const light_position) {
	if (light_index >= 1) return GFX_ERROR_NOLIGHTS;
	if (!light_position) return GFX_SUCCESS;
	// center of device
	light_position->x = 3;
	light_position->y = 2;
	light_position->z = 6;
	return GFX_SUCCESS;
}

GfxResult konepureoptical_gfx_get_light_color(GaminggearFxPlugin *self, unsigned int const light_index, uint32_t * const color) {
	KonepureopticalGaminggearFxPluginPrivate *private = (KonepureopticalGaminggearFxPluginPrivate *)self->private;
	if (light_index >= 1) return GFX_ERROR_NOLIGHTS;
	if (!color) return GFX_SUCCESS;
	return roccat_gfx_get_light_color(private->bus, KONEPUREOPTICAL_DBUS_SERVER_PATH, KONEPUREOPTICAL_DBUS_SERVER_INTERFACE, light_index, color);
}

GfxResult konepureoptical_gfx_set_light_color(GaminggearFxPlugin *self, unsigned int const light_index, uint32_t const color) {
	KonepureopticalGaminggearFxPluginPrivate *private = (KonepureopticalGaminggearFxPluginPrivate *)self->private;
	if (light_index >= 1) return GFX_ERROR_NOLIGHTS;
	return roccat_gfx_set_light_color(private->bus, KONEPUREOPTICAL_DBUS_SERVER_PATH, KONEPUREOPTICAL_DBUS_SERVER_INTERFACE, light_index, color);
}

GfxResult konepureoptical_gfx_update(GaminggearFxPlugin *self) {
	KonepureopticalGaminggearFxPluginPrivate *private = (KonepureopticalGaminggearFxPluginPrivate *)self->private;
	return roccat_gfx_update(private->bus, KONEPUREOPTICAL_DBUS_SERVER_PATH, KONEPUREOPTICAL_DBUS_SERVER_INTERFACE);
}

static void konepureoptical_gfx_finalize(GaminggearFxPlugin *self) {
	KonepureopticalGaminggearFxPluginPrivate *private = (KonepureopticalGaminggearFxPluginPrivate *)self->private;

	dbus_connection_unref(private->bus);
	g_free(private);
	g_free(self);
}

GAMINGGEAR_FX_PLUGIN_API GaminggearFxPlugin *gaminggear_fx_plugin_new(void) {
	GaminggearFxPlugin *plugin;
	KonepureopticalGaminggearFxPluginPrivate *private;
	DBusConnection *bus;

	bus = dbus_bus_get(DBUS_BUS_SESSION, NULL);

	if (!bus)
		return NULL;

	if (!dbus_check_interface(bus, ROCCAT_DBUS_SERVER_NAME, KONEPUREOPTICAL_DBUS_SERVER_PATH, KONEPUREOPTICAL_DBUS_SERVER_INTERFACE)) {
		dbus_connection_unref(bus);
		return NULL;
	}

	plugin = (GaminggearFxPlugin *)g_malloc0(sizeof(GaminggearFxPlugin));
	private = (KonepureopticalGaminggearFxPluginPrivate *)g_malloc(sizeof(KonepureopticalGaminggearFxPluginPrivate));

	private->bus = bus;

	plugin->private = private;
	plugin->get_device_description = konepureoptical_get_device_description;
	plugin->get_num_lights = konepureoptical_get_num_lights;
	plugin->get_light_description = konepureoptical_get_light_description;
	plugin->get_light_position = konepureoptical_get_light_position;
	plugin->get_light_color = konepureoptical_gfx_get_light_color;
	plugin->set_light_color = konepureoptical_gfx_set_light_color;
	plugin->update = konepureoptical_gfx_update;
	plugin->finalize = konepureoptical_gfx_finalize;

	return plugin;
}
