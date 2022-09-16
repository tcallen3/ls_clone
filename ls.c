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

void listDirectory(const char *dir_name, const Options *ls_options)
{
	DIR *dp;
	struct dirent *dir_entry;
	PathList *plist = newPathList();
	PathNode *curr = NULL;

	if ((dp = opendir(dir_name)) == NULL) {
		fprintf(stderr, "Could not open directory %s: %s\n",
		    dir_name, strerror(errno));
		exit(EXIT_FAILURE);
	}
	while ((dir_entry = readdir(dp)) != NULL) {
		if (dir_entry->d_name[0] != '.' || 
		    ls_options->show_dot_dirs) {
		    	addPath(plist, dir_entry->d_name);
			/*printf("%s\n", dir_entry->d_name);*/
		}
	}

	sortPaths(plist, ls_options);
	curr = plist->head;
	while (curr != NULL) {
		printf("%s\n", curr->path_name);
		curr = curr->next;
	}

	(void)closedir(dp);
	freePathList(plist);
}

int
main(int argc, char *argv[])
{
	int ch;
	const char *all_opts = "a";
	const char *synopsis = "a";
	const char *cwd_name = ".";
	Options prog_options;

	setprogname(argv[0]);

	setDefaultOptions(&prog_options);

	while ((ch = getopt(argc, argv, all_opts)) != -1) {
		switch (ch) {
		case 'a':
			printf("Option 'a' selected\n");
			prog_options.show_dot_dirs = 1;
			break;
		case '?':
		default:
			usage(synopsis);
			/* NOTREACHED */
		}
	}
	
	argc -= optind;
	argv += optind;

	if (argc == 0) {
		/* if no files are specified, list contents of CWD */
		listDirectory(cwd_name, &prog_options);
	}

	while (argc > 0) {
		printf("File argument: %s\n", argv[0]);
		argc--;
		argv++;
	}

	return EXIT_SUCCESS;
}
