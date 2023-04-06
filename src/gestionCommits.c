#include "gestionCommits.h"
#include "misc.h"
#include "fsop.h"
#include "workTree.h"
#include "hashFunc.h"

/**
 * @brief From http://www.cse.yorku.ca/~oz/hash.html
 */
static unsigned long sdbm(const char *s) {
  unsigned char *str = (unsigned char*)s;
  unsigned long hash = 0;
  int c;

  while ((c = *str++))
    hash = (unsigned long)c + (hash << 6) + (hash << 16) - hash;

  return hash;
}

kvp* createKeyVal(const char* key, const char* val) {
  assert(key != NULL); assert(val != NULL);
  kvp* k = (kvp*)malloc(sizeof(kvp));
  k->key = strdup(key);
  k->value = val!=NULL?strdup(val):NULL;
  return k;
}

void freeKeyVal(kvp* kv) {
  free(kv->key);
  free(kv->value);
  free(kv);
}

void freeCommit(Commit* c) {
  for(int i=0;i<COMMITN;++i) {
    if(c->T[i] != NULL)
      freeKeyVal(c->T[i]);
  }
  free(c->T);
  free(c);
}

/**
 * @brief Convert an element to a string in form "key: value"
 * 
 * @param k 
 * @return char* 
 */
char* kvts(const kvp* k) {
  char buf[MAXL];
  strcpy(buf, k->key);
  append(buf, ": ");
  append(buf, k->value);
  return strdup(buf);
}

/**
 * @brief Convert a string to a kvp element
 */
kvp* stkv(const char* s) {
  kvp* k = (kvp*)malloc(sizeof(kvp));
  int len = (int)strlen(s);
  for(int i=0;i<len;++i) {
    if(s[i] == ':') {
      k->key = strndup(s, (szt)i);
      k->value = strndup(s+i+2, (szt)(len-i-2));
    }
  }
  return k;
}

/**
 * @brief Get a new Commit object, the maximum of size is specified by COMMITN
 */
Commit* initCommit() {
  Commit* c = (Commit*)malloc(sizeof(Commit));
  c->size = COMMITN;
  c->n = 0;
  c->T = (kvp**)malloc(sizeof(kvp*)*(szt)(c->size));
  memset(c->T, 0, sizeof(kvp*)*(szt)(c->size));
  return c;
}

/**
 * @brief Get the pointer to the corresponding kvp object
 */
kvp* getKvpByKey(Commit* c, const char* key) {
  assert(c != NULL); assert(key != NULL);
  int h = (int)(sdbm(key)%(szt)COMMITN);
  while(c->T[h] != NULL && strcmp(key, c->T[h]->key)) h = (h+1)%COMMITN;
  if(c->T[h] == NULL) c->T[h] = createKeyVal(key, "");
  return c->T[h];
}

/**
 * @brief Add key:value pair into Commit c
 */
void commitSet(Commit* c, const char* key, const char* value) {
  assert(c != NULL); assert(key != NULL);
  kvp* k = getKvpByKey(c, key);
  free(k->value);
  k->value = strdup(value);
}

/**
 * @brief Create a Commit object with "tree":hash in it
 */
Commit* createCommit(const char* hash) {
  Commit* c = initCommit();
  commitSet(c, "tree", hash);
  return c;
}

/**
 * @brief Get the corresponding value of key
 * 
 * @return char* NULL if the key is not present, not giving the ownership
 */
const char* commitGet(Commit* c, const char* key) {
  assert(c != NULL); assert(key != NULL);
  kvp* k = getKvpByKey(c, key);
  return k->value;
}

/**
 * @brief Get the string representation of Commit c, all the "(key, value)" seperated by carriage
 */
char* cts(Commit* c) {
  int tot = 1;
  for(int i=0;i<COMMITN;++i) {
    if(c->T[i] != NULL) {
      tot += (int)strlen(c->T[i]->key) + (int)strlen(c->T[i]->value) + 4;
    }
  }
  char* s = (char*)malloc(sizeof(char)*(szt)tot);
  char* t = s;
  for(int i=0;i<COMMITN;++i) if(c->T[i] != NULL) {
    sprintf(t, "(%s,%s)\n", c->T[i]->key, c->T[i]->value);
    t += strlen(t);
  }
  return s;
}

/**
 * @brief Get a Commit object from the string representation
 */
Commit* stc(const char* s) {
  char buf1[MAXL], buf2[MAXL];
  buf1[0] = buf2[0] = 0;
  Commit* c = initCommit();
  while(*s) {
    sscanf(s, "(%[^,],%s", buf1, buf2);
    buf2[strlen(buf2)-1] = 0;
    commitSet(c, buf1, buf2);
    s += 4+strlen(buf1)+strlen(buf2);
  }
  return c;
}

/**
 * @brief Commit to file
 */
void ctf(Commit* c, const char* file) {
  char* s = cts(c);
  s2f(s, file);
  free(s);
}

/**
 * @brief File to commit
 */
Commit* ftc(const char* file) {
  char* s = f2s(file);
  Commit* c = stc(s);
  free(s);
  return c;
}

/**
 * @brief Hash to commit
 */
Commit* htc(const char* hash) {
  assert(hashValid(hash));
  char* s = hashToPathExt(hash, ".c");
  Commit* res = ftc(s);
  free(s);
  return res;
}

