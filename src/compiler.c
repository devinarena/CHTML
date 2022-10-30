
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compiler.h"

Compiler compiler;

/**
 * @brief Writes a string to a file. Called once the compiler has finished
 * generating HTML.
 *
 * @param file the file to write to.
 */
static void writeOutput(char* file) {
  FILE* f = fopen(file, "w");
  if (f == NULL) {
    printf("Error opening file!\n");
    exit(1);
  }

  int outputSize = strlen(compiler.output);
  compiler.output = (char*)realloc(compiler.output, outputSize + 1);
  compiler.output[outputSize] = '\0';

  fprintf(f, "%s", compiler.output);
  fclose(f);
}

/**
 * @brief Adds output to the compiler's output buffer. Contains the HTML
 * generated by the compiler.
 *
 * @param str the string to add to the output buffer.
 * @param freeMemory whether or not to free the memory allocated to the string.
 */
static void addOutput(char* str) {
  int len = strlen(str);
  if (!compiler.output) {
    compiler.output = malloc(len * sizeof(char*));
    strcpy(compiler.output, str);
  } else {
    int oldLen = strlen(compiler.output);
    compiler.output = realloc(compiler.output, (oldLen + len) * sizeof(char*));
    strcat(compiler.output, str);
  }
}

/**
 * @brief Displays a compilation error before terminating the program.
 *
 * @param message the error message to display.
 */
static void compileError(char* message) {
  printf("Compile error: %s", message);
  exit(1);
}

/**
 * @brief Pushes a token onto the token stack. (for closing tags)
 *
 * @param token the token to push onto the stack.
 */
static void pushStack(Token token) {
  *(compiler.stackTop++) = token;
}

/**
 * @brief Pops a token off the stack and returns it.
 *
 * @return Token the token that was popped off the stack.
 */
static Token popStack() {
  return *(--compiler.stackTop);
}

static Token peekStack(int depth) {
  return *(compiler.stackTop - depth - 1);
}

static char* removeQuotes(char* str, int len) {
  char* newStr = malloc((len - 1) * sizeof(char*));
  strncpy(newStr, str + 1, len);
  newStr[len - 1] = '\0';
  return newStr;
}

/**
 * @brief Generate closing tags for any open tags still on the stack.
 *
 * @param tabs the indentation level of which to stop generating closing tags.
 */
static void finishTags(int tabs) {
  while (compiler.stackTop - compiler.stack > 0 && peekStack(0).tab >= tabs) {
    Token token = popStack();
    switch (token.type) {
      case TOKEN_DOCUMENT:
        addOutput("</html>");
        break;
      case TOKEN_CONTAINER:
        addOutput("</div>");
        break;
      case TOKEN_HEAD:
        addOutput("</head>");
        break;
      case TOKEN_BODY:
        addOutput("</body>");
        break;
    }
  }
}

static void textTag(char* tagName) {
  Token text = scanToken();
  printToken(text);
  if (text.type != TOKEN_TEXT) {
    compileError("Expected text after text-tag token");
  }

  char* output = removeQuotes(text.start, text.length);

  int len = snprintf(NULL, 0, "<%s>\0", tagName);

  char* open = malloc(len);
  char* close = malloc(len + 1);
  sprintf(open, "<%s>\0", tagName);
  sprintf(close, "</%s>\0", tagName);
  addOutput(open);
  addOutput(output);
  addOutput(close);
  free(output);
}

static void heading(TokenType headingType) {
  char* heading = malloc(sizeof(char*) * 2);
  sprintf(heading, "h%d", headingType - TOKEN_HEADING1 + 1);
  textTag(heading);
  free(heading);
}

static void container() {
  Token token = scanToken();
  printToken(token);

  switch (token.type) {
    case TOKEN_DOCUMENT:
      addOutput("<html>");
      break;
    case TOKEN_CONTAINER:
      addOutput("<div>");
      break;
    case TOKEN_HEAD:
      addOutput("<head>");
      break;
    case TOKEN_BODY:
      addOutput("<body>");
      break;
    default:
      compileError("Expected container type.");
  }

  pushStack(token);
}

static void inStatement() {
  container();
}

void initCompiler() {
  compiler.stackTop = compiler.stack;
  compiler.output = NULL;
}

void compile() {
  addOutput("<!DOCTYPE html>");

  Token token = scanToken();

  while (token.type != TOKEN_EOF) {
    printToken(token);

    finishTags(token.tab);

    switch (token.type) {
      case TOKEN_IN:
        inStatement();
        break;
      case TOKEN_HEADING1:
      case TOKEN_HEADING2:
      case TOKEN_HEADING3:
      case TOKEN_HEADING4:
      case TOKEN_HEADING5:
      case TOKEN_HEADING6:
        heading(token.type);
        break;
      case TOKEN_TITLE:
        textTag("title");
        break;
      case TOKEN_RAW_HTML: {
        char* output = malloc(token.length - 1);
        memcpy(output, token.start + 1, token.length - 1);
        output[token.length - 1] = '\0';
        addOutput(output);
        free(output);
        break;
      }
      default:
        compileError("Unexpected token.");
        break;
    }

    token = scanToken();
  }
  printToken(token);

  finishTags(0);

  writeOutput("index.html");
}