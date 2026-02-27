#ifndef HISTORY_STORAGE_H
#define HISTORY_STORAGE_H

#include "config.h"
#include <vector>

void addHistoryEntry(const HistoryEntry &e);
size_t getHistoryCount();
bool getHistoryEntry(size_t index, HistoryEntry &out);

#endif // HISTORY_STORAGE_H
