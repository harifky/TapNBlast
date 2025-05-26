#include "../lib/gameLogic.h"
#include "../lib/global.h"

void PlaceBlock(Vector2 *shape, int count, int gridX, int gridY, int colorIndex) {
    for (int i = 0; i < count; i++) {
        int x = gridX + shape[i].x;
        int y = gridY + shape[i].y;
        if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT) {
            grid[y][x] = colorIndex;
        }
    }
}