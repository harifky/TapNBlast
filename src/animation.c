#include "../lib/animation.h"
#include "../lib/global.h"
#include "raylib.h"
#include <stdlib.h>
#include <math.h>

// Kontribusi: Faiz, Fariz

// ============ STRUKTUR DATA QUEUE UNTUK ANIMASI ============

// Queue untuk animasi penghapusan blok (clearing animation)
clearAnim* clearAnimFront = NULL;  // Pointer ke elemen pertama queue
clearAnim* clearAnimRear = NULL;   // Pointer ke elemen terakhir queue

// Queue untuk animasi penempatan blok (placing animation)
placeAnim* placeAnimFront = NULL;  // Pointer ke elemen pertama queue
placeAnim* placeAnimRear = NULL;   // Pointer ke elemen terakhir queue

// ============ FUNGSI ENQUEUE UNTUK MENAMBAH ANIMASI ============

/**
 * Menambahkan animasi penghapusan blok ke dalam queue
 * @param x: koordinat X blok yang akan dihapus
 * @param y: koordinat Y blok yang akan dihapus
 */
void EnqueueClearAnimation(int x, int y) {
    // Alokasi memori untuk node animasi baru
    clearAnim* newNode = (clearAnim*)malloc(sizeof(clearAnim));
    if (newNode == NULL) return;  // Gagal alokasi memori
    
    // Inisialisasi data node
    newNode->x = x;
    newNode->y = y;
    newNode->timer = 0.0f;  // Timer mulai dari 0
    newNode->next = NULL;

    // Tambahkan ke queue (FIFO - First In First Out)
    if (clearAnimRear == NULL) {
        // Queue kosong, node pertama
        clearAnimFront = clearAnimRear = newNode;
    } else {
        // Tambahkan di belakang
        clearAnimRear->next = newNode;
        clearAnimRear = newNode;
    }
}

/**
 * Menambahkan animasi penempatan blok ke dalam queue
 * @param x: koordinat X blok yang akan ditempatkan
 * @param y: koordinat Y blok yang akan ditempatkan
 * @param blockType: tipe/jenis blok yang akan ditempatkan
 */
void EnqueuePlaceAnimation(int x, int y, int blockType) {
    // Alokasi memori untuk node animasi baru
    placeAnim* newNode = (placeAnim*)malloc(sizeof(placeAnim));
    if (newNode == NULL) return;  // Gagal alokasi memori
    
    // Inisialisasi data node
    newNode->x = x;
    newNode->y = y;
    newNode->blockType = blockType;
    newNode->timer = 0.0f;  // Timer mulai dari 0
    newNode->next = NULL;

    // Tambahkan ke queue (FIFO)
    if (placeAnimRear == NULL) {
        // Queue kosong, node pertama
        placeAnimFront = placeAnimRear = newNode;
    } else {
        // Tambahkan di belakang
        placeAnimRear->next = newNode;
        placeAnimRear = newNode;
    }
}

// ============ FUNGSI EASING UNTUK ANIMASI SMOOTH ============

/**
 * Easing function: Bounce Out
 * Memberikan efek "memantul" di akhir animasi
 * @param t: progress animasi (0.0 - 1.0)
 * @return : nilai yang sudah di-ease
 */
float EaseOutBounce(float t) {
    if (t < 1.0f / 2.75f) {
        return 7.5625f * t * t;
    } else if (t < 2.0f / 2.75f) {
        t -= 1.5f / 2.75f;
        return 7.5625f * t * t + 0.75f;
    } else if (t < 2.5f / 2.75f) {
        t -= 2.25f / 2.75f;
        return 7.5625f * t * t + 0.9375f;
    } else {
        t -= 2.625f / 2.75f;
        return 7.5625f * t * t + 0.984375f;
    }
}

/**
 * Easing function: Quartic Out
 * Animasi dimulai cepat kemudian melambat
 * @param t: progress animasi (0.0 - 1.0)
 * @return: nilai yang sudah di-ease
 */
