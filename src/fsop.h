#ifndef DIROP
#define DIROP
#include "cellList.h"
List* listdir(char* root_dir);
int file_exists(char* file);
void cp(char* dest, char* src);
char* hashToPath(char* hash);
void blobFile(char* file);
#endif