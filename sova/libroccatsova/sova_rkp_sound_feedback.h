#ifndef __ROCCAT_SOVA_RKP_SOUND_FEEDBACK_H__
#define __ROCCAT_SOVA_RKP_SOUND_FEEDBACK_H__

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

typedef struct {
	guint8 typing_sound; // SovaRkpSoundFeedbackTypingSound
	guint8 macro_record; // SovaRkpSoundFeedbackNotification
	guint8 profile_switch; // SovaRkpSoundFeedbackNotification
	guint8 wakeup; // SovaRkpSoundFeedbackNotification
	guint8 sleep; // SovaRkpSoundFeedbackNotification
} __attribute__ ((packed)) SovaRkpSoundFeedback;

typedef enum {
	SOVA_RKP_SOUND_FEEDBACK_TYPING_SOUND_OFF = 0x00,
	SOVA_RKP_SOUND_FEEDBACK_TYPING_SOUND_CLICK = 0x01,
	SOVA_RKP_SOUND_FEEDBACK_TYPING_SOUND_TYPEWRITER = 0x02,
	SOVA_RKP_SOUND_FEEDBACK_TYPING_SOUND_BEAM = 0x03,
	SOVA_RKP_SOUND_FEEDBACK_TYPING_SOUND_SCIFI = 0x04,
} SovaRkpSoundFeedbackTypingSound;

typedef enum {
	SOVA_RKP_SOUND_FEEDBACK_NOTIFICATION_OFF = 0,
	SOVA_RKP_SOUND_FEEDBACK_NOTIFICATION_ON = 1,
} SovaRkpSoundFeedbackNotification;

gboolean sova_rkp_sound_feedback_equal(SovaRkpSoundFeedback *left, SovaRkpSoundFeedback const *right);
void sova_rkp_sound_feedback_copy(SovaRkpSoundFeedback *destination, SovaRkpSoundFeedback const *source);
SovaRkpSoundFeedback const *sova_rkp_sound_feedback_default(void);

G_END_DECLS

#endif
