#ifndef START_H
#define START_H

#include "../lib/global.h"
#include "../lib/gameLogic.h"
#include "../lib/ui.h"
#include "../lib/animation.h"
#include "../lib/boolean.h"
#include "../lib/sound.h"


#include "../src/animation.c"
#include "../src/gameLogic.c"
#include "../src/ui.c"
#include "../src/sound.c"

#include "raylib.h"

void StartGame();
void GenerateNewBatch(boolean* blockUsed);
int GetValidRandomBlockType();

#endif