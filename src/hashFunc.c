#include <unistd.h>
#include <stdlib.h>
#include "debug.h"
#include <string.h>
#define CMDL 233

void append(char* s, const char* t) { while(*s) ++s;
  while(*t) {
    *s = *t;
    ++s; ++t;
  }
  *s = 0;
}

// Dans l'énoncé, on demande de retourner une valeur int, pourquoi?
// Est-il possible de ne pas utiliser system, ce qui est préférable par l'énoncé?
void hashFile(char* source, char* dest) {
  /*
    Use sha256sum to hash source and save to dest
  */
  char cmd[CMDL] = "sha256sum ";
  append(cmd, source);
  append(cmd, " > ");
  append(cmd, dest);
  if(system(cmd)) err("%s failed\n", cmd);
}

// Il faut utiliser un pipe, une redirection, un fichier temporaire. Pipe je n'ai pas l'utilisé.
char* sha256file(char* file) {
  /*
    Return the sha256sum result of file
  */
  static char template[] = "/tmp/iamhoviadinXXXXXX";
  char fname[CMDL];
  strcpy(fname, template);
  int fd = mkstemp(fname);
  hashFile(file, fname);

  FILE* f = fdopen(fd, "r");
  char buf[CMDL];
  fgets(buf, CMDL, f);
  char* res = strdup(buf);

  if(fclose(f)) err("File closing error\n");
  if(remove(fname)) err("File removing error\n"); 
  return res;
}

int main(int argn, char** args) {
  char *s = sha256file("debug.h");
  err("%s\n", s);
  free(s);
  return 0;
}
