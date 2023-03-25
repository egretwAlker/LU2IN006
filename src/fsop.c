#include "fsop.h"
#include "misc.h"
#include <dirent.h>
#include "hashFunc.h"
#include <unistd.h>
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
  freeList(l);
  return res;
}

void cp(char* dest, char* src) {
  FILE *d = fopen(dest, "w"), *s = fopen(src, "r");
  assert(d); assert(s);
  char buf[MAXL];
  int n = (int)fread(buf, sizeof(char), MAXL, s);
  fwrite(buf, sizeof(char), (szt)n, d);
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
  int len = (int)strlen(hash);
  if(len < 3) {
    err("Hash too short error");
    return NULL;
  }
  char* s = malloc(sizeof(char)*(szt)(len+SPL+3));
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
 */
void blobFile(char* file) {
  char*s = blobFileExt(file, "");
  free(s);
}

/**
 * @brief Save the snapshot of file with extension ext like ".t" ".c" or ""
 * @return Return the sha256 hash
 */
char* blobFileExt(char* file, char* ext) {
  char* hash = sha256file(file);
  char* path = hashToPath(hash);
  path[SPL+3] = 0;
  char cmd1[MAXL] = "mkdir -p ";
  append(cmd1, path);
  system(cmd1);
  path[SPL+3] = '/';
  strcpy(cmd1, path);
  append(cmd1, ext);
  cp(cmd1, file);
  free(path);
  return hash;
}

/**
 * @brief Save the snapshop of string s and return the sha256 hash of it
 */
char* blobStringExt(char* s, char* ext) {
  char* fname = createTemp();
  s2f(s, fname);
  char* hash = blobFileExt(fname, ext);
  assert(remove(fname) == 0);
  free(fname);
  return hash;
}

/**
 * @brief Create a temporary file in /tmp
 * 
 * @return char* the name of the temporary file, starting by /tmp
 */
char* createTemp() {
  static const char template[] = "/tmp/iamhoviadinXXXXXXXX";
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