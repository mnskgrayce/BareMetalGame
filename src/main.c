// ----------------------------------- main.c -------------------------------------
#include "main.h"

#include "framebf.h"
#include "mbox.h"
#include "uart.h"

#define COLS 6
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

enum {
  GAME_MENU = 0,
  GAME_LEVEL_ONE = 1,
  GAME_LEVEL_TWO = 2
};

int state = GAME_LEVEL_ONE;

// Caveat: must be an even number
// If using an odd number,
// the middle chicken and the ship will be lined up,
// then if the ship is hit by the middle bullet, UI freezes (?)
unsigned int chickenColumns = COLS;
unsigned int bigChickenHealth = COLS;
unsigned int numChickens = 0;
int chickenDirection = -1;

int lives = 3;
int points = 0;
int velocity_x = 1;
int velocity_y = 1;

Object ship = {};
Object bullet = {};

// Level One enemies
Object chickens[COLS] = {};
Object chickenBullets[COLS] = {};
Object* hitChicken;

// Level Two enemy
Object bigChicken = {};
Object bigChickenBullets[3] = {};

// UI variables to display endgame messages
int zoom = 1;
int strwidth = 0;
int strheight = 8;

unsigned char userChar;  // user input

void main() {
  uart_init();     // set up serial console
  framebf_init();  // set up frame buffer

  // Enter game loop
  while (1) {
    switch (state) {
      case GAME_MENU:
        gameMenu();
        break;

      case GAME_LEVEL_ONE:
        levelOne();
        break;

      case GAME_LEVEL_TWO:
        levelTwo();
        break;

      default:
        break;
    }
  }
}

void gameMenu() {}

void resetGame() {
  userChar = 0;

  // Reset all values
  chickenColumns = COLS;
  bigChickenHealth = COLS;
  chickenDirection = -1;

  hitChicken = 0;

  velocity_x = 1;
  velocity_y = 1;

  // Keep lives and points if going to level two
  if (state != GAME_LEVEL_TWO) {
    lives = NUM_LIVES;
    points = 0;
  }

  // Update UI
  drawStars();
  drawScoreboard(points, lives);
}

