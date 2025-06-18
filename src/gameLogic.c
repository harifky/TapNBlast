#include "../lib/gameLogic.h"
#include "../lib/global.h"
#include "../lib/tree.h"
#include "../lib/animation.h"

// Kontribusi: Faiz (50), Fariz (40), Rifky (10)

// ============ SISTEM QUEUE UNTUK BLOK ============

/**
 * Menambahkan blok ke dalam queue
 * @param blockType: tipe blok yang akan ditambahkan (1-36)
 */
void Enqueue(int blockType) {
    QueueNode *newNode = (QueueNode *)malloc(sizeof(QueueNode));
    newNode->blockType = blockType;
    newNode->next = NULL;
    
    // Jika queue kosong, node baru menjadi front dan rear
    if (queueRear == NULL) {
        queueFront = queueRear = newNode;
    } else {
        // Tambahkan di belakang (rear)
        queueRear->next = newNode;
        queueRear = newNode;
    }
}

/**
 * Mengambil dan menghapus blok dari depan queue
 * @return: tipe blok yang diambil, atau -1 jika queue kosong
 */
int Dequeue(){
    if (queueFront == NULL) return -1;  // Queue kosong
    
    int value = queueFront->blockType;
    QueueNode *temp = queueFront;
    queueFront = queueFront->next;
    
    // Jika queue menjadi kosong, reset rear
    if (queueFront == NULL) queueRear = NULL;
    
    free(temp);  // Dealokasi memori
    return value;
}

/**
 * Melihat blok di depan queue tanpa menghapusnya
 * @return: tipe blok di depan, atau -1 jika queue kosong
 */
int GetQueue(){
    if (queueFront == NULL) {
        return -1; 
    }
    return queueFront->blockType;
}

/**
 * Mengambil blok pada index tertentu dalam queue
 * @param index: posisi blok dalam queue (0 = depan)
 * @return: tipe blok pada index tersebut, atau -1 jika tidak ada
 */
int GetQueueAt(int index) {
    QueueNode *current = queueFront;
    int count = 0;
    
    // Traverse queue sampai index yang diinginkan
    while (current != NULL) {
        if (count == index) return current->blockType;
        current = current->next;
        count++;
    }
    return -1;  // Index tidak ditemukan
}

// ============ SISTEM PENEMPATAN BLOK ============

/**
 * Menempatkan blok pada grid dengan posisi center tertentu
 * @param centerX: koordinat X center blok
 * @param centerY: koordinat Y center blok
 * @param blockType: tipe blok yang akan ditempatkan
 */
void PlaceBlock(int centerX, int centerY, int blockType){
    // Iterasi melalui semua bagian blok (bentuk blok)
    for (int i = 0; i < MAX_BLOCK_SIZE; i++) {
        // Hitung posisi absolute setiap bagian blok
        int bx = centerX + (int)blockShapes[blockType][i].x;
        int by = centerY + (int)blockShapes[blockType][i].y;
        
        // Pastikan posisi dalam batas grid
        if (bx >= 0 && bx < GRID_SIZE && by >= 0 && by < GRID_SIZE) {
            grid[by][bx] = blockType;  // Tempatkan di grid
        }
    }
}

/**
 * Cek apakah blok tertentu memiliki posisi penempatan yang valid di grid
 * @param blockType: tipe blok yang dicek
 * @return: true jika ada posisi valid, false jika tidak ada
 */
boolean HasValidPlacement(int blockType){
    if (blockType < 1 || blockType > 36) return false;  // Validasi tipe

    // Cek setiap posisi di grid
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            if (CanPlaceBlock(x, y, blockType)) return true;
        }
    }
    return false;  // Tidak ada posisi valid
}

/**
 * Cek apakah blok dapat ditempatkan pada posisi tertentu
 * @param centerX: koordinat X center
 * @param centerY: koordinat Y center
 * @param blockType: tipe blok
 * @return: true jika dapat ditempatkan, false jika tidak
 */
