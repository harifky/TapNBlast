#ifndef GLOBAL_H
#define GLOBAL_H


#include "raylib.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#define SCREEN_WIDTH 450
#define SCREEN_HEIGHT 650
#define TILE_SIZE 45
#define GRID_SIZE 9
#define MAX_UNDO 3
#define MAX_ANIM 20
#define cell
#define BLOCK_TYPES 40 
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

#define UI_BORDER (Color){ 80, 90, 120, 255 }
#define UI_GRID_BG (Color){ 200, 200, 210, 255 }
#define UI_EMPTY_CELL (Color){ 230, 230, 235, 255 }
#define UI_TEXT (Color){ 40, 45, 60, 255 }
#define UI_SCORE_TEXT (Color){ 20, 80, 150, 255 }
#define UI_PANEL (Color){ 220, 220, 230, 255 }


int grid[GRID_SIZE][GRID_SIZE] = {0};


Color blockColors[BLOCK_TYPES] = { 
    (Color){ 255, 90, 90, 255 },     // Red
    (Color){ 255, 150, 50, 255 },    // Orange
    (Color){ 180, 90, 255, 255 },    // Purple
    (Color){ 90, 180, 90, 255 },     // Green
    (Color){ 50, 150, 255, 255 },    // Blue
    (Color){ 255, 220, 60, 255 },     // Yellow
    (Color){ 255, 90, 90, 255 },     // Red
    (Color){ 255, 150, 50, 255 },    // Orange
    (Color){ 180, 90, 255, 255 },    // Purple
    (Color){ 90, 180, 90, 255 },     // Green
    (Color){ 50, 150, 255, 255 },    // Blue
    (Color){ 255, 220, 60, 255 },     // Yellow
    (Color){ 255, 90, 90, 255 },     // Red
    (Color){ 255, 150, 50, 255 },    // Orange
    (Color){ 180, 90, 255, 255 },    // Purple
    (Color){ 90, 180, 90, 255 },     // Green
    (Color){ 50, 150, 255, 255 },    // Blue
    (Color){ 255, 220, 60, 255 },     // Yellow
    (Color){ 255, 90, 90, 255 },     // Red
    (Color){ 255, 150, 50, 255 },    // Orange
    (Color){ 180, 90, 255, 255 },    // Purple
    (Color){ 90, 180, 90, 255 },     // Green
    (Color){ 50, 150, 255, 255 },    // Blue
    (Color){ 255, 220, 60, 255 },     // Yellow
    (Color){ 255, 90, 90, 255 },     // Red
    (Color){ 255, 150, 50, 255 },    // Orange
    (Color){ 180, 90, 255, 255 },    // Purple
    (Color){ 90, 180, 90, 255 },     // Green
    (Color){ 50, 150, 255, 255 },    // Blue
    (Color){ 255, 220, 60, 255 },     // Yellow
    (Color){ 255, 90, 90, 255 },     // Red
    (Color){ 255, 150, 50, 255 },    // Orange
    (Color){ 180, 90, 255, 255 },    // Purple
    (Color){ 90, 180, 90, 255 },     // Green
    (Color){ 50, 150, 255, 255 },    // Blue
    (Color){ 255, 220, 60, 255 },     // Yellow
    (Color){ 255, 90, 90, 255 },     // Red
    (Color){ 255, 150, 50, 255 },    // Orange
    (Color){ 180, 90, 255, 255 },    // Purple
    (Color){ 90, 180, 90, 255 },     // Green
    (Color){ 50, 150, 255, 255 },    // Blue
    (Color){ 255, 220, 60, 255 },     // Yellow
    (Color){ 255, 90, 90, 255 },     // Red
    (Color){ 255, 150, 50, 255 },    // Orange
    (Color){ 180, 90, 255, 255 },    // Purple
    (Color){ 90, 180, 90, 255 },     // Green
    (Color){ 50, 150, 255, 255 },    // Blue
    (Color){ 255, 220, 60, 255 },     // Yellow
};


int gridOriginX = 25;
int gridOriginY = 110;
int score = 0;
// External global variables
extern int gridOriginX;
extern int gridOriginY;
extern int undoblocks;

#endif