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

void DrawGameOverPanel() {
    DrawText("GAME OVER!", SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2 - 40, 28, (Color){200, 50, 50, 255});
    DrawText(TextFormat("Final Score: %d", score), SCREEN_WIDTH/2 - 90, SCREEN_HEIGHT/2, 20, UI_TEXT);
    DrawText("Press R to Restart", SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 + 30, 18, UI_TEXT);
}

void DrawNextBlocks(int selectedIndex, int currentBatch[3]) {
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
        int type = currentBatch[i]; // Menggunakan currentBatch instead of GetQueueAt(i)
        int blockX = startX + i * blockSpacing;
        int blockY = panelY + 35;
        
        // Draw slot background - berbeda untuk slot kosong dan terisi
        Color slotBg;
        if (type == -1) {
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
        Color borderColor = (i == selectedIndex) ? (Color){255, 255, 255, 255} : (Color){120, 120, 120, 255};
        DrawRectangleRoundedLines(
            (Rectangle){blockX - 5, blockY - 5, 70, 70},
            0.15f,
            10,
            borderColor
        );
        
        if (type != -1) {
            // Draw block preview dengan ukuran lebih besar
            float blockScale = 0.8f; // Meningkatkan skala blok preview
            for (int j = 0; j < MAX_BLOCK_SIZE; j++) {
                int bx = blockX + 30 + (int)(blockShapes[type][j].x * TILE_SIZES * blockScale);
                int by = blockY + 25 + (int)(blockShapes[type][j].y * TILE_SIZES * blockScale);
                
                DrawRectangleRounded(
                    (Rectangle){bx - TILE_SIZES*blockScale/2, by - TILE_SIZES*blockScale/2, TILE_SIZES*blockScale, TILE_SIZES*blockScale},
                    0.3f,
                    1,
                    blockColors[type]
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
        Color numberColor = (type == -1) ? (Color){100, 100, 100, 255} : UI_TEXT;
        DrawText(TextFormat("%d", i+1), blockX + 25, blockY + 55, 22, numberColor);
    }
}