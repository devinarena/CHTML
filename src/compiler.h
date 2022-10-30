
#ifndef CHTML_COMPILER_H
#define CHTML_COMPILER_H

#include "scanner.h"

typedef struct {
  Token stack[256];
  Token* stackTop;
  char* output;
} Compiler;

void initCompiler();
void compile();

#endif