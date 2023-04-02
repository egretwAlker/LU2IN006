#ifndef GESTIONCOMMITS
#define GESTIONCOMMITS
// reference files can be non existing, empty (point to none), or non empty
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
void freeCommit(Commit* c);
char* kvts(const kvp* k);
kvp* stkv(const char* s);
Commit* stc(const char* s);
char* cts(Commit* c);
char* commitGet(Commit* c, char* key);
Commit* createCommit(char* hash);
kvp* getKvpByKey(Commit* c, char* key);
Commit* initCommit();
void commitSet(Commit* c, char* key, char* value);
void ctf(Commit* c, char* file);
Commit* ftc(char* file);
char* blobCommit(Commit* c);
void initRefs();
void createUpdateRef(char* ref_name, char* hash);
void deleteRef(char * ref_name);
char* getRef(char* ref_name);
void myGitAdd(char* file_or_folder);
void myGitCommit(char* branch_name, char* message);
#endif