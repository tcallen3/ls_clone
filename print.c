#include <grp.h>
#include <limits.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "print.h"

#define STRMODE_LEN 12

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

/* 
 * NOTE: on BSD, ls -sh prints file size in human format,
 * rather than block size? However ls -lsh prints blocksize
 * as expected. We opt to print blocks human-readable in
 * both cases, since this seems less surprising. 
 */
static void
printBlockSize(unsigned long blocks, const Options *ls_options)
{
	const unsigned long stat_bsize = 512;
	long user_bsize = 512;
	unsigned long file_blocks = 0;

	if (ls_options->human_readable) {
		printHumanReadable(blocks * stat_bsize);
		return;
	}

	if (ls_options->report_in_kb) {
		user_bsize = 1024;
	} else {
		(void)getbsize(NULL, &user_bsize);
	}

	file_blocks = blocks * stat_bsize;
	file_blocks /= user_bsize;
	printf("%lu ", file_blocks);
}

static void
printUserAndGroup(const struct stat *sb)
{
	struct passwd *pass = NULL;
	struct group *grp = NULL;

	if ((pass = getpwuid(sb->st_uid)) == NULL) {
		/* fallback to numeric uid */
		printf("%4u ", (unsigned)sb->st_uid);
	} else {
		printf("%5s ", pass->pw_name);
	}

	if ((grp = getgrgid(sb->st_gid)) == NULL) {
		/* fallback to numeric uid */
		printf("%4u ", (unsigned)sb->st_gid);
	} else {
		printf("%5s ", grp->gr_name);
	}
}

static void
printLongFormat(FTSENT *fts_ent, const Options *ls_options)
{
	char fmode[STRMODE_LEN];
	struct stat *sb = fts_ent->fts_statp;

	strmode(sb->st_mode, fmode);
	printf("%11s ", fmode);

	printf("%2u ", (unsigned)sb->st_nlink);
	
	if (ls_options->print_numeric_uid_gid) {
		printf("%4u %4u ", (unsigned)sb->st_uid, 
			(unsigned)sb->st_gid);	
	} else {
		printUserAndGroup(sb);
	}

	if (ls_options->human_readable) {
		printHumanReadable((unsigned long)sb->st_size);
	} else {
		printf("%5lu ", (unsigned long)sb->st_size);
	}

	/*printFileTime(sb, ls_options);*/
}
static void
printFileName(const FTSENT *fts_ent, const Options *ls_options)
{
	struct stat *sb = fts_ent->fts_statp;
	const mode_t exec_comp = S_IXUSR | S_IXGRP | S_IXOTH;

	printf("%s", fts_ent->fts_name);

	if (fts_ent->fts_info == FTS_D && 
    	    fts_ent->fts_level == 0    &&
    	    !ls_options->plain_dirs) {
		printf(":");
	} else if (ls_options->print_file_type) {
		if (S_ISDIR(sb->st_mode)) {
			printf("/ ");	
		} else if (S_ISLNK(sb->st_mode)) {
			printf("@ ");
		} else if (fts_ent->fts_info == FTS_W) {
			printf("%% ");
		} else if (S_ISSOCK(sb->st_mode)) {
			printf("= ");	
		} else if (S_ISFIFO(sb->st_mode)) {
			printf("| ");
		} else if (sb->st_mode & exec_comp) {
			printf("* ");
		}
	}
}

void 
printEntry(FTSENT *fts_ent, const Options *ls_options)
{
	char symlink_path[PATH_MAX];
	ssize_t plen = 0;

	if (ls_options->print_inode) {
		printf("%ld ", (long)fts_ent->fts_statp->st_ino);
	}

	if (ls_options->print_bsize) {
		printBlockSize((unsigned long)fts_ent->fts_statp->st_blocks,
			       ls_options);
	}

	if (ls_options->print_long_format) {
		printLongFormat(fts_ent, ls_options);
	}

	printFileName(fts_ent, ls_options);

	if (ls_options->print_long_format && 
	    S_ISLNK(fts_ent->fts_statp->st_mode)) {
		if ((plen = readlink(fts_ent->fts_accpath, symlink_path,
		    PATH_MAX)) == -1) {
			perror("symlink traversal");
		} else {
			symlink_path[plen] = '\0';
			printf(" -> %s", symlink_path);
		}
	}

	printf("\n");
}
