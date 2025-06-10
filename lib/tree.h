#ifndef TREE_H
#define TREE_H

#include "global.h"

typedef struct TreeNode {
    int blockType;
    int x, y;           
    int score;         
    int linesCleared;   
    boolean canPlace;   
    struct TreeNode* children[36]; // Anak untuk setiap kemungkinan block
    int childCount;
} TreeNode;

typedef struct {
    int blocks[3];
    int totalScore;
    boolean valid;
} BatchResult;

// void GenerateBestBatch(boolean* blockUsed);
void GenerateBestBatch(boolean* blockUsedOutput);

// Fungsi pendukung
void CloneGrid(int source[GRID_SIZE][GRID_SIZE], int dest[GRID_SIZE][GRID_SIZE]);
int CalculateBlockScore(int cloneGrid[GRID_SIZE][GRID_SIZE], int centerX, int centerY, int blockType);
TreeNode* FindBestPositionForBlock(int cloneGrid[GRID_SIZE][GRID_SIZE], int blockType);
boolean CanPlaceBlockOnGrid(int testGrid[GRID_SIZE][GRID_SIZE], int centerX, int centerY, int blockType);
void PlaceBlockOnCloneGrid(int cloneGrid[GRID_SIZE][GRID_SIZE], int centerX, int centerY, int blockType);

// Fungsi tree 
TreeNode* BuildDecisionTree(int cloneGrid[GRID_SIZE][GRID_SIZE], int depth, int maxDepth);
void FreeTree(TreeNode* node);
BatchResult EvaluateBlockSequence(int cloneGrid[GRID_SIZE][GRID_SIZE], int block1, int block2, int block3);
TreeNode* FindBestPositionForBlock_ForAllTypes(int currentGrid[GRID_SIZE][GRID_SIZE]);

// void PrintTree(TreeNode* node, int depth);
int maxDepth=2;


#endif