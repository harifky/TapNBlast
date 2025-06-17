#include "../lib/ui.h"
#include "../lib/animation.h"
#include "../lib/leaderboard.h"
#include "../lib/global.h"

// Kontribusi: Fariz (80), Faiz (10), Rifky (10)

GameState currentGameState = GAME_STATE_MENU;

void DrawGrids() {
    // Draw grid background
    DrawRectangleRounded(
        (Rectangle){
            gridOriginX - 5, 
            gridOriginY - 5, 
            TILE_SIZE * GRID_SIZE + 10, 
            TILE_SIZE * GRID_SIZE + 10
        }, 
        0.05f, 
        10, 
        UI_GRID_BG
    );
    
    // Draw border for the grid
    DrawRectangleRoundedLines(
        (Rectangle){
            gridOriginX - 5, 
            gridOriginY - 5, 
            TILE_SIZE * GRID_SIZE + 10, 
            TILE_SIZE * GRID_SIZE + 10
        }, 
        0.05f, 
        10, 
        UI_BORDER
    );
    
    // Draw cells
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            Color c = (grid[y][x] == 0) ? UI_EMPTY_CELL : blockColors[grid[y][x]-1];
            float scale = 1.0f;
            
          
            
            int size = (int)(TILE_SIZE * scale);
            int dx = gridOriginX + x * TILE_SIZE + (TILE_SIZE - size) / 2;
            int dy = gridOriginY + y * TILE_SIZE + (TILE_SIZE - size) / 2;
            

            if (grid[y][x] == 0) {
                DrawRectangleRounded(
                    (Rectangle){dx, dy, size - 2, size - 2},
                    0.2f,
                    1, 
                    c
                );
            } else {
                DrawRectangleRounded(
                    (Rectangle){dx, dy, size - 2, size - 2},
                    0.3f,
                    1, 
                    c
                );
                
                // Add glossy effect
                DrawRectangleRounded(
                    (Rectangle){dx + size/4, dy + size/4, size/2, size/6},
                    0.5f,
                    1,
                    (Color){255, 255, 255, 80}
                );
            }
        }
    }
}

void DrawScorePanel() {
    // Draw Score panel
    DrawRectangleRounded(
        (Rectangle){10, 10, 140, 85},
        0.15f,
        10,
        UI_PANEL
    );
    
    DrawRectangleRoundedLines(
        (Rectangle){10, 10, 140, 85},
        0.15f,
        10,
        UI_BORDER
    );
    
    // Draw Score title
    DrawText("SCORE", 25, 15, 20, UI_TEXT);
    
    // Draw Score value with shadow for better visibility
    DrawText(TextFormat("%d", score), 28, 42, 32, (Color){0, 0, 0, 60});
    DrawText(TextFormat("%d", score), 25, 40, 32, UI_SCORE_TEXT);
}

void InitMainMenu() {
    currentMenuState = MENU_MAIN;
    menuAnimTime = 0.0f;
    titleBounce = 0.0f;
    backgroundOffset = 0.0f;

    // Tombol Play
    playButton = (MenuButton){
        .rect       = {SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 120, 200, 50},
        .color      = (Color){70, 130, 180, 255},      // Steel Blue
        .hoverColor = (Color){100, 149, 237, 255},     // Cornflower Blue
        .textColor  = WHITE,
        .animScale  = 1.0f,
        .text = "PLAY"
    };

    // Tombol Settings
    settingsButton = (MenuButton){
        .rect       = {SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 50, 200, 50},
        .color      = (Color){60, 179, 113, 255},      // Medium Sea Green
        .hoverColor = (Color){90, 195, 140, 255},
        .textColor  = WHITE,
        .animScale  = 1.0f,
        .text = "SETTINGS"
    };

    // Tombol Leaderboard
    leaderboardButton = (MenuButton){
        .rect       = {SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 20, 200, 50},
        .color      = (Color){180, 90, 255, 255},       // Darker Orange
        .hoverColor = (Color){255, 165, 50, 255},
        .textColor  = WHITE,
        .animScale  = 1.0f,
        .text = "LEADERBOARD"
    };

    // Tombol Exit
    exitButton = (MenuButton){
        .rect       = {SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 90, 200, 50},
        .color      = (Color){220, 20, 60, 255},       // Crimson
        .hoverColor = (Color){255, 69, 100, 255},
        .textColor  = WHITE,
        .animScale  = 1.0f,
        .text = "EXIT"
    };

    // Tombol Back (untuk sub-menu)
    backButton = (MenuButton){
        .rect       = {50, SCREEN_HEIGHT - 80, 120, 40},
        .color      = (Color){128, 128, 128, 255},     // Gray
        .hoverColor = (Color){160, 160, 160, 255},
        .textColor  = WHITE,
        .animScale  = 1.0f,
        .text = "BACK"
    };

    // Tombol About
    aboutButton = (MenuButton){
        .rect = {SCREEN_WIDTH / 2 - 140, 350, 120, 30},
        .color = (Color){255, 165, 0, 255},
        .hoverColor = (Color){255, 185, 50, 255},
        .textColor = WHITE,
        .animScale = 1.0f,
        .text = "ABOUT"
    };
}


