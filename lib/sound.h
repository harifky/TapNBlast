#ifndef SOUND_H
#define SOUND_H

#include "raylib.h"
#include "boolean.h"

extern boolean isSoundOn;
extern boolean isMusicOn;

void InitSounds();

void PlayClickSound();

void PlayScoreSound();

void PlayExplosionSound();

void PlayGameOverSound();

void PlayPlaceBlockSound();

void PlayBacksoundMenu();

void PlayBacksoundGame();

void UnloadSounds();

void PlayBacksoundMenu();

void PlayBacksoundGame();

void StopBacksound();


#endif