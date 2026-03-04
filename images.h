#ifndef IMAGES_H
#define IMAGES_H

#include "gba.h"

#define CAT_IMAGE_W 10
#define CAT_IMAGE_H 10
#define CAT_IMAGE_SIZE (CAT_IMAGE_W * CAT_IMAGE_H / 2)

void drawTitleFrame(int frame);
const u16* getCatImage(int frame);

#endif
