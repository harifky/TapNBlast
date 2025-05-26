#include "../lib/ui.h"
#include "../lib/animation.h"
#include "../lib/global.h"

void DrawGrids() {
    // Draw grid background
    DrawRectangleRounded(
        (Rectangle){
            gridOriginX - 5, 
            gridOriginY - 5, 
            TILE_SIZE * GRID_SIZE + 10, 
            TILE_SIZE * GRID_SIZE + 10
        }, 
        0.05f, 
        10, 
        UI_GRID_BG
    );
    
    // Draw border for the grid
    DrawRectangleRoundedLines(
        (Rectangle){
            gridOriginX - 5, 
            gridOriginY - 5, 
            TILE_SIZE * GRID_SIZE + 10, 
            TILE_SIZE * GRID_SIZE + 10
        }, 
        0.05f, 
        10, 
        UI_BORDER
    );
    
    // Draw cells
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            Color c = (grid[y][x] == 0) ? UI_EMPTY_CELL : blockColors[grid[y][x]-1];
            float scale = 1.0f;
            
          
            
            int size = (int)(TILE_SIZE * scale);
            int dx = gridOriginX + x * TILE_SIZE + (TILE_SIZE - size) / 2;
            int dy = gridOriginY + y * TILE_SIZE + (TILE_SIZE - size) / 2;
            

            if (grid[y][x] == 0) {
                DrawRectangleRounded(
                    (Rectangle){dx, dy, size - 2, size - 2},
                    0.2f,
                    1, 
                    c
                );
            } else {
                DrawRectangleRounded(
                    (Rectangle){dx, dy, size - 2, size - 2},
                    0.3f,
                    1, 
                    c
                );
                
                // Add glossy effect
                DrawRectangleRounded(
                    (Rectangle){dx + size/4, dy + size/4, size/2, size/6},
                    0.5f,
                    1,
                    (Color){255, 255, 255, 80}
                );
            }
        }
    }
}