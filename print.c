#include <stdio.h>
#include <stdlib.h>

#include "print.h"

static void
printHumanReadable(unsigned long size)
{
	static const size_t SUFF_LEN = 9;
	static const char suffixes[] = {'B', 'K', 'M', 'G', 'T', 
					'P', 'E', 'Z', 'Y'};

	const float scale = 1024.0;	
	const float scale_cutoff = 1000.0;
	const float print_cutoff = 10.0;
	float fsize = (float)size;
	size_t index = 0;

	while (fsize > scale_cutoff) {
		fsize /= scale;
		++index;
	}

	if (index > SUFF_LEN - 1) {
		fprintf(stderr, "file size too large to scale\n");
		return;
	}

	if (fsize > print_cutoff) {
		printf("%.0f%c ", fsize, suffixes[index]);
	} else {
		printf("%.1f%c ", fsize, suffixes[index]);
	}
}

static void
printBlockSize(FTSENT *fts_ent, const Options *ls_options)
{
	struct stat *sb = fts_ent->fts_statp;
	const unsigned long stat_bsize = 512;
	long user_bsize = 512;
	unsigned long file_blocks = 0;

	if (ls_options->human_readable) {
		printHumanReadable((unsigned long)sb->st_blocks * 
			(unsigned long)stat_bsize);
		return;
	}

	if (ls_options->report_in_kb) {
		user_bsize = 1024;
	} else {
		(void)getbsize(NULL, &user_bsize);
	}

	file_blocks = (unsigned long)sb->st_blocks * 
		      (unsigned long)stat_bsize;
	file_blocks /= user_bsize;
	printf("%lu ", file_blocks);
}

void 
printEntry(FTSENT *fts_ent, const Options *ls_options)
{
	if (ls_options->print_inode) {
		printf("%ld ", (long)fts_ent->fts_statp->st_ino);
	}

	if (ls_options->print_bsize) {
		printBlockSize(fts_ent, ls_options);
	}

	printf("%s", fts_ent->fts_name);

	if (fts_ent->fts_info == FTS_D && 
    	    fts_ent->fts_level == 0    &&
    	    !ls_options->plain_dirs) {
		printf(":");
	}

	printf("\n");
}
