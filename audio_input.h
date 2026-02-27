#ifndef AUDIO_INPUT_H
#define AUDIO_INPUT_H

#include "config.h"

void audioInputInit();
void audioInputStart();
void audioSetGain(float g);
void audioSetInputSource(InputSource s);

#endif // AUDIO_INPUT_H
