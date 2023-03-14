#include "cellList.h"
#include <stdio.h>
#include "misc.h"
#include <string.h>
#include <stdlib.h>

List* initList() {
  List* l = malloc(sizeof(List));
  *l = NULL;
  return l;
}

/**
 * @brief With strdup, create a cell representing s
 * 
 * @param s 
 * @return Cell* 
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
 * 
 * @param c 
 * @return char* 
 */
char* ctos(Cell* c) {
  return c->data;
}

/**
 * @brief Return the concatenation of strings of cells of l in the form "string1|string2|...|stringn"
 * 
 * @param l 
 * @return char* 
 */
char* ltos(List* l) {
  int len = 1;
  for(Cell* c=*l; c; c=c->next) {
    len += strlen(ctos(c))+1;
  }
  if(*l) --len;
  char* s = malloc(sizeof(char)*(len+1));
  char* t = s;
  for(Cell* c=*l; c; c=c->next) {
    strcpy(t, ctos(c));
    t += strlen(ctos(c));
    *(t++) = '|';
  }
  *t = 0;
  return s;
}

/**
 * @brief Return l[k] (counting from 0)
 * 
 * @param l 
 * @param k 
 * @return Cell* 
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
 * 
 * @param l 
 * @param s 
 * @return Cell* 
 */
Cell* searchList(List* l, char* s) {
 if(l == NULL) return NULL;
 for(Cell *c = *l; c; c=c->next) if(strcmp(c->data, s)==0) return c;
 return NULL;
}

/**
 * @brief Return a list representing s
 * 
 * @param s 
 * @return List* 
 */
List* stol(const char* s) {
  char* s1 = strdup(s);
  List* l = initList();
  char* x = s1;
  int len = strlen(s);
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
  FILE *f = fopen(path, "w");
  char *s = ltos(l);
  fprintf(f, "%s", s);
  free(s);
  fclose(f);
}

List* ftol(char* path) {
  FILE *f = fopen(path, "r");
  char buf[MAXL];
  fscanf(f, "%s", buf);
  List* res = stol(buf);
  fclose(f);
  return res;
}

void clearCell(Cell* c) {
  free(c->data);
  free(c);
}

void clearList(List *l) {
  for(Cell *c=*l, *p; c; c=p) {
    p=c->next;
    clearCell(c);
  }
  free(l);
}
