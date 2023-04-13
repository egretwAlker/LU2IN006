#ifndef CELLLIST
#define CELLLIST
typedef struct cell Cell;
struct cell {
  char* data;
  Cell* next;
};
typedef Cell* List;

/*
Data format:
  Char array (string) should end with '\0'
  A list must not be NULL; the order of the cells in a list is not important (à vérifier)
*/

List* initList();
Cell* buildCell(const char* s);
void insertFirst(List* l, Cell* c);
void insertFirstString(List* l, const char* s);
List* filterList(const List* l, const char* pattern);
char* ctos(const Cell* c);
char* ltos(const List* l);
Cell* listGet(List* l, int k);
Cell* searchList(const List* l, const char* s);
int listSize(const List* l);
List* stol(const char* s);
void ltof(const List* l, const char* path);
List* ftol(const char* path);
void freeCell(Cell* c);
void freeList(List *l);
void extendUnique(List* a, List* b);
#endif
