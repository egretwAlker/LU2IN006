#ifndef HASHFUNC
#define HASHFUNC
void hashFile(char* source, char* dest);
char* sha256file(char* file);
char* sha256string(char* s);
#endif