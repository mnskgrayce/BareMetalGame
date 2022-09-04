// ----------------------------------- framebf.c -------------------------------------
#include "framebf.h"

#include "mbox.h"
#include "terminal.h"
#include "uart.h"

// Use RGBA32 (32 bits for each pixel)
#define COLOR_DEPTH 32

// Pixel Order: BGR in memory order (little endian --> RGB in byte order)
#define PIXEL_ORDER 0  // Screen info

unsigned int width, height, pitch;

/* Frame buffer address
 * (declare as pointer of unsigned char to access each byte) */
unsigned char *fb;

void framebf_init() {
  mBuf[0] = 35 * 4;  // Length of message in bytes
  mBuf[1] = MBOX_REQUEST;

  mBuf[2] = MBOX_TAG_SETPHYWH;  // Set physical width-height
  mBuf[3] = 8;                  // Value size in bytes
  mBuf[4] = 0;                  // REQUEST CODE = 0
  mBuf[5] = WIDTH;              // Value(width)
  mBuf[6] = HEIGHT;             // Value(height)

  mBuf[7] = MBOX_TAG_SETVIRTWH;  // Set virtual width-height
  mBuf[8] = 8;
  mBuf[9] = 0;
  mBuf[10] = WIDTH;
  mBuf[11] = HEIGHT;

  mBuf[12] = MBOX_TAG_SETVIRTOFF;  // Set virtual offset
  mBuf[13] = 8;
  mBuf[14] = 0;
  mBuf[15] = 0;  // x offset
  mBuf[16] = 0;  // y offset

  mBuf[17] = MBOX_TAG_SETDEPTH;  // Set color depth
  mBuf[18] = 4;
  mBuf[19] = 0;
  mBuf[20] = COLOR_DEPTH;  // Bits per pixel

  mBuf[21] = MBOX_TAG_SETPXLORDR;  // Set pixel order
  mBuf[22] = 4;
  mBuf[23] = 0;
  mBuf[24] = PIXEL_ORDER;

  mBuf[25] = MBOX_TAG_GETFB;  // Get frame buffer
  mBuf[26] = 8;
  mBuf[27] = 0;
  mBuf[28] = 16;  // alignment in 16 bytes
  mBuf[29] = 0;   // will return Frame Buffer size in bytes

  mBuf[30] = MBOX_TAG_GETPITCH;  // Get pitch
  mBuf[31] = 4;
  mBuf[32] = 0;
  mBuf[33] = 0;  // Will get pitch value here
  mBuf[34] = MBOX_TAG_LAST;

  // Call Mailbox
  if (mbox_call(ADDR(mBuf), MBOX_CH_PROP)  // mailbox call is successful ?
      &&
      mBuf[20] == COLOR_DEPTH  // got correct color depth ?
      &&
      mBuf[24] == PIXEL_ORDER  // got correct pixel order ?
      &&
      mBuf[28] != 0  // got a valid address for frame buffer ?
  ) {
    /* Convert GPU address to ARM address (clear higher address bits) * Frame Buffer is located in RAM memory, which VideoCore MMU
     * maps it to bus address space starting at 0xC0000000.
     * Software accessing RAM directly use physical addresses
     * (based at 0x00000000)
     */
    mBuf[28] &= 0x3FFFFFFF;

    // Access frame buffer as 1 byte per each address
    fb = (unsigned char *)((unsigned long)mBuf[28]);
    // uart_puts("Got allocated Frame Buffer at RAM physical address: ");
    // uart_hex(mBuf[28]);
    // uart_puts("\n");

    // uart_puts("Frame Buffer Size (bytes): ");
    // uart_dec(mBuf[29]);
    // uart_puts("\n");

    width = mBuf[5];   // Actual physical width
    height = mBuf[6];  // Actual physical height
    pitch = mBuf[33];  // Number of bytes per line
  } else {
    uart_puts("Unable to get a frame buffer with provided settings\n");
  }
}

void drawPixel(int x, int y, unsigned char attr) {
  int offs = (y * pitch) + (x * 4);
  *((unsigned int *)(fb + offs)) = vgapal[attr & 0x0f];
}

