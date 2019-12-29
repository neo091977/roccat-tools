#ifndef __ROCCAT_SWARM_RMP_H__
#define __ROCCAT_SWARM_RMP_H__

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

#include <glib.h>

G_BEGIN_DECLS

enum {
	ROCCAT_SWARM_RMP_PROFILE_NAME_LENGTH = 12,
	ROCCAT_SWARM_RMP_GAMEFILE_LENGTH = 256,
	ROCCAT_SWARM_RMP_GAMEFILE_NUM = 5,
	ROCCAT_SWARM_RMP_OPENER_LENGTH = 256,
};

typedef struct _RoccatSwarmRmpHeader RoccatSwarmRmpHeader;
typedef struct _RoccatSwarmRmpTimer RoccatSwarmRmpTimer;

struct _RoccatSwarmRmpHeader {
	/* does not need to be zero terminated, utf16 */
	guint16 profile_name[ROCCAT_SWARM_RMP_PROFILE_NAME_LENGTH];
	guint8 unknown1[80];
	guint8 volume;
	guint8 unknown3;
	guint8 profile_autoswitch;
	guint8 unknown4[5];
} __attribute__ ((packed));

struct _RoccatSwarmRmpTimer {
	guint8 unknown[11];
} __attribute__ ((packed));

typedef gchar RoccatSwarmOpener[ROCCAT_SWARM_RMP_OPENER_LENGTH];
typedef gchar RoccatSwarmGamefile[ROCCAT_SWARM_RMP_GAMEFILE_LENGTH];

void roccat_swarm_rmp_header_set_profile_name(RoccatSwarmRmpHeader *header, gchar const *name);
gchar *roccat_swarm_rmp_header_get_profile_name(RoccatSwarmRmpHeader const *header);

guint32 roccat_swarm_rmp_read_bigendian32(gconstpointer *data);
gboolean roccat_swarm_rmp_read_unknown(gconstpointer *data);
gboolean roccat_swarm_rmp_read_unknown_with_count(gconstpointer *data);

/* Reads a fixed length string */
gchar *roccat_swarm_rmp_read_bigendian_string(gconstpointer *data, guint32 chars);

/* Reads a string with prefixed size */
gchar *roccat_swarm_rmp_read_bigendian_string_with_size(gconstpointer *data);

gboolean roccat_swarm_rmp_read_pictures(gconstpointer *data);

void roccat_swarm_rmp_write_bigendian32(gpointer *data, gsize *length, guint32 value);
gpointer roccat_swarm_rmp_write_with_size(gpointer *data, gsize *length, guint32 size);
gpointer roccat_swarm_rmp_write_unknown(gpointer *data, gsize *length, gsize size);
void roccat_swarm_rmp_write_unknown_with_count(gpointer *data, gsize *length, gsize count, gsize size);

/* Writes a string with fixed length */
void roccat_swarm_rmp_write_bigendian_string(gpointer *data, gsize *length, gchar const *string, guint32 chars);

/* Determines length of string and writes actual length string with size */
void roccat_swarm_rmp_write_bigendian_string_with_size(gpointer *data, gsize *length, gchar const *string, guint32 max_chars);

gpointer roccat_swarm_rmp_read_with_path(gchar const *path, gsize *length, GError **error);
gboolean roccat_swarm_rmp_write_with_path(gchar const *path, gconstpointer rmp, gsize length, GError **error);

/* Windows version implementation looks broken.
 * Name and duration are not stored directly which makes rmp not
 * transferable between systems.
 * It either encodes at least the duration or it's
 * just a reference inside entry TimerDatas in file.
 */
gboolean roccat_swarm_rmp_read_timers(gconstpointer *data);
void roccat_swarm_rmp_write_timers(gpointer *data, gsize *length);

G_END_DECLS

#endif