// Update button animation and interaction
void UpdateMenuButton(MenuButton* button) {


    Vector2 mousePos = GetMousePosition();
    bool wasPressed = button->isPressed;
    button->isHovered = CheckCollisionPointRec(mousePos, button->rect);
    button->isPressed = button->isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    
    if (button->isPressed && !wasPressed) {
        PlayClickSound();
    }
    
    // Smooth animation
    float targetScale = button->isHovered ? 1.05f : 1.0f;
    button->animScale += (targetScale - button->animScale) * 0.15f;

}

// Draw animated button
void DrawMenuButton(MenuButton* button) {
    Rectangle animRect = button->rect;
    
    // Apply scale animation
    float scaleDiff = (button->animScale - 1.0f) * animRect.width;
    animRect.x -= scaleDiff / 2;
    animRect.width += scaleDiff;
    animRect.height += scaleDiff * 0.25f;
    
    Color buttonColor = button->isHovered ? button->hoverColor : button->color;
    
    // Draw button shadow
    DrawRectangleRounded(
        (Rectangle){animRect.x + 3, animRect.y + 3, animRect.width, animRect.height},
        0.15f, 10, (Color){0, 0, 0, 80}
    );
    
    // Draw main button
    DrawRectangleRounded(animRect, 0.15f, 10, buttonColor);
    
    // Draw button border
    DrawRectangleRoundedLines(animRect, 0.15f, 10, (Color){255, 255, 255, 100});
    
    // Draw glossy effect
    DrawRectangleRounded(
        (Rectangle){
            animRect.x + animRect.width * 0.1f,
            animRect.y + animRect.height * 0.1f,
            animRect.width * 0.8f,
            animRect.height * 0.3f
        },
        0.2f, 5, (Color){255, 255, 255, 60}
    );
    
    // Draw button text
    int textWidth = MeasureText(button->text, 20);
    DrawText(
        button->text,
        animRect.x + (animRect.width - textWidth) / 2,
        animRect.y + (animRect.height - 20) / 2,
        20,
        button->textColor
    );
}

// Draw animated background
void DrawMenuBackground() {
    backgroundOffset += 0.5f;
    if (backgroundOffset > TILE_SIZE) backgroundOffset = 0;
    
    // Draw gradient background
    DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 
                          (Color){25, 25, 40, 255}, (Color){15, 15, 25, 255});
    
    // Draw floating blocks pattern
    for (int y = -1; y < SCREEN_HEIGHT / TILE_SIZE + 2; y++) {
        for (int x = -1; x < SCREEN_WIDTH / TILE_SIZE + 2; x++) {
            if ((x + y) % 4 == 0) {
                float alpha = 20 + 10 * sin(menuAnimTime * 0.8f + x * 0.5f + y * 0.3f);
                DrawRectangleRounded(
                    (Rectangle){
                        x * TILE_SIZE + backgroundOffset,
                        y * TILE_SIZE + backgroundOffset,
                        TILE_SIZE * 0.8f,
                        TILE_SIZE * 0.8f
                    },
                    0.3f, 5, (Color){100, 100, 150, (int)alpha}
                );
            }
        }
    }
}

