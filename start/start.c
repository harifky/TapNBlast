#include "start.h"

int GetValidRandomBlockType() {
    return GetRandomValue(1, 36);
}

void GenerateNewBatch(boolean* blockUsed) {
    ClearQueue();
    
    for (int i = 0; i < 3; i++) {
        int blockType;
        int attempts = 0;
        
        // Loop hingga mendapat blok yang valid
        do {
            blockType = GetValidRandomBlockType();
            attempts++;
            
            // Jika sudah 50 percobaan, gunakan blok sequential untuk memastikan validitas
            if (attempts >= 50) {
                blockType = (i % 36) + 1; // Gunakan blok 1-40 secara berurutan
                break;
            }
        } while (blockType < 1 || blockType > 36);
        
        // Double check - pastikan blockType valid
        if (blockType < 1 || blockType > 36) {
            blockType = (i % 40) + 1; // Fallback ke blok sequential
        }
        
        Enqueue(blockType);
        blockUsed[i] = false;
    }
    
    // Verifikasi bahwa queue berisi 3 blok yang valid
    for (int i = 0; i < 3; i++) {
        int queueBlock = GetQueueAt(i);
        if (queueBlock == -1 || queueBlock < 1 || queueBlock > 36) {
            // Jika ada blok yang tidak valid, ganti dengan blok default
            // Asumsi ada fungsi untuk mengatur elemen queue secara langsung
            // Atau bisa menggunakan ClearQueue dan rebuild seluruhnya
            ClearQueue();
            for (int j = 0; j < 3; j++) {
                Enqueue((j % 36) + 1); // Gunakan blok 1, 2, 3 sebagai fallback
                blockUsed[j] = false;
            }
            break;
        }
    }
}

// Fungsi tambahan untuk memvalidasi apakah masih ada gerakan yang valid
boolean HasAnyValidMove(boolean* blockUsed) {
    for (int i = 0; i < 3; i++) {
        int blockType = GetQueueAt(i);
        
        // Cek apakah slot ini berisi blok valid dan belum digunakan
        if (blockType >= 1 && blockType <= 36 && !blockUsed[i]) {
            // Cek apakah blok ini bisa ditempatkan di mana saja di grid
            if (HasValidPlacement(blockType)) {
                return true;
            }
        }
    }
    return false;
}

