#include "../lib/leaderboard.h"

// Helper function untuk membuat node baru
LeaderboardNode* CreateLeaderboardNode(const char* username, int score, int duration) {
    LeaderboardNode* newNode = (LeaderboardNode*)malloc(sizeof(LeaderboardNode));
    if (newNode == NULL) {
        printf("Error: Memory allocation failed!\n");
        return NULL;
    }
    
    // Set data
    strncpy(newNode->data.username, username, MAX_USERNAME_LENGTH - 1);
    newNode->data.username[MAX_USERNAME_LENGTH - 1] = '\0';
    newNode->data.score = score;
    newNode->data.duration = duration;
    newNode->data.timestamp = time(NULL);
    newNode->next = NULL;
    
    return newNode;
}

// Helper function untuk menghitung jumlah node
int CountLeaderboardNodes() {
    int count = 0;
    LeaderboardNode* current = leaderboardHead;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}

// Helper function untuk membebaskan semua node
void FreeLeaderboardList() {
    LeaderboardNode* current = leaderboardHead;
    while (current != NULL) {
        LeaderboardNode* temp = current;
        current = current->next;
        free(temp);
    }
    leaderboardHead = NULL;
}

// Inisialisasi sistem leaderboard dengan format text
void InitLeaderboard() {
    // Buat file leaderboard.txt jika belum ada
    FILE* file = fopen(LEADERBOARD_FILE, "r");
    if (file == NULL) {
        // File tidak ada, buat file kosong dengan header
        file = fopen(LEADERBOARD_FILE, "w");
        if (file != NULL) {
            fprintf(file, "0\n"); // Tulis jumlah entry awal (0)
            fclose(file);
        }
    } else {
        fclose(file);
    }
    
    // Load data dari file ke linked list
    LoadLeaderboardFromFile();
}

// Load data leaderboard dari file ke linked list
void LoadLeaderboardFromFile() {
    FILE* file = fopen(LEADERBOARD_FILE, "r");
    if (file == NULL) {
        return;
    }
    
    // Free existing list first
    FreeLeaderboardList();
    
    int count;
    if (fscanf(file, "%d", &count) != 1) {
        fclose(file);
        return;
    }
    
    // Read each entry and add to linked list
    for (int i = 0; i < count; i++) {
        char username[MAX_USERNAME_LENGTH];
        int score, duration;
        long timestamp;
        
        if (fscanf(file, "%s %d %d %ld", username, &score, &duration, &timestamp) == 4) {
            LeaderboardNode* newNode = CreateLeaderboardNode(username, score, duration);
            if (newNode != NULL) {
                newNode->data.timestamp = timestamp; // Override dengan timestamp dari file
                
                // Insert at end of list (will be sorted later)
                if (leaderboardHead == NULL) {
                    leaderboardHead = newNode;
                } else {
                    LeaderboardNode* current = leaderboardHead;
                    while (current->next != NULL) {
                        current = current->next;
                    }
                    current->next = newNode;
                }
            }
        } else {
            break; // Stop jika ada error parsing
        }
    }
    
    fclose(file);
    
    // Sort the loaded list
    SortLeaderboardList();
}