// Draw game title with animation
void DrawGameTitle() {
    titleBounce = sin(menuAnimTime * 2.0f) * 3.0f;
    
    // Title shadow
    DrawText("TAP N BLAST", SCREEN_WIDTH/2 - 140 + 3, 80 + titleBounce + 3, 48, (Color){0, 0, 0, 120});
    
    // Main title with gradient effect (simulated with multiple draws)
    DrawText("TAP N BLAST", SCREEN_WIDTH/2 - 140, 80 + titleBounce, 48, (Color){255, 215, 0, 255}); // Gold
    DrawText("TAP N BLAST", SCREEN_WIDTH/2 - 140, 78 + titleBounce, 48, (Color){255, 255, 255, 180}); // White highlight
    
    // Subtitle
    DrawText("Block Puzzle Game", SCREEN_WIDTH/2 - 85, 140, 18, (Color){200, 200, 200, 255});
}

// Draw main menu
void DrawMainMenu() {

    DrawMenuBackground();
    DrawGameTitle();
    
    // Update and draw buttons
    UpdateMenuButton(&playButton);
    UpdateMenuButton(&settingsButton);
    // UpdateMenuButton(&aboutButton);
    UpdateMenuButton(&leaderboardButton);
    UpdateMenuButton(&exitButton);
    
    DrawMenuButton(&playButton);
    DrawMenuButton(&settingsButton);
    // DrawMenuButton(&aboutButton);
    DrawMenuButton(&leaderboardButton);
    DrawMenuButton(&exitButton);
    
    // Draw version info
    DrawText("v0.0.1", SCREEN_WIDTH - 60, SCREEN_HEIGHT - 25, 12, (Color){150, 150, 150, 255});

}

// Draw settings menu
void DrawSettingsMenu() {
    DrawMenuBackground();
    
    // Settings title
    DrawText("SETTINGS", SCREEN_WIDTH/2 - 80, 100, 36, (Color){255, 255, 255, 255});
    
    // Settings options (placeholder)
    DrawRectangleRounded((Rectangle){SCREEN_WIDTH/2 - 150, 200, 300, 200}, 0.1f, 10, (Color){40, 40, 60, 200});
    DrawRectangleRoundedLines((Rectangle){SCREEN_WIDTH/2 - 150, 200, 300, 200}, 0.1f, 10, (Color){100, 100, 150, 255});

    Rectangle soundRect = {SCREEN_WIDTH/2 - 60, 230, 160, 25};
    Rectangle musicRect = {SCREEN_WIDTH/2 - 60, 260, 160, 25};
    
    Vector2 mouse = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mouse, soundRect)) {
            isSoundOn = !isSoundOn;
        } else if (CheckCollisionPointRec(mouse, musicRect)) {
            isMusicOn = !isMusicOn;

            if (!isMusicOn) {
                StopBacksound();
            } else {
                // Cek state menu atau game, mainkan kembali musik yang sesuai
                if (currentMenuState == MENU_MAIN) {
                    PlayBacksoundMenu();
                } else {
                    PlayBacksoundGame();
                }
            }
        }
    }

    DrawText("Sound", soundRect.x - 60, soundRect.y + 3, 20, WHITE);
    DrawText("Music", musicRect.x - 60, musicRect.y + 3, 20, WHITE);
    
    DrawRectangle(soundRect.x + 140, soundRect.y + 2, 40, 20, (Color){100, 100, 100, 255});
    DrawRectangle(isSoundOn ? soundRect.x + 162 : soundRect.x + 142, soundRect.y + 4, 16, 16, isSoundOn ? GREEN : RED);

    DrawRectangle(musicRect.x + 140, musicRect.y + 2, 40, 20, (Color){100, 100, 100, 255});
    DrawRectangle(isMusicOn ? musicRect.x + 162 : musicRect.x + 142, musicRect.y + 4, 16, 16, isMusicOn ? GREEN : RED);

    UpdateMenuButton(&aboutButton);
    DrawMenuButton(&aboutButton);
    if (aboutButton.isPressed) {
        currentMenuState = MENU_ABOUT;
    }
    
    UpdateMenuButton(&backButton);
    DrawMenuButton(&backButton);
}

