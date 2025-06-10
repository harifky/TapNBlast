#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include "global.h"
#include "boolean.h"

typedef struct QueueNode {
    int blockType;
    struct QueueNode *next;
} QueueNode;

QueueNode *queueFront = NULL;
QueueNode *queueRear = NULL;


const Vector2 blockShapes[BLOCK_TYPES][9] = {
	  {{0,0},{0,1},{0,2},{1,2}}, 
	  {{0,0},{0,1},{0,2},{-1,2}},
	  {{0,0},{1,0},{2,0},{0,-1}},
	  {{0,0},{1,0},{2,0},{0,1}},
	  {{0,0},{-1,0},{-2,0},{0,1}},
	  {{0,0},{-1,0},{-2,0},{0,-1}},
	  {{0,0},{1,0},{1,1},{0,1}},
	  {{0,0},{1,0},{0,1},{0,-1}},
	  {{0,0},{-1,0},{0,1},{0,-1}},
	  {{0,0},{1,0},{2,0},{1,-1}},
	  {{0,0},{1,0},{2,0},{1,1}},
	  {{0,0},{1,0}},
	  {{0,0},{0,1}},
	  {{0,0},{0,-1},{1,0}},
	  {{0,0},{0,1},{1,0}},
	  {{0,0},{1,-1},{1,0}},
	  {{0,0},{1,1},{1,0}},
	  {{0,0}},
	  {{0,0},{1,0},{2,0},{0,1},{1,1},{2,1},{0,2},{1,2},{2,2}},
	  {{0,0},{1,0},{0,1},{-1,1}},        
      {{0,0},{-1,0},{0,1},{1,1}},        
      {{0,0},{0,1},{1,1},{1,2}},         
      {{0,0},{0,1},{-1,1},{-1,2}},
	  {{0,0},{1,1},{2,2}},
	  {{0,0},{1,1}},
	  {{0,0},{1,0},{2,0},{3,0}},
	  {{0,0},{1,0},{2,0},{3,0},{4,0}},
	  {{0,0},{0,1},{0,2},{0,3}},
	  {{0,0},{0,1},{0,2},{0,3},{0,4}},
	  {{0,0},{1,0},{2,0}},
	  {{0,0},{0,1},{0,2}},
	  {{0,0},{1,0},{2,0},{0,1},{0,2}}, 
	  {{0,0},{-1,0},{-2,0},{0,1},{0,2}},
	  {{0,0},{-1,0},{-2,0},{0,-1},{0,-2}},
	  {{0,0},{1,0},{2,0},{0,-1},{0,-2}}, 
};

void Enqueue(int blockType);
int Dequeue();
int GetQueue();
void PlaceBlock(int centerX, int centerY, int blockType);
boolean HasValidPlacement(int blockType);
boolean CanPlaceBlock(int centerX, int centerY, int blockType);
void ClearFullLines();
int GetQueueAt(int index);
void ClearQueue();

typedef struct Move {
    int blockType;
    int centerX;
    int centerY;
    int queueIndex;
	boolean isScored;
    struct Move* next;
} Move;

extern Move* undoStack;
extern int undoCount;

void PushMove(Move** stack, int blockType, int centerX, int centerY, int queueIndex, boolean isScored);
bool PopMove(Move** stack, int* blockType, int* centerX, int* centerY, int* queueIndex, boolean* isScored);
void ClearStack(Move** stack);
bool PerformUndo(boolean* blockUsed);

void RemoveBlockFromGrid(int centerX, int centerY, int blockType);

#endif