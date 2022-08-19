// ----------------------------------- string.h -------------------------------------
/*
  STRING HELPER FUNCTIONS
 */

#define BUF_LEN 256  // buffer length

void clrstr(char* s);

char* strtrim(char* s);
char* strcpy(char* des, const char* src);
char* strtok(char* s, char* delim);
int strcmp(char* s1, char* s2);
int isin(char c, char* s);
char tolower(char c);

unsigned int strlen(char* s);
unsigned int count_terms(char* s, char* delim);

int isdigit(char c);
int isnum(const char* s);
unsigned int atoi(const char* s);
unsigned int atoi_b16(char* hex);
