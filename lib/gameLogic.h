#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include "global.h"

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
	  {{0,0},{1,0},{0,1},{1,1},{0,2},{1,2}},
	  {{0,0},{1,0},{2,0},{0,1},{1,1},{2,1}},
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
boolean HasAnyValidMove(boolean* blockUsed);
void GenerateNewBatch(boolean* blockUsed);
void InitializeRandomGrid();
void GenerateRandomBatch(boolean* blockUsed);
int GetValidRandomBlockType();
void RemoveBlockFromQueue(int index);

int comboCount = 0;
int lastClearTurn = -1;
int currentTurn = 0;

typedef struct Move {
    int blockType;
    Vector2 center;
    int queueIndex;
	boolean isScored;
    struct Move* next;
} Move;

extern Move* undoStack;
extern int undoCount;

void PushMove(Move** stack, int blockType, Vector2 center, int queueIndex, bool isScored);
bool PopMove(Move** stack, int* blockType, Vector2* center, int* queueIndex, bool* isScored);
void ClearStack(Move** stack);
boolean PerformUndo(boolean* blockUsed);
void RemoveBlockFromGrid(int x, int y, int blockType);
void ResetUndoSystem();

#endif