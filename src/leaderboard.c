#include "../lib/leaderboard.h"

// Kontribusi: Rifky (100)

/**
 * Fungsi untuk membuat node baru dalam linked list leaderboard
 * @param username: nama pengguna
 * @param score: skor yang dicapai
 * @param duration: durasi permainan dalam detik
 * @return pointer ke node baru atau NULL jika gagal
 */
LeaderboardNode* CreateLeaderboardNode(const char* username, int score, int duration) {
    // Alokasi memori untuk node baru
    LeaderboardNode* newNode = (LeaderboardNode*)malloc(sizeof(LeaderboardNode));
    if (newNode == NULL) {
        printf("Error: Memory allocation failed!\n");
        return NULL;
    }
    
    // Salin username dengan aman menggunakan strncpy untuk mencegah buffer overflow
    strncpy(newNode->data.username, username, MAX_USERNAME_LENGTH - 1);
    newNode->data.username[MAX_USERNAME_LENGTH - 1] = '\0'; // Pastikan null-terminated
    
    // Set data node
    newNode->data.score = score;
    newNode->data.duration = duration;
    newNode->data.timestamp = time(NULL); // Waktu saat ini
    newNode->next = NULL; // Inisialisasi pointer next
    
    return newNode;
}

/**
 * Menghitung jumlah node dalam linked list leaderboard
 * @return jumlah node
 */
