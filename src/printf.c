// ----------------------------------- printf.c -------------------------------------
#include "printf.h"

#include "oslib.h"
#include "uart.h"

#define MAX_PRINT_SIZE 256

void printf(char *s, ...) {
  va_list vl;
  va_start(vl, s);  // init args list

  char buf[MAX_PRINT_SIZE] = {0};  // return buffer
  char *tmp = {0}, *sarg = {0};    // temp buffers
  int i = 0;                       // iterable for input
  int j = 0;                       // iterable for return buffer
  int width = 0;                   // minimum print width
  int precision = -1;              // precision

  while (s && s[i]) {
    if (s[i] == '%') {  // begin flag
      i++;

      // Read width value
      while (isdigit(s[i])) {
        width = width * 10 + (s[i] - '0');
        i++;
      }

      // Read precision value
      if (s[i] == '.') {
        i++;
        precision = 0;
        while (isdigit(s[i])) {
          precision = precision * 10 + (s[i] - '0');
          i++;
        }
      }

      // Parse format specifier
      switch (s[i]) {
        // CHAR FORMAT
        case 'c': {
          while (1 < width--) buf[j++] = ' ';  // left pad until length match width
          buf[j] = (char)va_arg(vl, int);      // get char
          j++;
          break;
        }
        // SYMBOL %
        case '%': {
          while (1 < width--) buf[j++] = ' ';  // left pad until length match width
          buf[j] = (char)va_arg(vl, int);      // get char
          j++;
          break;
        }
        // STRING FORMAT
        case 's': {
          if (precision == -1) precision = MAX_PRINT_SIZE;
          sarg = va_arg(vl, char *);  // get string
          int len = strlen(sarg);
          while (len > precision) sarg[len--] = '\0';  // truncate string by precision
          while (len < width--) buf[j++] = ' ';        // left pad until length match width
          strcpy(&buf[j], sarg);                       // copy converted string to buffer
          j += len;                                    // advance return buffer pointer
          break;
        }
        // DECIMAL FORMAT
        case 'd': {
          itoa(va_arg(vl, int), tmp, 10, precision);  // convert number (base 10)
          int len = strlen(tmp);
          while (len < width--) buf[j++] = ' ';  // left pad until length match width
          strcpy(&buf[j], tmp);                  // copy converted string to buffer
          j += len;                              // advance return buffer pointer

          break;
        }
        // HEX FORMAT
        case 'x': {
          itoa(va_arg(vl, int), tmp, 16, precision);  // convert number (base 16)
          int len = strlen(tmp);
          while (len < width--) buf[j++] = ' ';  // left pad until length match width
          strcpy(&buf[j], tmp);                  // copy converted string to buffer
          j += len;                              // advance return buffer pointer
          break;
        }
        // OCTAL FORMAT
        case 'o': {
          itoa(va_arg(vl, int), tmp, 8, precision);  // convert number (base 8)
          int len = strlen(tmp);
          while (len < width--) buf[j++] = ' ';  // left pad until length match width
          strcpy(&buf[j], tmp);                  // copy converted string to buffer
          j += len;                              // advance return buffer pointer
          break;
        }
        // FLOAT FORMAT
        case 'f': {
          if (precision == -1) precision = 6;        // default precision
          dtoa(va_arg(vl, double), tmp, precision);  // convert float
          int len = strlen(tmp);
          while (len < width--) buf[j++] = ' ';  // left pad until length match width
          strcpy(&buf[j], tmp);                  // copy converted string to buffer
          j += len;                              // advance return buffer pointer
          break;
        }
      }
      // Reset width and precision after parsing flag
      width = 0;
      precision = -1;
    } else {  // normal input
      buf[j] = s[i];
      j++;
    }

    if (i == MAX_PRINT_SIZE - 1) break;  // buffer overflow
    i++;                                 // next
  }

  va_end(vl);

  // Print formatted string to user
  uart_puts(buf);
}

// Swap two values
void swap(char *x, char *y) {
  char t = *x;
  *x = *y;
  *y = t;
}

// Reverse a string section s[start...end]
char *reverse(char *s, int start, int end) {
  while (start < end) {
    swap(&s[start++], &s[end--]);
  }
  return s;
}

// Compute power of a number
int pow(int base, int exp) {
  int pow = base;
  for (int i = 1; i < exp; i++) {
    pow *= base;
  }
  return pow;
}

// Convert an integer to string
char *itoa(int num, char *s, int base, int precision) {
  if (base < 2 || base > 32) return s;  // invalid

  int n = num > 0 ? num : (num * -1);    // force to positive number
  int sign = (num < 0) && (base == 10);  // check if '-' sign needed

  int i = 0;
  while (n) {
    int d = n % base;
    if (d >= 10) {
      s[i++] = 'A' + (d - 10);  // for HEX digits
    } else {
      s[i++] = '0' + d;  // decimal
    }
    n /= base;
  }

  if (i == 0 && precision != 0) s[i++] = '0';  // input number is zero
  while (i < precision) s[i++] = '0';          // add leading zeros

  if (sign) s[i++] = '-';  // add minus sign
  if (base == 8) {         // OCTAL prefix
    s[i++] = 'o';
    s[i++] = '0';
  }
  if (base == 16) {  // HEX prefix
    s[i++] = 'x';
    s[i++] = '0';
  }
  s[i] = '\0';                  // null terminate
  return reverse(s, 0, i - 1);  // must reverse the string
}

// Convert a floating point number to string
char *dtoa(double num, char *s, int precision) {
  int i = (int)num;            // extract int part
  double f = num - (double)i;  // extract floating part
  f = (f > 0) ? f : (f * -1);  // force floating part to positive

  char *ret = s;
  itoa(i, ret, 10, 0);  // convert int part to string

  if (precision != 0) {
    int len = strlen(ret);
    ret[len] = '.';                              // append dot
    f = f * pow(10, precision);                  // left shift floating part
    itoa((int)f, ret + len + 1, 10, precision);  // append converted floating part
  }
  return ret;
}
