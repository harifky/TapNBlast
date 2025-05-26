#ifndef GLOBAL_H
#define GLOBAL_H


#include "raylib.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>

#define SCREEN_WIDTH 450
#define SCREEN_HEIGHT 650
#define TILE_SIZE 45
#define GRID_SIZE 9
#define MAX_UNDO 3
#define MAX_ANIM 20
#define cell


#define UI_PANEL_HEIGHT 90
#define UI_GRID_OFFSET_X 25
#define UI_GRID_OFFSET_Y 110
#define UI_BORDER_SIZE 2
#define UI_RADIUS 8



#define UI_BORDER (Color){ 80, 90, 120, 255 }
#define UI_GRID_BG (Color){ 200, 200, 210, 255 }
#define UI_EMPTY_CELL (Color){ 230, 230, 235, 255 }


int gridOriginX = 25;
int gridOriginY = 110;
// External global variables
extern int gridOriginX;
extern int gridOriginY;
extern int undoblocks;

#endif