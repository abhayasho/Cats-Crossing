#include "gba.h"
#include "print.h"
#include "analogSound.h"
#include "game.h"
#include "font.h"

unsigned short buttons;

unsigned short oldButtons;

typedef enum { START, GAME, PAUSE, WIN, LOSE } State;

static State state; 

#define FONT_W 6
#define FONT_H 8

static void drawChar(int x, int y, char c, unsigned short color){

    int idx=(unsigned char)c;
    if(idx>127) idx=0;
    const unsigned char *data=&fontdata[idx*FONT_W*FONT_H];
    for (int row=0; row<FONT_H; row++) {

        for(int col=0;col<FONT_W;col++){
            if(data[row*FONT_W+col]){

                setPixel(x+col,y+row,color);

            }
        }
    }
}

static void drawString(int x,int y,const char *s,unsigned short color) {
    while(*s){
        drawChar(x,y,*s++,color);
        x+=FONT_W;
    }
}

static void drawNumber(int x, int y, int n, unsigned short color){

    int d[8],nd=0;
    int t=n;

    if(t==0) d[nd++]=0;
    else while(t&&nd<8){ d[nd++]=t%10; t/=10; }
    for(int i=nd-1;i>=0;i--)
        drawChar(x+(nd-1-i)*FONT_W,y,'0'+d[i],color);

}

static void goToStart(void);
static void goToGame(void);

static void goToPause(void);

static void goToWin(void);
static void goToLose(void);

int main(void){
    mgba_open();

    REG_DISPCTL=MODE(3)|BG_ENABLE(2);

    buttons=REG_BUTTONS;
    oldButtons=buttons;

    initSound();

    goToStart();

    while(1){
        waitForVBlank();

        switch(state){
        case START:
            if(BUTTON_PRESSED(BUTTON_START)) goToGame();
            break;

        case GAME:
            updateGame();
            drawGame();

            if (BUTTON_PRESSED(BUTTON_START)) goToPause();
            if(getLives()<=0) goToLose();
            break;

        case PAUSE:
            if(BUTTON_PRESSED(BUTTON_START)) goToGame();
            break;

        case WIN:
        case LOSE:
            if(BUTTON_PRESSED(BUTTON_START)) goToStart();
            break;
        }

        oldButtons=buttons;
        buttons=REG_BUTTONS;
    }
}

static void goToStart(void){

    state=START;
    fillScreen(GRAY);
    drawRectangle(30,50,180,60,MAGENTA);
    drawString(78,55,"CAT'S CROSSING",WHITE);
    drawString(87,68,"PRESS START",WHITE);

}

static void goToGame(void) {

    state=GAME;
    initGame();
}

static void goToPause(void){

    state=PAUSE;
    drawRectangle(80,70,80,20,GRAY);
    drawString(102,76,"PAUSED",WHITE);

}

static void goToWin(void) {

    state=WIN;
    fillScreen(GREEN);
    drawString(99,70,"YOU WIN",WHITE);
    drawString(87,90,"PRESS START",WHITE);

}

static void goToLose(void){

    state=LOSE;
    fillScreen(RED);
    drawString(93,50,"GAME OVER",WHITE);
    drawString(72,68,"LEVELS:",WHITE);
    drawNumber(126,68,getLevelsCleared(),WHITE);
    drawString(78,84,"SCORE:",WHITE);
    drawNumber(126,84,getScore(),WHITE);
    drawString(87,102,"PRESS START",WHITE);

}