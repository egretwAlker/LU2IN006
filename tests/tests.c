#include <stdio.h>
#include <assert.h>
#include "../src/cellList.h"
#include "../src/fsop.h"
#include "../src/misc.h"
#include "../src/hashFunc.h"
#include "../src/workTree.h"
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
  clearList(l);
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
  assert(strcmp(s, ".mygit/ab/cdef") == 0);
  free(s);
}

void test4() {
  blobFile("test3.txt");
}

void test5() {
  char t[] = "name\thash\t123";
  WorkFile* wf = stwf(t);
  char *s = wfts(wf);
  err("%s", s);
  assert(strcmp(s, t) == 0);
  clearWf(wf);
  free(s);
}

void test6() {
  char t[] = "name\thash\t123\nname1\thash1\t1234\n";
  WorkTree* wt = stwt(t);
  char *s = wtts(wt);
  err("%s", s);
  assert(strcmp(s, t) == 0);
  clearWt(wt);
  free(s);
}

void test7() {
  List* l = listdir(".");
  char* s = ltos(l);
  err("%s\n", s);
  free(s);
  clearList(l);
}

void test8() {
  err("%d %d\n", isDir("dc"), isDir("test1.txt"));
}

void test9() {
  WorkTree* newWt = getWtFromPath(".");
  char *s = saveWorkTree(newWt, ".");
  free(s);
  clearWt(newWt);
}

int main(void) {
  // test3();
  // test7();
  test9();
  return 0;
}