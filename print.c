#include <stdio.h>

#include "print.h"

void 
printEntry(FTSENT *fts_ent, const Options *ls_options)
{
	printf("%s", fts_ent->fts_name);

	if (fts_ent->fts_info == FTS_D && 
    	    fts_ent->fts_level == 0    &&
    	    !ls_options->plain_dirs) {
		printf(":");
	}

	printf("\n");
}
