#include "fsop.h"
#include "misc.h"
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include "hashFunc.h"
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

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
      if(!strcmp(ep->d_name, ".") || !strcmp(ep->d_name, "..") || !strcmp(ep->d_name, SPFLDR)) continue;
      // err("%s, %d\n", ep->d_name, ep->d_type);
      insertFirstString(res, ep->d_name);
    }
  }
  closedir(dp);
  return res;
}

/**
 * @brief This function doesn't go deeper
 * 
 * @param file 
 * @return int 1 if file (can be a folder) exists at the depth 1
 */
int file_exists(char* file) {
  List* l = listdir(".");
  int res = (searchList(l, file) != NULL);
  clearList(l);
  return res;
}

void cp(char* dest, char* src) {
  FILE *d = fopen(dest, "w"), *s = fopen(src, "r");
  if(d == NULL) {
    err("Error when trying to open the file to write");
  }
  if(s == NULL) {
    err("Error when trying to open the file to read");
  }
  char buf[MAXL];
  while(fgets(buf, MAXL, s)) {
    fprintf(d, "%s", buf);
  }
  fclose(d);
  fclose(s);
}

/**
 * @brief Return '.mygit/??/..' from '??..'
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
  char* s = malloc(sizeof(char)*(len+SPL+2));
  strcpy(s, SPFLDR);
  s[SPL] = '/';
  s[SPL+1] = hash[0];
  s[SPL+2] = hash[1];
  s[SPL+3] = '/';
  strcpy(s+SPL+4, hash+2);
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
  path[SPL+3] = 0;
  char cmd1[MAXL] = "mkdir -p ";
  append(cmd1, path);
  system(cmd1);
  path[SPL+3] = '/';
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
  path[SPL+3] = 0;
  char cmd1[MAXL] = "mkdir -p ";
  append(cmd1, path);
  system(cmd1);
  path[SPL+3] = '/';
  strcpy(cmd1, path);
  append(cmd1, ".t");
  cp(cmd1, file);
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

int getChmod(const char * path) {
    struct stat ret;

    if(stat(path, &ret) == -1) {
        return -1;
    }

    // Copied from the poly, but Wouldn't that be ret.st_mode & (...|...)
    return ( ret.st_mode & S_IRUSR ) | ( ret.st_mode & S_IWUSR ) | ( ret.st_mode & S_IXUSR ) |
    /*owner*/
    ( ret.st_mode & S_IRGRP ) | ( ret.st_mode & S_IWGRP ) | ( ret.st_mode & S_IXGRP ) |
    /*group*/
    ( ret.st_mode & S_IROTH ) | ( ret.st_mode & S_IWOTH ) | ( ret.st_mode & S_IXOTH );
    /*other*/
}

int isDir(const char * path) {
    struct stat ret;

    if(stat(path, &ret) == -1) {
        return -1;
    }

    return (ret.st_mode & S_IFDIR) != 0;
}