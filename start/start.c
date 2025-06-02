#include "start.h"

void StartGame() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tap N Blast");
    SetTargetFPS(60);
    srand(time(NULL));

    int blockSize = MAX_BLOCK_SIZE;
    boolean GameOver = true;
    int selectedIndex = 0;
    int blocksUsedInBatch = 0; // Counter untuk melacak berapa blok yang sudah digunakan dalam batch
    
    // Inisialisasi queue dengan batch pertama (3 blok)
    for (int i = 0; i < 3; i++) {
        Enqueue(GetRandomValue(1, 40));
    }

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(PURPLE);
        
        // Ambil blok yang dipilih dari queue
        int currentBlock = GetQueueAt(selectedIndex);

        DrawGrids();

        // Posisi mouse terhadap grid
        // Vector2 mouse = GetMousePosition();
        // int mx = (int)((mouse.x - gridOriginX) / TILE_SIZE);
        // int my = (int)((mouse.y - gridOriginY) / TILE_SIZE);

        // // Gambar preview block transparan di posisi mouse (hanya jika ada blok yang valid)
        // if (currentBlock != -1) {
        //     for (int i = 0; i < blockSize; i++) {
        //         int x = mx + blockShapes[currentBlock][i].x;
        //         int y = my + blockShapes[currentBlock][i].y;
        //         if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT) {
        //             DrawRectangle(
        //                 gridOriginX + x * TILE_SIZE,
        //                 gridOriginY + y * TILE_SIZE,
        //                 BLOCK_SIZE,
        //                 BLOCK_SIZE,
        //                 Fade(blockColors[currentBlock], 0.5f)
        //             );
        //         }
        //     }
        // }

        // Cek klik mouse untuk menempatkan blok
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 m = GetMousePosition();
            int gx = (int)((m.x - gridOriginX) / TILE_SIZE);
            int gy = (int)((m.y - gridOriginY) / TILE_SIZE);
            
            if (gx >= 0 && gx < GRID_WIDTH && gy >= 0 && gy < GRID_HEIGHT && 
                currentBlock != -1) { // Pastikan slot tidak kosong
                
                if (CanPlaceBlock(gx, gy, currentBlock) && GameOver) {
                    PlaceBlock(gx, gy, currentBlock);
                    
                    // Set slot yang digunakan menjadi kosong (-1) tanpa menghapus dari queue
                    Dequeue();
                    blocksUsedInBatch++;
                    
                    // Jika semua 3 blok dalam batch sudah digunakan, generate batch baru
                    if (blocksUsedInBatch >= 3) {
                        // Enqueue batch baru
                        for (int i = 0; i < 3; i++) {
                            Enqueue(GetRandomValue(1, 40));
                        }
                        
                        blocksUsedInBatch = 0; // Reset counter
                        selectedIndex = 0; // Reset ke slot pertama
                    } else {
                        // Pindah ke slot berikutnya yang tidak kosong
                        int nextIndex = selectedIndex;
                        do {
                            nextIndex = (nextIndex + 1) % 3;
                        } while (GetQueueAt(nextIndex) == -1 && nextIndex != selectedIndex);
                        
                        // Jika ada slot yang tidak kosong, pindah ke sana
                        if (GetQueueAt(nextIndex) != -1) {
                            selectedIndex = nextIndex;
                        }
                    }

                    ClearFullLines();
                    
                    // Cek game over - apakah masih ada blok yang bisa ditempatkan
                    boolean hasValidMove = false;
                    for (int i = 0; i < 3; i++) {
                        int blockType = GetQueueAt(i);
                        if (blockType != -1 && HasValidPlacement(blockType)) {
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
        if (IsKeyPressed(KEY_ONE) && GetQueueAt(0) != -1) selectedIndex = 0;
        if (IsKeyPressed(KEY_TWO) && GetQueueAt(1) != -1) selectedIndex = 1;
        if (IsKeyPressed(KEY_THREE) && GetQueueAt(2) != -1) selectedIndex = 2;
        
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
            blocksUsedInBatch = 0;
            
           
            Dequeue();
            
            for (int i = 0; i < 3; i++) {
                Enqueue(GetRandomValue(1, 40));
            }
        }

        // Draw block shadow
        Vector2 mousePos = GetMousePosition();
        int blockType = GetQueueAt(selectedIndex);
        
        if (blockType != -1) {
            DrawBlockShadow((int)mousePos.x, (int)mousePos.y, blockType);
        }

        // Gambar informasi FPS dan debug info
        DrawText(TextFormat("FPS: %d", GetFPS()), 10, SCREEN_HEIGHT - 25, 14, BLACK);
        
        // Debug info untuk melihat status batch
        char batchInfo[100];
        sprintf(batchInfo, "Batch: [%s] [%s] [%s] - Used: %d/3", 
                GetQueueAt(0) == -1 ? "Empty" : "Block",
                GetQueueAt(1) == -1 ? "Empty" : "Block", 
                GetQueueAt(2) == -1 ? "Empty" : "Block",
                blocksUsedInBatch);
        DrawText(batchInfo, 10, SCREEN_HEIGHT - 45, 14, BLACK);
        
        DrawScorePanel();
        DrawNextBlocks(selectedIndex);

        EndDrawing();
    }

    CloseWindow();
}