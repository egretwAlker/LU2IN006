#include "gestionCommits.h"
#include "misc.h"
#include "fsop.h"
#include "workTree.h"
#include "hashFunc.h"

/**
 * @brief From http://www.cse.yorku.ca/~oz/hash.html
 */
static unsigned long sdbm(char *s) {
  unsigned char *str = (unsigned char*)s;
  unsigned long hash = 0;
  int c;

  while ((c = *str++))
    hash = (unsigned long)c + (hash << 6) + (hash << 16) - hash;

  return hash;
}

kvp* createKeyVal(char* key, char* val) {
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
    if(c->T[i] != NULL) {
      free(c->T[i]->key);
      free(c->T[i]->value);
      free(c->T[i]);
    }
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
kvp* getKvpByKey(Commit* c, char* key) {
  assert(c != NULL); assert(key != NULL);
  int h = (int)(sdbm(key)%(szt)COMMITN);
  while(c->T[h] != NULL && strcmp(key, c->T[h]->key)) h = (h+1)%COMMITN;
  if(c->T[h] == NULL) c->T[h] = createKeyVal(key, "");
  return c->T[h];
}

/**
 * @brief Add key:value pair into Commit c
 */
void commitSet(Commit* c, char* key, char* value) {
  assert(c != NULL); assert(key != NULL);
  kvp* k = getKvpByKey(c, key);
  free(k->value);
  k->value = strdup(value);
}

/**
 * @brief Create a Commit object with "tree":hash in it
 */
Commit* createCommit(char* hash) {
  Commit* c = initCommit();
  commitSet(c, "tree", hash);
  return c;
}

/**
 * @brief Get the corresponding value of key
 * 
 * @return char* NULL if the key is not present
 */
char* commitGet(Commit* c, char* key) {
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

void ctf(Commit* c, char* file) {
  char* s = cts(c);
  s2f(s, file);
  free(s);
}

Commit* ftc(char* file) {
  char* s = f2s(file);
  Commit* c = stc(s);
  free(s);
  return c;
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
void createUpdateRef(char* ref_name, char* hash) {
  char buff[MAXL];
  sprintf(buff, "%s/%s", REF, ref_name);
  stf(hash, buff);
}

void deleteRef(char* ref_name) {
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
char* getRef(char* ref_name) {
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
void myGitAdd(char* fn) {
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
    appendWorkTree(wt, fn, "FOLDER", getChmod(fn));
  } else {
    char* s = sha256file(fn);
    appendWorkTree(wt, fn, s, getChmod(fn));
    free(s);
  }
  wttf(wt, ADD);
  freeWt(wt);
}

void myGitCommit(char* branch_name, char* message) {
  if(!file_exists(REF)) {
    err("Il faut d'abord initialiser les references du projets\n");
    return;
  }
  if(!file_exists(ADD)) {
    err("Rien à ajouter\n");
    return;
  }
  char buf[MAXL];
  sprintf(buf, "%s/%s", REF, branch_name);
  if(!file_exists(buf)){
    err("La branche n'existe pas\n");
    return;
  }

  char* last_hash = getRef(branch_name);
  char* head_hash = getRef("HEAD");
  if(strcmp(last_hash, head_hash) == 0) {
    WorkTree* wt = ftwt(ADD);
    char* hashwt = saveWorkTree(wt, ".");
    Commit* c = createCommit(hashwt);
    if(strlen(last_hash) != 0)
      commitSet(c, "predecessor", last_hash);
    if(message != NULL)
      commitSet(c, "message", message);
    char* hashc = blobCommit(c);
    createUpdateRef(branch_name, hashc);
    createUpdateRef("HEAD", hashc);
    assert(remove(ADD) == 0);
    freeWt(wt);
    free(hashwt);
    free(hashc);
    freeCommit(c);
  } else err("HEAD doit pointer sur le dernier commit de la branche");
  free(last_hash);
  free(head_hash);
}