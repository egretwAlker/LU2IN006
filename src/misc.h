#ifndef MISC
#define MISC
#include<stdio.h>
#define err(...) fprintf(stderr, __VA_ARGS__)
#define MAXL 2333
#define WTS 100
#define SPFLDR ".mygit"
#define SPL ((int)strlen(SPFLDR))
#define COMMITN 23333
void append(char* s, const char* t);
typedef unsigned long ul;
typedef long unsigned int szt;
#endif
