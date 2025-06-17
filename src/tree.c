#include "../lib/tree.h"
#include "../lib/global.h"
#include "../lib/gameLogic.h"

// Kontribusi: Fariz (100)

boolean IsGridEmpty(int grid[GRID_SIZE][GRID_SIZE]) {
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            if (grid[y][x] != 0) {
                return false;
            }
        }
    }
    return true;
}

BatchResult EvaluateBlockSequence(int initialGrid[GRID_SIZE][GRID_SIZE], int block1, int block2, int block3) {
    BatchResult result = {0};
    result.blocks[0] = block1;
    result.blocks[1] = block2;
    result.blocks[2] = block3;
    result.totalScore = 0; 
    result.valid = true;

    int tempGrid[GRID_SIZE][GRID_SIZE];
    CloneGrid(initialGrid, tempGrid); 

    int currentBlocks[3] = {block1, block2, block3};

    for (int i = 0; i < 3; i++) {
        int currentBlock = currentBlocks[i];
       
        TreeNode* bestPosNode = FindBestPositionForBlock(tempGrid, currentBlock);

        if (!bestPosNode || !bestPosNode->canPlace) {
            result.valid = false;
            result.totalScore = INT_MIN; 
            if(bestPosNode) free(bestPosNode);
            break;
        }

        result.totalScore += bestPosNode->score;

        PlaceBlockOnCloneGrid(tempGrid, bestPosNode->x, bestPosNode->y, currentBlock);

        free(bestPosNode);
    }
    return result;
}

TreeNode* FindBestPositionForBlock(int cloneGrid[GRID_SIZE][GRID_SIZE], int blockType) {
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    node->blockType = blockType;
    node->score = INT_MIN;
    node->x = -1;
    node->y = -1;
    node->linesCleared = 0;
    node->canPlace = false;
    node->childCount = 0;
    
    for (int i = 0; i < 36; i++) {
        node->children[i] = NULL;
    }
    
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            if (CanPlaceBlockOnGrid(cloneGrid, x, y, blockType)) {
                int score = CalculateBlockScore(cloneGrid, x, y, blockType);
                
                if (score > node->score) {
                    node->score = score;
                    node->x = x;
                    node->y = y;
                    node->canPlace = true;
                }
            }
        }
    }
    
    return node;
}

void GenerateBestBatchs(boolean* blockUsed) {
    ClearQueue(); 

    BatchResult bestOverallResult = {0};
    bestOverallResult.totalScore = INT_MIN;
    bestOverallResult.valid = false;

    int initialGridState[GRID_SIZE][GRID_SIZE];
    CloneGrid(grid, initialGridState);

    for (int b1 = 1; b1 <= 36; b1++) {
        for (int b2 = 1; b2 <= 18; b2++) {
            for (int b3 = 1; b3 <= 9; b3++) {
                BatchResult currentSequenceResult = EvaluateBlockSequence(initialGridState, b1, b2, b3);

                if (currentSequenceResult.valid && currentSequenceResult.totalScore > bestOverallResult.totalScore) {
                    bestOverallResult = currentSequenceResult;
                }
            }
        }
    }

    if (bestOverallResult.valid) {
        for (int i = 0; i < 3; i++) {
            Enqueue(bestOverallResult.blocks[i]);
            if (blockUsed) blockUsed[i] = false; 
        }
    } else {
        printf("Info: Tidak ada kombinasi valid, menggunakan blok random.\n");
        for (int j = 0; j < 3; j++) {
            Enqueue(GetRandomValue(1, 36));
            if (blockUsed) blockUsed[j] = false;
        }
    }
    
    boolean needReset = false;
    for (int i = 0; i < 3; i++) {
        int queueBlock = GetQueueAt(i);
        if (!HasValidPlacement(queueBlock)){

            RemoveBlockFromQueue(queueBlock);
            Enqueue(GetRandomValue(1,36));
        }

        if (queueBlock < 1 || queueBlock > 36) {
            needReset = true;
            break;
        }
    }

    if (needReset) {
        ClearQueue();
        printf("Error: Blok tidak valid di queue. Reset dengan blok default.\n");
        for (int j = 0; j < 3; j++) {
            Enqueue((j % 36) + 1);
            if (blockUsed) blockUsed[j] = false;
        }
    }
}

