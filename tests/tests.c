#include <stdio.h>
#include <assert.h>
#include "../src/cellList.h"
#include "../src/fsop.h"
#include "../src/misc.h"
#include <assert.h>
#include <stdlib.h>

void test1() {
  List* l = ftol("test1.txt");
  ltof(l, "test2.txt");
  cleanList(l);
  err("Check the content of test1.txt and test2.txt");
}

void test2() {
  err("nonexisting check: (");
  cp("missingfile", "missingfile");
  err(")");
  cp("test4.txt", "test3.txt");
  assert(system("diff test3.txt test4.txt") == 0);
}

int main(void) {
  // test1();
  test2();
  return 0;
}
