#include <errno.h>
#include <grp.h>
#include <limits.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "print.h"

#define STRMODE_LEN 12
#define TMESG_SIZE 512

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
printBlockSize(unsigned long blocks, long user_bsize, 
		const Options *ls_options)
{
	const unsigned long stat_bsize = 512;
	unsigned long file_blocks = 0;

	if (ls_options->human_readable) {
		printHumanReadable(blocks * stat_bsize);
		return;
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
printFileTime(struct stat *sb, const Options *ls_options)
{
	time_t ftime;
	struct tm *tdata;
	const char format[] = "%b %d %H:%M";
	char tmsg[TMESG_SIZE];

	if (ls_options->sort_by_mtime) {
		ftime = sb->st_mtime;
	} else if (ls_options->sort_by_atime) {
		ftime = sb->st_atime;
	} else {
		ftime = sb->st_ctime;
	}

	if ((tdata = localtime(&ftime)) == NULL) {
		if ((tdata = gmtime(&ftime)) == NULL) {
			fprintf(stderr, "invalid time: %s\n", 
				strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	if (strftime(tmsg, TMESG_SIZE, format, tdata) == 0) {
		/* avoid printing time if format errors */
		printf(" ");
		return;
	}

	printf("%s ", tmsg);
}

/* FIXME: need to add blocksize printing before directory */
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

	printFileTime(sb, ls_options);
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
printEntry(FTSENT *fts_ent, long user_bsize, const Options *ls_options)
{
	char symlink_path[PATH_MAX];
	ssize_t plen = 0;

	if (ls_options->print_inode) {
		printf("%ld ", (long)fts_ent->fts_statp->st_ino);
	}

	if (ls_options->print_bsize) {
		printBlockSize((unsigned long)fts_ent->fts_statp->st_blocks,
			       user_bsize, ls_options);
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
