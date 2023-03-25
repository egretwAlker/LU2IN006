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
Cell* buildCell(char* s) {
  Cell* res = malloc(sizeof(Cell));
  res->data = strdup(s);
  res->next = NULL;
  return res;
}

void insertFirst(List* l, Cell* c) {
  c->next = *l;
  *l = c;
}

void insertFirstString(List* l, char* s) {
  insertFirst(l, buildCell(s));
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
Cell* searchList(List* l, char* s) {
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

void ltof(List* l, char* path) {
  char *s = ltos(l);
  s2f(s, path);
}

List* ftol(char* path) {
  char* s = f2s(path);
  List* res = stol(s);
  free(s);
  return res;
}

void freeCell(Cell* c) {
  free(c->data);
  free(c);
}

void freeList(List *l) {
  for(Cell *c=*l, *p; c; c=p) {
    p=c->next;
    freeCell(c);
  }
  free(l);
}
