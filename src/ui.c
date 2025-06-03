#include "../lib/ui.h"
#include "../lib/animation.h"
#include "../lib/global.h"

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
    
    // Initialize Play Button
    playButton = (MenuButton){
        .rect = {SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 - 60, 200, 50},
        .color = (Color){70, 130, 180, 255},      // Steel Blue
        .hoverColor = (Color){100, 149, 237, 255}, // Cornflower Blue
        .textColor = WHITE,
        .animScale = 1.0f,
    };
    strcpy(playButton.text, "PLAY GAME");
    
    // Initialize Settings Button
    settingsButton = (MenuButton){
        .rect = {SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 + 10, 200, 50},
        .color = (Color){60, 179, 113, 255},      // Medium Sea Green
        .hoverColor = (Color){90, 195, 140, 255},
        .textColor = WHITE,
        .animScale = 1.0f
    };
    strcpy(settingsButton.text, "SETTINGS");
    
    // Initialize About Button
    aboutButton = (MenuButton){
        .rect = {SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 + 80, 200, 50},
        .color = (Color){255, 165, 0, 255},       // Orange
        .hoverColor = (Color){255, 185, 50, 255},
        .textColor = WHITE,
        .animScale = 1.0f
    };
    strcpy(aboutButton.text, "ABOUT");
    
    // Initialize Exit Button
    exitButton = (MenuButton){
        .rect = {SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 + 150, 200, 50},
        .color = (Color){220, 20, 60, 255},       // Crimson
        .hoverColor = (Color){255, 69, 100, 255},
        .textColor = WHITE,
        .animScale = 1.0f
    };
    strcpy(exitButton.text, "EXIT");
    
    // Initialize Back Button
    backButton = (MenuButton){
        .rect = {50, SCREEN_HEIGHT - 80, 120, 40},
        .color = (Color){128, 128, 128, 255},     // Gray
        .hoverColor = (Color){160, 160, 160, 255},
        .textColor = WHITE,
        .animScale = 1.0f
    };
    strcpy(backButton.text, "BACK");
}

