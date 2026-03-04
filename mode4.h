#ifndef GBA_MODE4_H
#define GBA_MODE4_H

#include "gba.h"

#define FRONTBUFFER ((u8*) 0x06000000)
#define BACKBUFFER  ((u8*) 0x0600A000)

void flipPages();
void setPixel4(int x, int y, u8 colorIndex);
void drawRect4(int x, int y, int width, int height, volatile u8 colorIndex);
void fillScreen4(volatile u8 colorIndex);
void drawImage4(int x, int y, int width, int height, const u16* image);
void drawFullscreenImage4(const u16* image);

void drawChar4(int x, int y, char ch, u8 colorIndex);
void drawString4(int x, int y, const char* str, u8 colorIndex);
void drawNumber4(int x, int y, int n, u8 colorIndex);

#endif