// Attribute color is a HEX code
// 4 MSBs represent background
// 4 LSBs represent foreground
// If fill=1, background is the fill and foreground is the outline
// For example, attr=0x03 => 0 background (BLACK), 3 foreground (GREEN)
// See color indexes in vgapl array (terminal.h), there is a 16-color pallete
void drawRect(int x1, int y1, int x2, int y2, unsigned char attr, int fill) {
  int y = y1;

  while (y <= y2) {
    int x = x1;
    while (x <= x2) {
      if ((x == x1 || x == x2) || (y == y1 || y == y2))
        drawPixel(x, y, attr);
      else if (fill)
        drawPixel(x, y, (attr & 0xf0) >> 4);
      x++;
    }
    y++;
  }
}

void drawLine(int x1, int y1, int x2, int y2, unsigned char attr) {
  int dx, dy, p, x, y;

  dx = x2 - x1;
  dy = y2 - y1;
  x = x1;
  y = y1;
  p = 2 * dy - dx;

  while (x < x2) {
    if (p >= 0) {
      drawPixel(x, y, attr);
      y++;
      p = p + 2 * dy - 2 * dx;
    } else {
      drawPixel(x, y, attr);
      p = p + 2 * dy;
    }
    x++;
  }
}

void drawCircle(int x0, int y0, int radius, unsigned char attr, int fill) {
  int x = radius;
  int y = 0;
  int err = 0;

  while (x >= y) {
    if (fill) {
      drawLine(x0 - y, y0 + x, x0 + y, y0 + x, (attr & 0xf0) >> 4);
      drawLine(x0 - x, y0 + y, x0 + x, y0 + y, (attr & 0xf0) >> 4);
      drawLine(x0 - x, y0 - y, x0 + x, y0 - y, (attr & 0xf0) >> 4);
      drawLine(x0 - y, y0 - x, x0 + y, y0 - x, (attr & 0xf0) >> 4);
    }
    drawPixel(x0 - y, y0 + x, attr);
    drawPixel(x0 + y, y0 + x, attr);
    drawPixel(x0 - x, y0 + y, attr);
    drawPixel(x0 + x, y0 + y, attr);
    drawPixel(x0 - x, y0 - y, attr);
    drawPixel(x0 + x, y0 - y, attr);
    drawPixel(x0 - y, y0 - x, attr);
    drawPixel(x0 + y, y0 - x, attr);

    if (err <= 0) {
      y += 1;
      err += 2 * y + 1;
    }

    if (err > 0) {
      x -= 1;
      err -= 2 * x + 1;
    }
  }
}

void drawChar(unsigned char ch, int x, int y, unsigned char attr, int zoom) {
  unsigned char *glyph = (unsigned char *)&font + (ch < FONT_NUMGLYPHS ? ch : 0) * FONT_BPG;

  for (int i = 1; i <= (FONT_HEIGHT * zoom); i++) {
    for (int j = 0; j < (FONT_WIDTH * zoom); j++) {
      unsigned char mask = 1 << (j / zoom);
      unsigned char col = (*glyph & mask) ? attr & 0x0f : (attr & 0xf0) >> 4;

      drawPixel(x + j, y + i, col);
    }
    glyph += (i % zoom) ? 0 : FONT_BPL;
  }
}

void drawString(int x, int y, char *s, unsigned char attr, int zoom) {
  while (*s) {
    if (*s == '\r') {
      x = 0;
    } else if (*s == '\n') {
      x = 0;
      y += FONT_HEIGHT * zoom;
    } else {
      drawChar(*s, x, y, attr, zoom);
      x += FONT_WIDTH * zoom;
    }
    s++;
  }
}

void moveRect(int oldx, int oldy, int width, int height, int shiftx, int shifty, unsigned char attr) {
  unsigned int newx = oldx + shiftx, newy = oldy + shifty;
  unsigned int xcount = 0, ycount = 0;
  unsigned int bitmap[width][height];  // This is very unsafe if it's too big for the stack...
  unsigned int offs;

  // Copy current screen to bitmap array
  while (xcount < width) {
    while (ycount < height) {
      offs = ((oldy + ycount) * pitch) + ((oldx + xcount) * 4);

      bitmap[xcount][ycount] = *((unsigned int *)(fb + offs));
      ycount++;
    }
    ycount = 0;
    xcount++;
  }

  // "Delete" object (draw a black shape)
  drawRect(oldx, oldy, oldx + width, oldy + width, attr, 1);

  // Redraw bitmap
  for (int i = newx; i < newx + width; i++) {
    for (int j = newy; j < newy + height; j++) {
      offs = (j * pitch) + (i * 4);
      *((unsigned int *)(fb + offs)) = bitmap[i - newx][j - newy];
    }
  }
}