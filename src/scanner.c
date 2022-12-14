
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compiler.h"
#include "scanner.h"

/**
 * @file scanner.c
 * @author Devin Arena
 * @brief Contains the logic for converting a source string into a stream of
 *tokens.
 * @since 10/30/2022
 **/

Scanner scanner;

/**
 * @brief Peek at the current character in the source code.
 *
 * @return char the current character.
 */
char peek() {
  return *scanner.current;
}

/**
 * @brief Peek at the next character in the source code.
 *
 * @return char the next character in the source code.
 */
char peekNext() {
  if (peek() == '\0')
    return '\0';
  return scanner.current[1];
}

/**
 * @brief Advance the scanner to the next character.
 */
static void advance() {
  scanner.current++;
  scanner.col++;
}

/**
 * @brief Counts the number of tabs until the next non-tab character.
 */
static void countIndentation() {
  scanner.tabs = 0;

  char c;
  while ((c = peek()) == '\r' || c == '\t') {
    scanner.tabs++;

    advance();
  }

  scanner.start = scanner.current;
}

/**
 * @brief Updates values for the next line.
 */
static void newLine() {
  scanner.line++;
  scanner.col = 0;
  countIndentation();
}

/**
 * @brief Zeroes out the scanner's memory.
 *
 * @param source the source code to scan.
 */
void initScanner(char* source) {
  scanner.start = source;
  scanner.current = source;
  scanner.line = 1;
  countIndentation();
}

void insertMacro(int tabs, char* source) {
  char* newSource = malloc(strlen(source) * 2);
  int totalLength = 0;
  int lineNum = 0;
  // split the given source string by newlines
  char* token = strtok(source, "\n");
  while (token != NULL) {
    // add the tabs to the beginning of all but the first line
    if (lineNum > 0) {
      int length = strlen(token) + tabs + 1;
      char* line = malloc(length);
      for (int i = 0; i < tabs; i++) {
        line[i] = '\t';
      }
      strcat(line, token);
      line[tabs] = '\0';
      strcat(newSource, line);
      totalLength += length;
    } else {
      strcpy(newSource, token);
      totalLength += strlen(token);
    }
    lineNum++;
    token = strtok(NULL, "\n");
  }
  totalLength += strlen(scanner.start) + 1;
  newSource = realloc(newSource, totalLength);
  newSource[totalLength - 1] = '\0';
  strcat(newSource, scanner.start);
  scanner.start = newSource;
  scanner.current = scanner.start;
  printf("\tAFTER INSERTION:\n%s\n", scanner.start);
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

/**
 * @brief Creates a token with the given type and returns it.
 *
 * @param type the type of the token.
 * @return Token the token.
 */
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

/**
 * @brief Creates a quoted token '', "", ``.
 *
 * @param type the type of token.
 * @param end the character that ends the token.
 * @return Token the generated token.
 */
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

Token macro() {
  advance();

  char c;
  while (isdigit((c = peek())) || isalpha(c)) {
    advance();
  }

  int nameLen = scanner.current - scanner.start;
  char* name = malloc(nameLen * sizeof(char*));
  strncpy(name, scanner.start + 1, nameLen - 1);
  name[nameLen - 1] = '\0';

  scanner.start = scanner.current;
  skipWhitespace();

  char* start = scanner.start;

  Token token = scanToken();
  while (token.tab > 0) {
    printf("TOKEN: %s %d %d\n", token.start, token.length, token.tab);
    token = scanToken();
  }
  char* end = scanner.current - token.length;

  int len = end - start + 1;
  char* text = malloc(len * sizeof(char*));
  strncpy(text, start, len - 1);
  for (int i = len - 3; i >= 0; i--) {
    if (text[i] != ' ' && text[i] != '\t' && text[i] != '\r' && text[i] != '\n' && text[i] != '\0') {
      text[i + 1] = '\0';
      break;
    }
  }

  printf("DEFINED MACRO '%s' WITH TEXT '%s'\n", name, text);
  scanner.start = start;
  scanner.current = end;

  Token macroToken = makeToken(TOKEN_MACRO);

  addMacro(name, text);

  scanner.start = scanner.current;

  return macroToken;
}

/**
 * @brief Scans the next token (generates a token based on the current character
 * of the input string)
 *
 * @return Token the next token.
 */
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
    case '/':
      if (peekNext() == '/') {
        while (peek() != '\0' && peek() != '\n')
          advance();
        return scanToken();
      }
      return makeToken(TOKEN_ERROR);
    case '(':
      advance();
      return makeToken(TOKEN_LEFT_PAREN);
    case ')':
      advance();
      return makeToken(TOKEN_RIGHT_PAREN);
    case '!':
      advance();
      return makeToken(TOKEN_EXCLAMATION);
    case '@':
      return macro();
    default: {
      while (isdigit((c = peek())) || isalpha(c)) {
        advance();
      }

      size_t length = scanner.current - scanner.start + 1;
      if (length == 2 && *scanner.start == 'p')
        return makeToken(TOKEN_PARAGRAPH);

      char* token = malloc(length * sizeof(char*));
      memcpy(token, scanner.start, length - 1);
      token[length - 1] = '\0';
      //   printf("%s\n", token);

      Token output = makeToken(TOKEN_IDENTIFIER);

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
                        if (length == 4)
                          output = makeToken(TOKEN_CONTAINER);
                        else {
                          switch (token[3]) {
                            case 't':
                              if (length > 4) {
                                switch (token[4]) {
                                  case 'a':
                                    if (strcmp(token, "container") == 0) {
                                      output = makeToken(TOKEN_CONTAINER);
                                    }
                                    break;
                                  case 'e':
                                    if (strcmp(token, "content") == 0) {
                                      output = makeToken(TOKEN_BODY);
                                    }
                                    break;
                                }
                              }
                              break;
                          }
                        }
                      }
                      break;
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
    case TOKEN_CSS:
      printf("CSS (");
      break;
    case TOKEN_LEFT_PAREN:
      printf("LEFT_PAREN (");
      break;
    case TOKEN_RIGHT_PAREN:
      printf("RIGHT_PAREN (");
      break;
    case TOKEN_EXCLAMATION:
      printf("EXCLAMATION (");
      break;
    case TOKEN_MACRO:
      printf("MACRO (");
      break;
    case TOKEN_IDENTIFIER:
      printf("IDENTIFIER (");
      break;
    default:
      printf("UNKNOWN (");
      break;
  }
  printf("%d, %d, %d, %d, %s)\n", token.line, token.col, token.tab,
         token.length, token.start);
}