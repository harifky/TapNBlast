#include "start.h"

void StartGame() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tap N Blast");
    SetTargetFPS(60);
    srand(time(NULL));

    int blockSize = MAX_BLOCK_SIZE;
    boolean GameOver = true;
    int selectedIndex = 0;
    
    // Array untuk menyimpan 3 blok dalam batch
    int currentBatch[3];
    
    // Inisialisasi batch pertama
    for (int i = 0; i < 3; i++) {
        currentBatch[i] = GetRandomValue(1, 40);
    }

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(PURPLE);
        
        // Ambil blok yang dipilih dari batch
        int currentBlock = currentBatch[selectedIndex];

        DrawGrids();

        // Posisi mouse terhadap grid
        Vector2 mouse = GetMousePosition();
        int mx = (int)((mouse.x - gridOriginX) / TILE_SIZE);
        int my = (int)((mouse.y - gridOriginY) / TILE_SIZE);

        // Gambar preview block transparan di posisi mouse (hanya jika ada blok yang valid)
        if (currentBlock != -1) {
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
        }

        // Cek klik mouse untuk menempatkan blok
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 m = GetMousePosition();
            int gx = (int)((m.x - gridOriginX) / TILE_SIZE);
            int gy = (int)((m.y - gridOriginY) / TILE_SIZE);
            
            if (gx >= 0 && gx < GRID_WIDTH && gy >= 0 && gy < GRID_HEIGHT && 
                currentBlock != -1) { // Pastikan slot tidak kosong
                
                if (CanPlaceBlock(gx, gy, currentBlock) && GameOver) {
                    PlaceBlock(gx, gy, currentBlock);
                    
                    // Kosongkan slot yang digunakan (set ke -1)
                    currentBatch[selectedIndex] = -1;
                    
                    // Cek apakah semua slot sudah kosong
                    boolean allSlotsEmpty = true;
                    for (int i = 0; i < 3; i++) {
                        if (currentBatch[i] != -1) {
                            allSlotsEmpty = false;
                            break;
                        }
                    }
                    
                    // Jika semua slot kosong, generate batch baru
                    if (allSlotsEmpty) {
                        for (int i = 0; i < 3; i++) {
                            currentBatch[i] = GetRandomValue(1, 40);
                        }
                        selectedIndex = 0; // Reset ke slot pertama
                    } else {
                        // Pindah ke slot berikutnya yang tidak kosong
                        int nextIndex = selectedIndex;
                        do {
                            nextIndex = (nextIndex + 1) % 3;
                        } while (currentBatch[nextIndex] == -1 && nextIndex != selectedIndex);
                        
                        // Jika ada slot yang tidak kosong, pindah ke sana
                        if (currentBatch[nextIndex] != -1) {
                            selectedIndex = nextIndex;
                        }
                    }

                    ClearFullLines();
                    
                    // Cek game over - apakah masih ada blok yang bisa ditempatkan
                    boolean hasValidMove = false;
                    for (int i = 0; i < 3; i++) {
                        if (currentBatch[i] != -1 && HasValidPlacement(currentBatch[i])) {
                            hasValidMove = true;
                            break;
                        }
                    }
                    
                    if (!hasValidMove) {
                        GameOver = false;
                    }
                }
            }
        }

        // Input untuk memilih blok (hanya jika slot tidak kosong)
        if (IsKeyPressed(KEY_ONE) && currentBatch[0] != -1) selectedIndex = 0;
        if (IsKeyPressed(KEY_TWO) && currentBatch[1] != -1) selectedIndex = 1;
        if (IsKeyPressed(KEY_THREE) && currentBatch[2] != -1) selectedIndex = 2;
        
        // Panel game over
        if (!GameOver) {
            DrawGameOverPanel();
        }
        
        // Reset game
        if (!GameOver && IsKeyPressed(KEY_R)) {
            // Reset grid
            for (int y = 0; y < GRID_SIZE; y++) {
                for (int x = 0; x < GRID_SIZE; x++) {
                    grid[y][x] = 0;
                }
            }
            
            // Reset game state
            score = 0;
            GameOver = true;
            selectedIndex = 0;
            
            // Generate batch baru
            for (int i = 0; i < 3; i++) {
                currentBatch[i] = GetRandomValue(1, 40);
            }
        }

        // Gambar informasi FPS dan debug info
        DrawText(TextFormat("FPS: %d", GetFPS()), 10, SCREEN_HEIGHT - 25, 14, BLACK);
        
        // Debug info untuk melihat status batch
        char batchInfo[100];
        sprintf(batchInfo, "Batch: [%s] [%s] [%s]", 
                currentBatch[0] == -1 ? "Empty" : "Block",
                currentBatch[1] == -1 ? "Empty" : "Block", 
                currentBatch[2] == -1 ? "Empty" : "Block");
        DrawText(batchInfo, 10, SCREEN_HEIGHT - 45, 14, BLACK);
        
        DrawScorePanel();
        DrawNextBlocks(selectedIndex, currentBatch);

        EndDrawing();
    }

    CloseWindow();
}