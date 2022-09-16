#include <sys/stat.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "helpers.h"

static int STAT_ERROR = 1;
static int NODE_SUCCESS = 0;

void 
setDefaultOptions(Options *opts)
{
	opts->do_not_sort = 0;
	opts->show_dot_dirs = 0;
	opts->sort_by_name = 1;
}

PathList *
newPathList()
{
	PathList *plist = NULL;
	if ((plist = malloc(sizeof(*plist))) == NULL) {
		fprintf(stderr, "Could not allocate file path list: %s\n",
		    strerror(errno));
		exit(EXIT_FAILURE);
	}

	plist->head = NULL;
	plist->size = 0;
	return plist;
}

int 
addPath(PathList *plist, const char *path)
{
	size_t len = 0;
	struct stat *sb = NULL;
	PathNode *node = NULL;

	len = strlen(path) + 1;
	/* 
	   must pass pointer-to-pointer for name, since we need to
	   assign the pointer itself by reference - see man reallocarr 
	*/
	if ((node = malloc(sizeof(*node))) == NULL) {
		fprintf(stderr, "Could not allocate memory for node: %s\n",
		    strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (reallocarr(&node->path_name, len, sizeof(*node->path_name)) != 0) {
		fprintf(stderr, "Could not allocate memory for path: %s\n",
		    strerror(errno));
		exit(EXIT_FAILURE);
	}

	strncpy(node->path_name, path, len);

	if ((sb = malloc(sizeof(*sb))) == NULL) {
		fprintf(stderr, "Could not allocate memory for stat: %s\n",
		    strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (lstat(node->path_name, sb) == -1) {
		fprintf(stderr, "Cannot stat %s: %s\n", node->path_name, 
		    strerror(errno));

		/* this isn't a fatal error, cleanup and move on */
		(void)free(node->path_name);
		node->path_name = NULL;
		(void)free(sb);

		return STAT_ERROR;
	}

	node->next = plist->head;
	node->path_stat = sb;

	plist->head = node;
	plist->size++;

	return NODE_SUCCESS;
}

int 
nameComp(const void *a, const void *b)
{
	PathNode *first = *(PathNode **)a;
	PathNode *second = *(PathNode **)b;

	return strcmp(first->path_name, second->path_name);
}

/*
 * We sort the list by creating an array of pointers to entries
 * then indirecting through the array so we can use qsort. After,
 * we re-order the list from the array back-to-front, so it stays
 * ordered despite inserting at the head
 */
void 
sortPaths(PathList *plist, const Options *ls_options)
{
	PathList internal_list;

	size_t index = 0;
	size_t derived = 0;
	size_t plsize = plist->size;
	PathNode *curr_node = NULL;
	PathNode **parray = NULL;

	if (ls_options->do_not_sort || plsize == 0) {
		return;
	}

	if (reallocarr(&parray, plsize, sizeof(*parray)) != 0) {
		fprintf(stderr, "Could not allocate sorting array: %s\n",
		    strerror(errno));
		exit(EXIT_FAILURE);
	}

	curr_node = plist->head;
	index = 0;
	while (curr_node != NULL) {
		parray[index++] = curr_node;
		curr_node = curr_node->next;
	}
	
	/* for now, just sorting by name */
	qsort(parray, plsize, sizeof(*parray), nameComp);

	internal_list.head = NULL;
	internal_list.size = plsize;

	/* insert from head in reverse on new list */
	for (index = 0; index < plsize; index++) {
		derived = (plsize - 1) - index;
		parray[derived]->next = internal_list.head;
		internal_list.head = parray[derived];
	}

	/* swap pointers */
	plist->head = internal_list.head;
	plist->size = internal_list.size;

	(void)free(parray);
	parray = NULL;
}

void 
freePathList(PathList *plist)
{
	PathNode *curr = NULL;
	PathNode *temp = NULL;

	curr = plist->head;
	while (curr != NULL) {
		temp = curr;
		curr = curr->next;

		if (temp->path_name != NULL) {
			(void)free(temp->path_name);
			temp->path_name = NULL;
		}

		if (temp->path_stat != NULL) {
			(void)free(temp->path_stat);
			temp->path_stat = NULL;
		}

		(void)free(temp);
		temp = NULL;
	}

	(void)free(plist);
	plist = NULL;
}
