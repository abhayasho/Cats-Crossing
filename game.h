#ifndef GAME_H
#define GAME_H

#include "gba.h"

#define LANE_H  10
#define NUM_LANES 16
#define CAT_W 10
#define CAT_H  10

#define CAR_W  18
#define CAR_H 6

#define LOG_W 26
#define LOG_H  6

#define MAX_CARS 8
#define MAX_LOGS  8
#define STEP 10
#define POINTS_PER_GOAL 100
#define FONT_W  6
#define FONT_H 8

#define IDX_BLACK 0
#define IDX_WHITE 1
#define IDX_RED 2
#define IDX_GREEN 3
#define IDX_GRAY 4
#define IDX_DARK_GRAY 5
#define IDX_BLUE 6
#define IDX_MAGENTA 7
#define IDX_YELLOW 8
#define IDX_BROWN 9
#define IDX_CAT_ORANGE 10
#define IDX_CAT_WHITE 11
#define IDX_LOG_LIGHT 12
#define IDX_LOG_DARK 13

void loadPalette(void);

typedef struct {
    int x,y,w,h;
    int lives;
    int frame;
} Cat;

typedef struct{
    
    int x, y, w, h;
    int vx;
    int prevX;
} Car;

typedef struct {

    int x,y,w,h;
    int vx;
    int prevX;
} Log;

void initGame(void);
void updateGame(void);
void drawGame(void);

int getScore(void);
int getLives(void);
int getLevelsCleared(void);
int getHighScore(void);

#endif
