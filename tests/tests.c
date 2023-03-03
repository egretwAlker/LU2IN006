#include <stdio.h>
#include <assert.h>
#include "../src/cellList.h"
#include "../src/fsop.h"
#include "../src/misc.h"
#include "../src/hashFunc.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void test0() {
  char *s = sha256file("test1.txt");
  err("%s\n", s);
  free(s);
}

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

void test3() {
  char* s = hashToPath("abcdef");
  err("%s\n", s);
  assert(strcmp(s, "ab/cdef") == 0);
  free(s);
}

void test4() {
  blobFile("test3.txt");
}

int main(void) {
  // test0();
  // test1();
  // test2();
  // test3();
  test4();
  return 0;
}