#include "start.h"

UsernameInput usernameInput;
bool isInGameOverInput = false;
bool leaderboardSaved = false;
time_t gameStartTime;
int gameDuration;
int finalGameDuration = 0;
bool shouldReturnToMenu = false;

int GetValidRandomBlockType() {
  // Pastikan selalu mengembalikan blok yang valid (1-40)
  return GetRandomValue(1, 35);
}

void GenerateNewBatch(boolean *blockUsed) {
  ClearQueue();

  for (int i = 0; i < 3; i++) {
    int blockType;
    int attempts = 0;

    // Loop hingga mendapat blok yang valid
    do {
      blockType = GetValidRandomBlockType();
      attempts++;

      // Jika sudah 50 percobaan, gunakan blok sequential untuk memastikan
      // validitas
      if (attempts >= 50) {
        blockType = (i % 35) + 1; // Gunakan blok 1-40 secara berurutan
        break;
      }
    } while (blockType < 1 || blockType > 35);

    // Double check - pastikan blockType valid
    if (blockType < 1 || blockType > 35) {
      blockType = (i % 40) + 1; // Fallback ke blok sequential
    }

    Enqueue(blockType);
    blockUsed[i] = false;
  }

  // Verifikasi bahwa queue berisi 3 blok yang valid
  for (int i = 0; i < 3; i++) {
    int queueBlock = GetQueueAt(i);
    if (queueBlock == -1 || queueBlock < 1 || queueBlock > 35) {
      // Jika ada blok yang tidak valid, ganti dengan blok default
      // Asumsi ada fungsi untuk mengatur elemen queue secara langsung
      // Atau bisa menggunakan ClearQueue dan rebuild seluruhnya
      ClearQueue();
      for (int j = 0; j < 3; j++) {
        Enqueue((j % 35) + 1); // Gunakan blok 1, 2, 3 sebagai fallback
        blockUsed[j] = false;
      }
      break;
    }
  }
}

// Fungsi tambahan untuk memvalidasi apakah masih ada gerakan yang valid
boolean HasAnyValidMove(boolean *blockUsed) {
  for (int i = 0; i < 3; i++) {
    int blockType = GetQueueAt(i);

    // Cek apakah slot ini berisi blok valid dan belum digunakan
    if (blockType >= 1 && blockType <= 35 && !blockUsed[i]) {
      // Cek apakah blok ini bisa ditempatkan di mana saja di grid
      if (HasValidPlacement(blockType)) {
        return true;
      }
    }
  }
  return false;
}

// TAMBAHAN: Fungsi untuk reset game dan kembali ke menu
void ReturnToMainMenu() {
  // Reset semua variabel game
  isInGameOverInput = false;
  leaderboardSaved = false;
  shouldReturnToMenu = true; // Set flag instead of calling StartGame
}

