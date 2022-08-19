// ----------------------------------- main.c -------------------------------------
#include "main.h"

#include "framebf.h"
#include "mbox.h"
#include "oslib.h"
#include "printf.h"
#include "uart.h"

void main() {
  uart_init();     // set up serial console
  framebf_init();  // set up frame buffer
  banner();        // print welcome banner

  printf("Characters:%2c%c \n", 'a', 65);
  printf("Decimals: %.7d %.d\n", 1977, 650000);
  printf("Preceding with blanks:%5d%10d \n", 1998, 2022);
  printf("Some different radices: %d %x %o \n", -100, -100, -100);
  printf("floats:%10f %.2f\n", 12.34, 12.123456);
  printf("%2shoho\n", "A");
  printf("%.3s\n", "I'm-tired");
  printf("Symbol and number: %% %d \n", '%', 65);

  while (1) {
    uart_puts("\n> ");      // input indicator >
    char buf[MAX_BUF_LEN];  // init buffer
    int i = 0;

    while (1) {
      char c = uart_getc();

      if (c == '\n' || c == '\r' || c == '\f') {  // read char until newline
        char* trimmed = strtrim(buf);             // trim whitespace
        parsebuf(trimmed);                        // parse command
        clrstr(trimmed);                          // clear buffer
        break;
      } else {
        if (c == DELETE || c == BACKSPACE) {  // delete key detected
          uart_puts("\b \b");
          if (i > 0) {
            buf[--i] = '\0';  // remove char from buffer
          }
        } else {  // clean input before adding to buffer
          if ((c >= 'A' && c <= 'Z') ||
              (c >= 'a' && c <= 'z') ||
              (c >= '0' && c <= '9') ||
              (c == '-') ||
              (c == ' ')) {
            buf[i] = c;
            i++;            // advance buffer pointer
            uart_sendc(c);  // show char to user
          }
        }
      }
    }
  }
}

void parsebuf(char* buf) {
  char tmp[MAX_BUF_LEN];
  strcpy(tmp, buf);  // clone string to preserve original

  char* delim = " ";
  int terms = count_terms(tmp, delim);

  if (terms < 1 || terms > MAX_TERMS) {
    uart_puts("\nWrong syntax: invalid number of arguments\n");
    return;
  }

  char* token = strtok(tmp, delim);  // first token is a command
  int cmd = getcmd(token);           // get command index

  switch (cmd) {
    case HELP:
      help(token, delim, terms);
      break;
    case CLS:
      cls(terms);
      break;
    case BRDREV:
      brdrev(terms);
      break;
    case BRDMAC:
      brdmac(terms);
      break;
    case ARMMEM:
      armmem(terms);
      break;
    case ARMCLK:
      armclk(terms);
      break;
    case TEMPERATURE:
      temperature(terms);
      break;
    case SETCOLOR:
      setcolor(token, delim, terms);
      break;
    case SCRSIZE:
      scrsize(token, delim, terms);
      break;
    case DRAW:
      draw(token, delim, terms);
      break;
    default:
      uart_puts("\nWrong syntax, type 'help' to view supported commands\n");
      break;
  }
}

// FUNCTION 1/10: PRINT HELP
void help(char* token, char* delim, int terms) {
  if (terms == 1) {
    uart_puts((char*)TXT_HELP);  // get general help
    return;
  }

  if (terms == 2) {
    token = strtok(0, delim);  // next token is a command
    int cmd = getcmd(token);
    if (cmd != -1) {
      uart_puts((char*)TXT_HELP_CMD[cmd]);  // get specific help
    } else {
      uart_puts("\nWrong syntax, command '");
      uart_puts(token);
      uart_puts("' is not found\n");
    }
    return;
  }

  // Wrong syntax, print help
  uart_puts((char*)TXT_HELP_CMD[HELP]);
}

// FUNCTION 2/10: CLEAR SCREEN
void cls(int terms) {
  if (terms != 1) {
    uart_puts((char*)TXT_HELP_CMD[CLS]);
    return;
  }
  // Move cursor to top left and clear screen
  uart_puts("\033[H\033[J");
}

