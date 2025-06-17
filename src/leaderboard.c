#include "../lib/leaderboard.h"

LeaderboardNode* CreateLeaderboardNode(const char* username, int score, int duration) {
    LeaderboardNode* newNode = (LeaderboardNode*)malloc(sizeof(LeaderboardNode));
    if (newNode == NULL) {
        printf("Error: Memory allocation failed!\n");
        return NULL;
    }
    
    strncpy(newNode->data.username, username, MAX_USERNAME_LENGTH - 1);
    newNode->data.username[MAX_USERNAME_LENGTH - 1] = '\0';
    newNode->data.score = score;
    newNode->data.duration = duration;
    newNode->data.timestamp = time(NULL);
    newNode->next = NULL;
    
    return newNode;
}

int CountLeaderboardNodes() {
    int count = 0;
    LeaderboardNode* current = leaderboardHead;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}

void FreeLeaderboardList() {
    LeaderboardNode* current = leaderboardHead;
    while (current != NULL) {
        LeaderboardNode* temp = current;
        current = current->next;
        free(temp);
    }
    leaderboardHead = NULL;
}

void InitLeaderboard() {
    FILE* file = fopen(LEADERBOARD_FILE, "r");
    if (file == NULL) {
        file = fopen(LEADERBOARD_FILE, "w");
        if (file != NULL) {
            fprintf(file, "0\n");
            fclose(file);
        }
    } else {
        fclose(file);
    }
    
    LoadLeaderboardFromFile();
}

void LoadLeaderboardFromFile() {
    FILE* file = fopen(LEADERBOARD_FILE, "r");
    if (file == NULL) {
        return;
    }
    
    FreeLeaderboardList();
    
    int count;
    if (fscanf(file, "%d", &count) != 1) {
        fclose(file);
        return;
    }
    
    for (int i = 0; i < count; i++) {
        char username[MAX_USERNAME_LENGTH];
        int score, duration;
        long timestamp;
        
        if (fscanf(file, "%s %d %d %ld", username, &score, &duration, &timestamp) == 4) {
            LeaderboardNode* newNode = CreateLeaderboardNode(username, score, duration);
            if (newNode != NULL) {
                newNode->data.timestamp = timestamp;
                
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
            break;
        }
    }
    
    fclose(file);
    
    SortLeaderboardList();
}

void SaveLeaderboardToFile() {
    FILE* file = fopen(LEADERBOARD_FILE, "w");
    if (file == NULL) {
        printf("Error: Cannot save leaderboard to file!\n");
        return;
    }
    
    int count = CountLeaderboardNodes();
    fprintf(file, "%d\n", count);
    
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

void SortLeaderboardList() {
    if (leaderboardHead == NULL || leaderboardHead->next == NULL) {
        return;
    }
    
    boolean swapped;
    do {
        swapped = false;
        LeaderboardNode* current = leaderboardHead;
        LeaderboardNode* prev = NULL;
        
        while (current->next != NULL) {
            LeaderboardNode* nextNode = current->next;
            
            boolean shouldSwap = (current->data.score < nextNode->data.score) ||
                             (current->data.score == nextNode->data.score && 
                              current->data.duration > nextNode->data.duration);
            
            if (shouldSwap) {
                if (prev == NULL) {
                    leaderboardHead = nextNode;
                } else {
                    prev->next = nextNode;
                }
                
                current->next = nextNode->next;
                nextNode->next = current;
                
                swapped = true;
                prev = nextNode;
            } else {
                prev = current;
                current = current->next;
            }
        }
    } while (swapped);
}

void InsertSorted(LeaderboardNode* newNode) {
    if (leaderboardHead == NULL || 
        newNode->data.score > leaderboardHead->data.score ||
        (newNode->data.score == leaderboardHead->data.score && 
         newNode->data.duration < leaderboardHead->data.duration)) {
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

void AddToLeaderboard(const char* username, int score, int duration) {
    char cleanUsername[MAX_USERNAME_LENGTH];
    strncpy(cleanUsername, username, MAX_USERNAME_LENGTH - 1);
    cleanUsername[MAX_USERNAME_LENGTH - 1] = '\0';

    for (int i = 0; cleanUsername[i] != '\0'; i++) {
        if (cleanUsername[i] == ' ') {
            cleanUsername[i] = '_';
        }
    }

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
        if (score > foundNode->data.score) {
            foundNode->data.score = score;
            foundNode->data.duration = duration;
            foundNode->data.timestamp = time(NULL);
            printf("Leaderboard entry updated for %s: New Score: %d, New Duration: %d\n",
                   cleanUsername, score, duration);
            SortLeaderboardList();
        } else {
            printf("Leaderboard entry for %s not updated (score not higher).\n", cleanUsername);
        }
    } else {
        LeaderboardNode* newNode = CreateLeaderboardNode(cleanUsername, score, duration);
        if (newNode == NULL) {
            return;
        }

        InsertSorted(newNode);

        int count = CountLeaderboardNodes();
        if (count > MAX_LEADERBOARD_ENTRIES) {
            LeaderboardNode* current = leaderboardHead;
            LeaderboardNode* prev = NULL;

            for (int i = 0; i < MAX_LEADERBOARD_ENTRIES - 1 && current != NULL; i++) {
                prev = current;
                current = current->next;
            }

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

    SaveLeaderboardToFile();
}

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

void FormatDuration(int seconds, char* buffer, int bufferSize) {
    int minutes = seconds / 60;
    int secs = seconds % 60;
    snprintf(buffer, bufferSize, "%02d:%02d", minutes, secs);
}

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

void CleanupLeaderboard() {
    FreeLeaderboardList();
}

void InitUsernameInput(UsernameInput* input) {
    memset(input->inputText, 0, MAX_USERNAME_LENGTH);
    input->currentLength = 0;
    input->isActive = true;
    input->isComplete = false;
}

void UpdateUsernameInput(UsernameInput* input) {
    if (!input->isActive) return;
    
    int key = GetCharPressed();
    
    while (key > 0) {
        if ((key >= 32) && (key <= 125) && (input->currentLength < MAX_USERNAME_LENGTH - 1)) {
            input->inputText[input->currentLength] = (char)key;
            input->inputText[input->currentLength + 1] = '\0';
            input->currentLength++;
        }
        key = GetCharPressed();
    }
    
    if (IsKeyPressed(KEY_BACKSPACE) && input->currentLength > 0) {
        input->currentLength--;
        input->inputText[input->currentLength] = '\0';
    }
    
    if (IsKeyPressed(KEY_ENTER) && input->currentLength > 0) {
        input->isComplete = true;
        input->isActive = false;
    }
}

void ReturnToMainMenu() {
    isInGameOverInput = false;
    leaderboardSaved = false;
    shouldReturnToMenu = true;
}