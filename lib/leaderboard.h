#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_USERNAME_LENGTH 20
#define MAX_LEADERBOARD_ENTRIES 50
#define LEADERBOARD_FILE "leaderboard.txt"

// Struktur untuk menyimpan data pemain
typedef struct {
    char username[MAX_USERNAME_LENGTH];
    int score;
    int duration; // dalam detik
    time_t timestamp; // waktu pencatatan
} LeaderboardEntry;

// Struktur untuk state input username
typedef struct {
    char inputText[MAX_USERNAME_LENGTH];
    int currentLength;
    bool isActive;
    bool isComplete;
} UsernameInput;

// Fungsi-fungsi leaderboard
void InitLeaderboard();
int LoadLeaderboard(LeaderboardEntry entries[], int maxEntries);
void SaveLeaderboard(LeaderboardEntry entries[], int count);
void AddToLeaderboard(const char* username, int score, int duration);
void SortLeaderboard(LeaderboardEntry entries[], int count);
int GetLeaderboardRank(int score);
void DisplayLeaderboard(LeaderboardEntry entries[], int count);
void FormatDuration(int seconds, char* buffer, int bufferSize);

// Fungsi untuk input username
void InitUsernameInput(UsernameInput* input);
void UpdateUsernameInput(UsernameInput* input);
void DrawUsernameInput(UsernameInput* input, int score, int duration);


#endif