// FUNCTION 3/10: GET BOARD REVISION
void brdrev(int terms) {
  if (terms != 1) {
    uart_puts((char*)TXT_HELP_CMD[BRDREV]);
    return;
  }

  mBuf[0] = 7 * 4;                 // Message Buffer Size in bytes (7 elements * 4 bytes (32 bit) each)
  mBuf[1] = MBOX_REQUEST;          // Message Request Code (this is a request message)
  mBuf[2] = MBOX_TAG_GETREVISION;  // TAG Identifier: Get board revision
  mBuf[3] = 4;                     // Value buffer size in bytes (max of request and response lengths)
  mBuf[4] = 0;                     // REQUEST CODE = 0
  mBuf[5] = 0;                     // clear output buffer (response data in mBuf[5])
  mBuf[6] = MBOX_TAG_LAST;

  if (mbox_call(ADDR(mBuf), MBOX_CH_PROP)) {
    uart_puts("\nBoard Revision: ");
    uart_hex(mBuf[5]);  // 0x00A02082 (RPi 3B 1GB for QEMU)
    uart_puts("\n");
  } else {
    uart_puts("\nError: failed to call mailbox!\n");
  }
}

// FUNCTION 4/10: GET BOARD MAC ADDRESS
void brdmac(int terms) {
  if (terms != 1) {
    uart_puts((char*)TXT_HELP_CMD[BRDMAC]);
    return;
  }

  mBuf[0] = 8 * 4;                // Message Buffer Size in bytes (8 elements * 4 bytes (32 bit) each)
  mBuf[1] = MBOX_REQUEST;         // Message Request Code (this is a request message)
  mBuf[2] = MBOX_TAG_GETMACADDR;  // TAG Identifier: Get MAC address
  mBuf[3] = 6;                    // Value buffer size in bytes (max of request and response lengths)
  mBuf[4] = 0;                    // REQUEST CODE = 0
  mBuf[5] = 0;                    // clear output buffer (data in mBuf[5])
  mBuf[6] = 0;                    // clear output buffer
  mBuf[7] = MBOX_TAG_LAST;

  if (mbox_call(ADDR(mBuf), MBOX_CH_PROP)) {
    uart_puts("\nBoard MAC Address: ");  // aa:bb:cc:dd:ee:ff
    unsigned int d;
    unsigned int n;
    int i;
    int c;

    // Display first 4 bytes (this is just modified uart_hex())
    d = mBuf[5];
    n = 0;
    i = 0;
    for (c = 28; c >= 0; c = c - 4) {
      n = (d >> c) & 0xF;                // get highest 4-bit nibble
      n += (n > 9) ? (-10 + 'A') : '0';  // 0-9 => '0'-'9', 10-15 => 'A'-'F'
      uart_sendc(n);

      if (i == 1 || i == 3 || i == 5) uart_sendc(':');  // insert separator
      i++;
    }

    // Display remaining 2 bytes
    d = mBuf[6];
    n = 0;
    i = 0;
    for (c = 28; c >= 0; c = c - 4) {
      n = (d >> c) & 0xF;                // get highest 4-bit nibble
      n += (n > 9) ? (-10 + 'A') : '0';  // 0-9 => '0'-'9', 10-15 => 'A'-'F'

      if (i >= 4) uart_sendc(n);              // skip first 2 bytes (buffer is 4 bytes)
      if (i == 2 || i == 5) uart_sendc(':');  // insert separator
      i++;
    }
    uart_puts("\n");
  } else {
    uart_puts("\nError: failed to call mailbox!\n");
  }
}

// FUNCTION 5/10: GET ARM MEMORY
void armmem(int terms) {
  if (terms != 1) {
    uart_puts((char*)TXT_HELP_CMD[ARMMEM]);
    return;
  }

  mBuf[0] = 8 * 4;               // Message Buffer Size in bytes (8 elements * 4 bytes (32 bit) each)
  mBuf[1] = MBOX_REQUEST;        // Message Request Code (this is a request message)
  mBuf[2] = MBOX_TAG_GETARMMEM;  // TAG Identifier: Get ARM memory
  mBuf[3] = 8;                   // Value buffer size in bytes (max of request and response lengths)
  mBuf[4] = 0;                   // REQUEST CODE = 0
  mBuf[5] = 0;                   // clear output buffer
  mBuf[6] = 0;                   // clear output buffer (size data in mBuf[6])
  mBuf[7] = MBOX_TAG_LAST;

  if (mbox_call(ADDR(mBuf), MBOX_CH_PROP)) {
    uart_puts("\nARM Memory (bytes): ");
    uart_dec(mBuf[6]);  // 1006632960
    uart_puts("\n");
  } else {
    uart_puts("\nError: failed to call mailbox!\n");
  }
}