/**
 * @brief Take a snapshot of Commit c with extension .c
 * @return char* the hash of the string representation of c
 */
char* blobCommit(Commit* c) {
  char* s = cts(c);
  char* hash = blobStringExt(s, ".c");
  free(s);
  return hash;
}

/**
 * @brief Initialise the folder as a repository with branch master
 */
void initRefs() {
  char buf[MAXL];
  if (!file_exists(REF)) {
    sprintf(buf, "mkdir -p %s", REF);
    system(buf);
    sprintf(buf, "touch %s/master", REF);
    system(buf);
    sprintf(buf, "touch %s/HEAD", REF);
    system(buf);
  }
}

/**
 * @brief Replace the content of ref_name by hash, create the ref file if non existing
 */
void createUpdateRef(const char* ref_name, const char* hash) {
  char buff[MAXL];
  sprintf(buff, "%s/%s", REF, ref_name);
  stf(hash, buff);
}

void deleteRef(const char* ref_name) {
  char buff[MAXL];
  sprintf(buff, "%s/%s", REF, ref_name);
  if (!file_exists(buff)){
    err("The reference %s does not exist\n", ref_name);
  } else {
    assert(remove(buff) == 0);
  }
}

/**
 * @brief
 * @return Content of ref_name, NULL if non existing; "" if empty
 */
char* getRef(const char* ref_name) {
  char buf[MAXL];
  sprintf(buf, "%s/%s", REF, ref_name);
  if (!file_exists(buf)){
    err("The reference %s does not exist\n", ref_name);
    return NULL;
  }
  return fts(buf);
}

/**
 * @brief Append fn to workTree represented by .add file
 * 
 * @param fn path to of a file or a folder
 */
void myGitAdd(const char* fn) {
  if(file_exists(fn) == 0) {
    err("File does not exist\n");
    return;
  }
  WorkTree* wt;
  if (!file_exists(ADD)) {
    createFile(ADD);
    wt = initWorkTree();
  } else {
    wt = ftwt(ADD);
  }
  if(isDir(fn)) {
    appendWorkTree(wt, fn, "FOLDER", -1);
  } else {
    char* s = sha256file(fn);
    appendWorkTree(wt, fn, "FILE", -1);
    free(s);
  }
  wttf(wt, ADD);
  freeWt(wt);
}

void initBranch() {
  stf("master", CURB);
}

int branchExists(const char* branch) {
  List* refs = listdir(REF);
  if(strcmp(branch, "HEAD") == 0) return 0;
  int res = (searchList(refs, branch) != NULL);
  freeList(refs);
  return res;
}

/**
 * @brief Create a branch pointing to where the HEAD points
 */
void createBranch(const char* branch) {
  char* hash = getRef("HEAD");
  createUpdateRef(branch, hash);
  free(hash);
}

char* getCurrentBranch() {
  return fts(CURB);
}

/**
 * @brief Print the hash [and the message] of all the commits preceding (included) where the branch points
 */
void printBranch(const char* branch) {
  char* h = getRef(branch);
  while(hashValid(h)) {
    Commit* c = htc(h);
    if(commitGet(c, "message") != NULL)
      printf("%s -> %s\n", h, commitGet(c, "message"));
    else
      printf("%s\n", h);
    free(h);
    h = strdup(commitGet(c, "predecessor"));
    freeCommit(c);
  }
  free(h);
}

/**
 * @return The hashes of all the commits preceding (included) where the branch points
 */
List* branchList(const char* branch) {
  List* l = initList();
  char* h = getRef(branch);
  while(hashValid(h)) {
    Commit* c = htc(h);
    insertFirst(l, buildCell(h));
    free(h);
    h = strdup(commitGet(c, "predecessor"));
    freeCommit(c);
  }
  free(h);
  return l;
}

/**
 * @brief
 * @return The Hashes of all the commits (of all branches)
 */
List* getAllCommits() {
  List* l = initList();
  List* branches = listdir(REF);
  for(Cell* c = *branches; c; c=c->next) {
    extendUnique(l, branchList(c->data));
  }
  freeList(branches);
  return l;
}

void restoreCommit(const char* hash_commit) {
  Commit* c = htc(hash_commit);
  const char* hash_tree = commitGet(c, "tree");
  WorkTree* wt = htwt(hash_tree);
  restoreWorkTree(wt, ".");
  freeWt(wt);
  freeCommit(c);
}

// Does restoreCommit need to delete old files?
void myGitCheckoutBranch(const char* branch) {
  assert(branchExists(branch));
  stf(branch, CURB);
  char* h = getRef(branch);
  createUpdateRef("HEAD", h);
  // err("h : %s\n", h);
  if(hashValid(h)) restoreCommit(h);
  free(h);
}

/**
 * @brief Show information about commits whose hashes start by pattern
 */
void myGitCheckoutCommit(const char* pattern) {
  List* l = getAllCommits();
  List* filtered = filterList(l, pattern);
  int sz = listSize(filtered);
  if(sz == 0) {
    printf("Pattern not matched\n");
  } else if(sz > 1) {
    printf("Multiple matching found:\n");
    for(Cell* c = *filtered; c; c=c->next) {
      printf("-> %s\n", c->data);
    }
  } else if(sz == 1) {
    char* h = (*l)->data;
    createUpdateRef("HEAD", h);
    restoreCommit(h);
  }
  freeList(l);
  freeList(filtered);
}