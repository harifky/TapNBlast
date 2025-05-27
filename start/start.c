#include "start.h"

void StartGame() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tap N Blast");
    SetTargetFPS(60);

    srand(time(NULL));
    int currentBlock = rand() % BLOCK_TYPES;
    int blockSize = MAX_BLOCK_SIZE;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(PURPLE);

        DrawGrids();

        // Posisi mouse terhadap grid
        Vector2 mouse = GetMousePosition();
        int mx = (int)((mouse.x - gridOriginX) / TILE_SIZE);
        int my = (int)((mouse.y - gridOriginY) / TILE_SIZE);

        // Gambar preview block transparan di posisi mouse
        for (int i = 0; i < blockSize; i++) {
            int x = mx + blockShapes[currentBlock][i].x;
            int y = my + blockShapes[currentBlock][i].y;
            if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT) {
                DrawRectangle(
                    gridOriginX + x * TILE_SIZE,
                    gridOriginY + y * TILE_SIZE,
                    BLOCK_SIZE,
                    BLOCK_SIZE,
                    Fade(blockColors[currentBlock], 0.5f)
                );
            }
        }

        // Cek klik mouse untuk menempatkan blok
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 m = GetMousePosition();
            int gx = (int)((m.x - gridOriginX) / TILE_SIZE);
            int gy = (int)((m.y - gridOriginY) / TILE_SIZE);
            if (gx >= 0 && gx < GRID_WIDTH && gy >= 0 && gy < GRID_HEIGHT) {
                if (CanPlaceBlock(gx, gy, currentBlock + 1)) {
                    PlaceBlock(gx, gy, currentBlock + 1);
                    currentBlock = rand() % BLOCK_TYPES;  // Dapatkan blok baru
                    ClearFullLines();
                }
            }
        }

        // Gambar informasi FPS
        DrawText(TextFormat("FPS: %d", GetFPS()), 10, SCREEN_HEIGHT - 25, 14, BLACK);

        EndDrawing();
    }

    CloseWindow();
}