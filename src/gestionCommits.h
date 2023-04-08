#ifndef GESTIONCOMMITS
#define GESTIONCOMMITS
#include "cellList.h"
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
kvp* createKeyVal(const char* key, const char* val);
void freeKeyVal(kvp* kv);
void freeCommit(Commit* c);
char* kvts(const kvp* k);
kvp* stkv(const char* s);
Commit* stc(const char* s);
char* cts(Commit* c);
const char* commitGet(Commit* c, const char* key);
Commit* createCommit(const char* hash);
kvp* getKvpByKey(Commit* c, const char* key);
Commit* initCommit();
void commitSet(Commit* c, const char* key, const char* value);
void ctf(Commit* c, const char* file);
Commit* ftc(const char* file);
Commit* htc(const char* hash);
char* blobCommit(Commit* c);
void initRefs();
void createUpdateRef(const char* ref_name, const char* hash);
void deleteRef(const char * ref_name);
char* getRef(const char* ref_name);
void myGitAdd(const char* file_or_folder);
char* getCurrentBranch();
int branchExists(const char* branch);
void createBranch(const char* branch);
List* branchList(const char* branch);
void printBranch(const char* branch);
void initBranch();
#endif