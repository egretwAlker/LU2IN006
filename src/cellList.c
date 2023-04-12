#include "cellList.h"
#include "misc.h"

List* initList() {
  List* l = malloc(sizeof(List));
  *l = NULL;
  return l;
}

/**
 * @brief With strdup, create a cell representing s
 */
Cell* buildCell(const char* s) {
  Cell* res = malloc(sizeof(Cell));
  res->data = strdup(s);
  res->next = NULL;
  return res;
}

/**
 * @brief Absorb the ownership of c
 */
void insertFirst(List* l, Cell* c) {
  c->next = *l;
  *l = c;
}

void insertFirstString(List* l, const char* s) {
  insertFirst(l, buildCell(s));
}

/**
 * @brief insertFirst every element of b to a when it's datum doesn't exist there, take the ownership of b
 */
void extendUnique(List* a, List* b) {
  for(Cell *c = *b, *t; c; c = t) {
    t = c->next;
    if(searchList(a, c->data) == NULL) insertFirst(a, c);
    else freeCell(c);
  }
  free(b);
}

/**
 * @brief Don't take the ownership of elements in l
 * @return List* within are all the elements in l starting by pattern
 */
List* filterList(const List* l, const char* pattern) {
  szt np = strlen(pattern);
  List* res = initList();
  for(const Cell* c = *l; c; c = c->next) {
    if(strncmp(c->data, pattern, np) == 0)
      insertFirst(res, buildCell(c->data));
  }
  return res;
}

/**
 * @return How many elements in l
 */
int listSize(const List* l) {
  assert(l != NULL);
  int cnt = 0;
  for(const Cell* c = *l; c; c = c->next) ++cnt;
  return cnt;
}

/**
 * @brief Without strdup, return the string represented by c
 */
char* ctos(Cell* c) {
  return c->data;
}

/**
 * @brief Return the concatenation of strings of cells of l in the form "string1|string2|...|stringn"
 */
char* ltos(List* l) {
  int len = 1;
  for(Cell* c=*l; c; c=c->next) {
    len += (int)strlen(ctos(c));
    if(c->next) ++len;
  }
  char* s = malloc(sizeof(char)*(szt)len);
  char* t = s;
  for(Cell* c=*l; c; c=c->next) {
    strcpy(t, ctos(c));
    t += strlen(t);
    if(c->next) *(t++) = '|';
  }
  *t = 0;
  return s;
}

/**
 * @brief Return l[k] (counting from 0)
 */
Cell* listGet(List* l, int k) {
  Cell *c = *l;
  while(k && c) {
    c = c->next;
    --k;
  }
  if(c == NULL) err("index out of range");
  return c;
}

/**
 * @brief Return the first cell of which the data equals to s or NULL if not found.
 */
Cell* searchList(List* l, const char* s) {
 if(l == NULL) return NULL;
 for(Cell *c = *l; c; c=c->next) if(strcmp(c->data, s)==0) return c;
 return NULL;
}

/**
 * @brief Return a list representing s
 */
List* stol(const char* s) {
  char* s1 = strdup(s);
  List* l = initList();
  char* x = s1;
  int len = (int)strlen(s);
  for(int i=0;i<=len;++i) {
    if(s1[i]=='|' || !s1[i]) {
      s1[i] = 0;
      insertFirst(l, buildCell(x));
      x = s1+i+1;
    }
  }
  free(s1);
  return l;
}

void ltof(List* l, const char* path) {
  char *s = ltos(l);
  s2f(s, path);
}

List* ftol(const char* path) {
  char* s = f2s(path);
  List* res = stol(s);
  free(s);
  return res;
}

void freeCell(Cell* c) {
  if(c==NULL) return;
  free(c->data);
  free(c);
}

void freeList(List *l) {
  if(l==NULL) return;
  for(Cell *c=*l, *p; c; c=p) {
    p=c->next;
    freeCell(c);
  }
  free(l);
}