// Simpan data leaderboard dari linked list ke file text
void SaveLeaderboardToFile() {
    FILE* file = fopen(LEADERBOARD_FILE, "w");
    if (file == NULL) {
        printf("Error: Cannot save leaderboard to file!\n");
        return;
    }
    
    // Hitung dan simpan jumlah entry
    int count = CountLeaderboardNodes();
    fprintf(file, "%d\n", count);
    
    // Simpan semua entry dalam format: username score duration timestamp
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

// Sorting leaderboard linked list berdasarkan score (descending)
void SortLeaderboardList() {
    if (leaderboardHead == NULL || leaderboardHead->next == NULL) {
        return; // List kosong atau hanya 1 node
    }
    
    bool swapped;
    do {
        swapped = false;
        LeaderboardNode* current = leaderboardHead;
        LeaderboardNode* prev = NULL;
        
        while (current->next != NULL) {
            LeaderboardNode* nextNode = current->next;
            
            // Sort by score (descending), jika score sama sort by duration (ascending)
            bool shouldSwap = (current->data.score < nextNode->data.score) ||
                             (current->data.score == nextNode->data.score && 
                              current->data.duration > nextNode->data.duration);
            
            if (shouldSwap) {
                // Swap nodes
                if (prev == NULL) {
                    // Swapping head
                    leaderboardHead = nextNode;
                } else {
                    prev->next = nextNode;
                }
                
                current->next = nextNode->next;
                nextNode->next = current;
                
                swapped = true;
                prev = nextNode; // Update prev untuk iterasi berikutnya
            } else {
                prev = current;
                current = current->next;
            }
        }
    } while (swapped);
}

// Insert node secara terurut (alternative sorting method)
void InsertSorted(LeaderboardNode* newNode) {
    if (leaderboardHead == NULL || 
        newNode->data.score > leaderboardHead->data.score ||
        (newNode->data.score == leaderboardHead->data.score && 
         newNode->data.duration < leaderboardHead->data.duration)) {
        // Insert at beginning
        newNode->next = leaderboardHead;
        leaderboardHead = newNode;
        return;
    }
    
    LeaderboardNode* current = leaderboardHead;
    while (current->next != NULL &&
           (current->next->data.score > newNode->data.score ||
            (current->next->data.score == newNode->data.score &&
             current->next->data.duration <= newNode->data.duration))) {
        current = current->next;
    }
    
    newNode->next = current->next;
    current->next = newNode;
}

// Tambah entry baru ke leaderboard
void AddToLeaderboard(const char* username, int score, int duration) {
    // Validasi username - ganti spasi dengan underscore untuk format text
    char cleanUsername[MAX_USERNAME_LENGTH];
    strncpy(cleanUsername, username, MAX_USERNAME_LENGTH - 1);
    cleanUsername[MAX_USERNAME_LENGTH - 1] = '\0';
    
    // Replace spaces with underscores untuk kompatibilitas format text
    for (int i = 0; cleanUsername[i] != '\0'; i++) {
        if (cleanUsername[i] == ' ') {
            cleanUsername[i] = '_';
        }
    }
    
    // Buat node baru
    LeaderboardNode* newNode = CreateLeaderboardNode(cleanUsername, score, duration);
    if (newNode == NULL) {
        return;
    }
    
    // Insert secara terurut
    InsertSorted(newNode);
    
    // Batasi jumlah entries (hapus yang terakhir jika melebihi MAX_LEADERBOARD_ENTRIES)
    int count = CountLeaderboardNodes();
    if (count > MAX_LEADERBOARD_ENTRIES) {
        LeaderboardNode* current = leaderboardHead;
        LeaderboardNode* prev = NULL;
        
        // Cari node ke-(MAX_LEADERBOARD_ENTRIES-1)
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
    
    // Simpan ke file
    SaveLeaderboardToFile();
    
    // Debug: Print ke console
    printf("Leaderboard entry added: %s, Score: %d, Duration: %d\n", 
           cleanUsername, score, duration);
}

// Dapatkan ranking berdasarkan score
int GetLeaderboardRank(int score) {
    int rank = 1;
    LeaderboardNode* current = leaderboardHead;
    
    while (current != NULL) {
        if (score >= current->data.score) {
            return rank;
        }
        rank++;
        current = current->next;
    }
    
    return rank;
}

// Format waktu untuk display
void FormatDuration(int seconds, char* buffer, int bufferSize) {
    int minutes = seconds / 60;
    int secs = seconds % 60;
    snprintf(buffer, bufferSize, "%02d:%02d", minutes, secs);
}

// Get leaderboard data sebagai array (untuk compatibility dengan UI yang sudah ada)
int GetLeaderboardAsArray(LeaderboardEntry entries[], int maxEntries) {
    int count = 0;
    LeaderboardNode* current = leaderboardHead;
    
    while (current != NULL && count < maxEntries) {
        entries[count] = current->data;
        count++;
        current = current->next;
    }
    
    return count;
}

// Cleanup function - panggil saat program selesai
void CleanupLeaderboard() {
    FreeLeaderboardList();
}

// Inisialisasi input username
void InitUsernameInput(UsernameInput* input) {
    memset(input->inputText, 0, MAX_USERNAME_LENGTH);
    input->currentLength = 0;
    input->isActive = true;
    input->isComplete = false;
}

// Update input username (panggil setiap frame)
void UpdateUsernameInput(UsernameInput* input) {
    if (!input->isActive) return;
    
    // Dapatkan karakter yang ditekan
    int key = GetCharPressed();
    
    // Tambahkan karakter jika valid
    while (key > 0) {
        if ((key >= 32) && (key <= 125) && (input->currentLength < MAX_USERNAME_LENGTH - 1)) {
            input->inputText[input->currentLength] = (char)key;
            input->inputText[input->currentLength + 1] = '\0';
            input->currentLength++;
        }
        key = GetCharPressed();
    }
    
    // Handle backspace
    if (IsKeyPressed(KEY_BACKSPACE) && input->currentLength > 0) {
        input->currentLength--;
        input->inputText[input->currentLength] = '\0';
    }
    
    // Handle enter
    if (IsKeyPressed(KEY_ENTER) && input->currentLength > 0) {
        input->isComplete = true;
        input->isActive = false;
    }
}

void ReturnToMainMenu() {
    // Reset semua variabel game
    isInGameOverInput = false;
    leaderboardSaved = false;
    shouldReturnToMenu = true; // Set flag instead of calling StartGame
}