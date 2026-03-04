#include "gba.h"
#include "font.h"

volatile unsigned short* videoBuffer = (unsigned short *)0x6000000;

void DMANow(int channel, volatile void* src, volatile void* dest, unsigned int ctrl) {
    DMA[channel].ctrl = 0;
    DMA[channel].src = src;
    DMA[channel].dest = dest;
    DMA[channel].ctrl = ctrl | DMA_ON;
}

void drawRectangle(int x, int y, int width, int height, volatile unsigned short color) {
    for (int r = 0; r < height; r++) {
        DMANow(3,
               (volatile void*)&color,
               (volatile void*)&videoBuffer[OFFSET(x, y + r, SCREENWIDTH)],
               width | DMA_SOURCE_FIXED | DMA_DESTINATION_INCREMENT | DMA_16);
    }
}

void fillScreen(volatile unsigned short color) {
    DMANow(3,
           (volatile void*)&color,
           (volatile void*)videoBuffer,
           (SCREENWIDTH * SCREENHEIGHT) | DMA_SOURCE_FIXED | DMA_DESTINATION_INCREMENT | DMA_16);
}

void waitForVBlank() {
    while (REG_VCOUNT >= 160);
    while (REG_VCOUNT < 160);
}

int collision(int xA, int yA, int widthA, int heightA,
              int xB, int yB, int widthB, int heightB) {
    return yA <= yB + heightB - 1 && yA + heightA - 1 >= yB &&
           xA <= xB + widthB - 1 && xA + widthA - 1 >= xB;
}