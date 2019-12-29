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

#include "skeltr_rkp_sound_feedback.h"
#include <string.h>

static SkeltrRkpSoundFeedback const default_sound_feedback = {
		SKELTR_RKP_SOUND_FEEDBACK_TYPING_SOUND_OFF,
		SKELTR_RKP_SOUND_FEEDBACK_NOTIFICATION_OFF,
		SKELTR_RKP_SOUND_FEEDBACK_NOTIFICATION_OFF,
		SKELTR_RKP_SOUND_FEEDBACK_NOTIFICATION_OFF,
		SKELTR_RKP_SOUND_FEEDBACK_NOTIFICATION_OFF
};

gboolean skeltr_rkp_sound_feedback_equal(SkeltrRkpSoundFeedback *left, SkeltrRkpSoundFeedback const *right) {
	return memcmp(left, right, sizeof(SkeltrRkpSoundFeedback)) ? FALSE : TRUE;
}

void skeltr_rkp_sound_feedback_copy(SkeltrRkpSoundFeedback *destination, SkeltrRkpSoundFeedback const *source) {
	memcpy(destination, source, sizeof(SkeltrRkpSoundFeedback));
}

SkeltrRkpSoundFeedback const *skeltr_rkp_sound_feedback_default(void) {
	return &default_sound_feedback;
}
