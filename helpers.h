#ifndef LS_HELPERS
#define LS_HELPERS

#include <sys/stat.h>

typedef struct Options {
	int hide_self_parent;
	int list_dir_recursive;
	int show_dot_dirs;
	int sort_by_size;
	int sort_time;
	int sort_by_ctime;
	int sort_by_mtime;
	int sort_by_atime;
	int do_not_sort;
} Options;

typedef struct PathNode {
	struct PathNode *next;
	char *path_name;
	struct stat *path_stat;
} PathNode;

typedef struct PathList {
	struct PathNode *head;
	size_t size;
} PathList;

void setDefaultOptions(Options *);
void traverseShallow(char **, const Options *);
void traverseRecursive(char **, const Options *);

#endif /* LS_HELPERS */
