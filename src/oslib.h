// ----------------------------------- oslib.h -------------------------------------
/**
 * NON-STANDARD IMPLEMENTATIONS OF COMMON C FUNCTIONS
 * */

#define MAX_BUF_LEN 256

// <string.h> implementations
void clrstr(char* s);
char* strcpy(char* des, const char* src);
char* strtrim(char* s);
char* strtok(char* s, char* delim);
int strcmp(char* s1, char* s2);
unsigned int strlen(char* s);
unsigned int count_terms(char* s, char* delim);

// <ctype.h> implementations
int isin(char c, char* s);
char tolower(char c);
int isdigit(char c);
int isnum(const char* s);

// <stdlib.h> implementations
unsigned int atoi(const char* s);
unsigned int atoi_b16(char* hex);
