#ifndef LS_HELPERS
#define LS_HELPERS

#include <sys/stat.h>

typedef struct Options {
	int show_dot_dirs;
	int sort_by_name;
	int do_not_sort;
} Options;

typedef struct PathNode {
	struct PathNode *next;
	const char *path_name;
	struct stat *path_stat;
} PathNode;

typedef struct PathList {
	struct PathNode *head;
	size_t size;
} PathList;

void setDefaultOptions(Options *opts);

PathList *newPathList();
int addPath(PathList *plist, const char *path);
void sortPaths(PathList *plist, const Options *ls_options);
void freePathList(PathList *plist);

#endif /* LS_HELPERS */
