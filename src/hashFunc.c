#include <unistd.h>
#include <stdlib.h>
#include "misc.h"
#include "fsop.h"
#include <string.h>

/**
 * @brief Use sha256sum to hash source and save to dest;
 * @return int; -1 if failed
 */
int hashFile(const char* source, const char* dest) {
  char cmd[MAXL] = "sha256sum ";
  append(cmd, source);
  append(cmd, " | awk '{printf \"%s\",$1}' > ");
  append(cmd, dest);
  if(system(cmd)) {
    err("%s failed\n", cmd);
    return -1;
  }
  return 0;
}

/**
 * @brief Return the sha256sum result of file
 */
char* sha256file(const char* file) {
  char* tmp = createTemp();
  hashFile(file, tmp);
  char* s = fts(tmp);
  free(tmp);
  return s;
}

char* sha256string(const char* s) {
  char* fname = createTemp();
  s2f(s, fname);
  char* res = sha256file(fname);
  remove(fname);
  free(fname);
  return res;
}

/**
 * @param h 
 * @return h != NULL && strlen(h) > 0
 */
int hashValid(const char* h) {
  return h != NULL && strlen(h) > 0;
}