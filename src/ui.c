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

void DrawScorePanel() {
    // Draw Score panel
    DrawRectangleRounded(
        (Rectangle){10, 10, 140, 85},
        0.15f,
        10,
        UI_PANEL
    );
    
    DrawRectangleRoundedLines(
        (Rectangle){10, 10, 140, 85},
        0.15f,
        10,
        UI_BORDER
    );
    
    // Draw Score title
    DrawText("SCORE", 25, 15, 20, UI_TEXT);
    
    // Draw Score value with shadow for better visibility
    DrawText(TextFormat("%d", score), 28, 42, 32, (Color){0, 0, 0, 60});
    DrawText(TextFormat("%d", score), 25, 40, 32, UI_SCORE_TEXT);
}

void DrawGameOverPanel() {
    DrawText("GAME OVER!", SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2 - 40, 28, (Color){200, 50, 50, 255});
    DrawText(TextFormat("Final Score: %d", score), SCREEN_WIDTH/2 - 90, SCREEN_HEIGHT/2, 20, UI_TEXT);
    DrawText("Press R to Restart", SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 + 30, 18, UI_TEXT);
}
