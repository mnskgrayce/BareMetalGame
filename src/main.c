// ----------------------------------- main.c -------------------------------------
#include "framebf.h"
#include "mbox.h"
#include "oslib.h"
#include "printf.h"
#include "uart.h"

// Caveat: must be an even number
// If using an odd number,
// the middle chicken and the ship will be lined up,
// then if the ship is hit by the middle bullet, UI freezes (?)
#define COLS 6
unsigned int chickenColumns = COLS;

#define NUM_LIVES 3

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
  OBJ_CHICKEN = 1,
  OBJ_SHIP = 2,
  OBJ_BULLET = 3
};

unsigned int numChickens = 0;

struct Object chickens[COLS] = {};
struct Object chickenBullets[COLS] = {};

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

// Scan if the bullet has hit any of the chickens
struct Object *shipHitChicken(struct Object *with, int xoff, int yoff) {
  for (int i = 0; i < numChickens; i++) {
    if (&chickens[i] != with && chickens[i].alive == 1) {
      if (with->x + xoff > chickens[i].x + chickens[i].width || chickens[i].x > with->x + xoff + with->width) {
        // with (Object) is too far left or right to collide
      } else if (with->y + yoff > chickens[i].y + chickens[i].height || chickens[i].y > with->y + yoff + with->height) {
        // with (Object) is too far up or down to collide
      } else {
        // Collision!
        return &chickens[i];
      }
    }
  }
  return 0;
}

// Scan if one chicken bullet has hit the ship
int chickenHitShip(struct Object *with, int xoff, int yoff) {
  if (&ship != with && ship.alive == 1) {
    if (with->x + xoff > ship.x + ship.width || ship.x > with->x + xoff + with->width) {
      // with (Object) is too far left or right to collide
    } else if (with->y + yoff > ship.y + ship.height || ship.y > with->y + yoff + with->height) {
      // with (Object) is too far up or down to collide
    } else {
      // Collision!
      return 1;
    }
  }
  return 0;
}

void initShip() {
  int baseWidth = 60;
  int baseHeight = 20;
  int headWidth = 30;
  int headHeight = 15;
  int wedgeWidth = 10;
  int wedgeHeight = 7;

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

  // Draw wedge
  drawRect((WIDTH - wedgeWidth) / 2,
           (HEIGHT - MARGIN - baseHeight - headHeight - 1),
           (WIDTH - wedgeWidth) / 2 + wedgeWidth,
           (HEIGHT - MARGIN - baseHeight - 1 - (headHeight - wedgeHeight)), 0x00,
           1);

  ship.type = OBJ_SHIP;
  ship.x = (WIDTH - baseWidth) / 2;
  ship.y = (HEIGHT - MARGIN - baseHeight - headHeight - 1);
  ship.width = baseWidth;
  ship.height = baseHeight + headHeight + 1;
  ship.alive = 1;
}

void initBullet() {
  int bulletRadius = 4;

  drawCircle(ship.x + (ship.width / 2), ship.y - (bulletRadius * 2), bulletRadius, 0xee, 1);

  bullet.type = OBJ_BULLET;
  bullet.x = ship.x + (ship.width / 2) - bulletRadius;
  bullet.y = ship.y - (bulletRadius * 3);
  bullet.width = bulletRadius * 2;
  bullet.height = bulletRadius * 2;
  bullet.alive = 1;
}

void initChickens() {
  int baseWidth = 60;
  int baseHeight = 30;
  int headWidth = 25;
  int headHeight = 17;

  static int chickenColors[COLS] = {0xff, 0xaa, 0x77, 0x55, 0x33, 0xee};

  int yChicken = MARGIN + baseHeight;
  int xChicken = MARGIN + (VIRTWIDTH / COLS / 2) - (baseWidth / 2);

  for (int i = 0; i < COLS; i++) {
    // Draw head
    drawRect(xChicken + 10,
             yChicken,
             xChicken + 10 + headWidth,
             yChicken + headHeight,
             chickenColors[i],
             1);

    // Draw comb
    drawRect(xChicken + 10, yChicken, xChicken + 10 + headWidth / 2, yChicken + 5, 0xcc, 1);

    // Draw base
    drawRect(xChicken,
             yChicken + headHeight,
             xChicken + baseWidth,
             yChicken + headHeight + baseHeight,
             chickenColors[i],
             1);

    drawRect(xChicken + 54,
             yChicken + headHeight + 8,
             xChicken + 60,
             yChicken + headHeight + 12,
             0x00,
             1);

    // Draw corner (left)
    drawRect(xChicken,
             yChicken + headHeight + baseHeight - 5,
             xChicken + 5,
             yChicken + headHeight + baseHeight,
             0x00,
             1);

    // Draw corner (right)
    drawRect(xChicken + baseWidth - 5,
             yChicken + headHeight + baseHeight - 5,
             xChicken + baseWidth,
             yChicken + headHeight + baseHeight,
             0x00,
             1);

    // Draw eye
    drawRect(xChicken + 17, yChicken + 10, xChicken + 22, yChicken + 15, 0x00, 1);

    // Draw beak
    drawRect(xChicken + 4, yChicken + 11, xChicken + 12, yChicken + 16, 0x66, 1);

    // Add to chicken array
    chickens[numChickens].type = OBJ_CHICKEN;
    chickens[numChickens].x = xChicken;
    chickens[numChickens].y = yChicken;
    chickens[numChickens].width = baseWidth;
    chickens[numChickens].height = baseHeight + headHeight;
    chickens[numChickens].alive = 1;
    numChickens++;

    // Set cursor to next chicken
    xChicken += (VIRTWIDTH / COLS);
  }
}

