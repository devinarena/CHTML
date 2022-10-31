
#ifndef CHTML_COMPILER_H
#define CHTML_COMPILER_H

#include <stdint.h>

#include "scanner.h"

typedef struct {
  Token stack[256];
  Token* stackTop;
  char* output;
  uint16_t instruction;
} Compiler;

void initCompiler();
void compile();

#endif