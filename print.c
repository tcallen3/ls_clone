#include <stdio.h>
#include <stdlib.h>

#include "print.h"

void 
printEntry(FTSENT *fts_ent, const Options *ls_options)
{
	const long stat_bsize = 512;
	long user_bsize = 512;
	long file_blocks = 0;

	if (ls_options->print_inode) {
		printf("%ld ", (long)fts_ent->fts_statp->st_ino);
	}

	if (ls_options->print_bsize) {
		(void)getbsize(NULL, &user_bsize);
		file_blocks = (long)fts_ent->fts_statp->st_blocks *
			      stat_bsize;
		file_blocks /= user_bsize;
		printf("%ld ", file_blocks);
	}

	printf("%s", fts_ent->fts_name);

	if (fts_ent->fts_info == FTS_D && 
    	    fts_ent->fts_level == 0    &&
    	    !ls_options->plain_dirs) {
		printf(":");
	}

	printf("\n");
}
