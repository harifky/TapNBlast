#include "start.h"


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
        int menuResult = UpdateMainMenu(clickSound);  // Ini akan menggambar dan cek klik tombol

        EndDrawing();

        if (menuResult == 1) break;     // User menekan Play
        if (menuResult == -1) {
            CloseWindow();              // User menekan Exit
            return;
        }
    }

    boolean GameOver = true;
    int selectedIndex = 0;
    int blocksUsedInBatch = 0;
    boolean blockUsed[3] = {false, false, false};
    
    // Inisialisasi queue dengan batch pertama menggunakan best block selector
    GenerateNewBatch(blockUsed);
    
    // Pastikan selectedIndex menunjuk ke blok yang valid
    while (selectedIndex < 3 && (GetQueueAt(selectedIndex) == -1 || blockUsed[selectedIndex])) {
        selectedIndex++;
    }
    if (selectedIndex >= 3) selectedIndex = 0; // Reset ke 0 jika tidak ditemukan

    InitializeRandomGrid();

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
                    
                    blockUsed[selectedIndex] = true;
                    blocksUsedInBatch++;
                    
                    if (blocksUsedInBatch >= 3) {
                        // Semua blok dalam batch sudah digunakan - generate batch baru
                        // Grid akan otomatis diupdate untuk perhitungan best block berikutnya
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
            InitializeRandomGrid();
            // Reset game state
            score = 0;
            GameOver = true;
            selectedIndex = 0;
            blocksUsedInBatch = 0;
            
            // Generate batch baru yang valid dengan best block selector
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
        DrawText(batchInfo, 10, SCREEN_HEIGHT - 65, 12, BLACK);
        
        // Tampilkan peringatan jika ada slot kosong (untuk debugging)
        boolean hasEmptySlot = false;
        for (int i = 0; i < 3; i++) {
            if (GetQueueAt(i) == -1 || GetQueueAt(i) < 1 || GetQueueAt(i) > 36) {
                hasEmptySlot = true;
                break;
            }
        }
        if (hasEmptySlot) {
            DrawText("WARNING: Empty slots detected!", 10, SCREEN_HEIGHT - 105, 12, RED);
        }
        
        DrawScorePanel();
        DrawNextBlocks(selectedIndex, blockUsed);

        EndDrawing();
    }

    UnloadSound(clickSound);
    CloseAudioDevice();
    CloseWindow();
}