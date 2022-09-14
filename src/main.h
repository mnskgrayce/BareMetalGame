// ----------------------------------- main.h -------------------------------------
typedef struct Object Object;

void removeObject(Object *object);
void moveObject(Object *object, int xoff, int yoff);
Object *shipHitChicken(Object *with, int xoff, int yoff);
int chickenHitShip(Object *with, int xoff, int yoff);
void initShip();
void initBullet();
void initChickens();
void initChickenBullet(int i);
void drawScoreboard(int score, int lives);
void drawStars();
void clearGameMessages();
void parseShipMovement(char c);
void team_banner();

void gameMenu();
void resetGame();
void levelOne();
void levelTwo();