// FUNCTION 6/10: GET ARM CLOCK RATE
void armclk(int terms) {
  if (terms != 1) {
    uart_puts((char*)TXT_HELP_CMD[ARMCLK]);
    return;
  }

  mBuf[0] = 8 * 4;                // Message Buffer Size in bytes (8 elements * 4 bytes (32 bit) each)
  mBuf[1] = MBOX_REQUEST;         // Message Request Code (this is a request message)
  mBuf[2] = MBOX_TAG_GETCLKRATE;  // TAG Identifier: Get clock rate
  mBuf[3] = 8;                    // Value buffer size in bytes (max of request and response lengths)
  mBuf[4] = 0;                    // REQUEST CODE = 0
  mBuf[5] = 3;                    // clock id of ARM
  mBuf[6] = 0;                    // clear output buffer (clock rate data in mBuf[6])
  mBuf[7] = MBOX_TAG_LAST;

  if (mbox_call(ADDR(mBuf), MBOX_CH_PROP)) {
    uart_puts("\nARM Clock Rate (Hz): ");
    uart_dec(mBuf[6]);  // 700000000
    uart_puts("\n");
  } else {
    uart_puts("\nError: failed to call mailbox!\n");
  }
}

// FUNCTION 7/10: GET TEMPERATURE
void temperature(int terms) {
  if (terms != 1) {
    uart_puts((char*)TXT_HELP_CMD[TEMPERATURE]);
    return;
  }

  mBuf[0] = 8 * 4;                    // Message Buffer Size in bytes (8 elements * 4 bytes (32 bit) each)
  mBuf[1] = MBOX_REQUEST;             // Message Request Code (this is a request message)
  mBuf[2] = MBOX_TAG_GETTEMPERATURE;  // TAG Identifier: Get temperature
  mBuf[3] = 8;                        // Value buffer size in bytes (max of request and response lengths)
  mBuf[4] = 0;                        // REQUEST CODE = 0
  mBuf[5] = 0;                        // temperature id = 0
  mBuf[6] = 0;                        // clear output buffer (temperature data in mBuf[6])
  mBuf[7] = MBOX_TAG_LAST;

  if (mbox_call(ADDR(mBuf), MBOX_CH_PROP)) {
    uart_puts("\nChip Temperature (°C): ");
    uart_dec(mBuf[6] / 1000);  // return value in thousandths of a degree
    uart_puts("\n");
  } else {
    uart_puts("\nError: failed to call mailbox!\n");
  }
}

// FUNCTION 8/10: SET TEXT AND BACKGROUND COLOR
void setcolor(char* token, char* delim, int terms) {
  if (terms == 3 || terms == 5) {
    // Initialize flag and colors
    int tflag = -1;
    int tcolor = -1;
    int bcolor = -1;

    // Get next token (flag): setcolor -t color1 -b color2
    // i =                    0        1  2      3  4
    token = strtok(0, delim);

    // Go through remaining terms
    for (int i = 1; i < terms; i++) {
      if ((i % 2) == 1) {                // index is flag
        if (strcmp(token, "-t") == 0) {  // raise text color flag if not already
          if (tflag == 1) {
            uart_puts("\nWrong syntax, flag '-t' is duplicate\n");
            return;
          }
          tflag = 1;
        } else if (strcmp(token, "-b") == 0) {  // raise background color flag if not already
          if (tflag == 0) {
            uart_puts("\nWrong syntax, flag '-b' is duplicate\n");
            return;
          }
          tflag = 0;
        } else {  // invalid flag
          uart_puts("\nWrong syntax, flag '");
          uart_puts(token);
          uart_puts("' is invalid");
          return;
        }
      } else {  // index is color
        int color = getcolor(token);
        if (color == -1) {  // invalid color
          uart_puts("\nWrong syntax, color '");
          uart_puts(token);
          uart_puts("' is not found\n");
          return;
        }
        if (tflag)  // check flag then assign color
          tcolor = color;
        else
          bcolor = color;
      }

      // Get next token
      token = strtok(0, delim);
    }

    // Concatenate Select Graphic Rendition (SGR) command
    // * NOTE: sometimes it doesn't work if not visible enough?
    uart_puts("\x1b[");  // Control Sequence Inducer (CSI)
    if (tcolor != -1) {
      uart_puts("3");    // foreground prefix
      uart_dec(tcolor);  // color digit
    }
    if (tcolor != -1 && bcolor != -1) {
      uart_puts(";");  // params delimiter
    }
    if (bcolor != -1) {
      uart_puts("4");    // background prefix
      uart_dec(bcolor);  // color digit
    }
    uart_puts("m");  // final byte

    uart_puts("\033[J");  // fill screen with selected colors
    return;
  }

  // Wrong syntax, print help
  uart_puts((char*)TXT_HELP_CMD[SETCOLOR]);
}

