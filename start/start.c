#include "start.h"

int GetValidRandomBlockType() {
    // Atau jika Anda memiliki semua 40 blok valid, gunakan ini:
    return GetRandomValue(1, 40);
}

void GenerateNewBatch(boolean* blockUsed) {
    ClearQueue();
    
    for (int i = 0; i < 3; i++) {
        int attempts = 0;
        int blockType;
        
        // Coba hingga 10 kali untuk mendapatkan blok yang valid
        do {
            blockType = GetValidRandomBlockType();
            attempts++;
        } while (blockType == -1 && attempts < 40);
        
        // Jika masih -1 setelah 10 percobaan, gunakan blok default
        if (blockType == -1) {
            blockType = 1; // Blok default yang pasti ada
        }
        
        Enqueue(blockType);
        blockUsed[i] = false;
    }
}

void StartGame() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tap N Blast");
    SetTargetFPS(60);
    srand(time(NULL));

    InitMainMenu();

    // Tampilkan menu utama sampai user klik tombol Play atau Exit
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        int menuResult = UpdateMainMenu();  // Ini akan menggambar dan cek klik tombol

        EndDrawing();

        if (menuResult == 1) break;     // User menekan Play
        if (menuResult == -1) {
            CloseWindow();              // User menekan Exit
            return;
        }
    }


    // int blockSize = MAX_BLOCK_SIZE;
    boolean GameOver = true;
    int selectedIndex = 0;
    int blocksUsedInBatch = 0;
    boolean blockUsed[3] = {false, false, false};
    
    // Inisialisasi queue dengan batch pertama yang dijamin valid
    GenerateNewBatch(blockUsed);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(PURPLE);
        
        int currentBlock = GetQueueAt(selectedIndex);

        DrawGrids();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 m = GetMousePosition();
            int gx = (int)((m.x - gridOriginX) / TILE_SIZE);
            int gy = (int)((m.y - gridOriginY) / TILE_SIZE);
            
            if (gx >= 0 && gx < GRID_WIDTH && gy >= 0 && gy < GRID_HEIGHT && 
                currentBlock != -1 && !blockUsed[selectedIndex]) {
                
                if (CanPlaceBlock(gx, gy, currentBlock) && GameOver) {
                    PlaceBlock(gx, gy, currentBlock);
                    
                    blockUsed[selectedIndex] = true;
                    blocksUsedInBatch++;
                    
                    if (blocksUsedInBatch >= 3) {
                        // Generate batch baru yang dijamin valid
                        GenerateNewBatch(blockUsed);
                        blocksUsedInBatch = 0;
                        selectedIndex = 0;
                    } else {
                        // Pindah ke slot berikutnya yang belum digunakan
                        int nextIndex = selectedIndex;
                        do {
                            nextIndex = (nextIndex + 1) % 3;
                        } while (blockUsed[nextIndex] && nextIndex != selectedIndex);
                        
                        if (!blockUsed[nextIndex]) {
                            selectedIndex = nextIndex;
                        }
                    }

                    ClearFullLines();
                    
                    // Cek game over dengan validasi yang lebih ketat
                    boolean hasValidMove = false;
                    for (int i = 0; i < 3; i++) {
                        int blockType = GetQueueAt(i);
                        // Pastikan blok tidak -1, belum digunakan, dan bisa ditempatkan
                        if (blockType != -1 && blockType > 0 && !blockUsed[i] && HasValidPlacement(blockType)) {
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

        // Input untuk memilih blok (dengan validasi tambahan)
        if (IsKeyPressed(KEY_ONE) && !blockUsed[0] && GetQueueAt(0) != -1) selectedIndex = 0;
        if (IsKeyPressed(KEY_TWO) && !blockUsed[1] && GetQueueAt(1) != -1) selectedIndex = 1;
        if (IsKeyPressed(KEY_THREE) && !blockUsed[2] && GetQueueAt(2) != -1) selectedIndex = 2;
        
        if (!GameOver) {
            DrawGameOverPanel();
        }
        
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
            blocksUsedInBatch = 0;
            
            // Generate batch baru yang valid
            GenerateNewBatch(blockUsed);
        }

        // Draw block shadow dengan validasi
        Vector2 mousePos = GetMousePosition();
        int blockType = GetQueueAt(selectedIndex);
        
        if (blockType != -1 && blockType > 0 && !blockUsed[selectedIndex]) {
            DrawBlockShadow((int)mousePos.x, (int)mousePos.y, blockType);
        }

        // Debug info
        DrawText(TextFormat("FPS: %d", GetFPS()), 10, SCREEN_HEIGHT - 25, 14, BLACK);
        
        char batchInfo[150];
        sprintf(batchInfo, "Batch: [%s:%d] [%s:%d] [%s:%d] - Used: %d/3", 
                blockUsed[0] ? "Used" : "Block", GetQueueAt(0),
                blockUsed[1] ? "Used" : "Block", GetQueueAt(1), 
                blockUsed[2] ? "Used" : "Block", GetQueueAt(2),
                blocksUsedInBatch);
        DrawText(batchInfo, 10, SCREEN_HEIGHT - 45, 12, BLACK);
        
        DrawScorePanel();
        DrawNextBlocks(selectedIndex, blockUsed);

        EndDrawing();
    }

    CloseWindow();
}