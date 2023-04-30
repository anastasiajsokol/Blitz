#ifndef BLITZ_ERROR
#define BLITZ_ERROR

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

namespace blitz {

void compile_error(const char* error, const char* file, size_t line, size_t offset){
  fprintf(stderr, "%s\n\t%s (%ld:%ld)\n", error, file, line, offset);
  exit(1);
}

void error(const char* error, ...){
  va_list args;
  va_start(args, error);
  vfprintf(stderr, error, args);
  va_end(args);
  exit(1);
}

};

#endif