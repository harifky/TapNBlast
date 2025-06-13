#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "boolean.h"

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

// Node untuk linked list leaderboard
typedef struct LeaderboardNode {
    LeaderboardEntry data;
    struct LeaderboardNode* next;
} LeaderboardNode;

// Head pointer untuk linked list (global variable)
static LeaderboardNode* leaderboardHead = NULL;

// Struktur untuk state input username
typedef struct {
    char inputText[MAX_USERNAME_LENGTH];
    int currentLength;
    boolean isActive;
    boolean isComplete;
} UsernameInput;

// Fungsi-fungsi leaderboard
void InitLeaderboard();
void LoadLeaderboardFromFile();
void SaveLeaderboard(LeaderboardEntry entries[], int count);
void AddToLeaderboard(const char* username, int score, int duration);
void SortLeaderboardList();
int GetLeaderboardAsArray(LeaderboardEntry entries[], int maxEntries);
int GetLeaderboardRank(int score);
void FormatDuration(int seconds, char* buffer, int bufferSize);
void ReturnToMainMenu();

// Fungsi untuk input username
void InitUsernameInput(UsernameInput* input);
void UpdateUsernameInput(UsernameInput* input);
void DrawUsernameInput(UsernameInput* input, int score, int duration);

UsernameInput usernameInput;
boolean isInGameOverInput = false;
boolean leaderboardSaved = false;
time_t gameStartTime;
int gameDuration;
int finalGameDuration = 0;
boolean shouldReturnToMenu = false;


#endif