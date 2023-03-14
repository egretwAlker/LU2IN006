#include "fsop.h"
#include "misc.h"
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include "hashFunc.h"
#include <unistd.h>

/**
 * @brief Store all names of files and directories in a list which is then returned
 *
 * @param root_dir 
 * @return List* 
 */
List* listdir(char* root_dir) {
  List* res = initList();
  DIR* dp = opendir(root_dir);
  struct dirent *ep;
  if(dp != NULL) {
    while((ep = readdir(dp)) != NULL) {
      if(!strcmp(ep->d_name, ".") || !strcmp(ep->d_name, "..")) continue;
      // err("%s, %d\n", ep->d_name, ep->d_type);
      insertFirstString(res, ep->d_name);
    }
  }
  closedir(dp);
  return res;
}

int file_exists(char* file) {
  List* l = listdir(".");
  int res = (searchList(l, file) != NULL);
  clearList(l);
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

/**
 * @brief Return '??/*' from '??*'
 * 
 * @param hash 
 * @return char* 
 */
char* hashToPath(char* hash) {
  int len = strlen(hash);
  if(len < 3) {
    err("Hash too short error");
    return NULL;
  }
  char* s = malloc(sizeof(char)*(len+2));
  s[0] = hash[0];
  s[1] = hash[1];
  s[2] = '/';
  strcpy(s+3, hash+2);
  return s;
}

/**
 * @brief Save the snapshot of file
 * 
 * @param file 
 */
void blobFile(char* file) {
  char* hash = sha256file(file);
  char* path = hashToPath(hash);
  // err("(%s)", hash);
  path[2] = 0;
  char cmd1[MAXL] = "mkdir -p ";
  append(cmd1, path);
  system(cmd1);
  path[2] = '/';
  cp(path, file);
  free(hash);
  free(path);
}

/**
 * @brief Save the snapshot of file with extension .t
 * 
 * @param file 
 */
void blobFileExt(char* file) {
  char* hash = sha256file(file);
  char* path = hashToPath(hash);
  // err("(%s)", hash);
  path[2] = 0;
  char cmd1[MAXL] = "mkdir -p ";
  append(cmd1, path);
  system(cmd1);
  path[2] = '/';
  strcpy(cmd1, path);
  append(cmd1, ".t");
  cp(path, file);
  free(hash);
  free(path);
}

/**
 * @brief Create a temporary file in /tmp
 * 
 * @return char* the name of the temporary file, starting by /tmp
 */
char* createTemp() {
  static const char template[] = "/tmp/iamhoviadinXXXXXX";
  char* fname = strdup(template);
  int fd = mkstemp(fname);
  close(fd);
  return fname;
}