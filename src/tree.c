#include "../lib/tree.h"
#include "../lib/global.h"
#include "../lib/gameLogic.h"

// Kontribusi: Fariz (100)

/**
 * Mengecek apakah grid kosong (semua sel bernilai 0)
 * @param grid: Grid permainan 2D
 * @return true jika grid kosong, false jika ada blok
 */
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

/**
 * Mengevaluasi sequence 3 blok secara berurutan untuk mencari skor terbaik
 * Fungsi ini mensimulasikan penempatan 3 blok secara berurutan tanpa mengubah grid asli
 * @param initialGrid: Grid awal sebelum penempatan blok
 * @param block1, block2, block3: Tipe blok yang akan dievaluasi
 * @return BatchResult berisi skor total dan validitas sequence
 */
BatchResult EvaluateBlockSequence(int initialGrid[GRID_SIZE][GRID_SIZE], int block1, int block2, int block3) {
    BatchResult result = {0};
    result.blocks[0] = block1;
    result.blocks[1] = block2;
    result.blocks[2] = block3;
    result.totalScore = 0; 
    result.valid = true;

    // Buat salinan grid untuk simulasi
    int tempGrid[GRID_SIZE][GRID_SIZE];
    CloneGrid(initialGrid, tempGrid); 

    int currentBlocks[3] = {block1, block2, block3};

    // Simulasi penempatan 3 blok secara berurutan
    for (int i = 0; i < 3; i++) {
        int currentBlock = currentBlocks[i];
       
        // Cari posisi terbaik untuk blok saat ini
        TreeNode* bestPosNode = FindBestPositionForBlock(tempGrid, currentBlock);

        // Jika tidak ada posisi valid, batalkan sequence
        if (!bestPosNode || !bestPosNode->canPlace) {
            result.valid = false;
            result.totalScore = INT_MIN; 
            if(bestPosNode) free(bestPosNode);
            break;
        }

        // Akumulasi skor
        result.totalScore += bestPosNode->score;

        // Tempatkan blok di grid simulasi
        PlaceBlockOnCloneGrid(tempGrid, bestPosNode->x, bestPosNode->y, currentBlock);

        free(bestPosNode);
    }
    return result;
}

/**
 * Mencari posisi terbaik untuk menempatkan satu blok di grid
 * Menggunakan algoritma greedy - mencoba semua posisi valid dan pilih yang terbaik
 * @param cloneGrid: Salinan grid untuk simulasi
 * @param blockType: Tipe blok yang akan ditempatkan
 * @return TreeNode berisi posisi terbaik dan skornya
 */
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
    
    // Coba semua posisi di grid
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            if (CanPlaceBlockOnGrid(cloneGrid, x, y, blockType)) {
                int score = CalculateBlockScore(cloneGrid, x, y, blockType);
                
                // Update jika skor lebih baik
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

/**
 * Generate 3 blok terbaik menggunakan brute force
 * Mencoba semua kombinasi 3 blok (36x18x9) untuk menemukan sequence terbaik
 * Menggunakan nested loop dengan kompleksitas O(n³)
 */
void GenerateBestBatchs(boolean* blockUsed) {
    ClearQueue(); 

    BatchResult bestOverallResult = {0};
    bestOverallResult.totalScore = INT_MIN;
    bestOverallResult.valid = false;

    int initialGridState[GRID_SIZE][GRID_SIZE];
    CloneGrid(grid, initialGridState);

    // Brute force: coba semua kombinasi 3 blok
    // Kompleksitas: 36 * 18 * 9 = 5,832 kombinasi
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

    // Masukkan hasil terbaik ke queue
    if (bestOverallResult.valid) {
        for (int i = 0; i < 3; i++) {
            Enqueue(bestOverallResult.blocks[i]);
            if (blockUsed) blockUsed[i] = false; 
        }
    } else {
        // Fallback: gunakan blok random jika tidak ada kombinasi valid
        printf("Info: Tidak ada kombinasi valid, menggunakan blok random.\n");
        for (int j = 0; j < 3; j++) {
            Enqueue(GetRandomValue(1, 36));
            if (blockUsed) blockUsed[j] = false;
        }
    }
    
    // Validasi dan cleanup queue
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

    // Reset jika ada blok invalid
    if (needReset) {
        ClearQueue();
        printf("Error: Blok tidak valid di queue. Reset dengan blok default.\n");
        for (int j = 0; j < 3; j++) {
            Enqueue((j % 36) + 1);
            if (blockUsed) blockUsed[j] = false;
        }
    }
}

/**
 * Menyalin grid dari source ke destination
 * Utility function untuk membuat salinan grid tanpa mengubah yang asli
 */
void CloneGrid(int source[GRID_SIZE][GRID_SIZE], int dest[GRID_SIZE][GRID_SIZE]) {
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            dest[y][x] = source[y][x];
        }
    }
}