int CountLeaderboardNodes() {
    int count = 0;
    LeaderboardNode* current = leaderboardHead;
    
    // Traverse linked list dan hitung node
    while (current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}

/**
 * Membebaskan semua memori yang dialokasi untuk linked list leaderboard
 */
void FreeLeaderboardList() {
    LeaderboardNode* current = leaderboardHead;
    
    // Traverse dan free setiap node
    while (current != NULL) {
        LeaderboardNode* temp = current;
        current = current->next;
        free(temp); // Bebaskan memori node
    }
    leaderboardHead = NULL; // Reset head pointer
}

/**
 * Inisialisasi sistem leaderboard
 * Membuat file jika belum ada, atau load data yang sudah ada
 */
void InitLeaderboard() {
    // Coba buka file untuk membaca
    FILE* file = fopen(LEADERBOARD_FILE, "r");
    if (file == NULL) {
        // File tidak ada, buat file baru dengan data kosong
        file = fopen(LEADERBOARD_FILE, "w");
        if (file != NULL) {
            fprintf(file, "0\n"); // Tulis 0 entries
            fclose(file);
        }
    } else {
        fclose(file);
    }
    
    // Load data dari file
    LoadLeaderboardFromFile();
}

/**
 * Memuat data leaderboard dari file
 * Format file: [jumlah_entries]\n[username score duration timestamp]\n...
 */
void LoadLeaderboardFromFile() {
    FILE* file = fopen(LEADERBOARD_FILE, "r");
    if (file == NULL) {
        return; // File tidak ada
    }
    
    // Bersihkan list yang ada sebelumnya
    FreeLeaderboardList();
    
    int count;
    // Baca jumlah entries
    if (fscanf(file, "%d", &count) != 1) {
        fclose(file);
        return; // Format file salah
    }
    
    // Baca setiap entry
    for (int i = 0; i < count; i++) {
        char username[MAX_USERNAME_LENGTH];
        int score, duration;
        long timestamp;
        
        // Baca data entry
        if (fscanf(file, "%s %d %d %ld", username, &score, &duration, &timestamp) == 4) {
            LeaderboardNode* newNode = CreateLeaderboardNode(username, score, duration);
            if (newNode != NULL) {
                newNode->data.timestamp = timestamp; // Set timestamp dari file
                
                // Tambahkan ke linked list
                if (leaderboardHead == NULL) {
                    leaderboardHead = newNode; // Node pertama
                } else {
                    // Tambahkan di akhir list
                    LeaderboardNode* current = leaderboardHead;
                    while (current->next != NULL) {
                        current = current->next;
                    }
                    current->next = newNode;
                }
            }
        } else {
            break; // Format entry salah, hentikan pembacaan
        }
    }
    
    fclose(file);
    
    // Urutkan list setelah loading
    SortLeaderboardList();
}

/**
 * Menyimpan data leaderboard ke file
 */
void SaveLeaderboardToFile() {
    FILE* file = fopen(LEADERBOARD_FILE, "w");
    if (file == NULL) {
        printf("Error: Cannot save leaderboard to file!\n");
        return;
    }
    
    // Tulis jumlah entries
    int count = CountLeaderboardNodes();
    fprintf(file, "%d\n", count);
    
    // Tulis setiap entry
    LeaderboardNode* current = leaderboardHead;
    while (current != NULL) {
        fprintf(file, "%s %d %d %ld\n", 
                current->data.username, 
                current->data.score, 
                current->data.duration, 
                current->data.timestamp);
        current = current->next;
    }
    
    fclose(file);
}

/**
 * Mengurutkan linked list leaderboard menggunakan bubble sort
 * Kriteria: skor tertinggi dulu, jika sama maka durasi terpendek dulu
 */
void SortLeaderboardList() {
    if (leaderboardHead == NULL || leaderboardHead->next == NULL) {
        return; // List kosong atau hanya 1 element
    }
    
    boolean swapped;
    do {
        swapped = false;
        LeaderboardNode* current = leaderboardHead;
        LeaderboardNode* prev = NULL;
        
        while (current->next != NULL) {
            LeaderboardNode* nextNode = current->next;
            
            // Tentukan apakah perlu swap
            // Swap jika: skor current < skor next, atau
            // skor sama tapi durasi current > durasi next
            boolean shouldSwap = (current->data.score < nextNode->data.score) ||
                             (current->data.score == nextNode->data.score && 
                              current->data.duration > nextNode->data.duration);
            
            if (shouldSwap) {
                // Lakukan swap node
                if (prev == NULL) {
                    leaderboardHead = nextNode; // Swap dengan head
                } else {
                    prev->next = nextNode;
                }
                
                current->next = nextNode->next;
                nextNode->next = current;
                
                swapped = true;
                prev = nextNode; // Update prev untuk iterasi selanjutnya
            } else {
                prev = current;
                current = current->next;
            }
        }
    } while (swapped); // Ulangi sampai tidak ada swap
}

/**
 * Menyisipkan node baru ke dalam list yang sudah terurut
 * @param newNode: node baru yang akan disisipkan
 */
void InsertSorted(LeaderboardNode* newNode) {
    // Jika list kosong atau node baru harus di head
    if (leaderboardHead == NULL || 
        newNode->data.score > leaderboardHead->data.score ||
        (newNode->data.score == leaderboardHead->data.score && 
         newNode->data.duration < leaderboardHead->data.duration)) {
        newNode->next = leaderboardHead;
        leaderboardHead = newNode;
        return;
    }
    
    // Cari posisi yang tepat untuk menyisipkan
    LeaderboardNode* current = leaderboardHead;
    while (current->next != NULL &&
           (current->next->data.score > newNode->data.score ||
            (current->next->data.score == newNode->data.score &&
             current->next->data.duration <= newNode->data.duration))) {
        current = current->next;
    }
    
    // Sisipkan node
    newNode->next = current->next;
    current->next = newNode;
}

/**
 * Menambahkan entry baru ke leaderboard
 * @param username: nama pengguna
 * @param score: skor yang dicapai
 * @param duration: durasi permainan
 */
void AddToLeaderboard(const char* username, int score, int duration) {
    // Bersihkan username (ganti spasi dengan underscore)
    char cleanUsername[MAX_USERNAME_LENGTH];
    strncpy(cleanUsername, username, MAX_USERNAME_LENGTH - 1);
    cleanUsername[MAX_USERNAME_LENGTH - 1] = '\0';

    for (int i = 0; cleanUsername[i] != '\0'; i++) {
        if (cleanUsername[i] == ' ') {
            cleanUsername[i] = '_';
        }
    }

    // Cek apakah user sudah ada di leaderboard
    LeaderboardNode* current = leaderboardHead;
    LeaderboardNode* foundNode = NULL;

    while (current != NULL) {
        if (strcmp(current->data.username, cleanUsername) == 0) {
            foundNode = current;
            break;
        }
        current = current->next;
    }

    if (foundNode != NULL) {
        // User sudah ada, update hanya jika skor lebih tinggi
        if (score > foundNode->data.score) {
            foundNode->data.score = score;
            foundNode->data.duration = duration;
            foundNode->data.timestamp = time(NULL);
            printf("Leaderboard entry updated for %s: New Score: %d, New Duration: %d\n",
                   cleanUsername, score, duration);
            SortLeaderboardList(); // Re-sort karena skor berubah
        } else {
            printf("Leaderboard entry for %s not updated (score not higher).\n", cleanUsername);
        }
    } else {
        // User baru, buat node baru
        LeaderboardNode* newNode = CreateLeaderboardNode(cleanUsername, score, duration);
        if (newNode == NULL) {
            return; // Gagal membuat node
        }

        // Sisipkan ke posisi yang tepat
        InsertSorted(newNode);

        // Batasi jumlah entries maksimal
        int count = CountLeaderboardNodes();
        if (count > MAX_LEADERBOARD_ENTRIES) {
            // Hapus entries yang berlebih dari akhir list
            LeaderboardNode* current = leaderboardHead;
            LeaderboardNode* prev = NULL;

            // Navigasi ke entry ke-(MAX_LEADERBOARD_ENTRIES-1)
            for (int i = 0; i < MAX_LEADERBOARD_ENTRIES - 1 && current != NULL; i++) {
                prev = current;
                current = current->next;
            }

            // Hapus sisanya
            if (prev != NULL) {
                prev->next = NULL;
                while (current != NULL) {
                    LeaderboardNode* temp = current;
                    current = current->next;
                    free(temp);
                }
            }
        }
        printf("Leaderboard entry added: %s, Score: %d, Duration: %d\n",
               cleanUsername, score, duration);
    }

    // Simpan perubahan ke file
    SaveLeaderboardToFile();
}

/**
 * Menentukan ranking dari skor tertentu
 * @param score: skor yang ingin dicek rankingnya
 * @return ranking (1 = tertinggi)
 */
int GetLeaderboardRank(int score) {
    int rank = 1;
    LeaderboardNode* current = leaderboardHead;
    
    // Hitung berapa entry yang skornya lebih tinggi
    while (current != NULL) {
        if (score >= current->data.score) {
            return rank; // Ranking ditemukan
        }
        rank++;
        current = current->next;
    }
    
    return rank; // Ranking terakhir jika skor paling rendah
}

/**
 * Format durasi dari detik ke format MM:SS
 * @param seconds: durasi dalam detik
 * @param buffer: buffer untuk menyimpan string formatted
 * @param bufferSize: ukuran buffer
 */
void FormatDuration(int seconds, char* buffer, int bufferSize) {
    int minutes = seconds / 60;
    int secs = seconds % 60;
    snprintf(buffer, bufferSize, "%02d:%02d", minutes, secs);
}

/**
 * Mengkonversi linked list leaderboard ke array
 * @param entries: array untuk menyimpan data
 * @param maxEntries: kapasitas maksimal array
 * @return jumlah entries yang disalin
 */
int GetLeaderboardAsArray(LeaderboardEntry entries[], int maxEntries) {
    int count = 0;
    LeaderboardNode* current = leaderboardHead;
    
    // Salin data dari linked list ke array
    while (current != NULL && count < maxEntries) {
        entries[count] = current->data;
        count++;
        current = current->next;
    }
    
    return count;
}

/**
 * Membersihkan semua data leaderboard dari memori
 */
void CleanupLeaderboard() {
    FreeLeaderboardList();
}

/**
 * Inisialisasi struktur input username
 * @param input: pointer ke struktur UsernameInput
 */
void InitUsernameInput(UsernameInput* input) {
    memset(input->inputText, 0, MAX_USERNAME_LENGTH); // Kosongkan buffer
    input->currentLength = 0;
    input->isActive = true;     // Input aktif
    input->isComplete = false;  // Belum selesai
}

/**
 * Update input username berdasarkan input keyboard
 * @param input: pointer ke struktur UsernameInput
 */
void UpdateUsernameInput(UsernameInput* input) {
    if (!input->isActive) return; // Skip jika tidak aktif
    
    int key = GetCharPressed(); // Ambil karakter yang ditekan
    
    // Proses semua karakter yang ditekan
    while (key > 0) {
        // Terima karakter ASCII 32-125 (printable characters)
        if ((key >= 32) && (key <= 125) && (input->currentLength < MAX_USERNAME_LENGTH - 1)) {
            input->inputText[input->currentLength] = (char)key;
            input->inputText[input->currentLength + 1] = '\0'; // Null terminate
            input->currentLength++;
        }
        key = GetCharPressed(); // Ambil karakter berikutnya
    }
    
    // Handle backspace
    if (IsKeyPressed(KEY_BACKSPACE) && input->currentLength > 0) {
        input->currentLength--;
        input->inputText[input->currentLength] = '\0';
    }
    
    // Handle enter (selesai input)
    if (IsKeyPressed(KEY_ENTER) && input->currentLength > 0) {
        input->isComplete = true;
        input->isActive = false;
    }
}

/**
 * Kembali ke main menu
 * Reset berbagai flag state
 */
void ReturnToMainMenu() {
    isInGameOverInput = false;
    leaderboardSaved = false;
    shouldReturnToMenu = true;
}