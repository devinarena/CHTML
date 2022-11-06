

/**
 * @file main.c
 * @author Devin Arena
 * @brief Playing around with generating html files with C.
 * @version 1.0
 * @date 2022-10-28
 */

#include <stdio.h>
#include <stdlib.h>

#include "compiler.h"
#include "scanner.h"

static char* readFile(const char* path) {
  FILE* file = fopen(path, "rb");
  // file may not exist or be readable
  if (!file) {
    printf("Could not open file '%s'\n", path);
    exit(74);
  }

  // get file size and go back to beginning
  fseek(file, 0L, SEEK_END);
  size_t fileSize = ftell(file);
  rewind(file);

  // allocate memory for file contents
  char* buffer = (char*)malloc(fileSize + 1);
  // may not have enough memory to read whole file
  if (buffer == NULL) {
    fprintf(stderr, "Not enough memory to read '%s'\n", path);
    exit(74);
  }

  // read file contents into buffer
  size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);

  // if bytes read is not equal to file size, something went wrong
  if (bytesRead < fileSize) {
    fprintf(stderr, "Could not read file '%s'\n", path);
    exit(74);
  }

  // null terminate buffer
  buffer[bytesRead] = '\0';

  // close file
  fclose(file);

  return buffer;
}

int main(int argc, const char* argv[]) {
  if (argc <= 1) {
    printf("Usage: %s <file> [output]\n", argv[0]);
    return 1;
  }

  char* file = readFile(argv[1]);
  const char* outputName = argc >= 3 ? argv[2] : "index.html";

  initScanner(file);
  initCompiler();

  compile(outputName);

  return 0;
}