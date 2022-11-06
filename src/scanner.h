
/**
 * @file scanner.h
 * @author Devin Arena
 * @brief Header file for scanner module.
 * @since 10/30/2022
 **/

#ifndef CHTML_SCANNER_H
#define CHTML_SCANNER_H

typedef enum {
  TOKEN_EOF,
  TOKEN_ERROR,
  TOKEN_COMMENT,
  TOKEN_DOCUMENT,
  TOKEN_HEAD,
  TOKEN_BODY,
  TOKEN_TITLE,
  TOKEN_CONTAINER,
  TOKEN_HEADING1,
  TOKEN_HEADING2,
  TOKEN_HEADING3,
  TOKEN_HEADING4,
  TOKEN_HEADING5,
  TOKEN_HEADING6,
  TOKEN_PARAGRAPH,
  TOKEN_CSS,
  TOKEN_TEXT,
  TOKEN_RAW_HTML,
  TOKEN_LEFT_PAREN,
  TOKEN_RIGHT_PAREN,
  TOKEN_EXCLAMATION,
  TOKEN_MACRO,
  TOKEN_IDENTIFIER,
} TokenType;

typedef struct {
  TokenType type;
  int col;
  int line;
  int tab;
  char* start;
  int length;
} Token;

typedef struct {
  char* start;
  char* current;
  int line;
  int col;
  int tabs;
} Scanner;

void initScanner(char* source);
void insertMacro(int tabs, char* source);
Token scanToken();
void printToken(Token token);
char peek();

#endif