void levelOne() {
  // Reset all values and UI
  resetGame();

  // Initialize game entities
  initChickens();
  for (int i = 0; i < COLS; i++) {
    initChickenBullet(i);
  }
  initShip();
  initBullet();

  waitForKeyPress();

  // Start shooting!
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
        points += 10;
        drawScoreboard(points, lives);
      }
    }

    // Check each chicken to see if it has hit the ship
    for (int i = 0; i < COLS; i++) {
      if (chickenHitShip(&chickenBullets[i], velocity_x, velocity_y)) {
        // Ship is hit...
        lives--;

        // Ceasefire!
        for (int i = 0; i < COLS; i++) {
          removeObject(&chickenBullets[i]);
          if (chickens[i].alive) {
            initChickenBullet(i);
          }
        }

        // Re-initialize ship
        removeObject(&bullet);
        removeObject(&ship);
        wait_msec(500);  // Delay...
        initShip();
        initBullet();

        // Update scores
        drawScoreboard(points, lives);
      } else {
        // Chickens keep shooting down
        moveObject(&chickenBullets[i], 0, velocity_y * 2);

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
    moveObject(&bullet, 0, -velocity_y * 3);

    // Ship bullet is out of screen, draw a new one
    if (bullet.y <= (MARGIN + 70)) {
      removeObject(&bullet);
      initBullet();
    }

    // Change direction if chickens are moving out of bound
    if (chickens[0].x < (MARGIN) ||
        chickens[COLS - 1].x > (WIDTH - MARGIN - 60)) {
      chickenDirection *= -1;
    }

    // Move chickens left and right
    for (int i = 0; i < COLS; i++) {
      moveObject(&chickens[i], chickenDirection * velocity_x, 0);
      wait_msec(1800);  // Delay...
    }

    wait_msec(2200);  // Delay...
  }

  // Clear screen
  for (int i = 0; i < COLS; i++) {
    removeObject(&chickens[i]);
    removeObject(&chickenBullets[i]);
  }
  removeObject(&bullet);
  removeObject(&ship);

  // Display endgame messages
  wait_msec(500);  // Delay...
  if (chickenColumns == 0) {
    zoom = WIDTH / 192;
    strwidth = 8 * 8 * zoom;
    strheight = 8 * zoom;
    drawString((WIDTH / 2) - (strwidth / 2), (HEIGHT / 2) - (strheight / 2), "You won!", 0x02, zoom);
  } else {
    zoom = WIDTH / 192;
    strwidth = 9 * 8 * zoom;
    strheight = 8 * zoom;
    drawString((WIDTH / 2) - (strwidth / 2), (HEIGHT / 2) - (strheight / 2), "You lost!", 0x04, zoom);
  }

  // Player has won
  if (lives > 0 && chickenColumns == 0) {
    zoom = 2;
    strwidth = 25 * 8 * zoom;
    drawString((WIDTH / 2) - (strwidth / 2), (HEIGHT / 2) + 35, "Press <N> to go Level Two", 0x0b, zoom);
  } else {
    // Display replay message
    zoom = 2;
    strwidth = 19 * 8 * zoom;
    drawString((WIDTH / 2) - (strwidth / 2), (HEIGHT / 2) + 35, "Press <R> to replay", 0x0b, zoom);
  }

  strwidth = 25 * 8 * zoom;
  strheight = 8 * zoom;
  drawString((WIDTH / 2) - (strwidth / 2), (HEIGHT / 2) + 35 + strheight + 5, "or <M> to go back to menu", 0x0b, zoom);

  // Game has ended, wait for keypress
  while (1) {
    if ((userChar = getUart())) {
      if (userChar == 'n' || userChar == 'N') {
        clearGameMessages();  // clear screen
        state = GAME_LEVEL_TWO;
        break;
      } else if (userChar == 'r' || userChar == 'R') {
        clearGameMessages();  // clear screen
        state = GAME_LEVEL_ONE;
        break;
      } else if (userChar == 'm' || userChar == 'M') {
        clearGameMessages();  // clear screen
        state = GAME_MENU;
        break;
      }
    }
  };
}

void levelTwo() {
  // Reset all values and UI
  resetGame();

  // Initialize game entities
  initBigChicken();
  initBigChickenBullets();
  initShip();
  initBullet();

  waitForKeyPress();

  // Play until ship or big chicken runs out of lives
  while (lives > 0 && bigChickenHealth > 0) {
    if ((userChar = getUart())) {
      // Read char and move ship if necessary
      parseShipMovement(userChar);
    }

    // Did the ship hit the big chicken?
    if (shipHitBigChicken(&bullet, velocity_x, velocity_y)) {
      // Take that!
      bigChickenHealth--;
      points += 10;
      drawScoreboard(points, lives);
    }

    // Check each big chicken bullet to see if it has hit the ship

    // Ship keeps shooting up
    moveObject(&bullet, 0, -velocity_y);

    // Ship bullet is out of screen, draw a new one
    if (bullet.y <= (MARGIN + 70)) {
      removeObject(&bullet);
      initBullet();
    }

    wait_msec(4000);  // Delay...
  }

  // Clear screen
  removeObject(&bigChicken);
  removeObject(&bullet);
  removeObject(&ship);

  // Display endgame messages
  wait_msec(500);  // Delay...
  if (bigChickenHealth == 0) {
    zoom = WIDTH / 192;
    strwidth = 8 * 8 * zoom;
    strheight = 8 * zoom;
    drawString((WIDTH / 2) - (strwidth / 2), (HEIGHT / 2) - (strheight / 2), "You won!", 0x02, zoom);
  } else {
    zoom = WIDTH / 192;
    strwidth = 9 * 8 * zoom;
    strheight = 8 * zoom;
    drawString((WIDTH / 2) - (strwidth / 2), (HEIGHT / 2) - (strheight / 2), "You lost!", 0x04, zoom);
  }

  // Display endgame messages
  zoom = 2;
  strwidth = 19 * 8 * zoom;
  drawString((WIDTH / 2) - (strwidth / 2), (HEIGHT / 2) + 35, "Press <R> to replay", 0x0b, zoom);
  strwidth = 25 * 8 * zoom;
  strheight = 8 * zoom;
  drawString((WIDTH / 2) - (strwidth / 2), (HEIGHT / 2) + 35 + strheight + 5, "or <M> to go back to menu", 0x0b, zoom);

  // Game has ended, wait for keypress
  while (1) {
    if ((userChar = getUart())) {
      if (userChar == 'r' || userChar == 'R') {
        clearGameMessages();  // clear screen
        state = GAME_LEVEL_ONE;
        break;
      } else if (userChar == 'm' || userChar == 'M') {
        clearGameMessages();  // clear screen
        state = GAME_MENU;
        break;
      }
    }
  };
}