void CloneGrid(int source[GRID_SIZE][GRID_SIZE], int dest[GRID_SIZE][GRID_SIZE]) {
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            dest[y][x] = source[y][x];
        }
    }
}

int CalculateBlockScore(int originalGrid[GRID_SIZE][GRID_SIZE], int centerX, int centerY, int blockType) {
    int tempGrid[GRID_SIZE][GRID_SIZE];
    CloneGrid(originalGrid, tempGrid);

    for (int i = 0; i < MAX_BLOCK_SIZE; i++) {
        int bx = centerX + (int)blockShapes[blockType][i].x;
        int by = centerY + (int)blockShapes[blockType][i].y;
        if (bx >= 0 && bx < GRID_SIZE && by >= 0 && by < GRID_SIZE) {
            tempGrid[by][bx] = blockType; 
        }
    }

    int score = 0;
    int linesClearedThisTurn = 0;
    int filledCellsByBlock = 0;

    for (int i = 0; i < MAX_BLOCK_SIZE; i++) {
        int bx = centerX + (int)blockShapes[blockType][i].x;
        int by = centerY + (int)blockShapes[blockType][i].y;
        if (bx >= 0 && bx < GRID_SIZE && by >= 0 && by < GRID_SIZE) {
            if (originalGrid[by][bx] == 0) {
                filledCellsByBlock++;
            }
        }
    }

    score += filledCellsByBlock * SCORE_PER_FILLED_CELL;

    for (int y = 0; y < GRID_SIZE; y++) {
        bool fullRow = true;
        for (int x = 0; x < GRID_SIZE; x++) {
            if (tempGrid[y][x] == 0) {
                fullRow = false;
                break;
            }
        }
        if (fullRow) {
            linesClearedThisTurn++;
            for (int x = 0; x < GRID_SIZE; x++) tempGrid[y][x] = 0; 
        }
    }

    for (int x = 0; x < GRID_SIZE; x++) {
        bool fullCol = true;
        for (int y = 0; y < GRID_SIZE; y++) {
            if (tempGrid[y][x] == 0) {
                fullCol = false;
                break;
            }
        }
        if (fullCol) {
            linesClearedThisTurn++;
            for (int y = 0; y < GRID_SIZE; y++) tempGrid[y][x] = 0; 
        }
    }
    
    score += linesClearedThisTurn * SCORE_PER_LINE_CLEARED;
    if (linesClearedThisTurn > 1) {
        score += linesClearedThisTurn * BONUS_FOR_MULTIPLE_LINES_FACTOR;
    }

    int holes = 0;
    int aggregateHeight = 0;
    int bumpiness = 0;
    int columnHeights[GRID_SIZE] = {0};
    int currentMaxHeight = 0;

    for (int x = 0; x < GRID_SIZE; x++) {
        int height = 0;
        for (int y = 0; y < GRID_SIZE; y++) { 
            if (tempGrid[y][x] != 0) {
                height = GRID_SIZE - y; 
                break; 
            }
        }
        columnHeights[x] = height;
        aggregateHeight += height;
        if (height > currentMaxHeight) {
            currentMaxHeight = height;
        }
    }

    for (int x = 0; x < GRID_SIZE - 1; x++) {
        bumpiness += abs(columnHeights[x] - columnHeights[x+1]);
    }

    for (int x = 0; x < GRID_SIZE; x++) {
        bool blockFoundAbove = false;
        for (int y = 0; y < GRID_SIZE; y++) { 
            if (tempGrid[y][x] != 0) {
                blockFoundAbove = true;
            } else {
                if (blockFoundAbove) { 
                    holes++;
                }
            }
        }
    }

    score -= holes * PENALTY_PER_HOLE;
    score -= aggregateHeight * PENALTY_PER_AGGREGATE_HEIGHT_POINT;
    score -= bumpiness * PENALTY_PER_BUMPINESS_POINT;
    score -= currentMaxHeight * PENALTY_PER_MAX_HEIGHT_POINT;

    return score;
}

boolean CanPlaceBlockOnGrid(int testGrid[GRID_SIZE][GRID_SIZE], int centerX, int centerY, int blockType) {
    if (blockType < 1 || blockType > 36) return false;

    for (int i = 0; i < MAX_BLOCK_SIZE; i++) {
        int bx = centerX + (int)blockShapes[blockType][i].x;
        int by = centerY + (int)blockShapes[blockType][i].y;
        if (bx < 0 || bx >= GRID_SIZE || by < 0 || by >= GRID_SIZE || testGrid[by][bx] != 0) {
            return false;
        }
    }
    return true;
}

