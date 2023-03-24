#ifndef GESTIONCOMMITS
#define GESTIONCOMMITS
// key, value are never NULL
typedef struct {
  char* key;
  char* value;
} kvp;

typedef struct {
  kvp** T;
  int n;
  int size;
} HashTable;

typedef HashTable Commit;
kvp* createKeyVal(char* key, char* val);
void freeKeyVal(kvp* kv);
char* kvts(const kvp* k);
kvp* stkv(const char* s);
Commit* stc(const char* s);
char* cts(Commit* c);
char* commitGet(Commit* c, char* key);
Commit* createCommit(char* hash);
kvp* getKvpByKey(Commit* c, char* key);
Commit* initCommit();
void commitSet(Commit* c, char* key, char* value);
#endif