float EaseOutQuart(float t) {
    return 1.0f - powf(1.0f - t, 4.0f);
}

/**
 * Easing function: Quartic In
 * Animasi dimulai lambat kemudian mempercepat
 * @param t: progress animasi (0.0 - 1.0)
 * @return: nilai yang sudah di-ease
 */
float EaseInQuart(float t) {
    return t * t * t * t;
}

// ============ FUNGSI UPDATE DAN RENDER ANIMASI ============

/**
 * Update dan render semua animasi penghapusan blok
 * @param deltaTime: waktu yang berlalu sejak frame terakhir
 */
void UpdateAndDrawClearAnimations(float deltaTime) {
    clearAnim* curr = clearAnimFront;  // Mulai dari depan queue
    clearAnim* prev = NULL;

    // Iterasi melalui semua animasi dalam queue
    while (curr != NULL) {
        curr->timer += deltaTime;  // Update timer
        
        const float CLEAR_DURATION = 0.6f;  // Durasi animasi 0.6 detik
        float progress = curr->timer / CLEAR_DURATION;  // Progress 0-1
        
        // Jika animasi masih berjalan
        if (progress <= 1.0f) {
            // Perhitungan efek visual
            float alpha = 1.0f - EaseInQuart(progress);  // Transparansi berkurang
            float scale = 1.0f + (progress * 0.3f);     // Ukuran membesar 30%
            
            // Ambil warna blok dari grid
            Color color = blockColors[grid[curr->y][curr->x] - 1];
            color.a = (unsigned char)(255 * alpha);  // Terapkan transparansi
            
            // Hitung posisi render
            int dx = gridOriginX + curr->x * TILE_SIZE;
            int dy = gridOriginY + curr->y * TILE_SIZE;
            
            // Hitung ukuran dan offset untuk scaling
            int scaledSize = (int)((TILE_SIZE - 4) * scale);
            int offsetX = (TILE_SIZE - scaledSize) / 2;
            int offsetY = (TILE_SIZE - scaledSize) / 2;
            
            // Gambar blok dengan efek scaling dan fading
            DrawRectangle(dx + 2 + offsetX, dy + 2 + offsetY, scaledSize, scaledSize, color);
            
            // Efek glow untuk blok yang masih terlihat
            if (alpha > 0.3f) {
                Color glowColor = color;
                glowColor.a = (unsigned char)(50 * alpha);
                DrawRectangle(dx, dy, TILE_SIZE, TILE_SIZE, glowColor);
            }
        }

        // Jika animasi selesai
        if (curr->timer >= CLEAR_DURATION) {
            grid[curr->y][curr->x] = 0;  // Hapus blok dari grid

            // Hapus node dari queue dan dealokasi memori
            if (prev == NULL) {
                // Node pertama
                clearAnim* temp = curr;
                clearAnimFront = curr->next;
                if (clearAnimFront == NULL) clearAnimRear = NULL;
                curr = curr->next;
                free(temp);
            } else {
                // Node di tengah/akhir
                prev->next = curr->next;
                if (curr == clearAnimRear) clearAnimRear = prev;
                clearAnim* temp = curr;
                curr = prev->next;
                free(temp);
            }
        } else {
            // Lanjut ke node berikutnya
            prev = curr;
            curr = curr->next;
        }
    }
}

/**
 * Update dan render semua animasi penempatan blok
 * @param deltaTime: waktu yang berlalu sejak frame terakhir
 */