/**
 * Menghitung skor untuk penempatan blok pada posisi tertentu
 * Menggunakan heuristik AI Tetris: 
 * - Positif: cells filled, lines cleared, bonus multiple lines
 * - Negatif: holes, aggregate height, bumpiness, max height
 * @param originalGrid: Grid sebelum penempatan
 * @param centerX, centerY: Posisi center blok
 * @param blockType: Tipe blok
 * @return Skor kalkulasi (positif = baik, negatif = buruk)
 */
int CalculateBlockScore(int originalGrid[GRID_SIZE][GRID_SIZE], int centerX, int centerY, int blockType) {
    int tempGrid[GRID_SIZE][GRID_SIZE];
    CloneGrid(originalGrid, tempGrid);

    // Tempatkan blok di grid simulasi
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

    // Hitung sel yang diisi oleh blok ini
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

    // Hitung baris yang bisa dihapus
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

    // Hitung kolom yang bisa dihapus
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
    
    // Bonus untuk clearing lines
    score += linesClearedThisTurn * SCORE_PER_LINE_CLEARED;
    if (linesClearedThisTurn > 1) {
        score += linesClearedThisTurn * BONUS_FOR_MULTIPLE_LINES_FACTOR;
    }

    // Hitung penalty berdasarkan struktur grid (AI Tetris heuristics)
    int holes = 0;
    int aggregateHeight = 0;
    int bumpiness = 0;
    int columnHeights[GRID_SIZE] = {0};
    int currentMaxHeight = 0;

    // Hitung tinggi setiap kolom
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

    // Hitung bumpiness (perbedaan tinggi antar kolom)
    for (int x = 0; x < GRID_SIZE - 1; x++) {
        bumpiness += abs(columnHeights[x] - columnHeights[x+1]);
    }

    // Hitung holes (ruang kosong yang tertutup blok di atasnya)
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

    // Terapkan penalty (semakin banyak = semakin buruk)
    score -= holes * PENALTY_PER_HOLE;
    score -= aggregateHeight * PENALTY_PER_AGGREGATE_HEIGHT_POINT;
    score -= bumpiness * PENALTY_PER_BUMPINESS_POINT;
    score -= currentMaxHeight * PENALTY_PER_MAX_HEIGHT_POINT;

    return score;
}

/**
 * Mengecek apakah blok dapat ditempatkan pada posisi tertentu
 * @param testGrid: Grid untuk testing
 * @param centerX, centerY: Posisi center blok
 * @param blockType: Tipe blok
 * @return true jika bisa ditempatkan, false jika collision
 */
boolean CanPlaceBlockOnGrid(int testGrid[GRID_SIZE][GRID_SIZE], int centerX, int centerY, int blockType) {
    if (blockType < 1 || blockType > 36) return false;

    // Cek setiap bagian blok
    for (int i = 0; i < MAX_BLOCK_SIZE; i++) {
        int bx = centerX + (int)blockShapes[blockType][i].x;
        int by = centerY + (int)blockShapes[blockType][i].y;
        // Cek bounds dan collision
        if (bx < 0 || bx >= GRID_SIZE || by < 0 || by >= GRID_SIZE || testGrid[by][bx] != 0) {
            return false;
        }
    }
    return true;
}

/**
 * Menempatkan blok di grid dan melakukan line clearing
 * Fungsi ini memodifikasi grid dan melakukan line clearing otomatis
 * @param cloneGrid: Grid yang akan dimodifikasi
 * @param centerX, centerY: Posisi penempatan
 * @param blockType: Tipe blok
 */
void PlaceBlockOnCloneGrid(int cloneGrid[GRID_SIZE][GRID_SIZE], int centerX, int centerY, int blockType) {
    // Tempatkan blok
    for (int i = 0; i < MAX_BLOCK_SIZE; i++) {
        int bx = centerX + (int)blockShapes[blockType][i].x;
        int by = centerY + (int)blockShapes[blockType][i].y;
        if (bx >= 0 && bx < GRID_SIZE && by >= 0 && by < GRID_SIZE) {
            cloneGrid[by][bx] = blockType; 
        }
    }

    // Clear baris penuh
    for (int y = 0; y < GRID_SIZE; y++) {
        boolean full = true;
        for (int x = 0; x < GRID_SIZE; x++) {
            if (cloneGrid[y][x] == 0) {
                full = false;
                break;
            }
        }
        if (full) {
            // Clear baris horizontal
            for (int x = 0; x < GRID_SIZE; x++) {
                 cloneGrid[y][x] = 0;
            }
        }
    }

    // Clear kolom penuh
    for (int x = 0; x < GRID_SIZE; x++) {
        boolean full = true;
        for (int y = 0; y < GRID_SIZE; y++) {
            if (cloneGrid[y][x] == 0) {
                full = false;
                break;
            }
        }
        if (full) {
            // clear baris vertikal
            for (int y = 0; y < GRID_SIZE; y++) {
                cloneGrid[y][x] = 0;
            }
        }
    }
} 

/**
 * Mencari posisi terbaik untuk semua tipe blok (1-36)
 * Menggunakan brute force untuk mencari blok dan posisi terbaik secara keseluruhan
 * @param currentGrid: Grid saat ini
 * @return TreeNode dengan blok dan posisi terbaik
 */
