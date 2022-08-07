#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void
usage(const char *synopsis)
{
	fprintf(stderr, "usage: %s [-%s] [file...]\n", getprogname(), 
	    synopsis);
	exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
	int ch;
	const char *all_opts = "oj";
	const char *synopsis = "oj";

	setprogname(argv[0]);

	while ((ch = getopt(argc, argv, all_opts)) != -1) {
		switch (ch) {
		case 'o':
			printf("Option 'o' selected\n");
			break;
		case 'j':
			printf("Option 'j' selected\n");
			break;
		case '?':
		default:
			usage(synopsis);
			/* NOTREACHED */
		}
	}
	
	argc -= optind;
	argv += optind;

	while (argc > 0) {
		printf("File argument: %s\n", argv[0]);
		argc--;
		argv++;
	}

	return EXIT_SUCCESS;
}
