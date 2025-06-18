#include "start.h"

// Kontribusi: Faiz, Fariz, Rifky
void StartGame() {
    // ============================================
    // INISIALISASI APLIKASI
    // ============================================
    
    // Setup window dan sistem dasar
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tddp N Blast");
    SetTargetFPS(60);
    srand(time(NULL)); // Seed untuk random number generator
    
    // Inisialisasi sistem audio dan data
    InitAudioDevice();
    InitLeaderboard();
    InitSounds();

    // ============================================
    // MAIN APPLICATION LOOP
    // ============================================
    while (!WindowShouldClose()) {
        
        // --- MENU STATE ---
        InitMainMenu();
        PlayBacksoundMenu();
        shouldReturnToMenu = false;

        // Loop menu utama - menunggu input user
        while (!WindowShouldClose() && !shouldReturnToMenu) {
            BeginDrawing();
            ClearBackground(BLACK);
            
            int menuResult = UpdateMainMenu();
            UpdateMusic();
            
            EndDrawing();

            // Handle menu selection
            if (menuResult == 1) break;      // Start game
            if (menuResult == -1) {          // Exit game
                CloseAudioDevice();
                CloseWindow();
                return;
            }
        }

        // Check exit conditions
        if (shouldReturnToMenu) continue;
        if (WindowShouldClose()) break;

        // ============================================
        // GAME INITIALIZATION
        // ============================================
        
        // Reset game state variables
        gameStartTime = time(NULL);
        finalGameDuration = 0;
        boolean GameOver = true;         // true = game sedang berjalan
        int selectedIndex = 0;           // Index block yang dipilih (0-2)
        int blocksUsedInBatch = 0;       // Counter blocks yang sudah digunakan
        boolean blockUsed[3] = {false, false, false}; // Status penggunaan setiap block
        isInGameOverInput = false;       // Flag untuk input nama di game over
        leaderboardSaved = false;        // Flag untuk mencegah double save
        score = 0;
        isPaused = false;

        // Generate batch pertama blocks dan setup audio
        GenerateNewBatch(blockUsed);
        PlayBacksoundGame();
        InitPauseMenuButtons();

        // Cari block pertama yang bisa digunakan
        while (selectedIndex < 3 && (GetQueueAt(selectedIndex) == -1 || blockUsed[selectedIndex])) {
            selectedIndex++;
        }
        if (selectedIndex >= 3) selectedIndex = 0;

        // Setup grid awal
        InitializeRandomGrid();

        // ============================================
        // MAIN GAME LOOP
        // ============================================
        while (!WindowShouldClose() && !shouldReturnToMenu) {
            BeginDrawing();
            ClearBackground(PURPLE);

            // --- PAUSE TOGGLE ---
            // Handle pause/unpause dengan P atau ESC
            if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE)) {
                if (!isInGameOverInput) {
                    isPaused = !isPaused;
                }
            }

            // --- AUTO GRID RESET ---
            // Jika grid kosong, generate grid baru dan reset blocks
            if (IsGridEmpty(grid) && !isPaused && !isInGameOverInput) {
               InitializeRandomGrid();
               ClearQueue();
               GenerateNewBatch(blockUsed);
            } 

            // ============================================
            // GAME OVER STATE
            // ============================================
            if (isInGameOverInput) {
                // Bersihkan animasi dan handle input username
                ClearAnimationQueue();
                UpdateUsernameInput(&usernameInput);
                DrawGameOverPanel(&usernameInput, score, finalGameDuration);

                // Save ke leaderboard ketika username selesai diinput
                if (usernameInput.isComplete && !leaderboardSaved) {
                    AddToLeaderboard(usernameInput.inputText, score, finalGameDuration);
                    leaderboardSaved = true;
                    shouldReturnToMenu = true;
                }

                // ESC untuk kembali ke menu tanpa save
                if (IsKeyPressed(KEY_ESCAPE)) {
                    shouldReturnToMenu = true;
                }

                EndDrawing();
                if (shouldReturnToMenu) break;
                continue;
            }

            // ============================================
            // PAUSE STATE
            // ============================================
            if (isPaused) {
                // Render game state tapi tidak update logika
                DrawGrids();
                DrawScorePanel();
                DrawNextBlocks(selectedIndex, blockUsed);
                
                // Handle pause menu
                int pauseResult = DrawPauseMenu();
                
                if (pauseResult == 1) {
                    // Resume game
                    isPaused = false;
                } else if (pauseResult == 2) {
                    // Quit to menu - trigger game over input
                    time_t currentTime = time(NULL);
                    finalGameDuration = (int)(currentTime - gameStartTime);
                    isInGameOverInput = true;
                    isPaused = false;
                    InitUsernameInput(&usernameInput);
                }
                
                UpdateMusic();
                EndDrawing();
                continue;
            }

            // ============================================
            // ACTIVE GAMEPLAY STATE
            // ============================================
            
            int currentBlock = GetQueueAt(selectedIndex);
            DrawGrids();

            // --- BLOCK PLACEMENT ---
            // Handle klik mouse untuk menempatkan block
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !IsAnyAnimationActive()) {
                Vector2 m = GetMousePosition();
                
                // Convert koordinat mouse ke grid coordinates
                int gx = (int)((m.x - gridOriginX) / TILE_SIZE);
                int gy = (int)((m.y - gridOriginY) / TILE_SIZE);
                
                PlayPlaceBlockSound();

                // Validasi posisi dan block
                if (gx >= 0 && gx < GRID_WIDTH && gy >= 0 && gy < GRID_HEIGHT &&
                    currentBlock >= 1 && currentBlock <= 36 && !blockUsed[selectedIndex]) {

                    // Cek apakah block bisa ditempatkan di posisi tersebut
                    if (CanPlaceBlock(gx, gy, currentBlock) && GameOver) {
                        int previousScore = score;

                        // Enqueue animasi placement untuk setiap tile block
                        for (int i = 0; i < MAX_BLOCK_SIZE; i++) {
                            int bx = gx + (int)blockShapes[currentBlock][i].x;
                            int by = gy + (int)blockShapes[currentBlock][i].y;
                            if (bx >= 0 && bx < GRID_SIZE && by >= 0 && by < GRID_SIZE) {
                                EnqueuePlaceAnimation(bx, by, currentBlock);
                            }
                        }

                        // Eksekusi placement dan clear lines
                        PlaceBlock(gx, gy, currentBlock);
                        ClearFullLines();
                        
                        // Simpan move untuk undo system
                        boolean isScored = (score > previousScore);
                        Vector2 center = (Vector2){ gx, gy };
                        PushMove(&undoStack, currentBlock, center, selectedIndex, isScored);
                        undoCount = 0;  
                        
                        // Mark block sebagai used
                        blockUsed[selectedIndex] = true;

                        // Cek apakah semua blocks dalam batch sudah digunakan
                        int usedCount = 0;
                        for (int i = 0; i < 3; i++) {
                            if (blockUsed[i]) usedCount++;
                        }

                        if (usedCount >= 3) {
                            // Semua blocks used - generate batch baru
                            ClearStack(&undoStack);
                            undoCount = 0;
                            GenerateNewBatch(blockUsed);
                            blocksUsedInBatch = 0;
                            selectedIndex = 0;
                        } else {
                            // Cari block selanjutnya yang bisa digunakan
                            int nextIndex = selectedIndex;
                            int attempts = 0;
                            do {
                                nextIndex = (nextIndex + 1) % 3;
                                attempts++;
                            } while (attempts < 3 && (blockUsed[nextIndex] || GetQueueAt(nextIndex) < 1 || GetQueueAt(nextIndex) > 36));
                            
                            if (attempts < 3 && !blockUsed[nextIndex] && GetQueueAt(nextIndex) >= 1 && GetQueueAt(nextIndex) <= 36) {
                                selectedIndex = nextIndex;
                            }
                        }

                        // Clear lines lagi setelah placement
                        ClearFullLines();

                        // Cek game over condition
                        if (!HasAnyValidMove(blockUsed)) {
                            GameOver = false; // false = game over
                        }
                    }
                }
            }

            // --- KEYBOARD CONTROLS ---
            
            // Undo system (Z key)
            if (IsKeyPressed(KEY_Z)) {
                TraceLog(LOG_INFO, "Undo button clicked");
                PerformUndo(blockUsed);
            }

            // Block selection shortcuts (1, 2, 3 keys)
            if (IsKeyPressed(KEY_ONE)) {
                int blockType = GetQueueAt(0);
                if (!blockUsed[0] && blockType >= 1 && blockType <= 36) selectedIndex = 0;
            }
            if (IsKeyPressed(KEY_TWO)) {
                int blockType = GetQueueAt(1);
                if (!blockUsed[1] && blockType >= 1 && blockType <= 36) selectedIndex = 1;
            }
            if (IsKeyPressed(KEY_THREE)) {
                int blockType = GetQueueAt(2);
                if (!blockUsed[2] && blockType >= 1 && blockType <= 36) selectedIndex = 2;
            }

            // --- GAME OVER CHECK ---
            // Transition ke game over state
            if (!GameOver && !isInGameOverInput) {
                time_t currentTime = time(NULL);
                finalGameDuration = (int)(currentTime - gameStartTime);
                isInGameOverInput = true;
                InitUsernameInput(&usernameInput);
                PlayGameOverSound();
            }

            // --- VISUAL FEEDBACK ---
            
            // Draw shadow block di posisi mouse
            Vector2 mousePos = GetMousePosition();
            int blockType = GetQueueAt(selectedIndex);
            if (blockType >= 1 && blockType <= 36 && !blockUsed[selectedIndex]) {
                DrawBlockShadow((int)mousePos.x, (int)mousePos.y, blockType);
            }

            // Debug info
            DrawText(TextFormat("FPS: %d", GetFPS()), 10, SCREEN_HEIGHT - 25, 14, BLACK);

            // Warning untuk empty slots (debugging)
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

            // --- RENDER GAME ELEMENTS ---
            DrawScorePanel();
            DrawNextBlocks(selectedIndex, blockUsed);
            UpdateAndDrawAnimations(GetFrameTime());
            UpdateMusic();

            EndDrawing();
        }
    }

    // ============================================
    // CLEANUP
    // ============================================
    UnloadSounds();
    CloseAudioDevice();
    CloseWindow();
}