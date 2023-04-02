#include "gestionCommits.h"
#include "misc.h"
#include "fsop.h"
#include "workTree.h"

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
 * @brief Create a Commit object with "tree":hash init
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
  char buff[MAXL];
  sprintf(buff, "%s/.refs", SPFLDR);
  if (!file_exists(buff)){
    sprintf(buff, "mkdir %s/.refs", SPFLDR);
    system(buff);
    sprintf(buff, "touch %s/.refs/master", SPFLDR);
    system(buff);
    sprintf(buff, "touch %s/.refs/HEAD", SPFLDR);
    system(buff);
  }
}

void createUpdateRef(char* ref_name, char* hash) {
  char buff[MAXL];
  sprintf(buff, "echo %s > %s/.refs/%s", hash, SPFLDR, ref_name);
  system(buff);
}

void deleteRef(char* ref_name) {
  char buff[MAXL];
  sprintf(buff, "%s/.refs/%s", SPFLDR, ref_name);
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
char *getRef(char* ref_name) {
  FILE *fp;
  char *result = (char*)malloc(sizeof(char)*MAXL);
  char buff[MAXL];
  sprintf(buff, "%s/.refs/%s",SPFLDR, ref_name );
  if (!file_exists(buff)){
    err("The reference %s does not exist\n", ref_name);
  }
  fp = fopen(buff, "r");
  assert(fp);
  fread(result, sizeof(char),MAXL, fp);
  fclose(fp);
  return result;
}

void createFile(char* file){
  char buff[MAXL];
  sprintf(buff, "touch %s/%s", SPFLDR, file);
  system(buff);
}

void myGitAdd(char* file_or_folder){
  WorkTree* wt;
  char buff[MAXL];
  sprintf(buff, "%s/.add", SPFLDR);
  if (!file_exists(buff)){
    createFile(buff);
    wt = initWorkTree();
  } else {
    wt = ftwt(buff);
  }
  assert(file_exists(file_or_folder));
  appendWorkTree(wt, file_or_folder, NULL, 0);
  wttf(wt, buff);
  freeWt(wt);
}

void myGitCommit(char* branch_name, char* message){
  char buff[MAXL];
  sprintf(buff, "%s/.refs", SPFLDR);
  if(!file_exists(buff)){
    err("Il faut d'abord initialiser les references du projets\n");
    return;
  }
  char buff2[MAXL];
  sprintf(buff2, "%s/.refs/%s", SPFLDR, branch_name);
  if(!file_exists(buff2)){
    err("La branche n'existe pas\n");
    return;
  }
  char* last_hash = getRef(branch_name);
  char* head_hash = getRef("HEAD");
  if(strcmp(last_hash, head_hash)!=0){
    err("HEAD doit pointer sur le dernier commit de la branche");
    return;
  }
  char buff3[MAXL];
  sprintf(buff3, "%s/.add", SPFLDR);
  WorkTree* wt = ftwt(buff3);
  char buff4[MAXL];
  sprintf(buff4, "%s/.", SPFLDR);
  char *hashwt = saveWorkTree(wt, buff4);
  Commit* c = createCommit(hashwt);
  if(strlen(last_hash)!=0){
    commitSet(c, "predecessor", last_hash);
  } 
  if(message != NULL){
    commitSet(c, "message", message);
  }
  char* hashc = blobCommit(c);
  createUpdateRef(branch_name, hashc);
  createUpdateRef("HEAD", hashc);
  assert(!remove(buff3));
  free(wt);
}