// Update button animation and interaction
void UpdateMenuButton(MenuButton* button, Sound clickSound) {


    Vector2 mousePos = GetMousePosition();
    bool wasPressed = button->isPressed;
    button->isHovered = CheckCollisionPointRec(mousePos, button->rect);
    button->isPressed = button->isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    
    if (button->isPressed && !wasPressed) {
        TraceLog(LOG_INFO, "Button '%s' pressed!", button->text);
        PlaySound(clickSound);
        TraceLog(LOG_INFO, "Button '%s' sounded!", button->text);
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
void DrawMainMenu(Sound clickSound) {
    DrawMenuBackground();
    DrawGameTitle();
    
    // Update and draw buttons
    UpdateMenuButton(&playButton, clickSound);
    UpdateMenuButton(&settingsButton, clickSound);
    UpdateMenuButton(&aboutButton, clickSound);
    UpdateMenuButton(&exitButton, clickSound);
    
    DrawMenuButton(&playButton);
    DrawMenuButton(&settingsButton);
    DrawMenuButton(&aboutButton);
    DrawMenuButton(&exitButton);
    
    // Draw version info
    DrawText("v0.0.1", SCREEN_WIDTH - 60, SCREEN_HEIGHT - 25, 12, (Color){150, 150, 150, 255});

}

// Draw settings menu
void DrawSettingsMenu(Sound clickSound) {
    DrawMenuBackground();
    
    // Settings title
    DrawText("SETTINGS", SCREEN_WIDTH/2 - 80, 100, 36, (Color){255, 255, 255, 255});
    
    // Settings options (placeholder)
    DrawRectangleRounded((Rectangle){SCREEN_WIDTH/2 - 150, 200, 300, 200}, 0.1f, 10, (Color){40, 40, 60, 200});
    DrawRectangleRoundedLines((Rectangle){SCREEN_WIDTH/2 - 150, 200, 300, 200}, 0.1f, 10, (Color){100, 100, 150, 255});
    
    DrawText("Sound: ON", SCREEN_WIDTH/2 - 40, 230, 20, WHITE);
    DrawText("Music: ON", SCREEN_WIDTH/2 - 40, 260, 20, WHITE);
    DrawText("Difficulty: Normal", SCREEN_WIDTH/2 - 70, 290, 20, WHITE);
    DrawText("Controls: Mouse", SCREEN_WIDTH/2 - 65, 320, 20, WHITE);
    
    UpdateMenuButton(&backButton, clickSound);
    DrawMenuButton(&backButton);
}

// Draw about menu
void DrawAboutMenu(Sound clickSound) {
    DrawMenuBackground();
    
    // About title
    DrawText("ABOUT", SCREEN_WIDTH/2 - 60, 100, 36, (Color){255, 255, 255, 255});
    
    // About content
    DrawRectangleRounded((Rectangle){SCREEN_WIDTH/2 - 200, 180, 400, 300}, 0.1f, 10, (Color){40, 40, 60, 200});
    DrawRectangleRoundedLines((Rectangle){SCREEN_WIDTH/2 - 200, 180, 400, 300}, 0.1f, 10, (Color){100, 100, 150, 255});
    
    DrawText("TAP N BLAST", SCREEN_WIDTH/2 - 80, 210, 24, (Color){255, 215, 0, 255});
    DrawText("A fun block puzzle game", SCREEN_WIDTH/2 - 85, 240, 16, WHITE);
    DrawText("", SCREEN_WIDTH/2 - 0, 270, 16, WHITE);
    DrawText("HOW TO PLAY:", SCREEN_WIDTH/2 - 55, 300, 18, (Color){100, 255, 100, 255});
    DrawText("• Place blocks on the grid", SCREEN_WIDTH/2 - 90, 325, 14, WHITE);
    DrawText("• Fill rows or columns to clear them", SCREEN_WIDTH/2 - 105, 345, 14, WHITE);
    DrawText("• Score points and avoid game over!", SCREEN_WIDTH/2 - 105, 365, 14, WHITE);
    DrawText("", SCREEN_WIDTH/2 - 0, 395, 14, WHITE);
    DrawText("Made with Raylib & C", SCREEN_WIDTH/2 - 75, 420, 14, (Color){150, 150, 150, 255});
    
    UpdateMenuButton(&backButton, clickSound);
    DrawMenuButton(&backButton);
}

// Main menu update and render function
int UpdateMainMenu(Sound clickSound) {
    menuAnimTime += GetFrameTime();
    
    switch (currentMenuState) {
        case MENU_MAIN:
            DrawMainMenu(clickSound);
            
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
            if (exitButton.isPressed) {
                return -1; // Exit game
            }
            break;
            
        case MENU_SETTINGS:
            DrawSettingsMenu(clickSound);
            if (backButton.isPressed) {
                currentMenuState = MENU_MAIN;
            }
            break;
            
        case MENU_ABOUT:
            DrawAboutMenu(clickSound);
            if (backButton.isPressed) {
                currentMenuState = MENU_MAIN;
            }
            break;
    }
    
    return 0; // Stay in menu
}
void DrawGameOverPanel() {
    DrawText("GAME OVER!", SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2 - 40, 28, (Color){200, 50, 50, 255});
    DrawText(TextFormat("Final Score: %d", score), SCREEN_WIDTH/2 - 90, SCREEN_HEIGHT/2, 20, UI_TEXT);
    DrawText("Press R to Restart", SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 + 30, 18, UI_TEXT);
}

void DrawNextBlocks(int selectedIndex, boolean* blockUsed){
    // Posisi baru - di bawah grid dan di tengah
    int panelWidth = 370;  // Panel lebih lebar untuk jarak lebih banyak
    int panelHeight = 110; // Panel lebih tinggi untuk blok lebih besar
    int panelX = (SCREEN_WIDTH - panelWidth) / 2;
    int panelY = gridOriginY + GRID_SIZE * TILE_SIZE + 20; // 20px spacing dari grid
    
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
    
    // Draw Next Blocks title - dipindahkan ke tengah atas panel
    DrawText("NEXT BLOCKS", panelX + (panelWidth/2) - 60, panelY + 10, 16, UI_TEXT);
    
    // Draw the upcoming blocks - diposisikan lurus di tengah dengan jarak yang lebih jauh
    int blockSpacing = 110;  // Meningkatkan jarak antar blok secara signifikan
    int startX = panelX + (panelWidth - 3 * blockSpacing) / 2;
    
    for (int i = 0; i < 3; i++) {
        int type = GetQueueAt(i);
        int blockX = startX + i * blockSpacing;
        int blockY = panelY + 35;
        
        // TAMBAHAN: Cek apakah blok sudah digunakan
        boolean isUsed = blockUsed[i];
        boolean isEmpty = (type == -1);
        
        // Draw slot background - berbeda untuk slot kosong, terpakai, dan tersedia
        Color slotBg;
        if (isUsed) {
            // Blok sudah digunakan - warna abu-abu sangat gelap
            slotBg = (Color){ 40, 40, 40, 200 };
        } else if (isEmpty) {
            // Slot kosong - warna abu-abu gelap
            slotBg = (Color){ 60, 60, 60, 200 };
        } else if (i == selectedIndex) {
            // Slot terpilih - warna biru muda
            slotBg = (Color){ 100, 180, 255, 255 };
        } else {
            // Slot terisi tapi tidak terpilih - warna default
            slotBg = (Color){ 80, 80, 80, 150 };
        }
        
        DrawRectangleRounded(
            (Rectangle){blockX - 5, blockY - 5, 70, 70},
            0.15f,
            10,
            slotBg
        );
        
        // Draw slot border
        Color borderColor;
        if (isUsed) {
            borderColor = (Color){60, 60, 60, 255}; // Border gelap untuk blok terpakai
        } else if (i == selectedIndex) {
            borderColor = (Color){255, 255, 255, 255}; // Border putih untuk terpilih
        } else {
            borderColor = (Color){120, 120, 120, 255}; // Border default
        }
        
        DrawRectangleRoundedLines(
            (Rectangle){blockX - 5, blockY - 5, 70, 70},
            0.15f,
            10,
            borderColor
        );
        
        if (isUsed) {
            // Blok sudah digunakan - tampilkan "USED" dengan warna redup
            DrawText("USED", blockX + 18, blockY + 20, 12, (Color){100, 100, 100, 255});
        } else if (type != -1) {
            // Draw block preview dengan ukuran lebih besar (hanya jika belum digunakan)
            float blockScale = 0.8f; // Meningkatkan skala blok preview
            for (int j = 0; j < MAX_BLOCK_SIZE; j++) {
                int bx = blockX + 30 + (int)(blockShapes[type][j].x * TILE_SIZES * blockScale);
                int by = blockY + 25 + (int)(blockShapes[type][j].y * TILE_SIZES * blockScale);
                
                DrawRectangleRounded(
                    (Rectangle){bx - TILE_SIZES*blockScale/2, by - TILE_SIZES*blockScale/2, TILE_SIZES*blockScale, TILE_SIZES*blockScale},
                    0.3f,
                    1,
                    blockColors[type-1]
                );
                
                // Tambahkan efek glossy pada blok
                DrawRectangleRounded(
                    (Rectangle){
                        bx - TILE_SIZES*blockScale/2 + TILE_SIZES*blockScale/4, 
                        by - TILE_SIZES*blockScale/2 + TILE_SIZES*blockScale/4, 
                        TILE_SIZES*blockScale/2, 
                        TILE_SIZES*blockScale/6
                    },
                    0.5f,
                    1,
                    (Color){255, 255, 255, 80}
                );
            }
        } else {
            // Slot kosong - tampilkan teks "EMPTY"
            DrawText("EMPTY", blockX + 15, blockY + 20, 12, (Color){150, 150, 150, 255});
        }
        
        // Draw selection number di bawah blok
        Color numberColor;
        if (isUsed) {
            numberColor = (Color){80, 80, 80, 255}; // Nomor redup untuk blok terpakai
        } else if (type == -1) {
            numberColor = (Color){100, 100, 100, 255}; // Nomor abu untuk slot kosong
        } else {
            numberColor = UI_TEXT; // Nomor normal untuk blok tersedia
        }
        DrawText(TextFormat("%d", i+1), blockX + 25, blockY + 55, 22, numberColor);
    }
}

void DrawBlockShadow(int cursorX, int cursorY, int blockType) {
    if (blockType < 1 || blockType > 40) return;
    
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