// Draw about menu
void DrawAboutMenu() {
    DrawMenuBackground();
    
    // About title
    DrawText("ABOUT", SCREEN_WIDTH/2 - 60, 100, 36, (Color){255, 255, 255, 255});
    
    // About content
    DrawRectangleRounded((Rectangle){SCREEN_WIDTH/2 - 200, 180, 400, 300}, 0.1f, 10, (Color){40, 40, 60, 200});
    DrawRectangleRoundedLines((Rectangle){SCREEN_WIDTH/2 - 200, 180, 400, 300}, 0.1f, 10, (Color){100, 100, 150, 255});
    
    DrawText("TAP N BLAST", SCREEN_WIDTH/2 - 80, 210, 24, (Color){255, 215, 0, 255});
    DrawText("Adalah permainan Blok", SCREEN_WIDTH/2 - 85, 240, 16, WHITE);
    DrawText("", SCREEN_WIDTH/2 - 0, 270, 16, WHITE);
    DrawText("HOW TO PLAY:", SCREEN_WIDTH/2 - 55, 300, 18, (Color){100, 255, 100, 255});
    DrawText("• Letakkan Blok didalam grid", SCREEN_WIDTH/2 - 90, 325, 14, WHITE);
    DrawText("• Fill rows or columns to clear them", SCREEN_WIDTH/2 - 105, 345, 14, WHITE);
    DrawText("• Score points and avoid game over!", SCREEN_WIDTH/2 - 105, 365, 14, WHITE);
    DrawText("", SCREEN_WIDTH/2 - 0, 395, 14, WHITE);
    DrawText("Made with Raylib & C", SCREEN_WIDTH/2 - 75, 420, 14, (Color){150, 150, 150, 255});
    
    UpdateMenuButton(&backButton);
    DrawMenuButton(&backButton);
}

//leaderboard Menu
void DrawLeaderboardMenu() {
    DrawMenuBackground();

    DrawText("LEADERBOARD", SCREEN_WIDTH / 2 - 100, 80, 36, GOLD);

    // Kotak tabel
    int startX = SCREEN_WIDTH / 2 - 200;
    int startY = 130;
    int rowHeight = 30;

    DrawRectangleRounded((Rectangle){startX, startY, 400, 320}, 0.1f, 10, (Color){40, 40, 60, 200});
    DrawRectangleRoundedLines((Rectangle){startX, startY, 400, 320}, 0.1f, 10, (Color){100, 100, 150, 255});

    // Header tabel
    DrawText("Rank", startX + 10, startY + 10, 18, WHITE);
    DrawText("Name", startX + 70, startY + 10, 18, WHITE);
    DrawText("Score", startX + 220, startY + 10, 18, WHITE);
    DrawText("Time", startX + 310, startY + 10, 18, WHITE);

    // Ambil data leaderboard
    LeaderboardEntry entries[MAX_LEADERBOARD_ENTRIES];
    int count = GetLeaderboardAsArray(entries, 10);

    for (int i = 0; i < count; i++) {
        char rankStr[5];
        char scoreStr[12];
        char timeStr[12];

        sprintf(rankStr, "%d.", i + 1);
        sprintf(scoreStr, "%d", entries[i].score);
        FormatDuration(entries[i].duration, timeStr, sizeof(timeStr));

        int y = startY + 40 + i * rowHeight;
        DrawText(rankStr, startX + 10, y, 16, WHITE);
        DrawText(entries[i].username, startX + 70, y, 16, WHITE);
        DrawText(scoreStr, startX + 220, y, 16, WHITE);
        DrawText(timeStr, startX + 310, y, 16, WHITE);
    }

    UpdateMenuButton(&backButton);
    DrawMenuButton(&backButton);

}

