#include "fsop.h"
#include "misc.h"
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

// Peut-etre il faut faire récursivement (rentrer dans tous les dossiers...)?
List* listdir(char* root_dir) {
  /*
    Store all names of files and directories in a list which is then returned
  */
  List* res = initList();
  DIR* dp = opendir(root_dir);
  struct dirent *ep;
  if(dp != NULL) {
    while((ep = readdir(dp)) != NULL) {
      if(!strcmp(ep->d_name, ".") || !strcmp(ep->d_name, "..")) continue;
      insertFirstString(res, ep->d_name);
    }
  }
  closedir(dp);
  return res;
}

int file_exists(char* file) {
  List* l = listdir(".");
  int res = (searchList(l, file) != NULL);
  cleanList(l);
  return res;
}

void cp(char* dest, char* src) {
  if(!file_exists(src)) {
    err("%s does not exist in the current directory", src);
    return;
  }
  FILE *d = fopen(dest, "w"), *s = fopen(src, "r");
  char buf[MAXL];
  while(fgets(buf, MAXL, s)) {
    fprintf(d, "%s", buf);
  }
  fclose(d);
  fclose(s);
}

char* hashToPath(char* hash) {
  /*
    Return '??/*' from '??*'
  */
  char* s = malloc(sizeof(char)*(strlen(hash)+2));
  s[0] = hash[0];
  s[1] = hash[1];
  s[2] = '/';
  strcpy(s+3, hash+2);
  return s;
}