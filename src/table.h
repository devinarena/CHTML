
/**
 * @file table.h
 * @author Devin Arena
 * @brief header for hash table implementation.
 * @since 6/24/2022
 **/

#ifndef PALLADIUM_TABLE_H
#define PALLADIUM_TABLE_H

#include <stdbool.h>
#include <stdint.h>

#define HASH_STRING(str) hashString(str, strlen(str))

typedef struct {
  char* key;
  char* value;
} Entry;

typedef struct Table {
  int count;
  int capacity;
  Entry* entries;
} Table;

void initTable(Table* table);
void freeTable(Table* table);
char* tableGet(Table* table, char* key);
bool tableSet(Table* table, char* key, char* value);
bool tableDelete(Table* table, char* key);
void tableAddAll(Table* from, Table* to);
char* tableFindString(Table* table,
                      const char* chars,
                      int length,
                      uint32_t hash);

static inline uint32_t hashString(const char* key, int length) {
  uint32_t hash = 2166136261u;
  for (int i = 0; i < length; i++) {
    hash ^= key[i];
    hash *= 16777619;
  }
  return hash;
}

// void tableRemoveWhite(Table* table);
// void markTable(Table* table);

#endif