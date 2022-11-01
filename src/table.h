
/**
 * @file table.h
 * @author Devin Arena
 * @brief header for hash table implementation.
 * @since 6/24/2022
 **/

#include <stdint.h>

#ifndef PALLADIUM_TABLE_H
#define PALLADIUM_TABLE_H

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
bool tableGet(Table* table, char* key, char* value);
bool tableSet(Table* table, char* key, char* value);
bool tableDelete(Table* table, char* key);
void tableAddAll(Table* from, Table* to);
char* tableFindString(Table* table,
                           const char* chars,
                           int length,
                           uint32_t hash);
// void tableRemoveWhite(Table* table);
// void markTable(Table* table);

#endif