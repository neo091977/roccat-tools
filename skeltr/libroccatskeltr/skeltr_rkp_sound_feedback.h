#ifndef __ROCCAT_SKELTR_RKP_SOUND_FEEDBACK_H__
#define __ROCCAT_SKELTR_RKP_SOUND_FEEDBACK_H__

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
	guint8 typing_sound; // SkeltrRkpSoundFeedbackTypingSound
	guint8 macro_record; // SkeltrRkpSoundFeedbackNotification
	guint8 profile_switch; // SkeltrRkpSoundFeedbackNotification
	guint8 wakeup; // SkeltrRkpSoundFeedbackNotification
	guint8 sleep; // SkeltrRkpSoundFeedbackNotification
} __attribute__ ((packed)) SkeltrRkpSoundFeedback;

typedef enum {
	SKELTR_RKP_SOUND_FEEDBACK_TYPING_SOUND_OFF = 0x00,
	SKELTR_RKP_SOUND_FEEDBACK_TYPING_SOUND_CLICK = 0x01,
	SKELTR_RKP_SOUND_FEEDBACK_TYPING_SOUND_TYPEWRITER = 0x02,
	SKELTR_RKP_SOUND_FEEDBACK_TYPING_SOUND_BEAM = 0x03,
	SKELTR_RKP_SOUND_FEEDBACK_TYPING_SOUND_SCIFI = 0x04,
} SkeltrRkpSoundFeedbackTypingSound;

typedef enum {
	SKELTR_RKP_SOUND_FEEDBACK_NOTIFICATION_OFF = 0,
	SKELTR_RKP_SOUND_FEEDBACK_NOTIFICATION_ON = 1,
} SkeltrRkpSoundFeedbackNotification;

gboolean skeltr_rkp_sound_feedback_equal(SkeltrRkpSoundFeedback *left, SkeltrRkpSoundFeedback const *right);
void skeltr_rkp_sound_feedback_copy(SkeltrRkpSoundFeedback *destination, SkeltrRkpSoundFeedback const *source);
SkeltrRkpSoundFeedback const *skeltr_rkp_sound_feedback_default(void);

G_END_DECLS

#endif
