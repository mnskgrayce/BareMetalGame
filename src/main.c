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

  while (1) {
    char c = uart_getc();
    uart_sendc(c);
  }
}