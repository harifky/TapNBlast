#ifndef UI_H
#define UI_H

#include "global.h"
#include "gameLogic.h"
#include "animation.h"
#include "sound.h"
#include "leaderboard.h"
#include "raylib.h"
#include "leaderboard.h"


// UI functions
void DrawGrids();

//Score
void DrawScorePanel();

// GameOver
void DrawGameOverPanel(UsernameInput* usernameInput, int score, int duration);

// draw block preview
void DrawNextBlocks(int selectedIndex, boolean* blockUsed);

//draw block shadow
void DrawBlockShadow(int cursorX, int cursorY, int blockType);

void drawButton(Texture2D texture, Sound sfx);

//draw menu
void drawMenu();

// Menu state
typedef enum {
    MENU_MAIN,
    MENU_SETTINGS,
    MENU_ABOUT,
    MENU_LEADER
} MenuState;

typedef struct {
    Rectangle rect;
    char text[32];
    Color color;
    Color hoverColor;
    Color textColor;
    bool isHovered;
    bool isPressed;
    float animScale;
} MenuButton;

// Menu variables
static MenuState currentMenuState = MENU_MAIN;
static float menuAnimTime = 0.0f;
static float titleBounce = 0.0f;
static float backgroundOffset = 0.0f;

// Buttons
static MenuButton playButton;
static MenuButton settingsButton;
static MenuButton aboutButton;
static MenuButton leaderboardButton;
static MenuButton exitButton;
static MenuButton backButton;

// Tambahkan tombol untuk pop-up pause menu
static MenuButton resumeButton;
static MenuButton quitToMenuButton;

void InitMainMenu();
void UpdateMenuButton(MenuButton* button);
void DrawMenuBackground();
void DrawGameTitle();
void DrawMainMenu();
void DrawSettingsMenu();
void DrawAboutMenu();
void DrawLeaderboardMenu();
int UpdateMainMenu();
void InitPauseMenuButtons();
int DrawPauseMenu();

boolean isPaused = false;


#endif /* UI_H */