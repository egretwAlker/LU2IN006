#ifndef DIROP
#define DIROP
#include "cellList.h"
// We handle only text based files
List* listdir(char* root_dir);
int file_exists(char* file);
void cp(char* dest, char* src);
char* hashToPath(char* hash);
void blobFile(char* file);
void blobFileExt(char* file);
char* createTemp();
int getChmod(const char* path);
int isDir(const char * path);
#endif