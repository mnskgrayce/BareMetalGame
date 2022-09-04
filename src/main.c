// ----------------------------------- main.c -------------------------------------
#include "main.h"

#include "framebf.h"
#include "mbox.h"
#include "oslib.h"
#include "printf.h"
#include "uart.h"

struct Object {
  unsigned int type;
  unsigned int x;
  unsigned int y;
  unsigned int width;
  unsigned int height;
  unsigned char alive;
};

enum {
  OBJ_NONE = 0,
  OBJ_SHIP = 1,
};

unsigned int numobjs = 0;
struct Object *objects = (struct Object *)SAFE_ADDRESS;
struct Object *ship;

void moveObject(struct Object *object, int xoff, int yoff) {
  moveRect(object->x, object->y, object->width, object->height, xoff, yoff, 0x00);
  object->x = object->x + xoff;
  object->y = object->y + yoff;
}

void initShip() {
  int baseWidth = 60;
  int baseHeight = 20;
  int headWidth = 30;
  int headHeight = 15;

  // Draw base
  drawRect((WIDTH - baseWidth) / 2,
           (HEIGHT - MARGIN - baseHeight),
           (WIDTH - baseWidth) / 2 + baseWidth,
           (HEIGHT - MARGIN), 0x99,
           1);

  // Draw head
  drawRect((WIDTH - headWidth) / 2,
           (HEIGHT - MARGIN - baseHeight - headHeight - 1),
           (WIDTH - headWidth) / 2 + headWidth,
           (HEIGHT - MARGIN - baseHeight - 1), 0xbb,
           1);

  objects[numobjs].type = OBJ_SHIP;
  objects[numobjs].x = (WIDTH - baseWidth) / 2;
  objects[numobjs].y = (HEIGHT - MARGIN - baseHeight - headHeight - 1);
  objects[numobjs].width = baseWidth;
  objects[numobjs].height = baseHeight + headHeight + 1;
  objects[numobjs].alive = 1;
  ship = &objects[numobjs];
  numobjs++;
}

void main() {
  uart_init();     // set up serial console
  framebf_init();  // set up frame buffer

  team_banner();
  initShip();

  // drawCircle(960, 540, 250, 0x0e, 0);
  // drawCircle(960, 540, 50, 0x13, 1);
  // drawLine(100, 500, 350, 700, 0x0c);

  while (1) {
    char c = uart_getc();

    // Move ship left
    if (c == 'a') {
      if (ship->x >= MARGIN + (ship->width / 2)) {
        moveObject(ship, -(ship->width / 2), 0);
      }
    }

    // Move ship right
    if (c == 'd') {
      if (ship->x + ship->width + (ship->width / 2) <= WIDTH - MARGIN) {
        moveObject(ship, ship->width / 2, 0);
      }
    }

    // Move ship up
    if (c == 'w') {
      if (ship->y >= MARGIN + (ship->height / 2)) {
        moveObject(ship, 0, -(ship->height / 2));
      }
    }

    // Move ship down
    if (c == 's') {
      if (ship->y + ship->height + (ship->height / 2) <= HEIGHT - MARGIN) {
        moveObject(ship, 0, ship->height / 2);
      }
    }
  }
}

void team_banner() {
  drawRect(25, 25, 525, 125, 0xb0, 1);                          // cyan fill black border
  drawRect(20, 20, 520, 120, 0xf0, 1);                          // white fill black border
  drawString(40, 40, "Nguyen Minh Trang - s3751450", 0xf9, 2);  // indigo text white bg
  drawString(40, 60, "Tran Kim Long - s3755614", 0xfc, 2);      // red text white bg
  drawString(40, 80, "Truong Cong Anh - s3750788", 0xf2, 2);    // green text white bg
}