// Main menu update and render function
int UpdateMainMenu() {
    menuAnimTime += GetFrameTime();

    
    switch (currentMenuState) {
        case MENU_MAIN:
            PlayBacksoundMenu();
            DrawMainMenu();
            
            // Handle button clicks
            if (playButton.isPressed) {
                return 1; // Start game
            }
            if (settingsButton.isPressed) {
                currentMenuState = MENU_SETTINGS;
            }
            if (aboutButton.isPressed) {
                currentMenuState = MENU_ABOUT;
            }
            if (leaderboardButton.isPressed) {
                currentMenuState = MENU_LEADER;
            }
            if (exitButton.isPressed) {
                return -1; // Exit game
            }
            break;
            
        case MENU_SETTINGS:
            DrawSettingsMenu();
            if (backButton.isPressed) {
                currentMenuState = MENU_MAIN;
            }
            break;
            
        case MENU_ABOUT:
            DrawAboutMenu();
            if (backButton.isPressed) {
                currentMenuState = MENU_SETTINGS;
            }
            break;
        case MENU_LEADER:
            DrawLeaderboardMenu();
            if (backButton.isPressed) {
                currentMenuState = MENU_MAIN;
            }
            break;
    }
    
    return 0; // Stay in menu
}

void DrawGameOverPanel(UsernameInput* usernameInput, int score, int duration) {
    // Background overlay
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 180});

    // Main panel
    int panelWidth = 400;
    int panelHeight = 300;
    int panelX = (SCREEN_WIDTH - panelWidth) / 2;
    int panelY = (SCREEN_HEIGHT - panelHeight) / 2;

    DrawRectangleRounded(
        (Rectangle){panelX, panelY, panelWidth, panelHeight},
        0.15f, 10, (Color){40, 40, 60, 255}
    );

    DrawRectangleRoundedLines(
        (Rectangle){panelX, panelY, panelWidth, panelHeight},
        0.15f, 10, (Color){200, 200, 200, 255}
    );

    // Game Over title
    DrawText("GAME OVER!", panelX + (panelWidth/2) - 80, panelY + 30, 28, (Color){255, 100, 100, 255});

    // Score and duration - FIXED: Use the passed duration parameter (which is now final)
    char timeStr[16];
    FormatDuration(duration, timeStr, sizeof(timeStr));

    DrawText(TextFormat("Final Score: %d", score), panelX + 30, panelY + 80, 20, WHITE);
    DrawText(TextFormat("Time Played: %s", timeStr), panelX + 30, panelY + 110, 20, WHITE);

    // Check if this is a high score
    int rank = GetLeaderboardRank(score);
    if (rank <= MAX_LEADERBOARD_ENTRIES) { // Check against MAX_LEADERBOARD_ENTRIES
        DrawText(TextFormat("New High Score! Rank #%d", rank), panelX + 30, panelY + 140, 18, (Color){255, 215, 0, 255});
    }

    // Username input
    DrawText("Enter your name:", panelX + 30, panelY + 170, 18, WHITE);

    // Input box
    Rectangle inputBox = {panelX + 30, panelY + 195, panelWidth - 60, 30};
    DrawRectangleRounded(inputBox, 0.1f, 5, (Color){20, 20, 30, 255});
    DrawRectangleRoundedLines(inputBox, 0.1f, 5, WHITE);

    // Input text
    DrawText(usernameInput->inputText, inputBox.x + 5, inputBox.y + 5, 20, WHITE);

    // Cursor blink
    if (usernameInput->isActive && ((GetTime() * 2) - (int)(GetTime() * 2)) > 0.5f) {
        int textWidth = MeasureText(usernameInput->inputText, 20);
        DrawText("_", inputBox.x + 5 + textWidth, inputBox.y + 5, 20, WHITE);
    }

    // Instructions
    if (usernameInput->isActive) {
        // Tambahkan logika untuk default "Anonymous" jika input kosong saat ENTER
        if (IsKeyPressed(KEY_ENTER)) {
            if (usernameInput->currentLength == 0) {
                strcpy(usernameInput->inputText, "Anonymous");
                usernameInput->currentLength = strlen("Anonymous");
            }
            usernameInput->isComplete = true;
            usernameInput->isActive = false;
        }
        DrawText("Press ENTER to confirm (or leave blank for Anonymous)", panelX + 30, panelY + 240, 14, (Color){200, 200, 200, 255});
        DrawText("Press BACKSPACE to delete", panelX + 30, panelY + 260, 14, (Color){200, 200, 200, 255});
    } else if (usernameInput->isComplete) {
        DrawText("Score saved!", panelX + 30, panelY + 240, 16, (Color){100, 255, 100, 255});
    }
}

