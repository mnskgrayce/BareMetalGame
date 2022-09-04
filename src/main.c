// ----------------------------------- main.c -------------------------------------
#include "main.h"

#include "framebf.h"
#include "mbox.h"
#include "oslib.h"
#include "printf.h"
#include "uart.h"

void main() {
  team_banner();

  // drawRect(400, 400, 450, 450, 0x3f, 1);  // teal fill white border

  // drawCircle(960, 540, 250, 0x0e, 0);
  // drawCircle(960, 540, 50, 0x13, 1);

  // drawPixel(250, 250, 0x0e);

  // drawChar('O', 500, 500, 0x05, 10);
  // drawString(100, 100, "Hello world!", 0x04, 2);
  // drawString(200, 200, "Goodbye, cruel world.", 0x0f, 1);

  // drawLine(100, 500, 350, 700, 0x0c);

  while (1) {
    char c = uart_getc();
    uart_sendc(c);
  }
}

void team_banner() {
  uart_init();     // set up serial console
  framebf_init();  // set up frame buffer

  drawRect(25, 25, 525, 125, 0xb0, 1);                          // cyan fill black border
  drawRect(20, 20, 520, 120, 0xf0, 1);                          // white fill black border
  drawString(40, 40, "Nguyen Minh Trang - s3751450", 0xf9, 2);  // indigo text white bg
  drawString(40, 60, "Tran Kim Long - sXXXXXXX", 0xfc, 2);      // red text white bg
  drawString(40, 80, "Truong Cong Anh - s3750788", 0xf2, 2);    // green text white bg
}