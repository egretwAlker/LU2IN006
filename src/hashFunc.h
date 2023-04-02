#ifndef HASHFUNC
#define HASHFUNC
void hashFile(const char* source, const char* dest);
char* sha256file(const char* file);
char* sha256string(const char* s);
#endif