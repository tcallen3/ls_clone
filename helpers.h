#ifndef LS_HELPERS_H
#define LS_HELPERS_H

#include <sys/stat.h>

typedef struct Options {
	int show_self_parent;
	int list_dir_recursive;
	int plain_dirs;
	int print_bsize;
	int print_inode;
	int show_hidden;
	int show_dir_header;
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

void setReverseSort();
void setDefaultOptions(Options *);
void traverseShallow(char **, const Options *);
void traverseRecursive(char **, const Options *);

#endif /* LS_HELPERS_H */
