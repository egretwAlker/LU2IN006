#include "misc.h"

void append(char* s, const char* t) {
  while(*s) ++s;
  while(*t) {
    *s = *t;
    ++s; ++t;
  }
  *s = 0;
}