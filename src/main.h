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
static const char* TXT_HELP = {
    "\nI will help you!\n"};

// Command Help
static const char* TXT_HELP_CMD[10] = {
    "\nHelp for help\n",
    "\nHelp for cls\n",
    "\nHelp for brdrev\n",
    "\nHelp for brdmac\n",
    "\nHelp for armmem\n",
    "\nHelp for armclk\n",
    "\nHelp for temperature\n",
    "\nHelp for setcolor\n",
    "\nHelp for scrsize\n",
    "\nHelp for draw\n",
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
void setscreen(int w, int h, int pflag);

// Helpers
void parsebuf(char* buf);
int getcmd(char* s);
int getcolor(char* s);
void banner();
