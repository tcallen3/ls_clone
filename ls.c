#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "helpers.h"

/* FIXME: swap to using fts for directory traversal */

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

int
main(int argc, char *argv[])
{
	int ch;
	const char *all_opts = "AacfStu";
	char *local_default[2] = {".", NULL};
	char **file_targets = NULL;
	Options prog_options;

	setprogname(argv[0]);

	setDefaultOptions(&prog_options);

/* FIXME: sorting by time should require -t and only c or u modify it */
	while ((ch = getopt(argc, argv, all_opts)) != -1) {
		switch (ch) {
		case 'A':
			prog_options.show_dot_dirs = 1;
			prog_options.hide_self_parent = 1;
			break;
		case 'a':
			prog_options.show_dot_dirs = 1;
			break;
		case 'c':
			prog_options.sort_by_atime = 0;
			prog_options.sort_by_mtime = 0;
			prog_options.sort_by_ctime = 1;
			break;
		case 'f':
			/* as in NetBSD, we take -f to imply -a */
			prog_options.show_dot_dirs = 1;
			prog_options.do_not_sort = 1;
			break;
		case 'S':
			prog_options.sort_by_size = 1;
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
		case '?':
		default:
			usage(all_opts);
			/* NOTREACHED */
		}
	}
	
	argc -= optind;
	argv += optind;
	
	file_targets = argv;
	if (argc == 0) {
		/* if no files are specified, list contents of CWD */
		file_targets = local_default;
	}

	listDirectory(file_targets, &prog_options);

	return EXIT_SUCCESS;
}
