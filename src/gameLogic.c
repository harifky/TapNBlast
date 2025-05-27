#include "../lib/gameLogic.h"
#include "../lib/global.h"
#include "../lib/boolean.h"

void PlaceBlock(Vector2 *shape, int count, int gridX, int gridY, int colorIndex) {
    for (int i = 0; i < count; i++) {
        int x = gridX + shape[i].x;
        int y = gridY + shape[i].y;
        if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT) {
            grid[y][x] = colorIndex;
        }
    }
}

boolean HasValidPlacement(int blockType) {
    if (blockType < 1 || blockType > 40) return false;
    
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            if (CanPlaceBlock(x, y, blockType)) return true;
        }
    }
    return false;
}

boolean CanPlaceBlock(int centerX, int centerY, int blockType) {
    if (blockType < 1 || blockType > 40) return false;
    
    for (int i = 0; i < MAX_BLOCK_SIZE; i++) {
        int bx = centerX + (int)blockShapes[blockType - 1][i].x;
        int by = centerY + (int)blockShapes[blockType - 1][i].y;
        if (bx < 0 || bx >= GRID_SIZE || by < 0 || by >= GRID_SIZE || grid[by][bx] != 0) {
            return false;
        }
    }
    return true;
}