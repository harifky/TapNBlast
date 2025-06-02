#ifndef UI_H
#define UI_H

#include "global.h"
#include "gameLogic.h"
#include "animation.h"
#include "raylib.h"

// UI functions
void DrawGrids();

//Score
void DrawScorePanel();
// GameOver

void DrawGameOverPanel();

// draw block preview
void DrawNextBlocks(int selectedIndex, boolean* blockUsed);

void DrawBlockShadow(int cursorX, int cursorY, int blockType);



#endif /* UI_H */