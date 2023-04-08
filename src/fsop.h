#ifndef DIROP
#define DIROP
#include "cellList.h"
// We handle only text based files
List* listdir(const char* root_dir);
int file_exists(const char* file);
void cp(const char* dest, const char* src);
char* hashToPath(const char* hash);
char* hashToPathExt(const char* hash, const char* ext);
void blobFile(const char* file);
char* blobFileExt(const char* file, const char* ext);
char* blobStringExt(const char* file, const char* ext);
char* createTemp();
int getChmod(const char* path);
void setMode(int mode, const char * path);
int isDir(const char * path);
char* fts(const char* fn);
void stf(const char* s, const char* fn);
void createFile(const char* file);
char* concatPaths(const char* p1, const char* p2);
#endif