void UpdateAndDrawPlaceAnimations(float deltaTime) {
    placeAnim* curr = placeAnimFront;  // Mulai dari depan queue
    placeAnim* prev = NULL;

    // Iterasi melalui semua animasi dalam queue
    while (curr != NULL) {
        curr->timer += deltaTime;  // Update timer
        
        const float PLACE_DURATION = 0.4f;  // Durasi animasi 0.4 detik
        float progress = curr->timer / PLACE_DURATION;  // Progress 0-1
        
        // Jika animasi masih berjalan
        if (progress <= 1.0f) {
            // Perhitungan efek visual
            float scale = EaseOutBounce(progress);  // Efek bounce
            float alpha = EaseOutQuart(progress);   // Transparansi bertambah
            
            // Ambil warna blok berdasarkan tipe
            Color color = blockColors[curr->blockType - 1];
            color.a = (unsigned char)(255 * alpha);  // Terapkan transparansi
            
            // Hitung posisi render
            int dx = gridOriginX + curr->x * TILE_SIZE;
            int dy = gridOriginY + curr->y * TILE_SIZE;
            
            // Hitung ukuran dan offset untuk scaling
            int scaledSize = (int)((TILE_SIZE - 4) * scale);
            int offsetX = (TILE_SIZE - scaledSize) / 2;
            int offsetY = (TILE_SIZE - scaledSize) / 2;
            
            // Gambar blok dengan efek bounce dan fade in
            DrawRectangle(dx + 2 + offsetX, dy + 2 + offsetY, scaledSize, scaledSize, color);
            
            // Efek highlight putih di awal animasi
            if (progress < 0.3f) {
                Color highlightColor = WHITE;
                highlightColor.a = (unsigned char)(100 * (1.0f - progress / 0.3f));
                DrawRectangle(dx + 2 + offsetX, dy + 2 + offsetY, scaledSize, scaledSize, highlightColor);
            }
        }

        // Jika animasi selesai
        if (curr->timer >= PLACE_DURATION) {
            grid[curr->y][curr->x] = curr->blockType;  // Tempatkan blok di grid

            // Hapus node dari queue dan dealokasi memori
            if (prev == NULL) {
                // Node pertama
                placeAnim* temp = curr;
                placeAnimFront = curr->next;
                if (placeAnimFront == NULL) placeAnimRear = NULL;
                curr = curr->next;
                free(temp);
            } else {
                // Node di tengah/akhir
                prev->next = curr->next;
                if (curr == placeAnimRear) placeAnimRear = prev;
                placeAnim* temp = curr;
                curr = prev->next;
                free(temp);
            }
        } else {
            // Lanjut ke node berikutnya
            prev = curr;
            curr = curr->next;
        }
    }
}

// ============ FUNGSI UTILITAS ANIMASI ============

/**
 * Update dan render semua jenis animasi
 * @param deltaTime: waktu yang berlalu sejak frame terakhir
 */
void UpdateAndDrawAnimations(float deltaTime) {
    UpdateAndDrawClearAnimations(deltaTime);  // Update animasi penghapusan
    UpdateAndDrawPlaceAnimations(deltaTime);  // Update animasi penempatan
}

/**
 * Cek apakah ada animasi yang sedang berjalan
 * @return: true jika ada animasi aktif, false jika tidak
 */
boolean IsAnyAnimationActive() {
    return (clearAnimFront != NULL) || (placeAnimFront != NULL);
}

/**
 * Cek apakah ada animasi penghapusan yang sedang berjalan
 * @return: true jika ada animasi clear aktif, false jika tidak
 */
boolean IsClearAnimationActive() {
    return clearAnimFront != NULL;
}

/**
 * Cek apakah ada animasi penempatan yang sedang berjalan
 * @return: true jika ada animasi place aktif, false jika tidak
 */
boolean IsPlaceAnimationActive() {
    return placeAnimFront != NULL;
}

/**
 * Membersihkan semua animasi yang ada dalam queue
 * Digunakan untuk reset atau cleanup
 */
void ClearAnimationQueue() {
    // Hapus semua animasi penghapusan
    while (clearAnimFront != NULL) {
        clearAnim* temp = clearAnimFront;
        clearAnimFront = clearAnimFront->next;
        free(temp);  // Dealokasi memori
    }
    clearAnimRear = NULL;
    
    // Hapus semua animasi penempatan
    while (placeAnimFront != NULL) {
        placeAnim* temp = placeAnimFront;
        placeAnimFront = placeAnimFront->next;
        free(temp);  // Dealokasi memori
    }
    placeAnimRear = NULL;
}