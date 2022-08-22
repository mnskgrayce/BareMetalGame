// ----------------------------------- main.h -------------------------------------
#define BACKSPACE 8  // key code Backspace
#define DELETE 127   // key code Delete

#define COMMANDS_LEN 10     // number of commands
#define COLORS_LEN 8        // number of colors
#define MAX_TERMS 7         // maximum number of terms (args) supported
#define DEFAULT_WIDTH 1024  // default screen width
#define DEFAULT_HEIGHT 768  // default screen height

#define HELP 0  // index of help command in const array
#define CLS 1
#define BRDREV 2
#define BRDMAC 3
#define ARMMEM 4
#define ARMCLK 5
#define TEMPERATURE 6
#define SETCOLOR 7
#define SCRSIZE 8
#define DRAW 9

#define BLACK 0
#define RED 1
#define GREEN 2
#define YELLOW 3
#define BLUE 4
#define PURPLE 5
#define CYAN 6
#define WHITE 7

// General Help
static const char* TXT_HELP[] = {
    "\nFor more information about a specific command, type HELP <command_name>\n\n",
    "HELP                   Show brief information of all commands\n",
    "CLS                    Clear screen\n",
    "BRDREV                 Show board revision\n",
    "BRDMAC                 Show board MAC address\n",
    "ARMMEM                 Show ARM memory\n",
    "ARMCLK                 Show ARM clock rate\n",
    "TEMPERATURE            Show chip temperature\n",
    "SETCOLOR               Set console text and/or background color\n",
    "SCRSIZE                Set physical and/or virtual screen size\n",
    "DRAW                   Draw a graphic at custom position, size, and color\n",
};

// Command Help
static const char* TXT_HELP_CMD[10] = {
    "\n    Usage: HELP <command_name>\n    Show brief information of all commands\n\nIf <command_name> is provided, show detailed information of that command.\n\nExample:\n    HELP\n    HELP CLS\n",
    "\n    Usage: CLS\n    Clear screen and reset the cursor to top left corner\n",
    "\n    Usage: BRDREV\n    Show board revision of the Raspberry Pi\n",
    "\n    Usage: BRDMAC\n    Show MAC address of the Raspberry Pi\n",
    "\n    Usage: ARMMEM\n    Show ARM memory in bytes of the Raspberry Pi\n",
    "\n    Usage: ARMCLK\n    Show ARM clock rate in Hz of the Raspberry Pi\n",
    "\n    Usage: TEMPERATURE\n    Show chip temperature in Celsius of the Raspberry Pi\n",
    "\n    Usage: SETCOLOR <flag> <color>\n    Set console text and/or background color\n\nFlag is either -t (text) or -b (background).\nColor must be one of these values:\n    BLACK, RED, GREEN, YELLOW, BLUE, PURPLE, CYAN, WHITE\n\nExample:\n    SETCOLOR -t RED -b YELLOW\n    SETCOLOR -t BLUE\n",
    "\n    Usage: SCRSIZE <flag> <width> <height>\n    Set physical and/or virtual width/height of the screen\n\nFlag is either -p (physical) or -v (virtual).\nWidth and height are decimal numbers > 0.\nIf no flag is provided, set both physcial and virtual screens.\n\nExample:\n    SCRSIZE -p 1024 768 -v 720 480\n    SCRSIZE -v 300 300\n    SCRSIZE 640 640\n",
    "\n    Usage: DRAW <flag> <PARAMS> <color>\n    Draw a graphic at custom position, size, and color\n\nFlag is either -l (line), -r (rectangle) or -c (circle).\nColor can be one of theses values:\n    BLACK, RED, GREEN, YELLOW, BLUE, PURPLE, CYAN, WHITE\n    Else, color must be a 6-character HEX code.\n\nThe syntax for PARAMS varies with each flag:\n* For lines and rectangles, PARAMS are <x1> <y1> <x2> <y2>\n    where (x1, y1) is the coordinate to draw from and\n    (x2, y2) is the coordinate to draw to.\n* For circles, PARAMS are <x> <y> <radius>\n    where (x, y) is the starting coordinate and\n    radius is the radius of the circle.\n\nExample:\n    DRAW -l 100 100 200 200 PURPLE\n    DRAW -r 200 200 300 300 RED\n    DRAW -c 600 600 25 3F0FB7\n",
};

// Master Data
static const char* COMMANDS[10] = {
    "help",
    "cls",
    "brdrev",
    "brdmac",
    "armmem",
    "armclk",
    "temperature",
    "setcolor",
    "scrsize",
    "draw",
};

static const char* COLORS[8] = {
    "black",
    "red",
    "green",
    "yellow",
    "blue",
    "purple",
    "cyan",
    "white",
};

// CLI function prototypes
void help(char* token, char* delim, int terms);
void cls(int terms);
void brdrev(int terms);
void brdmac(int terms);
void armmem(int terms);
void armclk(int terms);
void temperature(int terms);
void setcolor(char* token, char* delim, int terms);
void scrsize(char* token, char* delim, int terms);
void draw(char* token, char* delim, int terms);

// Helpers
void setscreen(int w, int h, int pflag);
void parsebuf(char* buf);
int getcmd(char* s);
int getcolor(char* s);
void banner();