// Modifikasi fungsi StartGame - bagian yang relevan
void StartGame() {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tap N Blast");
  SetTargetFPS(60);
  srand(time(NULL));
  InitAudioDevice();

  InitLeaderboard();
  Sound clickSound = LoadSound("assets/buttonfx.wav");
  SetSoundVolume(clickSound, 1.0f);
  
  // PERBAIKAN 3: Main game loop dengan proper menu handling
  while (!WindowShouldClose()) {
    
    // MENU PHASE
    InitMainMenu();
    shouldReturnToMenu = false; // Reset flag
    
    // Show main menu
    while (!WindowShouldClose() && !shouldReturnToMenu) {
      BeginDrawing();
      ClearBackground(BLACK);
      int menuResult = UpdateMainMenu(clickSound);
      EndDrawing();

      if (menuResult == 1) break; // Play button pressed
      if (menuResult == -1) {
        // Exit button pressed
        UnloadSound(clickSound);
        CloseAudioDevice();
        CloseWindow();
        return;
      }
    }
    
    // If shouldReturnToMenu is true, continue to show menu again
    if (shouldReturnToMenu) continue;
    
    // If window should close, exit
    if (WindowShouldClose()) break;

    // GAME PHASE - Reset game variables
    gameStartTime = time(NULL);
    finalGameDuration = 0;
    boolean GameOver = true;
    int selectedIndex = 0;
    int blocksUsedInBatch = 0;
    boolean blockUsed[3] = {false, false, false};
    isInGameOverInput = false;
    leaderboardSaved = false;
    
    // Reset score if needed
    score = 0;
    
    // Initialize game
    GenerateNewBatch(blockUsed);
    
    // Pastikan selectedIndex menunjuk ke blok yang valid
    while (selectedIndex < 3 &&
           (GetQueueAt(selectedIndex) == -1 || blockUsed[selectedIndex])) {
      selectedIndex++;
    }
    if (selectedIndex >= 3) selectedIndex = 0;

    // GAME LOOP
    while (!WindowShouldClose() && !shouldReturnToMenu) {
      BeginDrawing();
      ClearBackground(PURPLE);

      // PERBAIKAN 4: Handle game over input dengan proper flow
      if (isInGameOverInput) {
        UpdateUsernameInput(&usernameInput);
        DrawGameOverPanel(&usernameInput, score, finalGameDuration);

        // Jika input selesai dan belum disimpan
        if (usernameInput.isComplete && !leaderboardSaved) {
          AddToLeaderboard(usernameInput.inputText, score, finalGameDuration); // Use finalGameDuration instead of gameDuration
          leaderboardSaved = true;
          
          // PERBAIKAN 5: Set flag instead of immediate return
          shouldReturnToMenu = true;
        }

        // Allow skip input with ESC
        if (IsKeyPressed(KEY_ESCAPE)) {
          shouldReturnToMenu = true;
        }

        EndDrawing();
        
        // PERBAIKAN 6: Check flag after EndDrawing
        if (shouldReturnToMenu) break;
        
        continue;
      }

      // Regular game logic here...
      int currentBlock = GetQueueAt(selectedIndex);
      DrawGrids();

      if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 m = GetMousePosition();
        int gx = (int)((m.x - gridOriginX) / TILE_SIZE);
        int gy = (int)((m.y - gridOriginY) / TILE_SIZE);

        if (gx >= 0 && gx < GRID_WIDTH && gy >= 0 && gy < GRID_HEIGHT &&
            currentBlock >= 1 && currentBlock <= 35 &&
            !blockUsed[selectedIndex]) {

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
              } while (attempts < 3 &&
                       (blockUsed[nextIndex] || GetQueueAt(nextIndex) < 1 ||
                        GetQueueAt(nextIndex) > 35));

              if (attempts < 3 && !blockUsed[nextIndex] &&
                  GetQueueAt(nextIndex) >= 1 && GetQueueAt(nextIndex) <= 35) {
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

      // Key input for block selection
      if (IsKeyPressed(KEY_ONE)) {
        int blockType = GetQueueAt(0);
        if (!blockUsed[0] && blockType >= 1 && blockType <= 35) {
          selectedIndex = 0;
        }
      }
      if (IsKeyPressed(KEY_TWO)) {
        int blockType = GetQueueAt(1);
        if (!blockUsed[1] && blockType >= 1 && blockType <= 35) {
          selectedIndex = 1;
        }
      }
      if (IsKeyPressed(KEY_THREE)) {
        int blockType = GetQueueAt(2);
        if (!blockUsed[2] && blockType >= 1 && blockType <= 35) {
          selectedIndex = 2;
        }
      }

      // PERBAIKAN 7: Proper game over transition
      if (!GameOver && !isInGameOverInput) {
        time_t currentTime = time(NULL);
        finalGameDuration = (int)(currentTime - gameStartTime);
        
        TraceLog(LOG_INFO, "Game ended. Start: %ld, End: %ld, Duration: %d",
                 gameStartTime, currentTime, finalGameDuration);

        isInGameOverInput = true;
        leaderboardSaved = false;
        InitUsernameInput(&usernameInput);
      }

      // Reset game with R - return to menu
      if (!GameOver && IsKeyPressed(KEY_R) && !isInGameOverInput) {
        shouldReturnToMenu = true;
      }

      // Draw game elements
      Vector2 mousePos = GetMousePosition();
      int blockType = GetQueueAt(selectedIndex);

      if (blockType >= 1 && blockType <= 35 && !blockUsed[selectedIndex]) {
        DrawBlockShadow((int)mousePos.x, (int)mousePos.y, blockType);
      }

      // Debug info
      DrawText(TextFormat("FPS: %d", GetFPS()), 10, SCREEN_HEIGHT - 25, 14, BLACK);

      char batchInfo[200];
      sprintf(batchInfo,
              "Batch: [%s:%d] [%s:%d] [%s:%d] - Used: %d/3 - Selected: %d",
              blockUsed[0] ? "Used" : "Ready", GetQueueAt(0),
              blockUsed[1] ? "Used" : "Ready", GetQueueAt(1),
              blockUsed[2] ? "Used" : "Ready", GetQueueAt(2), blocksUsedInBatch,
              selectedIndex + 1);
      DrawText(batchInfo, 10, SCREEN_HEIGHT - 45, 12, BLACK);

      boolean hasEmptySlot = false;
      for (int i = 0; i < 3; i++) {
        if (GetQueueAt(i) == -1 || GetQueueAt(i) < 1 || GetQueueAt(i) > 35) {
          hasEmptySlot = true;
          break;
        }
      }
      if (hasEmptySlot) {
        DrawText("WARNING: Empty slots detected!", 10, SCREEN_HEIGHT - 65, 12, RED);
      }

      DrawScorePanel();
      DrawNextBlocks(selectedIndex, blockUsed);

      EndDrawing();
      
      // Check if should return to menu
      if (shouldReturnToMenu) break;
    }
    
    // If window should close, exit main loop
    if (WindowShouldClose()) break;
  }

  // Cleanup
  UnloadSound(clickSound);
  CloseAudioDevice();
  CloseWindow();
}