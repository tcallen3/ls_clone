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

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "helpers.h"

void
usage(const char *synopsis)
{
	fprintf(stderr, "usage: %s [-%s] [file...]\n", getprogname(), 
	    synopsis);
	exit(EXIT_FAILURE);
}

void 
listDirectory(char **inputs, const Options *ls_options)
{
	if (ls_options->list_dir_recursive) {
		traverseRecursive(inputs, ls_options);
	} else {
		traverseShallow(inputs, ls_options);
	}
}

void
normalizeDirNames(const int argc, char **argv)
{
	int i = 0;
	size_t len = 0;

	for (i = 0; i < argc; i++) {
		len = strlen(argv[i]);
		if (len > 1 && (argv[i][len-1] == '/')) {
			argv[i][len-1] = '\0';
		}
	}
}

int
main(int argc, char *argv[])
{
	int ch;
	const char *all_opts = "AacdFfhiklnqRrSstuw";
	char *local_default[2] = {".", NULL};
	char **file_targets = NULL;
	Options prog_options;

	setprogname(argv[0]);

	setDefaultOptions(&prog_options);

	if (isatty(STDOUT_FILENO)) {
		prog_options.mark_nonprinting = 1;
	} else {
		prog_options.mark_nonprinting = 0;
	}

	while ((ch = getopt(argc, argv, all_opts)) != -1) {
		switch (ch) {
		case 'A':
			prog_options.show_hidden = 1;
			break;
		case 'a':
			prog_options.show_self_parent = 1;
			prog_options.show_hidden = 1;
			break;
		case 'c':
			prog_options.sort_by_atime = 0;
			prog_options.sort_by_mtime = 0;
			prog_options.sort_by_ctime = 1;
			break;
		case 'd':
			prog_options.plain_dirs = 1;
			/* is this hackish? */
			prog_options.show_dir_header = 1;
			break;
		case 'F':
			prog_options.print_file_type = 1;
			break;
		case 'f':
			/* as in NetBSD, we take -f to imply -a */
			prog_options.show_hidden = 1;
			prog_options.do_not_sort = 1;
			break;
		case 'h':
			prog_options.report_in_kb = 0;
			prog_options.human_readable = 1;
			break;
		case 'i':
			prog_options.print_inode = 1;
			break;
		case 'k':
			prog_options.human_readable = 0;
			prog_options.report_in_kb = 1;
			break;
		case 'l':
			prog_options.print_long_format = 1;
			prog_options.print_numeric_uid_gid = 0;
			break;
		case 'n':
			prog_options.print_long_format = 1;
			prog_options.print_numeric_uid_gid = 1;
			break;
		case 'q':
			prog_options.mark_nonprinting = 1;
			break;
		case 'R':
			prog_options.list_dir_recursive = 1;
			break;
		case 'r':
			setReverseSort();
			break;
		case 'S':
			prog_options.sort_by_size = 1;
			break;
		case 's':
			prog_options.print_bsize = 1;
			break;
		case 't':
			prog_options.sort_time = 1;
			prog_options.sort_by_atime = 0;
			prog_options.sort_by_mtime = 1;
			prog_options.sort_by_ctime = 0;
			break;
		case 'u':
			prog_options.sort_by_atime = 1;
			prog_options.sort_by_mtime = 0;
			prog_options.sort_by_ctime = 0;
			break;
		case 'w':
			prog_options.mark_nonprinting = 0;
			break;
		case '?':
		default:
			usage(all_opts);
			/* NOTREACHED */
		}
	}
	
	argc -= optind;
	argv += optind;

	if (argc > 1) {
		prog_options.show_dir_header = 1;
	}

	normalizeDirNames(argc, argv);
	
	file_targets = argv;
	if (argc == 0) {
		/* if no files are specified, list contents of CWD */
		file_targets = local_default;
	}

	listDirectory(file_targets, &prog_options);

	return EXIT_SUCCESS;
}
