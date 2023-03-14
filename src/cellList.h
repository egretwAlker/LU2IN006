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
Cell* buildCell(char* s);
void insertFirst(List* l, Cell* c);
void insertFirstString(List* l, char* s);
char* ctos(Cell* c);
char* ltos(List* l);
Cell* listGet(List* l, int k);
Cell* searchList(List* l, char* s);
List* stol(const char* s);
void ltof(List* l, char* path);
List* ftol(char* path);
void clearCell(Cell* c);
void clearList(List *l);
#endif