boolean CanPlaceBlock(int centerX, int centerY, int blockType){
    if (blockType < 1 || blockType > 36) return false;  // Validasi tipe

    // Cek setiap bagian blok
    for (int i = 0; i < MAX_BLOCK_SIZE; i++) {
        int bx = centerX + (int)blockShapes[blockType][i].x;
        int by = centerY + (int)blockShapes[blockType][i].y;
        
        // Cek apakah keluar dari batas atau bertabrakan dengan blok lain
        if (bx < 0 || bx >= GRID_SIZE || by < 0 || by >= GRID_SIZE || grid[by][bx] != 0) {
            return false;
        }
    }
    return true;
}

// ============ SISTEM PEMBERSIHAN BARIS/KOLOM ============

/**
 * Membersihkan baris dan kolom yang penuh
 * Menghitung skor, combo, dan memainkan animasi
 */
void ClearFullLines(){
    int clearedLines = 0;
    boolean hasCleared = false;
    
    // Cek baris yang penuh (horizontal)
    for (int y = 0; y < GRID_SIZE; y++) {
        boolean full = true;
        
        // Cek setiap sel dalam baris
        for (int x = 0; x < GRID_SIZE; x++) {
            if (grid[y][x] == 0) {
                full = false;
                break;
            }
        }
        
        // Jika baris penuh, tambahkan animasi clearing
        if (full) {
            for (int x = 0; x < GRID_SIZE; x++) {
                EnqueueClearAnimation(x, y);  // Animasi penghapusan
            }
            clearedLines++;
            hasCleared = true;
        }
    }
    
    // Cek kolom yang penuh (vertical)
    for (int x = 0; x < GRID_SIZE; x++) {
        boolean full = true;
        
        // Cek setiap sel dalam kolom
        for (int y = 0; y < GRID_SIZE; y++) {
            if (grid[y][x] == 0) {
                full = false;
                break;
            }
        }
        
        // Jika kolom penuh, tambahkan animasi clearing
        if (full) {
            for (int y = 0; y < GRID_SIZE; y++) {
                EnqueueClearAnimation(x, y);  // Animasi penghapusan
            }
            clearedLines++;
            hasCleared = true;
        }
    }
    
    // Hitung skor jika ada baris/kolom yang dibersihkan
    if (hasCleared) {
        // Skor dasar berdasarkan jumlah baris/kolom yang dibersihkan
        int baseScore = 0;
        switch(clearedLines) {
            case 1: baseScore = 100; break;   // 1 baris/kolom
            case 2: baseScore = 250; break;   // 2 baris/kolom
            case 3: baseScore = 400; break;   // 3 baris/kolom
            default: baseScore = 600; break;  // 4+ baris/kolom
        }
        
        // Sistem combo: jika clearing berturut-turut
        if (currentTurn == lastClearTurn + 1) {
            comboCount++;  // Increment combo
        } else {
            comboCount = 0;  // Reset combo
        }
        
        // Multiplier combo: setiap combo +50% skor
        float comboMultiplier = 1.0f + (comboCount * 0.5f);
        int finalScore = (int)(baseScore * comboMultiplier);
        
        score += finalScore;  // Tambahkan ke skor total
        lastClearTurn = currentTurn;  // Update turn terakhir clearing
        
        // Audio feedback
        PlayScoreSound();
        PlayExplosionSound();
        
        // Debug info
        printf("Lines cleared: %d, Base score: %d, Combo: %d, Final score: %d\n", 
               clearedLines, baseScore, comboCount, finalScore);
    }
    
    currentTurn++;  // Increment turn counter
}

// ============ UTILITAS QUEUE ============

/**
 * Menghapus blok pada index tertentu dari queue
 * @param index: posisi blok yang akan dihapus (0 = depan)
 */
void RemoveBlockFromQueue(int index) {
    if (index < 0 || queueFront == NULL) return;
    
    // Jika hapus elemen pertama
    if (index == 0) {
        Dequeue();
        return;
    }
    
    // Cari node sebelum target
    QueueNode* current = queueFront;
    for (int i = 0; i < index - 1 && current != NULL; i++) {
        current = current->next;
    }
    
    // Validasi node ditemukan
    if (current == NULL || current->next == NULL) return;
    
    // Hapus node target
    QueueNode* nodeToDelete = current->next;
    current->next = nodeToDelete->next;
    
    // Update rear jika node yang dihapus adalah rear
    if (nodeToDelete == queueRear) {
        queueRear = current;
    }
    
    free(nodeToDelete);  // Dealokasi memori
}

