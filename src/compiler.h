
#ifndef CHTML_COMPILER_H
#define CHTML_COMPILER_H

#include <stdint.h>

#include "scanner.h"

/**
 * @file compiler.h
 * @author Devin Arena
 * @brief Header file for compiler.
 * @since 11/1/2022
 **/

typedef struct {
  Token stack[256];
  Token* stackTop;
  char* output;
  uint16_t instruction;
  Token previous;
  Token current;
} Compiler;

void initCompiler();
void compile();

#endif