// Modifikasi fungsi StartGame - bagian yang relevan
void StartGame() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tap N Blast");
    SetTargetFPS(60);
    srand(time(NULL));
    InitAudioDevice();

    Sound clickSound = LoadSound("assets/buttonfx.wav");
    
    SetSoundVolume(clickSound, 1.0f);
    if (!IsAudioDeviceReady()) {
    TraceLog(LOG_ERROR, "Audio device not ready!");
}
    
    
    InitMainMenu();

    // Tampilkan menu utama sampai user klik tombol Play atau Exit
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        int menuResult = UpdateMainMenu(clickSound); 

        EndDrawing();

        if (menuResult == 1) break;     // User menekan Play
        if (menuResult == -1) {
            CloseWindow();              // User menekan Exit
            return;
        }
    }

    InitURUI();


    boolean GameOver = true;
    int selectedIndex = 0;
    int blocksUsedInBatch = 0;
    boolean blockUsed[3] = {false, false, false};
    
    // Inisialisasi queue dengan batch pertama yang dijamin valid
    GenerateNewBatch(blockUsed);
    
    // Pastikan selectedIndex menunjuk ke blok yang valid
    while (selectedIndex < 3 && (GetQueueAt(selectedIndex) == -1 || blockUsed[selectedIndex])) {
        selectedIndex++;
    }
    if (selectedIndex >= 3) selectedIndex = 0; // Reset ke 0 jika tidak ditemukan

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(PURPLE);
        
        int currentBlock = GetQueueAt(selectedIndex);

        DrawGrids();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 m = GetMousePosition();
            int gx = (int)((m.x - gridOriginX) / TILE_SIZE);
            int gy = (int)((m.y - gridOriginY) / TILE_SIZE);
            
            // Validasi tambahan untuk memastikan currentBlock valid
            if (gx >= 0 && gx < GRID_WIDTH && gy >= 0 && gy < GRID_HEIGHT && 
                currentBlock >= 1 && currentBlock <= 36 && !blockUsed[selectedIndex]) {
                
                if (CanPlaceBlock(gx, gy, currentBlock) && GameOver) {
                    PlaceBlock(gx, gy, currentBlock);
                    PushMove(&undoStack, currentBlock, gx, gy, selectedIndex);
                    ClearStack(&redoStack);
                    undoCount = 0;  
                    
                    blockUsed[selectedIndex] = true;
                    // blocksUsedInBatch++;

                    int usedCount = 0;
                    for (int i = 0; i < 3; i++) {
                        if (blockUsed[i]) usedCount++;
                    }
                    
                    if (usedCount >= 3) {
                        ClearStack(&undoStack);
                        ClearStack(&redoStack);
                        undoCount = 0;
                        redoCount = 0;
                        // Semua blok dalam batch sudah digunakan - generate batch baru
                        GenerateNewBatch(blockUsed);
                        blocksUsedInBatch = 0;
                        selectedIndex = 0;
                    } else {
                        // Cari slot berikutnya yang belum digunakan dan berisi blok valid
                        int nextIndex = selectedIndex;
                        int attempts = 0;
                        do {
                            nextIndex = (nextIndex + 1) % 3;
                            attempts++;
                        } while (attempts < 3 && (blockUsed[nextIndex] || GetQueueAt(nextIndex) < 1 || GetQueueAt(nextIndex) > 36));
                        
                        // Jika ditemukan slot valid, gunakan itu
                        if (attempts < 3 && !blockUsed[nextIndex] && GetQueueAt(nextIndex) >= 1 && GetQueueAt(nextIndex) <= 36) {
                            selectedIndex = nextIndex;
                        }
                    }


                    ClearFullLines();
                    
                    // Cek game over dengan validasi yang lebih ketat
                    if (!HasAnyValidMove(blockUsed)) {
                        GameOver = false;
                    }
                }
            }
        }

        if (undoButton.isPressed) {
            // TraceLog(LOG_INFO, "Undo button clicked");
            PerformUndo(blockUsed);
        }

        if (redoButton.isPressed) {
            // TraceLog(LOG_INFO, "Redo button clicked");
            PerformRedo(blockUsed);
        }

        // Input untuk memilih blok dengan validasi yang lebih ketat
        if (IsKeyPressed(KEY_ONE)) {
            int blockType = GetQueueAt(0);
            if (!blockUsed[0] && blockType >= 1 && blockType <= 36) {
                selectedIndex = 0;
            }
        }
        if (IsKeyPressed(KEY_TWO)) {
            int blockType = GetQueueAt(1);
            if (!blockUsed[1] && blockType >= 1 && blockType <= 36) {
                selectedIndex = 1;
            }
        }
        if (IsKeyPressed(KEY_THREE)) {
            int blockType = GetQueueAt(2);
            if (!blockUsed[2] && blockType >= 1 && blockType <= 36) {
                selectedIndex = 2;
            }
        }
        
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
            Dequeue();
            
            // Generate batch baru yang valid
            GenerateNewBatch(blockUsed);
        }

        // Draw block shadow dengan validasi yang lebih ketat
        Vector2 mousePos = GetMousePosition();
        int blockType = GetQueueAt(selectedIndex);
        
        if (blockType >= 1 && blockType <= 36 && !blockUsed[selectedIndex]) {
            DrawBlockShadow((int)mousePos.x, (int)mousePos.y, blockType);
        }

        // Debug info yang lebih informatif
        DrawText(TextFormat("FPS: %d", GetFPS()), 10, SCREEN_HEIGHT - 25, 14, BLACK);
        
        char batchInfo[200];
        sprintf(batchInfo, "Batch: [%s:%d] [%s:%d] [%s:%d] - Used: %d/3 - Selected: %d", 
                blockUsed[0] ? "Used" : "Ready", GetQueueAt(0),
                blockUsed[1] ? "Used" : "Ready", GetQueueAt(1), 
                blockUsed[2] ? "Used" : "Ready", GetQueueAt(2),
                blocksUsedInBatch, selectedIndex + 1);
        DrawText(batchInfo, 10, SCREEN_HEIGHT - 45, 12, BLACK);
        
        // Tampilkan peringatan jika ada slot kosong (untuk debugging)
        boolean hasEmptySlot = false;
        for (int i = 0; i < 3; i++) {
            if (GetQueueAt(i) == -1 || GetQueueAt(i) < 1 || GetQueueAt(i) > 36) {
                hasEmptySlot = true;
                break;
            }
        }
        if (hasEmptySlot) {
            DrawText("WARNING: Empty slots detected!", 10, SCREEN_HEIGHT - 65, 12, RED);
        }

        DrawUndoRedoButtons(clickSound);
        DrawScorePanel();
        DrawNextBlocks(selectedIndex, blockUsed);

        EndDrawing();
    }

    UnloadSound(clickSound);
    CloseAudioDevice();

    CloseWindow();
}