// Delete an entity and mark dead
void removeObject(Object* object) {
  drawRect(object->x, object->y, object->x + object->width, object->y + object->height, 0, 1);
  object->alive = 0;
}

// Move an entity on the screen
void moveObject(Object* object, int xoff, int yoff) {
  moveRect(object->x, object->y, object->width, object->height, xoff, yoff, 0x00);
  object->x = object->x + xoff;
  object->y = object->y + yoff;
}

// Scan if the bullet has hit any of the chickens
Object* shipHitChicken(Object* with, int xoff, int yoff) {
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

// Scan if the bullet has hit the big chicken
int shipHitBigChicken(Object* with, int xoff, int yoff) {
  if (bigChicken.alive == 1) {
    if (with->x + xoff > bigChicken.x + bigChicken.width || bigChicken.x > with->x + xoff + with->width) {
      // with (Object) is too far left or right to collide
      return 0;
    } else if (with->y + yoff > bigChicken.y + bigChicken.height || bigChicken.y > with->y + yoff + with->height) {
      // with (Object) is too far up or down to collide
      return 0;
    } else {
      // Collision!
      return 1;
    }
  }
  return 0;
}

// Scan if one chicken bullet has hit the ship
int chickenHitShip(Object* with, int xoff, int yoff) {
  if (&ship != with && ship.alive == 1 && with->alive) {
    if (with->x + xoff > ship.x + ship.width || ship.x > with->x + xoff + with->width) {
      // with (Object) is too far left or right to collide
      return 0;
    } else if (with->y + yoff > ship.y + ship.height || ship.y > with->y + yoff + with->height) {
      // with (Object) is too far up or down to collide
      return 0;
    } else {
      // Collision!
      return 1;
    }
  }
  return 0;
}

// Initialize ship position
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

// Initialize ship bullet position
void initBullet() {
  int bulletRadius = 5;

  drawCircle(ship.x + (ship.width / 2), ship.y - (bulletRadius * 2), bulletRadius, 0xe0, 1);

  bullet.type = OBJ_BULLET;
  bullet.x = ship.x + (ship.width / 2) - bulletRadius;
  bullet.y = ship.y - (bulletRadius * 3);
  bullet.width = bulletRadius * 2;
  bullet.height = bulletRadius * 2;
  bullet.alive = 1;
}

// Initialize chickens
void initChickens() {
  int baseWidth = 60;
  int baseHeight = 30;
  int headWidth = 25;
  int headHeight = 17;

  static int chickenColors[COLS] = {
      0xff,
      0xaa,
      0x77,
      0x55,
      0x33,
      0xee};

  int xChicken = MARGIN + (VIRTWIDTH / COLS / 2) - (baseWidth / 2);
  int yChicken = MARGIN + baseHeight;

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

// Draw a new chicken bullet for each chicken (by index)
void initChickenBullet(int i) {
  int bulletRadius = 7;

  drawCircle(chickens[i].x + (chickens[i].width / 2), chickens[i].y + chickens[i].height + (bulletRadius * 3), bulletRadius, 0xc0, 1);

  chickenBullets[i].type = OBJ_BULLET;
  chickenBullets[i].x = chickens[i].x + (chickens[i].width / 2) - bulletRadius;
  chickenBullets[i].y = chickens[i].y + chickens[i].height + (bulletRadius * 2);
  chickenBullets[i].width = bulletRadius * 2;
  chickenBullets[i].height = bulletRadius * 2;
  chickenBullets[i].alive = 1;
}

// Initialize big chicken
void initBigChicken() {
  int baseWidth = 200;
  int baseHeight = 100;
  int headWidth = 75;
  int headHeight = 50;

  int xChicken = (WIDTH / 2) - (baseWidth / 2);
  int yChicken = MARGIN + (baseHeight / 2);

  // Draw head
  drawRect(xChicken + 30,
           yChicken,
           xChicken + 30 + headWidth,
           yChicken + headHeight,
           0x11,
           1);

  // Draw comb
  drawRect(xChicken + 30, yChicken, xChicken + 30 + headWidth / 2, yChicken + 15, 0xcc, 1);
  drawRect(xChicken + 30, yChicken - 10, xChicken + 35, yChicken + 15, 0xcc, 1);
  drawRect(xChicken + 40, yChicken - 10, xChicken + 45, yChicken + 15, 0xcc, 1);
  drawRect(xChicken + 50, yChicken - 10, xChicken + 55, yChicken + 15, 0xcc, 1);

  // Draw base
  drawRect(xChicken,
           yChicken + headHeight,
           xChicken + baseWidth,
           yChicken + headHeight + baseHeight,
           0x11,
           1);

  drawRect(xChicken + baseWidth - 20,
           yChicken + headHeight + 10,
           xChicken + baseWidth,
           yChicken + headHeight + 20,
           0x00,
           1);

  // Draw corner (left)
  drawRect(xChicken,
           yChicken + headHeight + baseHeight - 15,
           xChicken + 15,
           yChicken + headHeight + baseHeight,
           0x00,
           1);

  // Draw corner (right)
  drawRect(xChicken + baseWidth - 15,
           yChicken + headHeight + baseHeight - 15,
           xChicken + baseWidth,
           yChicken + headHeight + baseHeight,
           0x00,
           1);

  // Draw eye
  drawRect(xChicken + 50, yChicken + headHeight - 20, xChicken + 60, yChicken + headHeight - 10, 0x00, 1);

  // Draw beak
  drawRect(xChicken + 10, yChicken + headHeight - 20, xChicken + 35, yChicken + headHeight - 10, 0x66, 1);

  // Set big chicken object
  bigChicken.type = OBJ_CHICKEN;
  bigChicken.x = xChicken;
  bigChicken.y = yChicken;
  bigChicken.width = baseWidth;
  bigChicken.height = baseHeight + headHeight + 10;
  bigChicken.alive = 1;
}

// Draw many bullets for big chicken
void initBigChickenBullets() {
  int bulletRadius = 7;
  int xBullet = bigChicken.x + (bigChicken.width / 2) - 60;
  int yBullet = bigChicken.y + bigChicken.height + (bulletRadius * 2);

  for (int i = 0; i < 3; i++) {
    drawCircle(xBullet, yBullet, bulletRadius, 0xc0, 1);

    // Add to bullet array
    bigChickenBullets[i].type = OBJ_BULLET;
    bigChickenBullets[i].x = xBullet;
    bigChickenBullets[i].y = yBullet;
    bigChickenBullets[i].width = bulletRadius * 2;
    bigChickenBullets[i].height = bulletRadius * 2;
    bigChickenBullets[i].alive = 1;

    // Set cursor to next bullet
    xBullet += 60;
  }
}

// Draw the scoreboard
void drawScoreboard(int score, int lives) {
  char tens = score / 10;
  score -= (10 * tens);
  char ones = score;

  drawString((WIDTH / 2) - 300, MARGIN - 10, "Score: 0                      Lives: ", 0x0b, 2);
  drawChar(tens + 0x30, (WIDTH / 2) - 300 + (8 * 8 * 2), MARGIN - 10, 0x0b, 2);
  drawChar(ones + 0x30, (WIDTH / 2) - 300 + (8 * 9 * 2), MARGIN - 10, 0x0b, 2);
  drawChar((char)lives + 0x30, (WIDTH / 2) - 30 + (8 * 20 * 2), MARGIN - 10, 0x0b, 2);
}

// Draw decorative stars
void drawStars() {
  int xStart = MARGIN + 5;

  for (int i = 0; i < 2; i++) {
    drawRect(xStart - 5, 180, xStart - 2, 195, 0x11, 1);
    drawRect(xStart - 10, 185, xStart + 3, 188, 0x11, 1);

    drawRect(xStart - 12, 280, xStart - 9, 295, 0x11, 1);
    drawRect(xStart - 17, 285, xStart - 4, 288, 0x11, 1);

    drawRect(xStart, 330, xStart + 3, 335, 0x11, 1);

    drawRect(xStart - 12, 420, xStart - 10, 435, 0x11, 1);
    drawRect(xStart - 17, 425, xStart - 5, 428, 0x11, 1);

    drawRect(xStart - 5, 500, xStart - 2, 515, 0x11, 1);
    drawRect(xStart - 10, 505, xStart + 3, 508, 0x11, 1);

    drawRect(xStart - 10, 550, xStart - 7, 555, 0x11, 1);

    xStart += WIDTH - (MARGIN * 2);
  }
}

// Clear messages in the middle of the screen
void clearGameMessages() {
  int xStart = MARGIN + 20;
  int xEnd = xStart + WIDTH - (MARGIN * 2) - 100;
  drawRect(xStart, HEIGHT / 2 - 100, xEnd, HEIGHT / 2 + 100, 0x00, 1);
}

// Read user input and move ship
void parseShipMovement(char c) {
  // Move ship left
  if (c == 'a' || c == 'A') {
    if (ship.x >= MARGIN + (ship.width / 3) + 20) {
      moveObject(&ship, -(ship.width / 3), 0);
    }
  }

  // Move ship right
  else if (c == 'd' || c == 'D') {
    if (ship.x + ship.width + (ship.width / 3) <= WIDTH - MARGIN - 20) {
      moveObject(&ship, ship.width / 3, 0);
    }
  }

  // Move ship up
  else if (c == 'w' || c == 'W') {
    if (ship.y >= MARGIN + (ship.height * 4)) {
      moveObject(&ship, 0, -(ship.height / 3));
    }
  }

  // Move ship down
  else if (c == 's' || c == 'S') {
    if (ship.y + ship.height + (ship.height / 3) <= HEIGHT - MARGIN) {
      moveObject(&ship, 0, ship.height / 3);
    }
  }

  wait_msec(100);  // Delay...
}

void waitForKeyPress() {
  // Wait for keypress
  zoom = 2;
  strwidth = 25 * 8 * zoom;
  drawString((WIDTH / 2) - (strwidth / 2), (HEIGHT / 2) + 35, "Press any key to start...", 0x0b, zoom);

  while (!getUart())
    ;
  clearGameMessages();
}

// Draw the team banner
void team_banner() {
  drawRect(25, 25, 525, 125, 0xb0, 1);                          // cyan fill black border
  drawRect(20, 20, 520, 120, 0xf0, 1);                          // white fill black border
  drawString(40, 40, "Nguyen Minh Trang - s3751450", 0xf9, 2);  // indigo text white bg
  drawString(40, 60, "Tran Kim Long - s3755614", 0xfc, 2);      // red text white bg
  drawString(40, 80, "Truong Cong Anh - s3750788", 0xf2, 2);    // green text white bg
}