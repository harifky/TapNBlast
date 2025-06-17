#include "start.h"

// Modifikasi fungsi StartGame - bagian yang relevan
void StartGame() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tddp N Blast");
    SetTargetFPS(60);
    srand(time(NULL));

    InitAudioDevice();
    InitLeaderboard();
    InitSounds();

    while (!WindowShouldClose()) {
        InitMainMenu();
        PlayBacksoundMenu();
        
        shouldReturnToMenu = false;

        while (!WindowShouldClose() && !shouldReturnToMenu) {
            BeginDrawing();
            ClearBackground(BLACK);
            int menuResult = UpdateMainMenu();
            UpdateMusic();
            EndDrawing();

            if (menuResult == 1) break;
            if (menuResult == -1) {
                CloseAudioDevice();
                CloseWindow();
                return;
            }
        }

        if (shouldReturnToMenu) continue;
        if (WindowShouldClose()) break;

        gameStartTime = time(NULL);
        finalGameDuration = 0;
        boolean GameOver = true;
        int selectedIndex = 0;
        int blocksUsedInBatch = 0;
        boolean blockUsed[3] = {false, false, false};
        isInGameOverInput = false;
        leaderboardSaved = false;
        score = 0;
        isPaused = false; // Reset pause state

        GenerateNewBatch(blockUsed);
        PlayBacksoundGame();

        // Initialize pause menu buttons
        InitPauseMenuButtons();

        while (selectedIndex < 3 && (GetQueueAt(selectedIndex) == -1 || blockUsed[selectedIndex])) {
            selectedIndex++;
        }
        if (selectedIndex >= 3) selectedIndex = 0;

        InitializeRandomGrid();

        while (!WindowShouldClose() && !shouldReturnToMenu) {
            BeginDrawing();
            ClearBackground(PURPLE);

            // Handle pause input
            if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE)) {
                if (!isInGameOverInput) {
                    isPaused = !isPaused;
                }
            }

            if (IsGridEmpty(grid) && !isPaused && !isInGameOverInput) {
               InitializeRandomGrid();
               ClearQueue();
               GenerateNewBatch(blockUsed);
            } 

            if (isInGameOverInput) {
                ClearAnimationQueue();
                UpdateUsernameInput(&usernameInput);
                DrawGameOverPanel(&usernameInput, score, finalGameDuration);

                if (usernameInput.isComplete && !leaderboardSaved) {
                    AddToLeaderboard(usernameInput.inputText, score, finalGameDuration);
                    leaderboardSaved = true;
                    shouldReturnToMenu = true;
                }

                if (IsKeyPressed(KEY_ESCAPE)) {
                    shouldReturnToMenu = true;
                }

                EndDrawing();
                if (shouldReturnToMenu) break;
                continue;
            }

            // Handle pause menu
            if (isPaused) {
                // Still draw the game state behind the pause menu
                DrawGrids();
                DrawScorePanel();
                DrawNextBlocks(selectedIndex, blockUsed);
                
                // Draw pause menu on top
                int pauseResult = DrawPauseMenu();
                
                if (pauseResult == 1) {
                    isPaused = false; // Resume game
                } else if (pauseResult == 2) {
                    // Quit to menu - trigger game over input to save score
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

            // Normal game logic (only when not paused)
            int currentBlock = GetQueueAt(selectedIndex);
            DrawGrids();

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !IsAnyAnimationActive()) {
                Vector2 m = GetMousePosition();
                int gx = (int)((m.x - gridOriginX) / TILE_SIZE);
                int gy = (int)((m.y - gridOriginY) / TILE_SIZE);
                PlayPlaceBlockSound();

                if (gx >= 0 && gx < GRID_WIDTH && gy >= 0 && gy < GRID_HEIGHT &&
                    currentBlock >= 1 && currentBlock <= 35 && !blockUsed[selectedIndex]) {

                    if (CanPlaceBlock(gx, gy, currentBlock) && GameOver) {
                        int previousScore = score;

                        // Add place animation for each block part
                        for (int i = 0; i < MAX_BLOCK_SIZE; i++) {
                            int bx = gx + (int)blockShapes[currentBlock][i].x;
                            int by = gy + (int)blockShapes[currentBlock][i].y;
                            if (bx >= 0 && bx < GRID_SIZE && by >= 0 && by < GRID_SIZE) {
                                EnqueuePlaceAnimation(bx, by, currentBlock);
                            }
                        }

                        PlaceBlock(gx, gy, currentBlock);
                        ClearFullLines();
                        boolean isScored = (score > previousScore);
                        Vector2 center = (Vector2){ gx, gy };
                        PushMove(&undoStack, currentBlock, center, selectedIndex, isScored);
                        // ClearStack(&undoStack);
                        undoCount = 0;  
                        blockUsed[selectedIndex] = true;

                        int usedCount = 0;
                        for (int i = 0; i < 3; i++) {
                            if (blockUsed[i]) usedCount++;
                        }

                        if (usedCount >= 3) {
                            ClearStack(&undoStack);
                            undoCount = 0;
                            GenerateNewBatch(blockUsed);
                            blocksUsedInBatch = 0;
                            selectedIndex = 0;
                        } else {
                            int nextIndex = selectedIndex;
                            int attempts = 0;
                            do {
                                nextIndex = (nextIndex + 1) % 3;
                                attempts++;
                            } while (attempts < 3 && (blockUsed[nextIndex] || GetQueueAt(nextIndex) < 1 || GetQueueAt(nextIndex) > 35));
                            if (attempts < 3 && !blockUsed[nextIndex] && GetQueueAt(nextIndex) >= 1 && GetQueueAt(nextIndex) <= 35) {
                                selectedIndex = nextIndex;
                            }
                        }

                        ClearFullLines();

                        if (!HasAnyValidMove(blockUsed)) {
                            GameOver = false;
                        }
                    }
                }
            }

            // Undo functionality
            if (IsKeyPressed(KEY_Z)) {
                TraceLog(LOG_INFO, "Undo button clicked");
                PerformUndo(blockUsed);
            }

            // Block selection
            if (IsKeyPressed(KEY_ONE)) {
                int blockType = GetQueueAt(0);
                if (!blockUsed[0] && blockType >= 1 && blockType <= 35) selectedIndex = 0;
            }
            if (IsKeyPressed(KEY_TWO)) {
                int blockType = GetQueueAt(1);
                if (!blockUsed[1] && blockType >= 1 && blockType <= 35) selectedIndex = 1;
            }
            if (IsKeyPressed(KEY_THREE)) {
                int blockType = GetQueueAt(2);
                if (!blockUsed[2] && blockType >= 1 && blockType <= 35) selectedIndex = 2;
            }

            // Game over check
            if (!GameOver && !isInGameOverInput) {
                time_t currentTime = time(NULL);
                finalGameDuration = (int)(currentTime - gameStartTime);
                isInGameOverInput = true;
                InitUsernameInput(&usernameInput);
                PlayGameOverSound();
            }

            // Draw game elements
            Vector2 mousePos = GetMousePosition();
            int blockType = GetQueueAt(selectedIndex);
            if (blockType >= 1 && blockType <= 35 && !blockUsed[selectedIndex]) {
                DrawBlockShadow((int)mousePos.x, (int)mousePos.y, blockType);
            }

            DrawText(TextFormat("FPS: %d", GetFPS()), 10, SCREEN_HEIGHT - 25, 14, BLACK);

            // Debug info
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

            DrawScorePanel();
            DrawNextBlocks(selectedIndex, blockUsed);
            UpdateAndDrawAnimations(GetFrameTime());
            UpdateMusic();

            EndDrawing();
        }
    }

    UnloadSounds();
    CloseAudioDevice();
    CloseWindow();
}