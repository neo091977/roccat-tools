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

#include "sova_rkp_sound_feedback.h"
#include <string.h>

static SovaRkpSoundFeedback const default_sound_feedback = {
		SOVA_RKP_SOUND_FEEDBACK_TYPING_SOUND_OFF,
		SOVA_RKP_SOUND_FEEDBACK_NOTIFICATION_OFF,
		SOVA_RKP_SOUND_FEEDBACK_NOTIFICATION_OFF,
		SOVA_RKP_SOUND_FEEDBACK_NOTIFICATION_OFF,
		SOVA_RKP_SOUND_FEEDBACK_NOTIFICATION_OFF
};

gboolean sova_rkp_sound_feedback_equal(SovaRkpSoundFeedback *left, SovaRkpSoundFeedback const *right) {
	return memcmp(left, right, sizeof(SovaRkpSoundFeedback)) ? FALSE : TRUE;
}

void sova_rkp_sound_feedback_copy(SovaRkpSoundFeedback *destination, SovaRkpSoundFeedback const *source) {
	memcpy(destination, source, sizeof(SovaRkpSoundFeedback));
}

SovaRkpSoundFeedback const *sova_rkp_sound_feedback_default(void) {
	return &default_sound_feedback;
}