void DrawNextBlocks(int selectedIndex, boolean* blockUsed){
    // Posisi baru - di bawah grid dan di tengah
    int panelWidth = 380;
    int panelHeight = 130;
    int panelX = (SCREEN_WIDTH - panelWidth) / 2;
    int panelY = gridOriginY + GRID_SIZE * TILE_SIZE + 20;
    
    // Draw Next Blocks panel
    DrawRectangleRounded(
        (Rectangle){panelX, panelY, panelWidth, panelHeight},
        0.15f,
        10,
        UI_PANEL
    );
    
    DrawRectangleRoundedLines(
        (Rectangle){panelX, panelY, panelWidth, panelHeight},
        0.15f,
        10,
        UI_BORDER
    );
    
    // Draw Next Blocks title
    DrawText("NEXT BLOCKS", panelX + (panelWidth/2) - 60, panelY + 10, 16, UI_TEXT);
    
    // Ukuran slot dan jarak yang diperbaiki
    int slotSize = 80;  // Ukuran slot lebih besar
    int blockSpacing = 100;  // Jarak antar slot
    int startX = panelX + (panelWidth - 3 * blockSpacing + (blockSpacing - slotSize)) / 2;
    
    for (int i = 0; i < 3; i++) {
        int type = GetQueueAt(i);
        int slotX = startX + i * blockSpacing;
        int slotY = panelY + 35;
        
        // Cek status blok
        boolean isUsed = blockUsed[i];
        boolean isEmpty = (type == -1);
        
        // Draw slot background
        Color slotBg;
        if (isUsed) {
            slotBg = (Color){ 40, 40, 40, 200 };
        } else if (isEmpty) {
            slotBg = (Color){ 60, 60, 60, 200 };
        } else if (i == selectedIndex) {
            slotBg = (Color){ 100, 180, 255, 255 };
        } else {
            slotBg = (Color){ 80, 80, 80, 150 };
        }
        
        DrawRectangleRounded(
            (Rectangle){slotX, slotY, slotSize, slotSize},
            0.15f,
            10,
            slotBg
        );
        
        // Draw slot border
        Color borderColor;
        if (isUsed) {
            borderColor = (Color){60, 60, 60, 255};
        } else if (i == selectedIndex) {
            borderColor = (Color){255, 255, 255, 255};
        } else {
            borderColor = (Color){120, 120, 120, 255};
        }
        
        DrawRectangleRoundedLines(
            (Rectangle){slotX, slotY, slotSize, slotSize},
            0.15f,
            10,
            borderColor
        );
        
        if (isUsed) {
            // Tampilkan "USED" di tengah slot
            int textWidth = MeasureText("USED", 12);
            DrawText("USED", 
                slotX + (slotSize - textWidth) / 2, 
                slotY + slotSize / 2 - 6, 
                12, 
                (Color){100, 100, 100, 255}
            );
        } else if (type != -1) {
            // Hitung bounding box dari blok untuk centering yang tepat
            int minX = 0, maxX = 0, minY = 0, maxY = 0;
            boolean firstTile = true;
            
            for (int j = 0; j < MAX_BLOCK_SIZE; j++) {
                if (firstTile) {
                    minX = maxX = blockShapes[type][j].x;
                    minY = maxY = blockShapes[type][j].y;
                    firstTile = false;
                } else {
                    if (blockShapes[type][j].x < minX) minX = blockShapes[type][j].x;
                    if (blockShapes[type][j].x > maxX) maxX = blockShapes[type][j].x;
                    if (blockShapes[type][j].y < minY) minY = blockShapes[type][j].y;
                    if (blockShapes[type][j].y > maxY) maxY = blockShapes[type][j].y;
                }
            }
            
            // Hitung ukuran blok dan skala yang sesuai
            int blockWidth = (maxX - minX + 1);
            int blockHeight = (maxY - minY + 1);
            
            // Tentukan skala berdasarkan ukuran slot (dengan margin)
            int availableSpace = slotSize - 20; // 10px margin di setiap sisi
            float scaleX = (float)availableSpace / (blockWidth * TILE_SIZE);
            float scaleY = (float)availableSpace / (blockHeight * TILE_SIZE);
            float blockScale = (scaleX < scaleY) ? scaleX : scaleY;
            
            // Batasi skala maksimum
            if (blockScale > 0.8f) blockScale = 0.8f;
            
            // Hitung posisi tengah blok
            float scaledTileSize = TILE_SIZE * blockScale;
            float totalBlockWidth = blockWidth * scaledTileSize;
            float totalBlockHeight = blockHeight * scaledTileSize;
            
            int blockCenterX = slotX + slotSize / 2;
            int blockCenterY = slotY + slotSize / 2;
            
            // Offset untuk centering berdasarkan bounding box
            float offsetX = -(minX + maxX) * scaledTileSize / 2;
            float offsetY = -(minY + maxY) * scaledTileSize / 2;
            
            // Draw blok dengan posisi yang sudah diperbaiki
            for (int j = 0; j < MAX_BLOCK_SIZE; j++) {
                float tileX = blockCenterX + offsetX + blockShapes[type][j].x * scaledTileSize;
                float tileY = blockCenterY + offsetY + blockShapes[type][j].y * scaledTileSize;
                
                // Draw main tile
                DrawRectangleRounded(
                    (Rectangle){
                        tileX - scaledTileSize/2, 
                        tileY - scaledTileSize/2, 
                        scaledTileSize, 
                        scaledTileSize
                    },
                    0.3f,
                    1,
                    blockColors[type-1]
                );
                
                // Draw glossy effect
                DrawRectangleRounded(
                    (Rectangle){
                        tileX - scaledTileSize/2 + scaledTileSize/4, 
                        tileY - scaledTileSize/2 + scaledTileSize/4, 
                        scaledTileSize/2, 
                        scaledTileSize/6
                    },
                    0.5f,
                    1,
                    (Color){255, 255, 255, 80}
                );
            }
        } else {
            // Slot kosong - tampilkan "EMPTY" di tengah
            int textWidth = MeasureText("EMPTY", 12);
            DrawText("EMPTY", 
                slotX + (slotSize - textWidth) / 2, 
                slotY + slotSize / 2 - 6, 
                12, 
                (Color){150, 150, 150, 255}
            );
        }
        
        // Draw selection number di bawah slot
        Color numberColor;
        if (isUsed) {
            numberColor = (Color){80, 80, 80, 255};
        } else if (type == -1) {
            numberColor = (Color){100, 100, 100, 255};
        } else {
            numberColor = UI_TEXT;
        }
        
        char numberText[8];
        sprintf(numberText, "%d", i+1);
        int numberWidth = MeasureText(numberText, 22);
        DrawText(numberText, 
            slotX + (slotSize - numberWidth) / 2, 
            slotY + slotSize + 5, 
            22, 
            numberColor
        );
    }
}

