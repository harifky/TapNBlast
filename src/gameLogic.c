#include "../lib/gameLogic.h"
#include "../lib/global.h"
#include "../lib/boolean.h"


void Enqueue(int blockType) {
    QueueNode *newNode = (QueueNode *)malloc(sizeof(QueueNode));
    newNode->blockType = blockType;
    newNode->next = NULL;
    if (queueRear == NULL) {
        queueFront = queueRear = newNode;
    } else {
        queueRear->next = newNode;
        queueRear = newNode;
    }
}

int Dequeue() {
    if (queueFront == NULL) return -1;
    int value = queueFront->blockType;
    QueueNode *temp = queueFront;
    queueFront = queueFront->next;
    if (queueFront == NULL) queueRear = NULL;
    free(temp);
    return value;
}

int GetQueue(){
    QueueNode *current = queueFront;
    return current->blockType;
}


void PlaceBlock(int centerX, int centerY, int blockType) {
    for (int i = 0; i < MAX_BLOCK_SIZE; i++) {
        int bx = centerX + (int)blockShapes[blockType - 1][i].x;
        int by = centerY + (int)blockShapes[blockType - 1][i].y;
        if (bx >= 0 && bx < GRID_SIZE && by >= 0 && by < GRID_SIZE) {
            grid[by][bx] = blockType;
        }
    }
}

boolean HasValidPlacement(int blockType) {
    if (blockType < 1 || blockType > 40) return false;
    
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            if (CanPlaceBlock(x, y, blockType)) return true;
        }
    }
    return false;
}

boolean CanPlaceBlock(int centerX, int centerY, int blockType) {
    if (blockType < 1 || blockType > 40) return false;
    
    for (int i = 0; i < MAX_BLOCK_SIZE; i++) {
        int bx = centerX + (int)blockShapes[blockType - 1][i].x;
        int by = centerY + (int)blockShapes[blockType - 1][i].y;
        if (bx < 0 || bx >= GRID_SIZE || by < 0 || by >= GRID_SIZE || grid[by][bx] != 0) {
            return false;
        }
    }
    return true;
}

void ClearFullLines(){
   
    for (int y = 0; y < GRID_SIZE; y++) {
        boolean full = true;
        for (int x = 0; x < GRID_SIZE; x++) {
            if (grid[y][x] == 0) {
                full = false;
                break;
            }
        }
        if (full) {
            for (int x = 0; x < GRID_SIZE; x++) grid[y][x] = 0;
            score = score + 100;
        }
    }
    
    for (int x = 0; x < GRID_SIZE; x++) {
        boolean full = true;
        for (int y = 0; y < GRID_SIZE; y++) {
            if (grid[y][x] == 0) {
                full = false;
                break;
            }
        }
        if (full) {
            for (int y = 0; y < GRID_SIZE; y++) grid[y][x] = 0;
            score = score + 100;
        }
    }
}

