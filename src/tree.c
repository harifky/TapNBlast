#include "../lib/tree.h"
#include "../lib/global.h"
#include "../lib/gameLogic.h"

// fungsi untuk generate block dan mereturn best block (3 block)
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

//fungsi untuk CloneGrid (papan permainan)
void CloneGrid(int source[GRID_SIZE][GRID_SIZE], int dest[GRID_SIZE][GRID_SIZE]) {
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            dest[y][x] = source[y][x];
        }
    }
}

//fungsi untuk menghitung score dan penaltynya
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

// Fungsi untuk mencari posisi terbaik untuk satu block
TreeNode* FindBestPositionForBlock(int cloneGrid[GRID_SIZE][GRID_SIZE], int blockType) {
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    node->blockType = blockType;
    node->score = INT_MIN;
    node->x = -1;
    node->y = -1;
    node->linesCleared = 0;
    node->canPlace = false;
    node->childCount = 0;
    
    // Inisialisasi children
    for (int i = 0; i < 36; i++) {
        node->children[i] = NULL;
    }
    
    // Coba semua posisi yang mungkin
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

// Fungsi untuk mengecek apakah block bisa ditempatkan di grid tertentu (untuk grid clone)
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

// Fungsi untuk menempatkan block di grid clone dan membersihkan lines
void PlaceBlockOnCloneGrid(int cloneGrid[GRID_SIZE][GRID_SIZE], int centerX, int centerY, int blockType) {
    // Place block
    for (int i = 0; i < MAX_BLOCK_SIZE; i++) {
        int bx = centerX + (int)blockShapes[blockType][i].x;
        int by = centerY + (int)blockShapes[blockType][i].y;
        if (bx >= 0 && bx < GRID_SIZE && by >= 0 && by < GRID_SIZE) {
            cloneGrid[by][bx] = blockType; 
        }
    }

    // Clear full rows
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

    // Clear full columns
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

// fungsi utama untuk 
void GenerateBestBatch(boolean* blockUsed) {
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
        for (int j = 0; j < 3; j++) {
            Enqueue(GetRandomValue(1, 36));
            if (blockUsed) blockUsed[j] = false;
        }
    }

    for (int i = 0; i < 3; i++) {
        int queueBlock = GetQueueAt(i);
        if (queueBlock < 1 || queueBlock > 35) {
            ClearQueue();
            for (int j = 0; j < 3; j++) {
                Enqueue((j % 35) + 1); 
                if (blockUsed) blockUsed[j] = false;
            }
            break;
        }
    }
}

// Fungsi tree
TreeNode* BuildDecisionTree(int cloneGrid[GRID_SIZE][GRID_SIZE], int depth, int maxDepth) {
    if (depth >= maxDepth) return NULL;
    
    TreeNode* rootNode = (TreeNode*)malloc(sizeof(TreeNode));
    rootNode->childCount = 0;
    rootNode->score = INT_MIN;
    
    // Inisialisasi children
    for (int i = 0; i < 36; i++) {
        rootNode->children[i] = NULL;
    }
    
    // Test setiap block type
    for (int blockType = 1; blockType <= 36; blockType++) {
        TreeNode* childNode = FindBestPositionForBlock(cloneGrid, blockType);
        
        if (childNode->canPlace) {
            // Buat clone grid untuk child
            int childGrid[GRID_SIZE][GRID_SIZE];
            CloneGrid(cloneGrid, childGrid);
            PlaceBlockOnCloneGrid(childGrid, childNode->x, childNode->y, blockType);
            
            // Rekursi untuk level berikutnya
            TreeNode* subTree = BuildDecisionTree(childGrid, depth + 1, maxDepth);
            if (subTree != NULL) {
                childNode->children[0] = subTree;
                childNode->childCount = 1;
                childNode->score += subTree->score; // Kombinasi score
            }
            
            rootNode->children[rootNode->childCount] = childNode;
            rootNode->childCount++;
        } else {
            free(childNode);
        }
    }
    
    return rootNode;
}

// Fungsi untuk membersihkan memory tree
void FreeTree(TreeNode* node) {
    if (node == NULL) return;
    
    for (int i = 0; i < node->childCount; i++) {
        FreeTree(node->children[i]);
    }
    
    free(node);
}

// // Fungsi untuk print tree (debugging)
// void PrintTree(TreeNode* node, int depth) {
//     if (node == NULL) return;
    
//     for (int i = 0; i < depth; i++) printf("  ");
//     printf("Block %d: Score %d, Pos(%d,%d), CanPlace: %s\n", 
//            node->blockType, node->score, node->x, node->y, 
//            node->canPlace ? "Yes" : "No");
    
//     for (int i = 0; i < node->childCount; i++) {
//         PrintTree(node->children[i], depth + 1);
//     }
// }