void PlaceBlockOnCloneGrid(int cloneGrid[GRID_SIZE][GRID_SIZE], int centerX, int centerY, int blockType) {
    for (int i = 0; i < MAX_BLOCK_SIZE; i++) {
        int bx = centerX + (int)blockShapes[blockType][i].x;
        int by = centerY + (int)blockShapes[blockType][i].y;
        if (bx >= 0 && bx < GRID_SIZE && by >= 0 && by < GRID_SIZE) {
            cloneGrid[by][bx] = blockType; 
        }
    }

    for (int y = 0; y < GRID_SIZE; y++) {
        bool full = true;
        for (int x = 0; x < GRID_SIZE; x++) {
            if (cloneGrid[y][x] == 0) {
                full = false;
                break;
            }
        }
        if (full) {
            for (int r = y; r > 0; r--) { 
                for (int c = 0; c < GRID_SIZE; c++) {
                    cloneGrid[r][c] = cloneGrid[r-1][c];
                }
            }
            for (int c = 0; c < GRID_SIZE; c++) { 
                cloneGrid[0][c] = 0;
            }

            for (int x = 0; x < GRID_SIZE; x++) {
                 cloneGrid[y][x] = 0;
            }
        }
    }

    for (int x = 0; x < GRID_SIZE; x++) {
        bool full = true;
        for (int y = 0; y < GRID_SIZE; y++) {
            if (cloneGrid[y][x] == 0) {
                full = false;
                break;
            }
        }
        if (full) {
            for (int y = 0; y < GRID_SIZE; y++) {
                cloneGrid[y][x] = 0;
            }
        }
    }
}

TreeNode* FindBestPositionForBlock_ForAllTypes(int currentGrid[GRID_SIZE][GRID_SIZE]) {
    TreeNode* bestNodeOverall = NULL;
    int bestOverallScore = INT_MIN;

    for (int blockType = 1; blockType <= 36; blockType++) {
        TreeNode* currentNode = FindBestPositionForBlock(currentGrid, blockType);
        if (currentNode != NULL) {
            if (currentNode->canPlace && currentNode->score > bestOverallScore) {
                if (bestNodeOverall != NULL) {
                    free(bestNodeOverall);
                }
                bestNodeOverall = currentNode;
                bestOverallScore = currentNode->score;
            } else {
                free(currentNode);
            }
        }
    }
    return bestNodeOverall;
}

