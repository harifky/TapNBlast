#include "../lib/gameLogic.h"
#include "../lib/global.h"
#include "../lib/tree.h"
#include "../lib/animation.h"  

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

int Dequeue(){
    if (queueFront == NULL) return -1;
    int value = queueFront->blockType;
    QueueNode *temp = queueFront;
    queueFront = queueFront->next;
    if (queueFront == NULL) queueRear = NULL;
    free(temp);
    return value;
}

int GetQueue(){
    if (queueFront == NULL) {

        return -1; 
    }

    return queueFront->blockType;
}

int GetQueueAt(int index) {
    QueueNode *current = queueFront;
    int count = 0;
    while (current != NULL) {
        if (count == index) return current->blockType;
        current = current->next;
        count++;
    }
    return -1;
}

void PlaceBlock(int centerX, int centerY, int blockType){
    for (int i = 0; i < MAX_BLOCK_SIZE; i++) {
        int bx = centerX + (int)blockShapes[blockType][i].x;
        int by = centerY + (int)blockShapes[blockType][i].y;
        if (bx >= 0 && bx < GRID_SIZE && by >= 0 && by < GRID_SIZE) {
            grid[by][bx] = blockType;
        }
    }
}

boolean HasValidPlacement(int blockType){
    if (blockType < 1 || blockType > 36) return false;

    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            if (CanPlaceBlock(x, y, blockType)) return true;
        }
    }
    return false;
}

boolean CanPlaceBlock(int centerX, int centerY, int blockType){
    if (blockType < 1 || blockType > 36) return false;

    for (int i = 0; i < MAX_BLOCK_SIZE; i++) {
        int bx = centerX + (int)blockShapes[blockType][i].x;
        int by = centerY + (int)blockShapes[blockType][i].y;
        if (bx < 0 || bx >= GRID_SIZE || by < 0 || by >= GRID_SIZE || grid[by][bx] != 0) {
            return false;
        }
    }
    return true;
}

void ClearFullLines(){
    
    int clearedLines = 0;
    boolean hasCleared = false;
    
    // Check dan clear baris penuh
    for (int y = 0; y < GRID_SIZE; y++) {
        boolean full = true;
        for (int x = 0; x < GRID_SIZE; x++) {
            if (grid[y][x] == 0) {
                full = false;
                break;
            }
        }
        if (full) {
            // Gunakan animasi clear 
            for (int x = 0; x < GRID_SIZE; x++) {
                EnqueueClearAnimation(x, y);
            }
            clearedLines++;
            hasCleared = true;
        }
    }
    
    // Check dan clear kolom penuh
    for (int x = 0; x < GRID_SIZE; x++) {
        boolean full = true;
        for (int y = 0; y < GRID_SIZE; y++) {
            if (grid[y][x] == 0) {
                full = false;
                break;
            }
        }
        if (full) {
            // Gunakan animasi clear 
            for (int y = 0; y < GRID_SIZE; y++) {
                EnqueueClearAnimation(x, y);
            }
            clearedLines++;
            hasCleared = true;
        }
    }
    
    if (hasCleared) {
        // Hitung base score berdasarkan jumlah baris/kolom yang di-clear
        int baseScore = 0;
        switch(clearedLines) {
            case 1: baseScore = 100; break;
            case 2: baseScore = 250; break;
            case 3: baseScore = 400; break;
            default: 
                // Untuk 4 atau lebih lines (lebih dari 3 sekaligus)
                baseScore = 600;
                break;
        }
        
        // Check apakah ini combo (clear berturut-turut)
        if (currentTurn == lastClearTurn + 1) {
            comboCount++;
        } else {
            comboCount = 0;  // Reset combo jika tidak berturut-turut
        }
        
        // Hitung combo bonus
        float comboMultiplier = 1.0f + (comboCount * 0.5f);  // +50% per combo
        int finalScore = (int)(baseScore * comboMultiplier);
        
        // Tambahkan ke total score
        score += finalScore;
        
        // Update tracking variables
        lastClearTurn = currentTurn;
        
        // Play sound
        PlayScoreSound();
        PlayExplosionSound();
        
        // Optional: Print info untuk debugging
        printf("Lines cleared: %d, Base score: %d, Combo: %d, Final score: %d\n", 
               clearedLines, baseScore, comboCount, finalScore);
    }
    
    // Increment turn counter (panggil ini setiap kali piece ditempatkan)
    currentTurn++;
}

void RemoveBlockFromQueue(int index) {
    if (index < 0 || queueFront == NULL) return;
    
    if (index == 0) {
        // Hapus elemen pertama
        Dequeue();
        return;
    }
    
    // Cari node sebelum target
    QueueNode* current = queueFront;
    for (int i = 0; i < index - 1 && current != NULL; i++) {
        current = current->next;
    }
    
    if (current == NULL || current->next == NULL) return;
    
    // Hapus node target
    QueueNode* nodeToDelete = current->next;
    current->next = nodeToDelete->next;
    
    if (nodeToDelete == queueRear) {
        queueRear = current;
    }
    
    free(nodeToDelete);
}

int GetQueueSize() {
    int size = 0;
    QueueNode* current = queueFront;
    while (current != NULL) {
        size++;
        current = current->next;
    }
    return size;
}

void ClearQueue() {
    while (Dequeue() != -1);
}

int GetValidRandomBlockType() {

    return GetRandomValue(1, 36);
}

