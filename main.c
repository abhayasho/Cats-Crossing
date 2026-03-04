#include "gba.h"
#include "print.h"
#include "analogSound.h"
#include "game.h"
#include "font.h"
#include "mode4.h"
#include "images.h"

unsigned short buttons;

unsigned short oldButtons;

typedef enum { START, GAME, PAUSE, SCOREBOARD, WIN, LOSE } State;

static State state; 
static State prevState;
static int startAnimTick;

static void goToStart(void);
static void goToGame(void);

static void goToPause(void);
static void goToScoreboard(void);

static void goToWin(void);
static void goToLose(void);

int main(void) {
    mgba_open();

    REG_DISPCTL = MODE(4) | BG_ENABLE(2);
    loadPalette();

    buttons = REG_BUTTONS;
    oldButtons = buttons;

    initSound();

    goToStart();

    while (1) {
        waitForVBlank();

        switch (state) {
        case START:
            drawTitleFrame((startAnimTick / 12) % 3);
            drawString4(78, 55, "CAT'S CROSSING", IDX_WHITE);
            drawString4(87, 68, "PRESS START", IDX_WHITE);
            if (BUTTON_PRESSED(BUTTON_START)) goToGame();
            if (BUTTON_PRESSED(BUTTON_SELECT)) goToScoreboard();
            startAnimTick++;
            break;

        case GAME:
            updateGame();
            drawGame();
            if (BUTTON_PRESSED(BUTTON_START)) goToPause();
            if (getLives() <= 0) goToLose();
            break;

        case PAUSE:
            fillScreen4(IDX_GRAY);
            drawString4(102, 76, "PAUSED", IDX_WHITE);
            if (BUTTON_PRESSED(BUTTON_START)) goToGame();
            if (BUTTON_PRESSED(BUTTON_SELECT)) goToScoreboard();
            break;

        case SCOREBOARD:
            fillScreen4(IDX_BLUE);
            drawString4(72, 40, "SCOREBOARD", IDX_WHITE);
            drawString4(66, 64, "HIGH SCORE:", IDX_WHITE);
            drawNumber4(144, 64, getHighScore(), IDX_WHITE);
            drawString4(42, 120, "SELECT: BACK", IDX_WHITE);
            if (BUTTON_PRESSED(BUTTON_SELECT)) state = prevState;
            break;

        case WIN:
            fillScreen4(IDX_GREEN);
            drawString4(99, 70, "YOU WIN", IDX_WHITE);
            drawString4(87, 90, "PRESS START", IDX_WHITE);
            if (BUTTON_PRESSED(BUTTON_START)) goToStart();
            break;

        case LOSE:
            fillScreen4(IDX_RED);
            drawString4(96, 40, "GAME OVER", IDX_WHITE);
            drawString4(72, 64, "LEVELS CLEARED", IDX_WHITE);
            drawNumber4(120, 80, getLevelsCleared(), IDX_WHITE);
            drawString4(96, 100, "SCORE", IDX_WHITE);
            drawNumber4(132, 100, getScore(), IDX_WHITE);
            drawString4(72, 120, "PRESS START", IDX_WHITE);
            if (BUTTON_PRESSED(BUTTON_START)) goToStart();
            break;
        }

        flipPages();
        oldButtons = buttons;
        buttons = REG_BUTTONS;
    }
}

static void goToStart(void) {
    state = START;
    startAnimTick = 0;
}

static void goToGame(void) {
    state = GAME;
    initGame();
}

static void goToPause(void) {
    state = PAUSE;
}

static void goToScoreboard(void) {
    prevState = state;
    state = SCOREBOARD;
}

static void goToWin(void) {
    state = WIN;
}

static void goToLose(void) {
    state = LOSE;
}