void GenerateBestBatch(boolean* used) {
    ClearQueue();

    int best[3] = {-1, -1, -1};
    int x1 = -1, y1 = -1;
    int x2 = -1, y2 = -1;

    int grid0[GRID_SIZE][GRID_SIZE];
    CloneGrid(grid, grid0);

    TreeNode* root0 = BuildDecisionTree(grid0, 0, 2);

    if (root0 != NULL && root0->childCount > 0) {
        TreeNode* best1 = NULL;
        int max1 = INT_MIN;

        for (int i = 0; i < root0->childCount; i++) {
            if (root0->children[i] && root0->children[i]->canPlace) {
                if (root0->children[i]->score > max1) {
                    max1 = root0->children[i]->score;
                    best1 = root0->children[i];
                }
            }
        }

        if (best1) {
            best[0] = best1->blockType;
            x1 = best1->x;
            y1 = best1->y;

            if (best1->childCount > 0 && best1->children[0] && best1->children[0]->childCount > 0) {
                TreeNode* root1 = best1->children[0];
                TreeNode* best2 = NULL;
                int max2 = INT_MIN;

                for (int i = 0; i < root1->childCount; i++) {
                    if (root1->children[i] && root1->children[i]->canPlace) {
                        if (root1->children[i]->score > max2) {
                            max2 = root1->children[i]->score;
                            best2 = root1->children[i];
                        }
                    }
                }

                if (best2) {
                    best[1] = best2->blockType;
                    x2 = best2->x;
                    y2 = best2->y;
                }
            }
        }
    }

    FreeTree(root0);

    int simGrid[GRID_SIZE][GRID_SIZE];
    CloneGrid(grid, simGrid);

    if (best[0] != -1) {
        PlaceBlockOnCloneGrid(simGrid, x1, y1, best[0]);
    } else {
        printf("Info: b1 tidak ditemukan dari pohon, mencari secara greedy.\n");
        TreeNode* greedy1 = FindBestPositionForBlock_ForAllTypes(simGrid);
        if (greedy1 && greedy1->canPlace) {
            best[0] = greedy1->blockType;
            PlaceBlockOnCloneGrid(simGrid, greedy1->x, greedy1->y, best[0]);
            free(greedy1);
        } else {
            if (greedy1) free(greedy1);
            printf("Peringatan: Tidak ada blok pertama yang bisa ditempatkan!\n");
        }
    }

    if (best[0] != -1) {
        if (best[1] != -1) {
            PlaceBlockOnCloneGrid(simGrid, x2, y2, best[1]);
        } else {
            printf("Info: b2 tidak ditemukan dari pohon, mencari b2 secara greedy.\n");
            TreeNode* greedy2 = FindBestPositionForBlock_ForAllTypes(simGrid);
            if (greedy2 && greedy2->canPlace) {
                best[1] = greedy2->blockType;
                PlaceBlockOnCloneGrid(simGrid, greedy2->x, greedy2->y, best[1]);
                free(greedy2);
            } else {
                if (greedy2) free(greedy2);
                printf("Peringatan: Tidak ada blok kedua yang bisa ditempatkan setelah b1!\n");
            }
        }
    }

    if (best[0] != -1 && best[1] != -1) {
        printf("Info: Mencari b3 secara greedy.\n");
        TreeNode* greedy3 = FindBestPositionForBlock_ForAllTypes(simGrid);
        if (greedy3 && greedy3->canPlace) {
            best[2] = greedy3->blockType;
            free(greedy3);
        } else {
            if (greedy3) free(greedy3);
            printf("Peringatan: Tidak ada blok ketiga yang bisa ditempatkan setelah b1 & b2!\n");
        }
    }

    for (int i = 0; i < 3; i++) {
        if (best[i] != -1) {
            Enqueue(best[i]);
        } else {
            printf("Info: Mengisi blok %d dengan random.\n", i + 1);
            Enqueue(GetRandomValue(1, 36));
        }
        if (used) used[i] = false;
    }
    
    boolean needReset = false;
    for (int i = 0; i < 3; i++) {
        int queueBlock = GetQueueAt(i);
        if (queueBlock < 1 || queueBlock > 36) {
            needReset = true;
            break;
        }
    }

    if (needReset) {
        ClearQueue();
        printf("Error: Blok tidak valid di queue. Reset dengan blok default.\n");
        for (int j = 0; j < 3; j++) {
            Enqueue((j % 36) + 1);
            if (used) used[j] = false;
        }
    }
}

TreeNode* BuildDecisionTree(int cloneGrid[GRID_SIZE][GRID_SIZE], int depth, int maxDepth) {
    if (depth >= maxDepth) return NULL;
    
    TreeNode* rootNode = (TreeNode*)malloc(sizeof(TreeNode));
    rootNode->childCount = 0;
    rootNode->score = INT_MIN;
    
    for (int i = 0; i < 36; i++) {
        rootNode->children[i] = NULL;
    }
    
    for (int blockType = 1; blockType <= 36; blockType++) {
        TreeNode* childNode = FindBestPositionForBlock(cloneGrid, blockType);
        
        if (childNode->canPlace) {
            int childGrid[GRID_SIZE][GRID_SIZE];
            CloneGrid(cloneGrid, childGrid);
            PlaceBlockOnCloneGrid(childGrid, childNode->x, childNode->y, blockType);
            
            TreeNode* subTree = BuildDecisionTree(childGrid, depth + 1, maxDepth);
            if (subTree != NULL) {
                childNode->children[0] = subTree;
                childNode->childCount = 1;
                childNode->score += subTree->score;
            } 
            
            rootNode->children[rootNode->childCount] = childNode;
            rootNode->childCount++;
        } else {
            free(childNode);
        }
    }
    
    return rootNode;
}

void FreeTree(TreeNode* node) {
    if (node == NULL) return;
    
    for (int i = 0; i < node->childCount; i++) {
        FreeTree(node->children[i]);
    }
    
    free(node);
}