/*

BSD 3-Clause License

Copyright (c) 2023, Thomas Allen

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include <sys/stat.h>
#include <sys/types.h>

#include <errno.h>
#include <fts.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "helpers.h"
#include "print.h"

static int scaling = 1;

typedef int (*CompPointer)(const FTSENT **, const FTSENT**);

void 
setReverseSort()
{
	scaling = -1;
}

void 
setDefaultOptions(Options *opts)
{
	opts->show_self_parent = 0;
	opts->list_dir_recursive = 0;
	opts->human_readable = 0;
	opts->mark_nonprinting = 1;
	opts->plain_dirs = 0;
	opts->print_bsize = 0;
	opts->print_file_type = 0;
	opts->print_inode = 0;
	opts->print_long_format = 0;
	opts->print_numeric_uid_gid = 0;
	opts->report_in_kb = 0;
	opts->do_not_sort = 0;
	opts->show_hidden = 0;
	opts->show_dir_header = 0;
	opts->sort_by_size = 0;
	opts->sort_time = 0;
	opts->sort_by_ctime = 0;
	opts->sort_by_mtime = 0;
	opts->sort_by_atime = 0;
}

static int 
nameComp(const FTSENT **first, const FTSENT **second)
{
	return scaling * strcmp((*first)->fts_name, (*second)->fts_name);
}

static int
sizeComp(const FTSENT **first, const FTSENT **second)
{
	const off_t s1 = (*first)->fts_statp->st_size;
	const off_t s2 = (*second)->fts_statp->st_size;

	/* we want largest file to sort first, so we reverse a < b logic */
	if (s1 < s2) {
		return 1 * scaling;
	} else if (s1 > s2) {
		return -1 * scaling;
	}

	/* sizes equal, sort lexicographically */
	return nameComp(first, second);
}

static int
ctimeComp(const FTSENT **first, const FTSENT **second)
{
	const time_t t1 = (*first)->fts_statp->st_ctime;
	const time_t t2 = (*second)->fts_statp->st_ctime;

	/* we want most recent to sort first, so we reverse a < b logic */
	if (t1 < t2) {
		return 1 * scaling;
	} else if (t1 > t2) {
		return -1 * scaling;
	}

	/* times equal, so sort lexicographically */
	return nameComp(first, second);
}

static int
mtimeComp(const FTSENT **first, const FTSENT **second)
{
	const time_t t1 = (*first)->fts_statp->st_mtime;
	const time_t t2 = (*second)->fts_statp->st_mtime;

	/* we want most recent to sort first, so we reverse a < b logic */
	if (t1 < t2) {
		return 1 * scaling;
	} else if (t1 > t2) {
		return -1 * scaling;
	}

	/* times equal, so sort lexicographically */
	return nameComp(first, second);
}

static int
atimeComp(const FTSENT **first, const FTSENT **second)
{
	const time_t t1 = (*first)->fts_statp->st_atime;
	const time_t t2 = (*second)->fts_statp->st_atime;

	/* we want most recent to sort first, so we reverse a < b logic */
	if (t1 < t2) {
		return 1 * scaling;
	} else if (t1 > t2) {
		return -1 * scaling;
	}

	/* times equal, so sort lexicographically */
	return nameComp(first, second);
}

CompPointer
chooseSort(const Options *ls_options)
{
	CompPointer cptr = nameComp;

	if (ls_options->do_not_sort) {
		cptr = NULL;
	} else if (ls_options->sort_by_size) {
		cptr = sizeComp;
	} else if (ls_options->sort_time) {
		cptr = mtimeComp;	

		if (ls_options->sort_by_atime) {
			cptr = atimeComp;
		} else if (ls_options->sort_by_ctime) {
			cptr = ctimeComp;
		}
	}
	
	return cptr;
}

static int
showEntry(FTSENT *fts_ent, const Options *ls_options)
{
	int dot_exceptions = 0;

	/* don't show implicit '.' at file hierarchy root */
	if (fts_ent->fts_info == FTS_D 	&& 
	    fts_ent->fts_level == 0   	&&
	    !ls_options->show_dir_header) {
		return 0;
	}

	/* don't show directories a second time */
	if (fts_ent->fts_info == FTS_DP) {
		return 0;
	}

	dot_exceptions = ls_options->show_hidden || 
			 (ls_options->show_dir_header &&
				fts_ent->fts_level == 0);
	if (fts_ent->fts_name[0] == '.' && !dot_exceptions) {
		return 0;
	}
	
	/* default to showing */
	return 1;
}

void
traverseShallow(char **inputs, const Options *ls_options)
{
	FTS *fts_hier = NULL;
	FTSENT *fts_ent = NULL;
	CompPointer fcomp = NULL;

	int fts_options = FTS_PHYSICAL;
	int fts_term = 0;

	long user_bsize = 512;

	if (ls_options->show_self_parent) {
		fts_options |= FTS_SEEDOT;
	}

	fcomp = chooseSort(ls_options);	

	if (ls_options->report_in_kb) {
		user_bsize = 1024;
	} else {
		(void)getbsize(NULL, &user_bsize);
	}
	
	fts_hier = fts_open(inputs, fts_options, fcomp);
	while ((fts_ent = fts_read(fts_hier)) != NULL) {
		if (fts_ent->fts_errno != 0) {
			printf("%s: %s: %s\n", getprogname(),
				fts_ent->fts_accpath, 
				strerror(fts_ent->fts_errno));	
			continue;
		}

		if (showEntry(fts_ent, ls_options)) {
			printEntry(fts_ent, user_bsize, ls_options);
		}

		fts_term = (fts_ent->fts_level != 0) || 
			   (ls_options->plain_dirs); 

		if (fts_ent->fts_info == FTS_D && fts_term) {
			if (fts_set(fts_hier, fts_ent, FTS_SKIP) != 0) {
				perror("fts_set() entry");
				exit(EXIT_FAILURE);
			}
		}
	}

	if (errno != 0) {
		perror("FTS traversal");
		exit(EXIT_FAILURE);
	}

	(void)fts_close(fts_hier);
}

void
traverseRecursive(char **inputs, const Options *ls_options)
{
	FTS *fts_hier = NULL;
	FTSENT *fts_ent = NULL;
	CompPointer fcomp = NULL;

	short curr_level = 1;
	int fts_options = FTS_PHYSICAL;
	long user_bsize = 512;

	if (ls_options->show_self_parent) {
		fts_options |= FTS_SEEDOT;
	}

	fcomp = chooseSort(ls_options);	

	if (ls_options->report_in_kb) {
		user_bsize = 1024;
	} else {
		(void)getbsize(NULL, &user_bsize);
	}
	
	fts_hier = fts_open(inputs, fts_options, fcomp);
	while ((fts_ent = fts_read(fts_hier)) != NULL) {
		if (fts_ent->fts_errno != 0) {
			printf("%s: %s: %s\n", getprogname(),
				fts_ent->fts_name, 
				strerror(fts_ent->fts_errno));	
			continue;
		}

		if (fts_ent->fts_level > curr_level) {
			printf("\n");
			printf("%s:\n", fts_ent->fts_parent->fts_name);
			curr_level = fts_ent->fts_level;
		}

		if (showEntry(fts_ent, ls_options)) {
			printEntry(fts_ent, user_bsize, ls_options);
		}
	}

	if (errno != 0) {
		perror("FTS traversal");
		exit(EXIT_FAILURE);
	}

	(void)fts_close(fts_hier);

	return;
}
