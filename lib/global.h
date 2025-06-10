#ifndef GLOBAL_H
#define GLOBAL_H

#include "raylib.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include "boolean.h"

#define SCREEN_WIDTH 450
#define SCREEN_HEIGHT 650
#define TILE_SIZE 45
#define GRID_SIZE 9
#define MAX_UNDO 3
#define MAX_ANIM 20
#define cell
#define BLOCK_TYPES 36
#define TILE_SIZES 25

#define UI_PANEL_HEIGHT 90
#define UI_GRID_OFFSET_X 25
#define UI_GRID_OFFSET_Y 110
#define UI_BORDER_SIZE 2
#define UI_RADIUS 8
#define MAX_BLOCK_SIZE 9
#define BLOCK_SIZE 45
#define PADDING 2
#define GRID_WIDTH 10
#define GRID_HEIGHT 10

#define SCORE_PER_LINE_CLEARED 100
#define SCORE_PER_FILLED_CELL 5
#define BONUS_FOR_MULTIPLE_LINES_FACTOR 50

#define PENALTY_PER_HOLE 75
#define PENALTY_PER_AGGREGATE_HEIGHT_POINT 2 
#define PENALTY_PER_BUMPINESS_POINT 5      
#define PENALTY_PER_MAX_HEIGHT_POINT 1     

#define UI_BORDER (Color){ 80, 90, 120, 255 }
#define UI_GRID_BG (Color){ 200, 200, 210, 255 }
#define UI_EMPTY_CELL (Color){ 230, 230, 235, 255 }
#define UI_TEXT (Color){ 40, 45, 60, 255 }
#define UI_SCORE_TEXT (Color){ 20, 80, 150, 255 }
#define UI_PANEL (Color){ 220, 220, 230, 255 }

// Grid global
int grid[GRID_SIZE][GRID_SIZE] = {0};

// Array warna dengan 40 warna unik dan bervariasi
Color blockColors[BLOCK_TYPES] = { 
    // Baris 1: Warna dasar cerah
    (Color){ 255, 90, 90, 255 },     // Red
    (Color){ 255, 150, 50, 255 },    // Orange  
    (Color){ 255, 220, 60, 255 },    // Yellow
    (Color){ 90, 180, 90, 255 },     // Green
    (Color){ 50, 150, 255, 255 },    // Blue
    (Color){ 180, 90, 255, 255 },    // Purple
    (Color){ 255, 120, 180, 255 },   // Pink
    (Color){ 90, 220, 220, 255 },    // Cyan
    
    // Baris 2: Warna medium
    (Color){ 200, 60, 60, 255 },     // Dark Red
    (Color){ 220, 120, 40, 255 },    // Dark Orange
    (Color){ 200, 180, 50, 255 },    // Dark Yellow
    (Color){ 70, 140, 70, 255 },     // Dark Green
    (Color){ 40, 120, 200, 255 },    // Dark Blue
    (Color){ 140, 70, 200, 255 },    // Dark Purple
    (Color){ 200, 90, 140, 255 },    // Dark Pink
    (Color){ 70, 170, 170, 255 },    // Dark Cyan
    
    // Baris 3: Warna pastel
    (Color){ 255, 180, 180, 255 },   // Light Red
    (Color){ 255, 200, 150, 255 },   // Light Orange
    (Color){ 255, 240, 160, 255 },   // Light Yellow
    (Color){ 180, 220, 180, 255 },   // Light Green
    (Color){ 150, 200, 255, 255 },   // Light Blue
    (Color){ 200, 180, 255, 255 },   // Light Purple
    (Color){ 255, 180, 220, 255 },   // Light Pink
    (Color){ 180, 240, 240, 255 },   // Light Cyan
    
    // Baris 4: Warna earthtone
    (Color){ 160, 80, 50, 255 },     // Brown
    (Color){ 180, 140, 90, 255 },    // Tan
    (Color){ 120, 100, 80, 255 },    // Olive Brown
    (Color){ 100, 120, 60, 255 },    // Olive Green
    (Color){ 80, 100, 120, 255 },    // Steel Blue
    (Color){ 120, 80, 100, 255 },    // Mauve
    (Color){ 140, 120, 100, 255 },   // Beige
    (Color){ 100, 140, 140, 255 },   // Teal
    
    // Baris 5: Warna vivid
    (Color){ 255, 60, 120, 255 },    // Hot Pink
    (Color){ 255, 100, 0, 255 },     // Bright Orange
    (Color){ 200, 255, 0, 255 },     // Lime
    (Color){ 0, 255, 100, 255 },     // Spring Green
    (Color){ 0, 200, 255, 255 },     // Sky Blue
    (Color){ 100, 0, 255, 255 },     // Electric Purple
    (Color){ 255, 0, 200, 255 },     // Magenta
    (Color){ 0, 255, 255, 255 },     // Electric Cyan
};

typedef enum {
    GAME_STATE_MENU,
    GAME_STATE_PLAYING,
    GAME_STATE_PAUSED
} GameState;

int gridOriginX = 25;
int gridOriginY = 110;

// Skor
int score = 0;

// External declarations (untuk file lain yang menggunakan variabel ini)
extern int undoblocks;

// Fungsi helper untuk mendapatkan warna blok dengan aman
static inline Color GetBlockColor(int blockType) {
    if (blockType <= 0 || blockType > BLOCK_TYPES) {
        return GRAY; // Warna default untuk tipe tidak valid
    }
    return blockColors[blockType - 1]; // blockType 1-40 -> index 0-39
}

#endif