// FUNCTION 9/10: SET SCREEN SIZE
void scrsize(char* token, char* delim, int terms) {
  // Initialize flag and width / height values
  int pflag = -1;
  int pw = -1;
  int ph = -1;
  int vw = -1;
  int vh = -1;

  if (terms == 3 || terms == 4 || terms == 7) {
    // CASE 1: terms = 4 or terms = 7
    // Get next token (flag): scrsize -p <width> <height> -t <width> <height>
    // i =                    0       1  2       3        4  5       6

    // CASE 2: terms = 3
    // Get next token (width): scrsize <width> <height>
    // i =                     0       1       2
    token = strtok(0, delim);

    // Go through remaining terms
    for (int i = 1; i < terms; i++) {
      if ((terms == 4 || terms == 7) && (i % 3) == 1) {  // index is flag
        if (strcmp(token, "-p") == 0) {                  // raise physical screen flag if not already
          if (pflag == 1) {
            uart_puts("\nWrong syntax, flag '-p' is duplicate\n");
            return;
          }
          pflag = 1;
        } else if (strcmp(token, "-v") == 0) {  // raise virtual screen flag if not already
          if (pflag == 0) {
            uart_puts("\nWrong syntax, flag '-v' is duplicate\n");
            return;
          }
          pflag = 0;
        } else {  // invalid flag
          uart_puts("\nWrong syntax, flag '");
          uart_puts(token);
          uart_puts("' is invalid");
          return;
        }
      } else {                // index is width or height
        if (!isnum(token)) {  // invalid number
          uart_puts("\nWrong syntax, input is not a number\n");
          return;
        } else {
          // Set values for both physical and virtual screens
          if (terms == 3) {
            if (i == 1) {  // set width at index 1, height at index 2
              pw = atoi(token);
              vw = atoi(token);
            } else {
              ph = atoi(token);
              vh = atoi(token);
            }
          } else {
            // Set value for one screen only
            if (pflag) {       // physical
              if (i % 3 == 2)  // width at index 2 or 5, height at index 3 or 6
                pw = atoi(token);
              else
                ph = atoi(token);
            } else {  // virtual
              if (i % 3 == 2)
                vw = atoi(token);
              else
                vh = atoi(token);
            }
          }
        }
      }

      // Get next token
      token = strtok(0, delim);
    }

    // Set screen size if has valid numbers
    if (pw > 0 && ph > 0)  // physical
      setscreen(pw, ph, 1);
    if (vw > 0 && vh > -0)  // virtual
      setscreen(vw, vh, 0);
    return;
  }

  // Wrong syntax, print help
  uart_puts((char*)TXT_HELP_CMD[SCRSIZE]);
}

// FUNCTION 9b: SET SCREEN SIZE (FOR ONE SCREEN)
void setscreen(int w, int h, int pflag) {
  mBuf[0] = 8 * 4;         // Message Buffer Size in bytes (9 elements * 4 bytes each)
  mBuf[1] = MBOX_REQUEST;  // Message Request Code (this is a request message)

  mBuf[2] = pflag ? MBOX_TAG_SETPHYWH : MBOX_TAG_SETVIRTWH;  // Set physical or virtual width-height

  mBuf[3] = 8;  // Value size in bytes
  mBuf[4] = 0;  // REQUEST CODE = 0
  mBuf[5] = w;  // Value (width)
  mBuf[6] = h;  // Value (height)
  mBuf[7] = MBOX_TAG_LAST;

  char* scrname = pflag ? "physical" : "virtual";
  if (mbox_call(ADDR(mBuf), MBOX_CH_PROP)) {
    uart_puts("\nSet ");
    uart_puts(scrname);
    uart_puts(" screen width: ");
    uart_dec(mBuf[5]);
    uart_puts("\nSet ");
    uart_puts(scrname);
    uart_puts(" screen height: ");
    uart_dec(mBuf[6]);
    uart_puts("\n");
  } else {
    uart_puts("\nError: failed to call mailbox!\n");
  }
}

