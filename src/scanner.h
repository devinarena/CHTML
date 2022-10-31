
#ifndef CHTML_SCANNER_H
#define CHTML_SCANNER_H

typedef enum {
  TOKEN_EOF,
  TOKEN_ERROR,
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
  TOKEN_TEXT,
  TOKEN_RAW_HTML,
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
Token scanToken();
void printToken(Token token);

#endif