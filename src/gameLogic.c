#include "../lib/gameLogic.h"
#include "../lib/global.h"


void PlaceBlock(int centerX, int centerY, int blockType) {
    for (int i = 0; i < MAX_BLOCK_SIZE; i++) {
        int bx = centerX + (int)blockShapes[blockType - 1][i].x;
        int by = centerY + (int)blockShapes[blockType - 1][i].y;
        if (bx >= 0 && bx < GRID_SIZE && by >= 0 && by < GRID_SIZE) {
            grid[by][bx] = blockType;
        }
    }
}