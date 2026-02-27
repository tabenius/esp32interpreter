#ifndef AUDIO_OUTPUT_H
#define AUDIO_OUTPUT_H

#include "config.h"

void audioOutputInit();
void audioOutputStart();
void audioSetVolume(int v); // 0-100

#endif // AUDIO_OUTPUT_H
