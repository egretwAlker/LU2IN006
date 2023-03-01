#include <stdio.h>
#include <assert.h>
#include "../src/cellList.h"

void test1() {
  List* l = ftol("tests/test1.txt");
  ltof(l, "tests/test2.txt");
  cleanList(l);
}

int main(void) {
  test1();
  return 0;
}
