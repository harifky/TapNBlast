#include "start.h"

void StartGame(){
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tap N Blast");
    SetTargetFPS(60);

    srand(time(NULL));

    int currentBlock = rand() % BLOCK_TYPES;
    int blockSize = MAX_BLOCK_SIZE; // could use custom size per shape
    Color currentColor = blockColors[rand() % 6];

    while (!WindowShouldClose()){


        BeginDrawing();
        ClearBackground(PURPLE);
        DrawGrids();

        Vector2 mouse = GetMousePosition();
        int mx = mouse.x / (BLOCK_SIZE + PADDING);
        int my = mouse.y / (BLOCK_SIZE + PADDING);

        for (int i = 0; i < blockSize; i++) {
            int x = mx + blockShapes[currentBlock][i].x;
            int y = my + blockShapes[currentBlock][i].y;
            if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT) {
                DrawRectangle(x * (BLOCK_SIZE + PADDING), y * (BLOCK_SIZE + PADDING), BLOCK_SIZE, BLOCK_SIZE, Fade(currentColor, 0.5f));
            }
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            PlaceBlock((Vector2 *)blockShapes[currentBlock], blockSize, mx, my, (currentColor.r + currentColor.g + currentColor.b) % 6 + 1);
            currentBlock = rand() % BLOCK_TYPES;
            currentColor = blockColors[rand() % 6];
        }

        DrawText(TextFormat("FPS: %d", GetFPS()), 10, SCREEN_HEIGHT - 25, 14, BLACK);

        EndDrawing();
    }

    CloseWindow();
}