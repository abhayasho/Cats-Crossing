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

typedef struct {
    int x,y,w,h;
    int lives;
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

#endif