void DrawBlockShadow(int cursorX, int cursorY, int blockType) {
    if (blockType < 1 || blockType > 36) return;
    
    // Hitung posisi grid dari posisi kursor
    int gridX = (int)((cursorX - gridOriginX) / TILE_SIZE);
    int gridY = (int)((cursorY - gridOriginY) / TILE_SIZE);
    
    // Cek apakah blok dapat ditempatkan di posisi ini
    boolean canPlace = CanPlaceBlock(gridX, gridY, blockType);
    
    // Gambar bayangan untuk setiap bagian blok
    for (int i = 0; i < MAX_BLOCK_SIZE; i++) {
        int bx = gridX + (int)blockShapes[blockType][i].x;
        int by = gridY + (int)blockShapes[blockType][i].y;
        
        // Skip jika di luar grid
        if (bx < 0 || bx >= GRID_SIZE || by < 0 || by >= GRID_SIZE) continue;
        
        int screenX = gridOriginX + bx * TILE_SIZE;
        int screenY = gridOriginY + by * TILE_SIZE;
        
        Color shadowColor;
        Color borderColor;
        
        if (canPlace) {
            // Blok dapat ditempatkan - gunakan warna normal dengan transparansi
            shadowColor = blockColors[blockType-1];
            shadowColor.a = 180;  // Transparansi untuk shadow
            borderColor = BLACK;
        } else {
            // Blok tidak dapat ditempatkan - gunakan warna merah
            shadowColor = (Color){255, 50, 50, 180};  // Merah dengan transparansi
            borderColor = (Color){200, 0, 0, 255};    // Merah gelap untuk border
        }
        
        // Gambar bayangan blok
        DrawRectangle(
            screenX + 1,  // Offset sedikit untuk efek bayangan
            screenY + 1,
            TILE_SIZE - 4, TILE_SIZE - 4,
            shadowColor
        );
        
        // Gambar garis tepi dengan warna yang sesuai
        DrawRectangleLines(
            screenX,
            screenY,
            TILE_SIZE - 2, TILE_SIZE - 2,
            borderColor
        );
        
        if (canPlace) {
            // Tambahkan highlight di sudut untuk visualisasi lebih baik (hanya jika bisa ditempatkan)
            DrawRectangle(screenX + 5, screenY + 5, 5, 5, WHITE);
        } else {
            // Tambahkan tanda X untuk indikator tidak bisa ditempatkan
            DrawLine(screenX + 5, screenY + 5, screenX + TILE_SIZE - 7, screenY + TILE_SIZE - 7, (Color){255, 255, 255, 200});
            DrawLine(screenX + TILE_SIZE - 7, screenY + 5, screenX + 5, screenY + TILE_SIZE - 7, (Color){255, 255, 255, 200});
        }
    }
}