// FUNCTION 10/10: DRAW ON FRAME BUFFER
void draw(char* token, char* delim, int terms) {
  if (terms == 6 || terms == 7) {
    // Initialize params
    int flag = -1;
    int x1 = -1, y1 = -1, x2 = -1, y2 = -1;
    int hexcolor = -1;

    // Get flag
    token = strtok(0, delim);
    if (strcmp(token, "-l") == 0) flag = 0;  // line
    if (strcmp(token, "-r") == 0) flag = 1;  // rectangle
    if (strcmp(token, "-c") == 0) flag = 2;  // circle

    // Circle only takes x, y, r
    if (flag == 2 && terms != 6) {
      uart_puts((char*)TXT_HELP_CMD[DRAW]);
      return;
    }

    if (flag != -1) {
      int params_len = (flag == 2) ? 3 : 4;

      // Get coordinates
      for (int i = 0; i < params_len; i++) {
        token = strtok(0, delim);
        if (!isnum(token)) {  // invalid number
          uart_puts("\nWrong syntax, input is not a number\n");
          return;
        } else {
          if (i == 0) x1 = atoi(token);
          if (i == 1) y1 = atoi(token);
          if (i == 2) x2 = atoi(token);
          if (i == 3) y2 = atoi(token);
        }
      }

      token = strtok(0, delim);  // get color
      int color = getcolor(token);

      if (color != -1) {  // valid color, generate hex code
        switch (color) {
          case BLACK:
            hexcolor = 0x2e3436;
            break;
          case RED:
            hexcolor = 0xcc0000;
            break;
          case GREEN:
            hexcolor = 0x4e9a06;
            break;
          case YELLOW:
            hexcolor = 0xc4a000;
            break;
          case BLUE:
            hexcolor = 0x3465a4;
            break;
          case PURPLE:
            hexcolor = 0x75507b;
            break;
          case CYAN:
            hexcolor = 0x06989a;
            break;
          case WHITE:
            hexcolor = 0xd3d7cf;
            break;
        }
      } else {  // color not in DB, try to parse as hex string
        char* p = token;
        while (*p) {
          if (isdigit(*p) || (*p >= 'a' && *p <= 'f') || (*p >= 'A' && *p <= 'F'))
            p++;
          else {
            uart_puts("\nWrong syntax, color is invalid\n");
            return;
          }
        }
        hexcolor = atoi_b16(token);
      }

      // Start drawing
      switch (flag) {
        case 0:  // line
          drawLineARGB32(x1, y1, x2, y2, hexcolor);
          break;
        case 1:  // rectangle
          drawRectARGB32(x1, y1, x2, y2, hexcolor, 1);
          break;
        case 2:  // circle
          drawCircleARGB32(x1, y1, x2, hexcolor, 1);
          break;
      }
      uart_puts("\nDrawing finished, check your screen!\n");
      return;

    } else {  // invalid flag
      uart_puts("\nWrong syntax, flag '");
      uart_puts(token);
      uart_puts("' is invalid");
      return;
    }
  }

  // Wrong syntax, print help
  uart_puts((char*)TXT_HELP_CMD[DRAW]);
}

// Return index of a command in const array
int getcmd(char* s) {
  for (int i = 0; i < COMMANDS_LEN; i++) {
    if (strcmp(s, (char*)COMMANDS[i]) == 0) {
      return i;
    }
  }
  return -1;  // not found
}

// Return index of a color in const array
int getcolor(char* s) {
  for (int i = 0; i < COLORS_LEN; i++) {
    if (strcmp(s, (char*)COLORS[i]) == 0) {
      return i;
    }
  }
  return -1;  // not found
}

// Print the welcome banner
void banner() {
  for (int i = 0; i < BANNER_LEN; i++) {
    uart_puts((char*)BANNER[i]);
  }
}
