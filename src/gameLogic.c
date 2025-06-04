#include "../lib/gameLogic.h"
#include "../lib/global.h"
#include "../lib/boolean.h"

// Variabel global untuk tracking combo
int comboCount = 0;
int lastClearTurn = -1;  // Turn terakhir ada clear
int currentTurn = 0;     // Turn saat ini

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
        //fprintf(stderr, "GetQueue: Queue is empty!\n");
        return -1; // Or a defined error/empty indicator
    }
    // QueueNode *current = queueFront; // Unnecessary intermediate variable
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
    Sound Scoresound = LoadSound("assets/scorecoint.wav");
    
    int clearedLines = 0;
    bool hasCleared = false;
    
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
            for (int x = 0; x < GRID_SIZE; x++) grid[y][x] = 0;
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
            for (int y = 0; y < GRID_SIZE; y++) grid[y][x] = 0;
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
        PlaySound(Scoresound);
        
        // Optional: Print info untuk debugging
        printf("Lines cleared: %d, Base score: %d, Combo: %d, Final score: %d\n", 
               clearedLines, baseScore, comboCount, finalScore);
    }
    
    // Increment turn counter (panggil ini setiap kali piece ditempatkan)
    currentTurn++;
}

// Fungsi helper untuk reset combo saat game over atau restart
void ResetCombo() {
    comboCount = 0;
    lastClearTurn = -1;
    currentTurn = 0;
}

// Fungsi untuk mendapatkan combo count saat ini (untuk UI)
int GetCurrentCombo() {
    return comboCount;
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