// Draw a new chicken bullet (by chicken index)
void initChickenBullet(int i) {
  int bulletRadius = 6;

  drawCircle(chickens[i].x + (chickens[i].width / 2), chickens[i].y + chickens[i].height + (bulletRadius * 3), bulletRadius, 0xcc, 1);

  chickenBullets[i].type = OBJ_BULLET;
  chickenBullets[i].x = chickens[i].x + (chickens[i].width / 2) - bulletRadius;
  chickenBullets[i].y = chickens[i].y + chickens[i].height + (bulletRadius * 2);
  chickenBullets[i].width = bulletRadius * 2;
  chickenBullets[i].height = bulletRadius * 2;
  chickenBullets[i].alive = 1;

  numChickens++;
}

void drawScoreboard(int score, int lives) {
  char tens = score / 10;
  score -= (10 * tens);
  char ones = score;

  drawString((WIDTH / 2) - 300, MARGIN - 10, "Score: 0                      Lives: ", 0x0b, 2);
  drawChar(tens + 0x30, (WIDTH / 2) - 300 + (8 * 8 * 2), MARGIN - 10, 0x0b, 2);
  drawChar(ones + 0x30, (WIDTH / 2) - 300 + (8 * 9 * 2), MARGIN - 10, 0x0b, 2);
  drawChar((char)lives + 0x30, (WIDTH / 2) - 30 + (8 * 20 * 2), MARGIN - 10, 0x0b, 2);
}

void parseShipMovement(char c) {
  // Move ship left
  if (c == 'a' || c == 'A') {
    if (ship.x >= MARGIN + (ship.width / 3)) {
      moveObject(&ship, -(ship.width / 3), 0);
    }
  }

  // Move ship right
  else if (c == 'd' || c == 'D') {
    if (ship.x + ship.width + (ship.width / 3) <= WIDTH - MARGIN) {
      moveObject(&ship, ship.width / 3, 0);
    }
  }

  // Move ship up
  else if (c == 'w' || c == 'W') {
    if (ship.y >= MARGIN + (ship.height / 3)) {
      moveObject(&ship, 0, -(ship.height / 3));
    }
  }

  // Move ship down
  else if (c == 's' || c == 'S') {
    if (ship.y + ship.height + (ship.height / 3) <= HEIGHT - MARGIN) {
      moveObject(&ship, 0, ship.height / 3);
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

void main() {
  uart_init();     // set up serial console
  framebf_init();  // set up frame buffer
  // team_banner();

  unsigned char userChar = 0;  // user input

  int lives = NUM_LIVES;
  int points = 0;

  int velocity_x = 1;
  int velocity_y = 1;

  struct Object *hitChicken;

  initChickens();
  for (int i = 0; i < COLS; i++) {
    initChickenBullet(i);
  }

  initShip();
  initBullet();

  drawScoreboard(points, lives);

  // Wait for keypress
  while (!getUart())
    ;

  while (lives > 0 && chickenColumns > 0) {
    if ((userChar = getUart())) {
      // Read char and move ship if necessary
      parseShipMovement(userChar);
    }

    // Did the ship hit any of the chickens?
    hitChicken = shipHitChicken(&bullet, velocity_x, velocity_y);
    if (hitChicken) {
      if (hitChicken->type == OBJ_CHICKEN) {
        removeObject(hitChicken);
        chickenColumns--;

        points++;
        drawScoreboard(points, lives);
      }
    }

    // Check each chicken to see if it has hit the ship
    for (int i = 0; i < COLS; i++) {
      if (chickenHitShip(&chickenBullets[i], velocity_x, velocity_y)) {
        // Ship is hit...
        lives--;

        removeObject(&chickenBullets[i]);
        removeObject(&bullet);
        removeObject(&ship);

        initShip();
        initBullet();

        drawScoreboard(points, lives);

      } else {
        // Chickens keep shooting down
        moveObject(&chickenBullets[i], 0, velocity_y);

        // Chicken bullet is out of screen, draw a new one
        if (chickenBullets[i].y + chickenBullets[i].height >= HEIGHT - MARGIN) {
          removeObject(&chickenBullets[i]);

          if (chickens[i].alive) {
            initChickenBullet(i);
          }
        }
      }
    }

    // Ship keeps shooting up
    moveObject(&bullet, 0, -velocity_y * 2);

    // Ship bullet is out of screen, draw a new one
    if (bullet.y <= (MARGIN + 10)) {
      removeObject(&bullet);
      initBullet();
    }

    wait_msec(4000);  // Delay...

    // for (int i = 0; i < COLS; i++) {
    //   moveObject(&chickens[i], -velocity_x, 0);
    // }
  }

  int zoom = WIDTH / 192;
  int strwidth = 10 * 8 * zoom;
  int strheight = 8 * zoom;

  if (chickenColumns == 0)
    drawString((WIDTH / 2) - (strwidth / 2), (HEIGHT / 2) - (strheight / 2), "You won!", 0x02, zoom);
  else
    drawString((WIDTH / 2) - (strwidth / 2), (HEIGHT / 2) - (strheight / 2), "Game over!", 0x04, zoom);

  while (1)
    ;
}
