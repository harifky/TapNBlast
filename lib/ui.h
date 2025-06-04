#ifndef UI_H
#define UI_H

#include "global.h"
#include "gameLogic.h"
#include "animation.h"
#include "raylib.h"



// UI functions
void DrawGrids();

//Score
void DrawScorePanel();

// GameOver
void DrawGameOverPanel();

// draw block preview
void DrawNextBlocks(int selectedIndex, boolean* blockUsed);

//draw block shadow
void DrawBlockShadow(int cursorX, int cursorY, int blockType);

//draw button
void drawButton(Texture2D texture, Sound sfx);

//draw menu
void drawMenu();

// Menu state
typedef enum {
    MENU_MAIN,
    MENU_SETTINGS,
    MENU_ABOUT
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
    Sound clickSound;
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
static MenuButton exitButton;
static MenuButton backButton;

void InitMainMenu();
void UpdateMenuButton(MenuButton* button, Sound clickSound);
void DrawMenuBackground();
void DrawGameTitle();
void DrawMainMenu(Sound clickSound);
void DrawSettingsMenu(Sound clickSound);
void DrawAboutMenu(Sound clickSound);
int UpdateMainMenu(Sound clickSound);

static MenuButton undoButton;
static MenuButton redoButton;

void DrawUndoRedoButtons(Sound clickSound);

void InitURUI();

#endif /* UI_H */