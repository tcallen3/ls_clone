#ifndef LS_PRINT_H
#define LS_PRINT_H

#include <sys/stat.h>

#include <fts.h>

#include "helpers.h"

void printEntry(FTSENT *fts_ent, const Options *ls_options);

#endif /* LS_PRINT_H */
