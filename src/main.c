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

  drawChar('O', 500, 500, 0x05, 10);
  drawString(100, 100, "Hello world!", 0x04, 2);
  drawString(200, 200, "Goodbye, cruel world.", 0x0f, 1);

  drawLine(100, 500, 350, 700, 0x0c);

  while (1) {
    char c = uart_getc();
    uart_sendc(c);
  }
}