TreeNode* FindBestPositionForBlock_ForAllTypes(int currentGrid[GRID_SIZE][GRID_SIZE]) {
    TreeNode* bestNodeOverall = NULL;
    int bestOverallScore = INT_MIN;

    // Coba semua tipe blok
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

/**
 * Generate 3 blok terbaik menggunakan decision tree (lebih efisien dari brute force)
 * Menggunakan tree search dengan depth 2 untuk optimasi yang lebih cerdas
 * Fallback ke greedy search jika tree tidak lengkap
 */
void GenerateBestBatch(boolean* used) {
    ClearQueue();

    int best[3] = {-1, -1, -1};
    int x1 = -1, y1 = -1;
    int x2 = -1, y2 = -1;

    int grid0[GRID_SIZE][GRID_SIZE];
    CloneGrid(grid, grid0);

    // Build decision tree depth 2
    TreeNode* root0 = BuildDecisionTree(grid0, 0, 2);

    // Ekstrak blok pertama dan kedua dari tree
    if (root0 != NULL && root0->childCount > 0) {
        TreeNode* best1 = NULL;
        int max1 = INT_MIN;

        // Cari child terbaik untuk blok pertama
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

            // Cari blok kedua dari subtree
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

    // Simulasi penempatan dan cari blok yang tersisa secara greedy
    int simGrid[GRID_SIZE][GRID_SIZE];
    CloneGrid(grid, simGrid);

    // Tempatkan blok pertama
    if (best[0] != -1) {
        PlaceBlockOnCloneGrid(simGrid, x1, y1, best[0]);
    } else {
        // Fallback: greedy search untuk blok pertama
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

    // Tempatkan blok kedua
    if (best[0] != -1) {
        if (best[1] != -1) {
            PlaceBlockOnCloneGrid(simGrid, x2, y2, best[1]);
        } else {
            // Fallback: greedy search untuk blok kedua
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

    // Cari blok ketiga secara greedy
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

    // Masukkan hasil ke queue
    for (int i = 0; i < 3; i++) {
        if (best[i] != -1) {
            Enqueue(best[i]);
        } else {
            printf("Info: Mengisi blok %d dengan random.\n", i + 1);
            Enqueue(GetRandomValue(1, 36));
        }
        if (used) used[i] = false;
    }
    
    // Validasi queue
    boolean needReset = false;
    for (int i = 0; i < 3; i++) {
        int queueBlock = GetQueueAt(i);
        if (queueBlock < 1 || queueBlock > 36) {
            needReset = true;
            break;
        }
    }

    // Reset jika ada masalah
    if (needReset) {
        ClearQueue();
        printf("Error: Blok tidak valid di queue. Reset dengan blok default.\n");
        for (int j = 0; j < 3; j++) {
            Enqueue((j % 36) + 1);
            if (used) used[j] = false;
        }
    }
}

/**
 * Membangun decision tree untuk optimasi lookahead
 * Menggunakan recursive tree building dengan depth limit
 * @param cloneGrid: Grid saat ini
 * @param depth: Kedalaman saat ini
 * @param maxDepth: Batas kedalaman maksimum
 * @return Root node dari subtree
 */
TreeNode* BuildDecisionTree(int cloneGrid[GRID_SIZE][GRID_SIZE], int depth, int maxDepth) {
    if (depth >= maxDepth) return NULL;
    
    TreeNode* rootNode = (TreeNode*)malloc(sizeof(TreeNode));
    rootNode->childCount = 0;
    rootNode->score = INT_MIN;
    
    // Inisialisasi children
    for (int i = 0; i < 36; i++) {
        rootNode->children[i] = NULL;
    }
    
    // Coba semua tipe blok untuk level ini
    for (int blockType = 1; blockType <= 36; blockType++) {
        TreeNode* childNode = FindBestPositionForBlock(cloneGrid, blockType);
        
        if (childNode->canPlace) {
            // Buat grid setelah penempatan blok ini
            int childGrid[GRID_SIZE][GRID_SIZE];
            CloneGrid(cloneGrid, childGrid);
            PlaceBlockOnCloneGrid(childGrid, childNode->x, childNode->y, blockType);
            
            // Recursive build untuk level berikutnya
            TreeNode* subTree = BuildDecisionTree(childGrid, depth + 1, maxDepth);
            if (subTree != NULL) {
                childNode->children[0] = subTree;
                childNode->childCount = 1;
                childNode->score += subTree->score; // Akumulasi skor dari subtree
            } 
            
            rootNode->children[rootNode->childCount] = childNode;
            rootNode->childCount++;
        } else {
            free(childNode);
        }
    }
    
    return rootNode;
}

/**
 * Membebaskan memori yang digunakan oleh decision tree
 * Menggunakan recursive traversal untuk free semua node
 * @param node: Root node yang akan di-free
 */
void FreeTree(TreeNode* node) {
    if (node == NULL) return;
    
    // Recursive free untuk semua children
    for (int i = 0; i < node->childCount; i++) {
        FreeTree(node->children[i]);
    }
    
    // Free node saat ini
    free(node);
}