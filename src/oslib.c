// ----------------------------------- oslib.c -------------------------------------
#include "oslib.h"

// Clear a char array
void clrstr(char* s) {
  int i = 0;
  while (s[i] != '\0') {  // set each char to null
    s[i] = '\0';
    i++;
  }
}

// Clone a string
char* strcpy(char* des, const char* src) {
  char* tmp = des;
  while ((*des++ = *src++))  // copy value and advance pointer until null
    ;
  return tmp;  // pointer to clone string
}

// Trim leading and trailing whitespace in a string
char* strtrim(char* s) {
  while (*s == ' ') s++;  // while space, advance pointer
  char* p = s;            // point to start of string
  while (*p) p++;         // advance to end of string
  p--;                    // back off null term
  while (*p == ' ') {     // while space, set new end of string
    *p = 0;
    p--;
  }
  return s;  // pointer to trimmed string
}

// Tokenize a string from delimiters
char* strtok(char* s, char* delim) {
  static char* backup;  // start of the next search

  if (!s)
    s = backup;

  if (!s)
    return 0;  // bad input

  while (1) {
    if (isin(*s, delim)) {  // skip delimiters at start of string
      s++;
      continue;
    }
    if (*s == '\0') {  // end of string
      return 0;        // return null pointer to next token
    }
    break;
  }

  char* ret = s;
  while (1) {
    if (*s == '\0') {
      backup = s;  // end of the input string, next run will return NULL
      return ret;
    }
    if (isin(*s, delim)) {
      *s = '\0';
      backup = s + 1;
      return ret;
    }
    s++;
  }
}

// Compare two strings (case insensitive)
int strcmp(char* s1, char* s2) {
  while (tolower(*s1) == tolower(*s2)) {  // compare each char until one string ends
    if (*s1 == '\0' || *s2 == '\0')
      break;

    s1++;
    s2++;
  }
  if (*s1 == '\0' && *s2 == '\0') {  // check if both strings end
    return 0;
  }
  return -1;
}

// Count number of chars in a string
unsigned int strlen(char* s) {
  int len = 0;
  while (*s != '\0') {
    len++;
    s++;
  }
  return len;
}

// Count terms in a string separated by delimiters
unsigned int count_terms(char* s, char* delim) {
  char tmp[MAX_BUF_LEN];
  strcpy(tmp, s);  // clone string to preserve original
  int terms = 0;

  char* token = strtok(tmp, delim);
  while (token) {  // extract until last token
    terms++;
    token = strtok(0, delim);
  }
  return terms;
}

// Check if a char exists inside a string
int isin(char c, char* s) {
  while (*s != '\0') {
    if (c == *s) {  // found
      return 1;
    }
    s++;
  }
  return 0;
}

// Convert a char to lowercase
char tolower(char c) {
  if (c >= 'A' && c <= 'Z') {  // check if letter
    c = 'a' + (c - 'A');
  }
  return c;
}

// Check if a char is a digit 0-9
int isdigit(char c) {
  return (c >= '0' && c <= '9');
}

// Check if a string is a number (all chars are digits)
int isnum(const char* s) {
  char* p = (char*)s;
  while (*p != '\0') {
    if (!isdigit(*p)) return 0;  // invalid
    p++;
  }
  return 1;
}

// Convert a string to integer
unsigned int atoi(const char* s) {
  int num = 0;
  int i = 0;

  while (s[i] && isdigit(s[i])) {
    num = num * 10 + (s[i] - '0');
    i++;
  }
  return num;
}

// Convert a HEX string to integer
unsigned int atoi_b16(char* hex) {
  int val = 0;
  while (*hex) {
    char byte = *hex++;  // get current digit
    // Transform HEX digit to 4-bit equivalent number
    if (byte >= '0' && byte <= '9')
      byte = byte - '0';
    else if (byte >= 'a' && byte <= 'f')
      byte = byte - 'a' + 10;
    else if (byte >= 'A' && byte <= 'F')
      byte = byte - 'A' + 10;

    // Shift 4 to make space and add the 4 bits of the new digit
    val = (val << 4) | (byte & 0xF);
  }
  return val;
}
