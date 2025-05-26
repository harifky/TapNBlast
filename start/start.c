#include "start.h"

void StartGame(){
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tap N Blast");
    SetTargetFPS(60);

    while (!WindowShouldClose()){


        BeginDrawing();
        ClearBackground(PURPLE);
        DrawGrids();
        DrawText(TextFormat("FPS: %d", GetFPS()), 10, SCREEN_HEIGHT - 25, 14, BLACK);

        EndDrawing();
    }

    CloseWindow();
}