void GenerateRandomBatch(boolean* blockUsed) {
    ClearQueue();
    
    for (int i = 0; i < 3; i++) {
        int blockType;
        int attempts = 0;
        
        // Loop hingga mendapat blok yang valid
        do {
            blockType = GetValidRandomBlockType();
            attempts++;
            
            // Jika sudah 50 percobaan, gunakan blok sequential untuk memastikan validitas
            if (attempts >= 50) {
                blockType = (i % 36) + 1; // Gunakan blok 1-40 secara berurutan
                break;
            }
        } while (blockType < 1 || blockType > 36);
        
        // Double check - pastikan blockType valid
        if (blockType < 1 || blockType > 36) {
            blockType = (i % 40) + 1; // Fallback ke blok sequential
        }
        
        Enqueue(blockType);
        blockUsed[i] = false;
    }
    
    // Verifikasi bahwa queue berisi 3 blok yang valid
    for (int i = 0; i < 3; i++) {
        int queueBlock = GetQueueAt(i);
        if (queueBlock == -1 || queueBlock < 1 || queueBlock > 36) {
            // Jika ada blok yang tidak valid, ganti dengan blok default
            // Asumsi ada fungsi untuk mengatur elemen queue secara langsung
            // Atau bisa menggunakan ClearQueue dan rebuild seluruhnya
            ClearQueue();
            for (int j = 0; j < 3; j++) {
                Enqueue((j % 36) + 1); // Gunakan blok 1, 2, 3 sebagai fallback
                blockUsed[j] = false;
            }
            break;
        }
    }
}

void InitializeRandomGrid() {
    // Kosongkan grid terlebih dahulu
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            grid[y][x] = 0;
        }
    }
    
    // Tentukan jumlah block random (15-25% dari total grid)
    int totalCells = GRID_SIZE * GRID_SIZE;
    int randomBlockCount = GetRandomValue(totalCells * 20 / 50, totalCells * 30 / 50);
    
    // Tempatkan block random
    for (int i = 0; i < randomBlockCount; i++) {
        int x = GetRandomValue(0, GRID_SIZE - 1);
        int y = GetRandomValue(0, GRID_SIZE - 1);
        
        // Jika posisi sudah terisi, cari posisi kosong
        if (grid[y][x] != 0) {
            bool found = false;
            for (int attempts = 0; attempts < 50 && !found; attempts++) {
                x = GetRandomValue(0, GRID_SIZE - 1);
                y = GetRandomValue(0, GRID_SIZE - 1);
                if (grid[y][x] == 0) {
                    found = true;
                }
            }
            if (!found) continue; // Skip jika tidak bisa menemukan posisi kosong
        }
        
        // Tempatkan block random
        grid[y][x] = GetRandomValue(1, 6);
    }
}

void GenerateNewBatch(boolean* blockUsed) {
    // Gunakan best block selector sebagai default
    
    // Generate best block
    
    GenerateBestBatch(blockUsed); //tree
    
    
    // Verifikasi hasil - jika gagal, fallback ke random
    boolean validBatch = true;
    for (int i = 0; i < 3; i++) {
        int queueBlock = GetQueueAt(i);
        if (queueBlock == -1 || queueBlock < 1 || queueBlock > 36) {
            validBatch = false;
            break;
        }
    }
    
    // Jika best block selector gagal, gunakan random generation
    if (!validBatch) {
        GenerateRandomBatch(blockUsed);
    }
}

// Fungsi tambahan untuk memvalidasi apakah masih ada gerakan yang valid
boolean HasAnyValidMove(boolean* blockUsed) {
    for (int i = 0; i < 3; i++) {
        int blockType = GetQueueAt(i);
        
        // Cek apakah slot ini berisi blok valid dan belum digunakan
        if (blockType >= 1 && blockType <= 36 && !blockUsed[i]) {
            // Cek apakah blok ini bisa ditempatkan di mana saja di grid
            if (HasValidPlacement(blockType)) {
                return true;
            }
        }
    }
    return false;
}



Move* undoStack = NULL;

void PushMove(Move** stack, int blockType, Vector2 center, int queueIndex, bool isScored) {
    Move* newMove = (Move*)malloc(sizeof(Move));
    newMove->blockType = blockType;
    newMove->center = center;
    newMove->queueIndex = queueIndex;
    newMove->isScored = isScored;
    newMove->next = *stack;
    *stack = newMove;
}

bool PopMove(Move** stack, int* blockType, Vector2* center, int* queueIndex, bool* isScored) {
    if (*stack == NULL) return false;

    Move* temp = *stack;
    *blockType = temp->blockType;
    *center = temp->center;
    *queueIndex = temp->queueIndex;
    *isScored = temp->isScored;

    *stack = temp->next;
    free(temp);
    return true;
}

void ClearStack(Move** stack) {
    while (*stack != NULL) {
        Move* temp = *stack;
        *stack = temp->next;
        free(temp);
    }
}

void RemoveBlockFromGrid(int x, int y, int blockType) {
    for (int i = 0; i < MAX_BLOCK_SIZE; i++) {
        int bx = x + (int)blockShapes[blockType][i].x;
        int by = y + (int)blockShapes[blockType][i].y;
        if (bx >= 0 && bx < GRID_SIZE && by >= 0 && by < GRID_SIZE) {
            grid[by][bx] = 0;
        }
    }
}

int undoCount = 0;

boolean PerformUndo(boolean* blockUsed) {
    if (undoStack == NULL || undoCount >= 3) return false;

    int blockType, queueIndex;
    Vector2 center;
    bool isScored;

    if (PopMove(&undoStack, &blockType, &center, &queueIndex, &isScored)) {
        if (isScored) return false; // Tidak bisa undo jika menghasilkan skor

        RemoveBlockFromGrid((int)center.x, (int)center.y, blockType);
        blockUsed[queueIndex] = false;
        undoCount++;
        return true;
    }
    return false;
}