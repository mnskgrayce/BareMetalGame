// ----------------------------------- main.c -------------------------------------
#include "main.h"

#include "framebf.h"
#include "mbox.h"
#include "oslib.h"
#include "printf.h"
#include "uart.h"

struct Object {
  unsigned int x;
  unsigned int y;
  unsigned int width;
  unsigned int height;
  unsigned char alive;
};

struct Object ship = {};
struct Object bullet = {};

void removeObject(struct Object *object) {
  drawRect(object->x, object->y, object->x + object->width, object->y + object->height, 0, 1);
  object->alive = 0;
}

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

  ship.x = (WIDTH - baseWidth) / 2;
  ship.y = (HEIGHT - MARGIN - baseHeight - headHeight - 1);
  ship.width = baseWidth;
  ship.height = baseHeight + headHeight + 1;
  ship.alive = 1;
}

void initBullet() {
  // int bulletRadius = 4;

  // drawCircle(ship->x + (ship->width) / 2, ship->y - (bulletRadius * 2), bulletRadius, 0xee, 1);

  // bullet->x = (WIDTH / 2) - bulletRadius;
  // bullet->y = (HEIGHT - MARGIN - ship->height - (bulletRadius * 2));
  // bullet->width = bulletRadius * 2;
  // bullet->height = bulletRadius * 2;
  // bullet->alive = 1;

  int ballradius = 5;

  drawCircle(WIDTH / 2, HEIGHT / 2, ballradius, 0xee, 1);

  bullet.x = (WIDTH / 2) - ballradius;
  bullet.y = (HEIGHT / 2) - ballradius;
  bullet.width = ballradius * 2;
  bullet.height = ballradius * 2;
  bullet.alive = 1;
}

void parseShipMovement(char c) {
  // Move ship left
  if (c == 'a') {
    if (ship.x >= MARGIN + (ship.width / 3)) {
      moveObject(&ship, -(ship.width / 3), 0);
    }
  }

  // Move ship right
  else if (c == 'd') {
    if (ship.x + ship.width + (ship.width / 3) <= WIDTH - MARGIN) {
      moveObject(&ship, ship.width / 3, 0);
    }
  }

  // Move ship up
  else if (c == 'w') {
    if (ship.y >= MARGIN + (ship.height / 3)) {
      moveObject(&ship, 0, -(ship.height / 3));
    }
  }

  // Move ship down
  else if (c == 's') {
    if (ship.y + ship.height + (ship.height / 3) <= HEIGHT - MARGIN) {
      moveObject(&ship, 0, ship.height / 3);
    }
  }
}

void main() {
  uart_init();     // set up serial console
  framebf_init();  // set up frame buffer

  team_banner();
  initShip();
  initBullet();

  int velocity_x = 0;
  int velocity_y = 1;
  unsigned char c = 0;  // user input
  int endgame = 0;

  // Wait for keypress
  while (!getUart())
    ;

  // Later, refactor this to endgame condition
  while (!endgame) {
    if ((c = getUart())) {
      // Read c and move ship if necessary
      parseShipMovement(c);
    }

    wait_msec(4000);  // wait a bit
    moveObject(&bullet, velocity_x, -velocity_y);

    // Check we're in the game arena still
    if (bullet.x + bullet.width >= WIDTH - MARGIN) {
      velocity_x = -velocity_x;
    } else if (bullet.x <= MARGIN) {
      velocity_x = -velocity_x;
    } else if (bullet.y + bullet.height >= HEIGHT - MARGIN) {
      // In the paddle game, it's time to decrease lives

      removeObject(&bullet);
      removeObject(&ship);

      initBullet();
      initShip();
    } else if (bullet.y <= MARGIN) {  // Bullet hit the ceiling
      velocity_y = -velocity_y;
    }
  }

  while (1)
    ;
}

void team_banner() {
  drawRect(25, 25, 525, 125, 0xb0, 1);                          // cyan fill black border
  drawRect(20, 20, 520, 120, 0xf0, 1);                          // white fill black border
  drawString(40, 40, "Nguyen Minh Trang - s3751450", 0xf9, 2);  // indigo text white bg
  drawString(40, 60, "Tran Kim Long - s3755614", 0xfc, 2);      // red text white bg
  drawString(40, 80, "Truong Cong Anh - s3750788", 0xf2, 2);    // green text white bg
}