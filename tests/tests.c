#include "../src/cellList.h"
#include "../src/fsop.h"
#include "../src/misc.h"
#include "../src/hashFunc.h"
#include "../src/workTree.h"
#include "../src/gestionCommits.h"

void test0() {
  char *s = sha256file("test1.txt");
  err("%s\n", s);
  free(s);
}

void test1() {
  List* l = ftol("test1.txt");
  ltof(l, "test2.txt");
  freeList(l);
  err("Check the content of test1.txt and test2.txt");
}

void test2() {
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
  freeWf(wf);
  free(s);
}

void test6() {
  char t[] = "name\thash\t123\nname1\thash1\t1234\n";
  WorkTree* wt = stwt(t);
  char *s = wtts(wt);
  err("%s", s);
  assert(strcmp(s, t) == 0);
  freeWt(wt);
  free(s);
}

void test7() {
  List* l = listdir(".");
  char* s = ltos(l);
  err("%s\n", s);
  free(s);
  freeList(l);
}

void test8() {
  err("%d %d\n", isDir("dc"), isDir("test1.txt"));
}

void test9() {
  WorkTree* newWt = getWtFromPath(".");
  char *s = saveWorkTree(newWt, ".");
  free(s);
  freeWt(newWt);
}

void test10() {
  char s[] = "123: 456";
  kvp* k = stkv(s);
  char* t = kvts(k);
  err("s: %s; t: %s;\n", s, t);
  assert(strcmp(s, t) == 0);
  freeKeyVal(k);
  free(t);
}

void test11() {
  Commit* c = createCommit("123");
  commitSet(c, "olex", "hovi");
  commitSet(c, "olex", "adin");
  char* s = cts(c);
  err("[%s]\n", s);
  freeCommit(c);
  Commit* a = stc(s);
  char* t = cts(a);
  err("[%s]\n", t);
  free(s);
  free(t);
  freeCommit(a);
}

void test12() {
  Commit* c = createCommit("123");
  commitSet(c, "al", "hov");
  ctf(c, "ctftest");
  Commit* c1 = ftc("ctftest");
  char *s = cts(c), *t = cts(c1);
  err("%s\n", s);
  assert(strcmp(s, t) == 0);
  free(s); free(t);
  freeCommit(c);
  freeCommit(c1);
}

void test13() {
  Commit* c = createCommit("thehash");
  free(blobCommit(c));
  freeCommit(c);
}

void test14() {
  WorkTree* newWt = getWtFromPath(".");
  char *s = saveWorkTree(newWt, ".");
  free(s);
  system("rm -r fdt");
  restoreWorkTree(newWt, ".");
  freeWt(newWt);
}

void test15() {
  initRefs();
  createUpdateRef("main", "gank");
  char* s = getRef("main");
  err("%s\n", s);
  free(s);
}

int main(void) {
  test15();
  return 0;
}