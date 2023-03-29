#include "misc.h"
#include <stdio.h>

void append(char* s, const char* t) {
  while(*s) ++s;
  while(*t) {
    *s = *t;
    ++s; ++t;
  }
  *s = 0;
}

/**
 * @brief Write string s to file fn
 */
void s2f(const char* s, const char* fn) {
  FILE* f = fopen(fn, "w");
  assert(f != NULL);
  fwrite(s, sizeof(char), strlen(s), f);
  fclose(f);
}

/**
 * @brief Get string from file fn
 */
char* f2s(const char* fn) {
  char* s = NULL; szt len;
  FILE* f = fopen(fn, "r");
  assert(f != NULL);
  assert(getdelim(&s, &len, '\0', f) != -1);
  fclose(f);
  return s;
}

/**
 * @brief Get concatenated result of s and t
 */
char* newconcat(const char* s, const char* t) {
  int n = (int)strlen(s), m = (int)strlen(t);
  char* res = (char*)malloc(sizeof(char)*(szt)(n+m+1));
  strcpy(res, s);
  strcat(res, t);
  return res;
}