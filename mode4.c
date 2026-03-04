#include "mode4.h"
#include "font.h"
#include "gba.h"

static u8* drawBuffer = BACKBUFFER;

void setPixel4(int x, int y, u8 colorIndex) {
    if (x >= 0 && x < SCREENWIDTH && y >= 0 && y < SCREENHEIGHT) {
        drawBuffer[OFFSET(x, y, SCREENWIDTH)] = colorIndex;
    }
}

void drawRect4(int x, int y, int width, int height, volatile u8 colorIndex) {
    if (x + width <= 0 || x >= SCREENWIDTH || y + height <= 0 || y >= SCREENHEIGHT) return;
    int x0 = x < 0 ? 0 : x;
    int y0 = y < 0 ? 0 : y;
    int x1 = x + width > SCREENWIDTH ? SCREENWIDTH : x + width;
    int y1 = y + height > SCREENHEIGHT ? SCREENHEIGHT : y + height;
    int w = x1 - x0;
    if (w <= 0) return;
    u16 pair = (u8)colorIndex | ((u8)colorIndex << 8);
    int pairs = w / 2;
    int hasOdd = w & 1;
    for (int py = y0; py < y1; py++) {
        int offset = OFFSET(x0, py, SCREENWIDTH);
        if (pairs > 0) {
            DMANow(3,
                   &pair,
                   (volatile void*)&((u16*)drawBuffer)[offset / 2],
                   pairs | DMA_SOURCE_FIXED | DMA_DESTINATION_INCREMENT | DMA_16);
        }
        if (hasOdd) {
            setPixel4(x0 + pairs * 2, py, colorIndex);
        }
    }
}

void fillScreen4(volatile u8 colorIndex) {
    u16 pair = (u8)colorIndex | ((u8)colorIndex << 8);
    DMANow(3,
           (volatile void*)&pair,
           (volatile void*)drawBuffer,
           (SCREENWIDTH * SCREENHEIGHT / 2) | DMA_SOURCE_FIXED | DMA_DESTINATION_INCREMENT | DMA_16);
}

void drawImage4(int x, int y, int width, int height, const u16* image) {
    if (width <= 0 || height <= 0 || (width & 1) != 0) return;
    for (int r = 0; r < height; r++) {
        int sy = y + r;
        if (sy < 0 || sy >= SCREENHEIGHT) continue;
        int byteOffset = OFFSET(x, sy, SCREENWIDTH);
        if (byteOffset < 0 || byteOffset + width > SCREENWIDTH * SCREENHEIGHT) continue;
        DMANow(3,
               (volatile void*)&((const u16*)image)[r * (width / 2)],
               (volatile void*)((u16*)(drawBuffer + byteOffset)),
               (width / 2) | DMA_SOURCE_INCREMENT | DMA_DESTINATION_INCREMENT | DMA_16);
    }
}

void drawFullscreenImage4(const u16* image) {
    DMANow(3,
           (volatile void*)image,
           (volatile void*)drawBuffer,
           (SCREENWIDTH * SCREENHEIGHT / 2) | DMA_SOURCE_INCREMENT | DMA_DESTINATION_INCREMENT | DMA_16);
}

void drawChar4(int x, int y, char ch, u8 colorIndex) {
    int idx = (unsigned char)ch;
    if (idx > 127) idx = 0;
    const unsigned char* data = &fontdata[idx * 6 * 8];
    for (int c = 0; c < 6; c++) {
        for (int r = 0; r < 8; r++) {
            if (data[c + 6 * r]) setPixel4(x + c, y + r, colorIndex);
        }
    }
}

void drawString4(int x, int y, const char* str, u8 colorIndex) {
    while (*str) {
        drawChar4(x, y, *str, colorIndex);
        str++;
        x += 6;
    }
}

void drawNumber4(int x, int y, int n, u8 colorIndex) {
    int d[8], nd = 0;
    int t = n;
    if (t == 0) d[nd++] = 0;
    else while (t && nd < 8) { d[nd++] = t % 10; t /= 10; }
    for (int i = nd - 1; i >= 0; i--)
        drawChar4(x + (nd - 1 - i) * 6, y, '0' + d[i], colorIndex);
}

void flipPages(void) {
    REG_DISPCTL ^= DISP_BACKBUFFER;
    if (REG_DISPCTL & DISP_BACKBUFFER) {
        drawBuffer = FRONTBUFFER;
    } else {
        drawBuffer = BACKBUFFER;
    }
}