void InitPauseMenuButtons() {
    // Tombol Lanjut
    resumeButton = (MenuButton){
        .rect       = {SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 40, 200, 50},
        .color      = (Color){70, 130, 180, 255},      // Steel Blue
        .hoverColor = (Color){100, 149, 237, 255},     // Cornflower Blue
        .textColor  = WHITE,
        .animScale  = 1.0f,
        .text = "LANJUTKAN"
    };

    // Tombol Keluar ke Menu Utama
    quitToMenuButton = (MenuButton){
        .rect       = {SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 30, 200, 50},
        .color      = (Color){220, 20, 60, 255},       // Crimson
        .hoverColor = (Color){255, 69, 100, 255},
        .textColor  = WHITE,
        .animScale  = 1.0f,
        .text = "KELUAR"
    };
}

int DrawPauseMenu() {
    // Overlay gelap
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 150});

    // Panel pop-up
    int panelWidth = 300;
    int panelHeight = 200;
    int panelX = (SCREEN_WIDTH - panelWidth) / 2;
    int panelY = (SCREEN_HEIGHT - panelHeight) / 2;

    DrawRectangleRounded((Rectangle){panelX, panelY, panelWidth, panelHeight}, 0.15f, 10, (Color){40, 40, 60, 255});
    DrawRectangleRoundedLines((Rectangle){panelX, panelY, panelWidth, panelHeight}, 0.15f, 10, (Color){200, 200, 200, 255});

    // Judul
    DrawText("PAUSE", panelX + (panelWidth / 2) - MeasureText("PAUSE", 30) / 2, panelY + 30, 30, WHITE);

    // Update dan gambar tombol
    UpdateMenuButton(&resumeButton);
    UpdateMenuButton(&quitToMenuButton);

    DrawMenuButton(&resumeButton);
    DrawMenuButton(&quitToMenuButton);

    if (resumeButton.isPressed) {
        return 1; // Lanjutkan permainan
    }
    if (quitToMenuButton.isPressed) {
        return 2; // Keluar ke menu utama (dan game over input)
    }

    return 0; // Tetap di menu pause
}