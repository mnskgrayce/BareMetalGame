// ----------------------------------- main.h -------------------------------------
typedef struct Object Object;

// Game functions
void gameMenu();
void resetGame();
void levelOne();
void levelTwo();

// Generic move/delete object functions
void removeObject(Object *object);
void moveObject(Object *object, int xoff, int yoff);

// Collision detector
Object *shipHitChicken(Object *with, int xoff, int yoff);
int shipHitBigChicken(Object *with, int xoff, int yoff);
int chickenHitShip(Object *with, int xoff, int yoff);

// Entity initialization
void initShip();
void initBullet();
void initChickens();
void initChickenBullet(int i);
void initBigChicken();
void initBigChickenBullets();

// UI functions
void drawScoreboard(int score, int lives);
void drawBigChickenHealth(int health);
void drawStars();
void clearGameMessages();

// Utilities
void parseShipMovement(char c);
void waitForKeyPress();

// Other
void team_banner();