/**
 * Menghitung jumlah elemen dalam queue
 * @return: jumlah blok dalam queue
 */
int GetQueueSize() {
    int size = 0;
    QueueNode* current = queueFront;
    
    // Traverse seluruh queue
    while (current != NULL) {
        size++;
        current = current->next;
    }
    return size;
}

/**
 * Mengosongkan seluruh queue
 */
void ClearQueue() {
    while (Dequeue() != -1);  // Dequeue sampai kosong
}

// ============ SISTEM GENERASI BLOK ============

/**
 * Menghasilkan tipe blok random yang valid
 * @return: tipe blok random (1-36)
 */
int GetValidRandomBlockType() {
    return GetRandomValue(1, 36);  // Raylib function
}

/**
 * Menghasilkan batch 3 blok random
 * @param blockUsed: array status penggunaan blok
 */
void GenerateRandomBatch(boolean* blockUsed) {
    ClearQueue();  // Kosongkan queue lama
    
    // Generate 3 blok random
    for (int i = 0; i < 3; i++) {
        int blockType;
        int attempts = 0;
        
        // Coba generate blok valid dengan retry mechanism
        do {
            blockType = GetValidRandomBlockType();
            attempts++;
            
            // Fallback setelah 50 percobaan
            if (attempts >= 50) {
                blockType = (i % 36) + 1;
                break;
            }
        } while (blockType < 1 || blockType > 36);
        
        // Validasi akhir dan fallback
        if (blockType < 1 || blockType > 36) {
            blockType = (i % 40) + 1;  // Note: mungkin bug, seharusnya 36
        }
        
        Enqueue(blockType);
        blockUsed[i] = false;  // Mark as unused
    }
    
    // Validasi batch yang di-generate
    for (int i = 0; i < 3; i++) {
        int queueBlock = GetQueueAt(i);
        if (queueBlock == -1 || queueBlock < 1 || queueBlock > 36) {
            // Jika ada masalah, generate ulang dengan fallback sederhana
            ClearQueue();
            for (int j = 0; j < 3; j++) {
                Enqueue((j % 36) + 1);
                blockUsed[j] = false;
            }
            break;
        }
    }
}

/**
 * Menginisialisasi grid dengan blok-blok random
 * Digunakan untuk mode challenge atau setup awal
 */
void InitializeRandomGrid() {
    // Reset grid
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            grid[y][x] = 0;
        }
    }
    
    // Hitung jumlah blok random (20-30% dari total sel)
    int totalCells = GRID_SIZE * GRID_SIZE;
    int randomBlockCount = GetRandomValue(totalCells * 20 / 50, totalCells * 30 / 50);
    
    // Tempatkan blok-blok random
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
            if (!found) continue;  // Skip jika tidak ada posisi kosong
        }
        
        // Tempatkan blok sederhana (tipe 1-6)
        grid[y][x] = GetRandomValue(1, 6);
    }
}

/**
 * Generate batch baru menggunakan algoritma terbaik (AI)
 * Fallback ke random jika gagal
 * @param blockUsed: array status penggunaan blok
 */
void GenerateNewBatch(boolean* blockUsed) { 
    GenerateBestBatch(blockUsed);  // Fungsi AI (dari tree.h)

    // Validasi batch yang di-generate
    boolean validBatch = true;
    for (int i = 0; i < 3; i++) {
        int queueBlock = GetQueueAt(i);
        if (queueBlock == -1 || queueBlock < 1 || queueBlock > 36) {
            validBatch = false;
            break;
        }
    }
    
    // Fallback ke random jika batch AI tidak valid
    if (!validBatch) {
        GenerateRandomBatch(blockUsed);
    }
}

/**
 * Cek apakah masih ada gerakan valid yang tersisa
 * @param blockUsed: array status penggunaan blok
 * @return: true jika masih ada gerakan valid, false jika game over
 */
