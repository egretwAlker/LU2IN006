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
List* listdir(const char* root_dir) {
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
int file_exists(const char* file) {
  struct stat buffer;
  return (stat(file, &buffer) == 0);
}

void cp(const char* dest, const char* src) {
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
char* hashToPath(const char* hash) {
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
 * @return p1/p2
 */
char* concatPaths(const char* p1, const char* p2) {
    int n = (int)strlen(p1)+(int)strlen(p2)+2;
    char* s = malloc(sizeof(char)*(szt)n);
    strcpy(s, p1);
    append(s, "/");
    append(s, p2);
    return s;
}

char* hashToPathExt(const char* hash, const char* ext) {
  char* s = hashToPath(hash);
  char* res = newconcat(s, ext);
  free(s);
  return res;
}

/**
 * @brief Save the snapshot of file
 */
void blobFile(const char* file) {
  char*s = blobFileExt(file, "");
  free(s);
}

/**
 * @brief Save the snapshot of file with extension ext like ".t" ".c" or ""
 * @return Return the sha256 hash
 */
char* blobFileExt(const char* file, const char* ext) {
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
char* blobStringExt(const char* s, const char* ext) {
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

void setMode(int mode, const char * path) {
    char buf[MAXL];
    sprintf(buf, "chmod %o %s", mode, path);
    system(buf);
}

int getChmod(const char * path) {
    struct stat ret;
    assert(stat(path, &ret) != -1);

    return ( ret.st_mode & S_IRUSR ) | ( ret.st_mode & S_IWUSR ) | ( ret.st_mode & S_IXUSR ) |
    /*owner*/
    ( ret.st_mode & S_IRGRP ) | ( ret.st_mode & S_IWGRP ) | ( ret.st_mode & S_IXGRP ) |
    /*group*/
    ( ret.st_mode & S_IROTH ) | ( ret.st_mode & S_IWOTH ) | ( ret.st_mode & S_IXOTH );
    /*other*/
}

int isDir(const char * path) {
    struct stat ret;
    if(stat(path, &ret) == -1) return -1;
    return (ret.st_mode & S_IFDIR) != 0;
}

/**
 * @brief File to string
 */
char* fts(const char* fn) {
  char buf[MAXL];
  FILE* f = fopen(fn, "r");
  assert(f);
  int n = (int)fread(buf, sizeof(char), MAXL, f);
  fclose(f);
  return strndup(buf, (szt)n);
}

/**
 * @brief String to file
 */
void stf(const char* s, const char* fn) {
  FILE* f = fopen(fn, "w");
  assert(f);
  fprintf(f, "%s", s);
  fclose(f);
}

void createFile(const char* file) {
  char buf[MAXL];
  sprintf(buf, "touch %s", file);
  system(buf);
}