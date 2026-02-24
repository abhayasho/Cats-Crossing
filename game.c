#include "game.h"
#include "analogSound.h"
#include "font.h"

// game constants
#define DARK_GRAY RGB(8, 8, 8)
#define WATER BLUE
#define GOAL_ZONE GREEN
#define CAT_ORANGE RGB(31, 18, 0)
#define CAT_WHITE RGB(28, 28, 28)
#define LOG_LIGHT RGB(28, 16, 4)
#define LOG_DARK RGB(12, 6, 0)

static Cat cat;
static Car cars[MAX_CARS];
static Log logs[MAX_LOGS];
static int score;
static int levelsCleared;
static int numCars,numLogs;
static int cat_prev_x, catPrevY;
static int goalReached;
static int move_tick;
static unsigned int rngSeed;
static int level_just_changed;
static int saved_car_x[MAX_CARS],savedLogX[MAX_LOGS];

static const int START_X = 115;
static const int START_Y = 153;
static const int GOAL_Y_MAX = 10;
static const int ROAD_Y_MIN = 20;
static const int ROAD_Y_MAX = 70;

static int isInWaterLane(int catY){
    int lane=catY/LANE_H;
    return (lane>=8 && lane<=9)||(lane>=11 && lane<=12);
}

static unsigned short getLaneColor( int lane ) {
    if(lane<=0) return GOAL_ZONE;
    if (lane==1||lane==4||lane==7||lane==10||lane==13) return BLACK;
    if(lane == 2 || lane == 5) return GRAY;
    if (lane==3||lane==6) return DARK_GRAY;
    if((lane>=8&&lane<=9)||(lane>=11&&lane<=12)) return WATER;
    return BLACK;
}

static void drawBackgroundRect(int x,int y,int w,int h){
    int py=y;
    while(py<y+h){
        int lane=py/LANE_H;
        if(lane<0) lane=0;
        if (lane>=NUM_LANES) lane=NUM_LANES-1;
        unsigned short c=getLaneColor(lane);
        int py2=py+1;
        while (py2<y+h && py2/LANE_H==lane) py2++;
        int hh=py2-py;
        drawRectangle(x,py,w,hh,c);
        py=py2;
    }
}

static void eraseRect(int x, int y, int w, int h) {
    int ex=x,ey=y,ew=w,eh=h;
    if(ex<0){ ew+=ex; ex=0; }
    if(ey<0){ eh+=ey; ey=0; }
    if (ex+ew>SCREENWIDTH) ew=SCREENWIDTH-ex;
    if(ey+eh>SCREENHEIGHT) eh=SCREENHEIGHT-ey;
    if(ew>0 && eh>0) drawBackgroundRect(ex,ey,ew,eh);
}

static void drawFullBackground(void){
    for(int i=0;i<NUM_LANES;i++){
        drawRectangle(0,i*LANE_H,SCREENWIDTH,LANE_H,getLaneColor(i));
    }
}

