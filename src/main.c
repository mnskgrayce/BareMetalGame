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

  drawRect(150, 150, 300, 300, 0x04, 0);  // black fill red border
  drawRect(400, 400, 450, 450, 0x3f, 1);  // teal fill white border

  drawCircle(960, 540, 250, 0x0e, 0);
  drawCircle(960, 540, 50, 0x13, 1);

  drawPixel(250, 250, 0x0e);

  drawChar('O', 500, 500, 0x05);
  drawString(100, 100, "Hello world!", 0x04);
  drawString(200, 200, "Goodbye, cruel world", 0x0f);

  drawLine(100, 500, 350, 700, 0x0c);
  drawGraphicChar('0', 700, 700, 0x05);
  

  while (1) {
    char c = uart_getc();
    uart_sendc(c);
  }
}