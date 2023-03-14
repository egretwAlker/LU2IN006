#include <unistd.h>
#include <stdlib.h>
#include "misc.h"
#include "fsop.h"
#include <string.h>

void append(char* s, const char* t) {
  while(*s) ++s;
  while(*t) {
    *s = *t;
    ++s; ++t;
  }
  *s = 0;
}

// Est-il possible de ne pas utiliser system, ce qui est préférable par l'énoncé?
/**
 * @brief Use sha256sum to hash source and save to dest;
 * 
 * @param source 
 * @param dest 
 * @return int; -1 if failed
 */
int hashFile(char* source, char* dest) {
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
 * 
 * @param file 
 * @return char* 
 */
char* sha256file(char* file) {
  char* fname = createTemp();
  hashFile(file, fname);

  FILE* f = fopen(fname, "r");
  char buf[MAXL];
  fgets(buf, MAXL, f);
  char* res = strdup(buf);

  if(fclose(f)) err("File closing error\n");
  if(remove(fname)) err("File removing error\n"); 
  free(fname);
  return res;
}