static void drawChar(int x, int y, char c, unsigned short color){
    int idx=(unsigned char)c;
    if(idx>127) idx=0;
    const unsigned char *data=&fontdata[idx*FONT_W*FONT_H];
    for (int row=0; row<FONT_H; row++) {
        for(int col=0;col<FONT_W;col++){
            if(data[row*FONT_W+col])
                setPixel(x+col,y+row,color);
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

static void drawLog(int x,int y,int w,int h){
    drawRectangle(x,y,w,h,BROWN);
    drawRectangle(x,y,w,1,LOG_LIGHT);
    drawRectangle(x,y+h-1,w,1,LOG_DARK);
}

static void drawCat(int x, int y) {
    int w=CAT_W,h=CAT_H;
    drawRectangle(x + 2, y + 4, w - 4, h - 4, CAT_ORANGE);
    drawRectangle(x + 1, y + 2, w - 2, 4, CAT_ORANGE);
    drawRectangle(x, y, 4, 4, CAT_ORANGE);
    drawRectangle(x + w - 4, y, 4, 4, CAT_ORANGE);
    drawRectangle(x + 2, y + 3, 2, 2, BLACK);
    drawRectangle(x + 6, y + 3, 2, 2, BLACK);
    drawRectangle(x + 3, y + 5, 2, 2, CAT_WHITE);
}

void initGame(void){
    score=0;
    levelsCleared=0;
    goalReached=0;
    level_just_changed=0;
    rngSeed=12345u;
    cat.x=START_X;
    cat.y=START_Y;
    cat.w=CAT_W;
    cat.h=CAT_H;
    cat.lives=3;
    cat_prev_x=cat.x;
    catPrevY=cat.y;

    numCars=8;
    cars[0].x=0;   cars[0].y=22; cars[0].w=CAR_W; cars[0].h=CAR_H; cars[0].vx=1;
    cars[1].x=90;  cars[1].y=22; cars[1].w=CAR_W; cars[1].h=CAR_H; cars[1].vx=1;
    cars[2].x=200; cars[2].y=32; cars[2].w=CAR_W; cars[2].h=CAR_H; cars[2].vx=-1;
    cars[3].x=80;  cars[3].y=32; cars[3].w=CAR_W; cars[3].h=CAR_H; cars[3].vx=-1;
    cars[4].x=0;   cars[4].y=52; cars[4].w=CAR_W; cars[4].h=CAR_H; cars[4].vx=1;
    cars[5].x=100; cars[5].y=52; cars[5].w=CAR_W; cars[5].h=CAR_H; cars[5].vx=1;
    cars[6].x=220; cars[6].y=62; cars[6].w=CAR_W; cars[6].h=CAR_H; cars[6].vx=-1;
    cars[7].x=120; cars[7].y=62; cars[7].w=CAR_W; cars[7].h=CAR_H; cars[7].vx=-1;
    for(int i=0;i<numCars;i++) cars[i].prevX=cars[i].x;

    numLogs=8;
    logs[0].x=0;   logs[0].y=82;  logs[0].w=LOG_W; logs[0].h=LOG_H; logs[0].vx=1;
    logs[1].x=90;  logs[1].y=82;  logs[1].w=LOG_W; logs[1].h=LOG_H; logs[1].vx=1;
    logs[2].x=200; logs[2].y=92;  logs[2].w=LOG_W; logs[2].h=LOG_H; logs[2].vx=-1;
    logs[3].x=60;  logs[3].y=92;  logs[3].w=LOG_W; logs[3].h=LOG_H; logs[3].vx=-1;
    logs[4].x=0;   logs[4].y=112; logs[4].w=LOG_W; logs[4].h=LOG_H; logs[4].vx=1;
    logs[5].x=100; logs[5].y=112; logs[5].w=LOG_W; logs[5].h=LOG_H; logs[5].vx=1;
    logs[6].x=220; logs[6].y=122; logs[6].w=LOG_W; logs[6].h=LOG_H; logs[6].vx=-1;
    logs[7].x=80;  logs[7].y=122; logs[7].w=LOG_W; logs[7].h=LOG_H; logs[7].vx=-1;
    for (int i=0; i<numLogs; i++) logs[i].prevX=logs[i].x;

    move_tick=-1;
    fillScreen(BLACK);
    drawFullBackground();
    drawRectangle(0, 0, SCREENWIDTH, LANE_H, GOAL_ZONE);
    drawString(72, 1, "THE FINISH", WHITE);
}

#define MOVE_EVERY_N 2

static void updateCars(void){  // move cars each tick
    if(move_tick%MOVE_EVERY_N!=0) return;
    for(int i=0;i<numCars;i++){
        cars[i].x+=cars[i].vx;
        if (cars[i].vx>0 && cars[i].x>SCREENWIDTH) cars[i].x=-cars[i].w;
        if(cars[i].vx<0 && cars[i].x<-cars[i].w) cars[i].x=SCREENWIDTH;
    }
}

static void updateLogs(void) {
    if(move_tick%MOVE_EVERY_N!=0) return;
    for (int i=0; i<numLogs; i++) {
        logs[i].x+=logs[i].vx;
        if(logs[i].vx>0&&logs[i].x>SCREENWIDTH) logs[i].x=-logs[i].w;
        if (logs[i].vx<0&&logs[i].x<-logs[i].w) logs[i].x=SCREENWIDTH;
    }
}

/* returns log index if cat is on one else -1 */
static int catOnLog(void){
    for(int i=0;i<numLogs;i++){
        if(collision(cat.x,cat.y,cat.w,cat.h,logs[i].x,logs[i].y,logs[i].w,logs[i].h))
            return i;
    }
    return -1;
}

static int catHitCar(void) {
    for (int i=0; i<numCars; i++) {
        if (collision(cat.x,cat.y,cat.w,cat.h,cars[i].x,cars[i].y,cars[i].w,cars[i].h))
            return 1;
    }
    return 0;
}

static void resetCat(void){
    cat.x=START_X;
    cat.y=START_Y;
}

static int randNext(void){
    rngSeed=rngSeed*1103515245u+12345u;
    return (int)((rngSeed>>16)&0x7FFFu);
}

static void randomizeMap(void){
    rngSeed=12345u+(unsigned int)levelsCleared*1000u;
    for(int i=0;i<numCars;i++){
        cars[i].x=randNext()%(SCREENWIDTH+40)-20;
        cars[i].vx=(randNext()&1)?1:-1;
        if(cars[i].vx>0&&cars[i].x>SCREENWIDTH) cars[i].x=-cars[i].w;
        if (cars[i].vx<0&&cars[i].x<-cars[i].w) cars[i].x=SCREENWIDTH;
        cars[i].prevX=cars[i].x;
    }
    for (int lane=0; lane<4; lane++) {
        int i0=lane*2,i1=lane*2+1;
        int vx=(randNext()&1)?1:-1;
        logs[i0].vx=vx;
        logs[i1].vx=vx;
        if(vx>0){
            logs[i0].x=randNext()%60;
            logs[i1].x=100+randNext()%80;
        }else{
            logs[i1].x=randNext()%60;
            logs[i0].x=100+randNext()%80;
        }
        if(logs[i0].x+LOG_W>SCREENWIDTH) logs[i0].x=SCREENWIDTH-LOG_W;
        if (logs[i1].x+LOG_W>SCREENWIDTH) logs[i1].x=SCREENWIDTH-LOG_W;
        logs[i0].prevX=logs[i0].x;
        logs[i1].prevX=logs[i1].x;
    }
}

void updateGame(void){
    move_tick++;

    if(BUTTON_PRESSED(BUTTON_UP)&&cat.y>=STEP) cat.y-=STEP;
    if (BUTTON_PRESSED(BUTTON_DOWN) && cat.y<=START_Y-STEP) cat.y+=STEP;
    if(BUTTON_PRESSED(BUTTON_LEFT)&&cat.x>=STEP) cat.x-=STEP;
    if (BUTTON_PRESSED(BUTTON_RIGHT) && cat.x<=SCREENWIDTH-cat.w-STEP) cat.x+=STEP;
    if(cat.x<0) cat.x=0;
    if (cat.x>SCREENWIDTH-cat.w) cat.x=SCREENWIDTH-cat.w;

    if(isInWaterLane(cat.y)){
        int on_log=catOnLog();
        if(on_log>=0){
            if (move_tick%MOVE_EVERY_N==0) {
                cat.x+=logs[on_log].vx;
                int logNewX=logs[on_log].x+logs[on_log].vx;
                int logLeft=logNewX;
                int logRight=logNewX+logs[on_log].w-cat.w;
                if(cat.x<logLeft) cat.x=logLeft;
                if (cat.x>logRight) cat.x=logRight;
            }
            if(cat.x<-cat.w||cat.x>SCREENWIDTH){
                cat.lives--;
                playAnalogSound(6);
                resetCat();
            }
        }else{
            cat.lives--;
            playAnalogSound(6);
            resetCat();
        }
    }

    updateCars();
    updateLogs();

    if (cat.y>=ROAD_Y_MIN && cat.y<=ROAD_Y_MAX){
        if(catHitCar()){
            cat.lives--;
            playAnalogSound(6);
            resetCat();
        }
    }

    if(cat.y<=GOAL_Y_MAX && !goalReached){
        goalReached=1;
        levelsCleared++;
        score+=POINTS_PER_GOAL;
        playAnalogSound(16);
        resetCat();
        for(int i=0;i<numCars;i++) saved_car_x[i]=cars[i].x;
        for (int i=0; i<numLogs; i++) savedLogX[i]=logs[i].x;
        randomizeMap();
        level_just_changed=1;
        goalReached=0;
    }
}

static void drawHUD(void){
    drawRectangle(0,0,SCREENWIDTH,LANE_H,BLACK);
    drawRectangle(30,0,180,LANE_H,GOAL_ZONE);
    drawString(72,1,"THE FINISH",WHITE);
    drawString(2,1,"SCORE:",WHITE);
    drawNumber(38,1,score,WHITE);
    for(int i=0;i<cat.lives;i++)
        drawRectangle(SCREENWIDTH-24+i*8,2,6,6,YELLOW);
}

void drawGame(void){
    drawHUD();
    if(level_just_changed){
        for(int i=0;i<numCars;i++)
            eraseRect(saved_car_x[i],cars[i].y,cars[i].w,cars[i].h);
        for (int i=0; i<numLogs; i++)
            eraseRect(savedLogX[i],logs[i].y,logs[i].w,logs[i].h);
        level_just_changed=0;
    }
    eraseRect(cat_prev_x,catPrevY,cat.w,cat.h);
    for(int i=0;i<numCars;i++){
        if (cars[i].prevX!=cars[i].x)
            eraseRect(cars[i].prevX,cars[i].y,cars[i].w,cars[i].h);
    }
    for (int i=0; i<numLogs; i++) {
        if(logs[i].prevX!=logs[i].x)
            eraseRect(logs[i].prevX,logs[i].y,logs[i].w,logs[i].h);
    }

    for(int i=0;i<numCars;i++){
        int cx=cars[i].x,cw=cars[i].w;
        if(cx+cw>0 && cx<SCREENWIDTH){
            int dx=cx<0?0:cx;
            int dw=(cx+cw>SCREENWIDTH?SCREENWIDTH:cx+cw)-dx;
            drawRectangle(dx,cars[i].y,dw,cars[i].h,RED);
        }
        cars[i].prevX=cars[i].x;
    }
    for (int i=0; i<numLogs; i++) {
        int lx=logs[i].x,lw=logs[i].w;
        if(lx+lw>0&&lx<SCREENWIDTH){
            int dx=lx<0?0:lx;
            int dw=(lx+lw>SCREENWIDTH?SCREENWIDTH:lx+lw)-dx;
            drawLog(dx,logs[i].y,dw,logs[i].h);
        }
        logs[i].prevX=logs[i].x;
    }
    drawCat(cat.x,cat.y);
    cat_prev_x=cat.x;
    catPrevY=cat.y;
    drawHUD();
}

int getScore(void){ return score; }
int getLives(void){ return cat.lives; }
int getLevelsCleared(void){ return levelsCleared; }
