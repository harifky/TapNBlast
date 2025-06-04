#include "../lib/leaderboard.h"

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
}

// Load data leaderboard dari file text
int LoadLeaderboard(LeaderboardEntry entries[], int maxEntries) {
    FILE* file = fopen(LEADERBOARD_FILE, "r");
    if (file == NULL) {
        return 0;
    }
    
    int count;
    if (fscanf(file, "%d", &count) != 1) {
        fclose(file);
        return 0;
    }
    
    // Batasi jumlah entry yang dibaca
    if (count > maxEntries) {
        count = maxEntries;
    }
    
    int actualRead = 0;
    for (int i = 0; i < count; i++) {
        if (fscanf(file, "%s %d %d %ld", 
                   entries[i].username, 
                   &entries[i].score, 
                   &entries[i].duration, 
                   &entries[i].timestamp) == 4) {
            actualRead++;
        } else {
            break; // Stop jika ada error parsing
        }
    }
    
    fclose(file);
    return actualRead;
}

// Simpan data leaderboard ke file text
void SaveLeaderboard(LeaderboardEntry entries[], int count) {
    FILE* file = fopen(LEADERBOARD_FILE, "w");
    if (file == NULL) {
        printf("Error: Cannot save leaderboard to file!\n");
        return;
    }
    
    // Simpan jumlah entry di baris pertama
    fprintf(file, "%d\n", count);
    
    // Simpan semua entry dalam format: username score duration timestamp
    for (int i = 0; i < count; i++) {
        fprintf(file, "%s %d %d %ld\n", 
                entries[i].username, 
                entries[i].score, 
                entries[i].duration, 
                entries[i].timestamp);
    }
    
    fclose(file);
}

// Sorting leaderboard berdasarkan score (descending)
void SortLeaderboard(LeaderboardEntry entries[], int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            // Sort by score (descending), jika score sama sort by duration (ascending)
            if (entries[j].score < entries[j + 1].score || 
                (entries[j].score == entries[j + 1].score && entries[j].duration > entries[j + 1].duration)) {
                
                LeaderboardEntry temp = entries[j];
                entries[j] = entries[j + 1];
                entries[j + 1] = temp;
            }
        }
    }
}

// Tambah entry baru ke leaderboard
void AddToLeaderboard(const char* username, int score, int duration) {
    LeaderboardEntry entries[MAX_LEADERBOARD_ENTRIES];
    int count = LoadLeaderboard(entries, MAX_LEADERBOARD_ENTRIES);
    
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
    
    // Buat entry baru
    LeaderboardEntry newEntry;
    strncpy(newEntry.username, cleanUsername, MAX_USERNAME_LENGTH - 1);
    newEntry.username[MAX_USERNAME_LENGTH - 1] = '\0';
    newEntry.score = score;
    newEntry.duration = duration;
    newEntry.timestamp = time(NULL);
    
    // Tambahkan ke array
    if (count < MAX_LEADERBOARD_ENTRIES) {
        entries[count] = newEntry;
        count++;
    } else {
        // Jika sudah penuh, ganti entry terakhap jika score lebih tinggi
        entries[MAX_LEADERBOARD_ENTRIES - 1] = newEntry;
    }
    
    // Sort dan simpan
    SortLeaderboard(entries, count);
    SaveLeaderboard(entries, count);
    
    // Debug: Print ke console
    printf("Leaderboard entry added: %s, Score: %d, Duration: %d\n", 
           cleanUsername, score, duration);
}

// Dapatkan ranking berdasarkan score
int GetLeaderboardRank(int score) {
    LeaderboardEntry entries[MAX_LEADERBOARD_ENTRIES];
    int count = LoadLeaderboard(entries, MAX_LEADERBOARD_ENTRIES);
    
    for (int i = 0; i < count; i++) {
        if (score >= entries[i].score) {
            return i + 1;
        }
    }
    
    return count + 1;
}

// Format waktu untuk display
void FormatDuration(int seconds, char* buffer, int bufferSize) {
    int minutes = seconds / 60;
    int secs = seconds % 60;
    snprintf(buffer, bufferSize, "%02d:%02d", minutes, secs);
}

// Display leaderboard (untuk debugging atau console)
void DisplayLeaderboard(LeaderboardEntry entries[], int count) {
    printf("\n=== LEADERBOARD ===\n");
    printf("Rank | Username          | Score    | Time\n");
    printf("-----|-------------------|----------|--------\n");
    
    for (int i = 0; i < count && i < 10; i++) {
        char timeStr[16];
        FormatDuration(entries[i].duration, timeStr, sizeof(timeStr));
        
        // Replace underscores back to spaces untuk display
        char displayName[MAX_USERNAME_LENGTH];
        strncpy(displayName, entries[i].username, MAX_USERNAME_LENGTH);
        for (int j = 0; displayName[j] != '\0'; j++) {
            if (displayName[j] == '_') {
                displayName[j] = ' ';
            }
        }
        
        printf("%4d | %-17s | %8d | %s\n", 
               i + 1, displayName, entries[i].score, timeStr);
    }
    printf("\n");
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