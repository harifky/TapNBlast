#include "start.h"

UsernameInput usernameInput;
bool isInGameOverInput = false;
bool leaderboardSaved = false;
time_t gameStartTime;
int gameDuration;
int finalGameDuration = 0;
bool shouldReturnToMenu = false;

void StartGame() {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tap N Blast");
  SetTargetFPS(60);
  srand(time(NULL));
  InitAudioDevice();

    InitLeaderboard();
    Sound clickSound = LoadSound("assets/buttonfx.wav");
    SetSoundVolume(clickSound, 1.0f);

    while (!WindowShouldClose()) {
        InitMainMenu();
        shouldReturnToMenu = false;

        while (!WindowShouldClose() && !shouldReturnToMenu) {
            BeginDrawing();
            ClearBackground(BLACK);
            int menuResult = UpdateMainMenu(clickSound);
            EndDrawing();

            if (menuResult == 1) break;
            if (menuResult == -1) {
                UnloadSound(clickSound);
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

        GenerateNewBatch(blockUsed);

        while (selectedIndex < 3 && (GetQueueAt(selectedIndex) == -1 || blockUsed[selectedIndex])) {
            selectedIndex++;
        }
        if (selectedIndex >= 3) selectedIndex = 0;

        InitializeRandomGrid();

        while (!WindowShouldClose() && !shouldReturnToMenu) {
            BeginDrawing();
            ClearBackground(PURPLE);

            if (isInGameOverInput) {
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

            int currentBlock = GetQueueAt(selectedIndex);
            DrawGrids();

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 m = GetMousePosition();
                int gx = (int)((m.x - gridOriginX) / TILE_SIZE);
                int gy = (int)((m.y - gridOriginY) / TILE_SIZE);

                if (gx >= 0 && gx < GRID_WIDTH && gy >= 0 && gy < GRID_HEIGHT &&
                    currentBlock >= 1 && currentBlock <= 35 && !blockUsed[selectedIndex]) {

                    if (CanPlaceBlock(gx, gy, currentBlock) && GameOver) {
                        PlaceBlock(gx, gy, currentBlock);
                        blockUsed[selectedIndex] = true;
                        blocksUsedInBatch++;

                        if (blocksUsedInBatch >= 3) {
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

            if (!GameOver && !isInGameOverInput) {
                time_t currentTime = time(NULL);
                finalGameDuration = (int)(currentTime - gameStartTime);
                isInGameOverInput = true;
                InitUsernameInput(&usernameInput);
            }

            Vector2 mousePos = GetMousePosition();
            int blockType = GetQueueAt(selectedIndex);
            if (blockType >= 1 && blockType <= 35 && !blockUsed[selectedIndex]) {
                DrawBlockShadow((int)mousePos.x, (int)mousePos.y, blockType);
            }

            DrawText(TextFormat("FPS: %d", GetFPS()), 10, SCREEN_HEIGHT - 25, 14, BLACK);

            char batchInfo[200];
            sprintf(batchInfo, "Batch: [%s:%d] [%s:%d] [%s:%d] - Used: %d/3 - Selected: %d",
                    blockUsed[0] ? "Used" : "Ready", GetQueueAt(0),
                    blockUsed[1] ? "Used" : "Ready", GetQueueAt(1),
                    blockUsed[2] ? "Used" : "Ready", GetQueueAt(2),
                    blocksUsedInBatch, selectedIndex + 1);
            DrawText(batchInfo, 10, SCREEN_HEIGHT - 65, 12, BLACK);

            DrawScorePanel();
            DrawNextBlocks(selectedIndex, blockUsed);

            EndDrawing();
        }
    }

    UnloadSound(clickSound);
    CloseAudioDevice();
    CloseWindow();
}
