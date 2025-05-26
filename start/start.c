#include "start.h"

void StartGame(){
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tap N Blast");
    SetTargetFPS(60);

    srand(time(NULL));

    int currentBlock = rand() % BLOCK_TYPES;
    int blockSize = MAX_BLOCK_SIZE; 
    Color currentColor = blockColors[rand() % 6];

    while (!WindowShouldClose()){


        BeginDrawing();
        ClearBackground(PURPLE);
        DrawGrids();

        
        Vector2 mouse = GetMousePosition();
        int mx = (mouse.x - gridOriginX) / TILE_SIZE;
        int my = (mouse.y - gridOriginY) / TILE_SIZE;


        for (int i = 0; i < blockSize; i++) {
        int x = mx + blockShapes[currentBlock][i].x;
        int y = my + blockShapes[currentBlock][i].y;
         if (x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE) {
        int dx = gridOriginX + x * TILE_SIZE;
        int dy = gridOriginY + y * TILE_SIZE;

        DrawRectangleRounded(
            (Rectangle){dx + 1, dy + 1, TILE_SIZE - 2, TILE_SIZE - 2}, 0.3f, 1, Fade(currentColor, 0.5f)
        );
    }
}


        DrawText(TextFormat("FPS: %d", GetFPS()), 10, SCREEN_HEIGHT - 25, 14, BLACK);

        EndDrawing();
    }

    CloseWindow();
}