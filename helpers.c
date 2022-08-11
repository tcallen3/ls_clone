#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "helpers.h"

PathList *newPathList()
{
	PathList *plist = (PathList *)malloc(sizeof(PathList));
	if (plist == NULL) {
		fprintf(stderr, "Could not allocate file path list: %s\n",
		    strerror(errno));
		exit(EXIT_FAILURE);
	}

	plist->head = NULL;
	return plist;
}

int addPath(PathList *plist, const char *path)
{
	// FIXME: implement
}

void sortPaths(PathList *plist, const Options *ls_options)
{
	// FIXME: implement
}

void freePathList(PathList *plist)
{
	PathNode *curr;
	PathNode *temp;

	curr = plist->head;
	while (curr != NULL) {
		temp = curr;
		curr = curr->next;

		if (temp->path_name != NULL) {
			free(temp->path_name);
		}

		if (temp->path_stat != NULL) {
			free(temp->path_stat);
		}

		free(temp);
	}

	free(plist);
	plist = NULL;
}