boolean HasAnyValidMove(boolean* blockUsed) {
    // Cek setiap blok dalam queue
    for (int i = 0; i < 3; i++) {
        int blockType = GetQueueAt(i);
        
        // Jika blok valid dan belum digunakan
        if (blockType >= 1 && blockType <= 36 && !blockUsed[i]) {
            if (HasValidPlacement(blockType)) {
                return true;  // Ada gerakan valid
            }
        }
    }
    return false;  // Tidak ada gerakan valid = Game Over
}

// ============ SISTEM UNDO ============
// ============ SISTEM UNDO ============

Move* undoStack = NULL;  // Stack untuk menyimpan gerakan

/**
 * Menambahkan gerakan ke undo stack
 * @param stack: pointer ke stack
 * @param blockType: tipe blok yang ditempatkan
 * @param center: posisi center blok
 * @param queueIndex: index blok dalam queue
 * @param isScored: apakah gerakan ini menghasilkan skor
 */
void PushMove(Move** stack, int blockType, Vector2 center, int queueIndex, bool isScored) {
    Move* newMove = (Move*)malloc(sizeof(Move));
    newMove->blockType = blockType;
    newMove->center = center;
    newMove->queueIndex = queueIndex;
    newMove->isScored = isScored;
    newMove->next = *stack;  // Link ke stack lama
    *stack = newMove;        // Update stack head
}

/**
 * Mengambil gerakan terakhir dari undo stack
 * @param stack: pointer ke stack
 * @param blockType: output tipe blok
 * @param center: output posisi center
 * @param queueIndex: output index queue
 * @param isScored: output status skor
 * @return: true jika berhasil, false jika stack kosong
 */
int stacks = 4;

bool PopMove(Move** stack, int* blockType, Vector2* center, int* queueIndex, bool* isScored) {
    if (*stack == NULL) return false;  // Stack kosong
    // Ambil data dari top of stack
    Move* temp = *stack;
    *blockType = temp->blockType;
    *center = temp->center;
    *queueIndex = temp->queueIndex;
    *isScored = temp->isScored;

    // Update stack dan dealokasi
    *stack = temp->next;
    free(temp);
    return true;
}

/**
 * Mengosongkan seluruh undo stack
 * @param stack: pointer ke stack
 */
void ClearStack(Move** stack) {
    while (*stack != NULL) {
        Move* temp = *stack;
        *stack = temp->next;
        free(temp);
    }
}

/**
 * Menghapus blok dari grid berdasarkan posisi dan tipe
 * @param x: koordinat X center
 * @param y: koordinat Y center
 * @param blockType: tipe blok yang akan dihapus
 */
void RemoveBlockFromGrid(int x, int y, int blockType) {
    // Hapus setiap bagian blok
    for (int i = 0; i < MAX_BLOCK_SIZE; i++) {
        int bx = x + (int)blockShapes[blockType][i].x;
        int by = y + (int)blockShapes[blockType][i].y;
        
        // Pastikan dalam batas grid
        if (bx >= 0 && bx < GRID_SIZE && by >= 0 && by < GRID_SIZE) {
            grid[by][bx] = 0;  // Kosongkan sel
        }
    }
}

int undoCount = 0;  // Counter untuk membatasi undo

/**
 * Melakukan operasi undo gerakan terakhir
 * @param blockUsed: array status penggunaan blok
 * @return: true jika berhasil undo, false jika gagal
 */
boolean PerformUndo(boolean* blockUsed) {
    // Cek apakah undo tersedia (max 3 undo per game)
    if (undoStack == NULL || undoCount >= 3) return false;

    int blockType, queueIndex;
    Vector2 center;
    bool isScored;
    // Pop gerakan terakhir dari stack
    if (PopMove(&undoStack, &blockType, &center, &queueIndex, &isScored)) {
        // Tidak bisa undo gerakan yang sudah menghasilkan skor
        if (stacks < 0) return false;
        stacks--;
        if (isScored) return false;
        // Hapus blok dari grid dan restore status queue
        RemoveBlockFromGrid((int)center.x, (int)center.y, blockType);
        blockUsed[queueIndex] = false;  // Mark block as unused again
        undoCount++;  // Increment undo counter
        stacks--;
        return true;
    }
    return false;
}

void ResetUndoSystem() {
    stacks = 4;
    ClearStack(&undoStack);
}