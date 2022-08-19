#include "../gcclib/stdarg.h"
#include "../gcclib/stddef.h"
#include "../gcclib/stdint.h"

void printf(char *s, ...);

void swap(char *x, char *y);
char *reverse(char *s, int start, int end);
int pow(int base, int exp);
char *itoa(int num, char *s, int base, int precision);
char *dtoa(double num, char *s, int precision);
