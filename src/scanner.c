
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scanner.h"

Scanner scanner;

char peek() {
  return *scanner.current;
}

char peekNext() {
  if (peek() == '\0')
    return '\0';
  return scanner.current[1];
}

static void advance() {
  scanner.current++;
  scanner.col++;
}

static void countIndentation() {
  scanner.tabs = 0;

  char c;
  while ((c = peek()) == '\r' || c == '\t') {
    scanner.tabs++;

    advance();
  }

  scanner.start = scanner.current;
}

static void newLine() {
  scanner.line++;
  scanner.col = 0;
  countIndentation();
}

void initScanner(char* source) {
  scanner.start = source;
  scanner.current = source;
  scanner.line = 1;
  countIndentation();
}

/**
 * @brief Skips whitespace characters, counting the number of tabs.
 *
 * @return int the number of tabs.
 */
static void skipWhitespace() {
  char c;
  while ((c = peek()) != '\0' &&
         (c == ' ' || c == '\r' || c == '\t' || c == '\n')) {
    advance();

    if (c == '\n')
      newLine();
  }
  scanner.start = scanner.current;
}

static Token makeToken(TokenType type) {
  Token token;
  token.type = type;
  token.line = scanner.line;
  token.length = (int)(scanner.current - scanner.start);
  token.col = scanner.col - token.length;
  token.tab = scanner.tabs;
  token.start = scanner.start;
  return token;
}

static Token quotedToken(TokenType type, char end) {
  advance();

  while (peek() != end) {
    if (peek() == '\n') {
      scanner.line++;
      scanner.col = 0;
    } else if (peek() == '\0') {
      return makeToken(TOKEN_ERROR);
    }
    advance();
  }

  Token token = makeToken(type);

  advance();
  scanner.start = scanner.current;

  return token;
}

Token scanToken() {
  skipWhitespace();

  char c = peek();
  switch (c) {
    case '\0':
      return makeToken(TOKEN_EOF);
    case '"':
      return quotedToken(TOKEN_TEXT, '"');
    case '`':
      return quotedToken(TOKEN_RAW_HTML, '`');
    case 'p': {
      Token output = makeToken(TOKEN_PARAGRAPH);
      advance();
      return output;
    }
    default: {
      while ((c = peek()) != '\0' && c != ' ' && c != '\r' && c != '\t' &&
             c != '\n') {
        advance();
      }

      size_t length = scanner.current - scanner.start + 1;
      char* token = malloc(length);
      memcpy(token, scanner.start, length - 1);
      token[length - 1] = '\0';
      //   printf("%s\n", token);

      Token output = makeToken(TOKEN_ERROR);

      switch (token[0]) {
        case 'b': {
          if (strcmp(token, "body") == 0) {
            output = makeToken(TOKEN_BODY);
          }
          break;
        }
        case 'c': {
          // structurally correct but became messy, probably could be cleaned
          // up.
          if (length > 1) {
            switch (token[1]) {
              case 'o':
                if (length > 2) {
                  switch (token[2]) {
                    case 'n':
                      if (length > 3) {
                        switch (token[3]) {
                          case 't':
                            if (length > 3) {
                              switch (token[3]) {
                                case 'a':
                                  if (strcmp(token, "container") == 0) {
                                    output = makeToken(TOKEN_CONTAINER);
                                  }
                                  break;
                                case 't':
                                  if (strcmp(token, "content") == 0) {
                                    output = makeToken(TOKEN_BODY);
                                  }
                                  break;
                              }
                            }
                        }
                      } else {
                        output = makeToken(TOKEN_CONTAINER);
                      }
                  }
                }
                break;
              case 's':
                if (strcmp(token, "css") == 0) {
                  output = makeToken(TOKEN_CSS);
                }
                break;
            }
          }
          break;
        }
        case 'd': {
          if (length > 1) {
            switch (token[1]) {
              case 'o':
                if (strcmp(token, "document") == 0) {
                  output = makeToken(TOKEN_DOCUMENT);
                }
                break;
              case 'i':
                if (strcmp(token, "div") == 0) {
                  output = makeToken(TOKEN_CONTAINER);
                }
                break;
              case 'a':
                if (strcmp(token, "data") == 0) {
                  output = makeToken(TOKEN_HEAD);
                }
                break;
            }
          }
          break;
        }
        case 'h': {
          if (length > 1) {
            switch (token[1]) {
              case '1': {
                output = makeToken(TOKEN_HEADING1);
                break;
              }
              case '2': {
                output = makeToken(TOKEN_HEADING2);
                break;
              }
              case '3': {
                output = makeToken(TOKEN_HEADING3);
                break;
              }
              case '4': {
                output = makeToken(TOKEN_HEADING4);
                break;
              }
              case '5': {
                output = makeToken(TOKEN_HEADING5);
                break;
              }
              case '6': {
                output = makeToken(TOKEN_HEADING6);
                break;
              }
              case 'e':
                if (strcmp(token, "head") == 0) {
                  output = makeToken(TOKEN_HEAD);
                }
                break;
            }
          }
          break;
        }
        case 't': {
          if (strcmp(token, "title") == 0) {
            output = makeToken(TOKEN_TITLE);
          }
          break;
        }
        default:
          break;
      }

      free(token);

      scanner.start = scanner.current;

      return output;
    }
  }
}

/**
 * @brief Prints a token formattted nicely.
 *
 * @param token the token to print.
 */
void printToken(Token token) {
  switch (token.type) {
    case TOKEN_EOF:
      printf("EOF (");
      break;
    case TOKEN_ERROR:
      printf("ERROR (");
      break;
    case TOKEN_DOCUMENT:
      printf("DOCUMENT (");
      break;
    case TOKEN_HEAD:
      printf("HEAD (");
      break;
    case TOKEN_TITLE:
      printf("TITLE (");
      break;
    case TOKEN_CONTAINER:
      printf("CONTAINER (");
      break;
    case TOKEN_BODY:
      printf("BODY (");
      break;
    case TOKEN_HEADING1:
      printf("HEADING1 (");
      break;
    case TOKEN_HEADING2:
      printf("HEADING2 (");
      break;
    case TOKEN_HEADING3:
      printf("HEADING3 (");
      break;
    case TOKEN_HEADING4:
      printf("HEADING4 (");
      break;
    case TOKEN_HEADING5:
      printf("HEADING5 (");
      break;
    case TOKEN_HEADING6:
      printf("HEADING6 (");
      break;
    case TOKEN_PARAGRAPH:
      printf("PARAGRAPH (");
      break;
    case TOKEN_TEXT:
      printf("TEXT (");
      break;
    case TOKEN_RAW_HTML:
      printf("RAW_HTML (");
      break;
    default:
      printf("UNKNOWN (");
      break;
  }
  printf("%d, %d, %d, %d, %s)\n", token.line, token.col, token.tab,
         token.length, token.start);
}