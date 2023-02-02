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

#ifndef LS_HELPERS_H
#define LS_HELPERS_H

#include <sys/stat.h>

typedef struct Options {
	int show_self_parent;
	int list_dir_recursive;
	int human_readable;
	int mark_nonprinting;
	int plain_dirs;
	int print_bsize;
	int print_file_type;
	int print_inode;
	int print_long_format;
	int print_numeric_uid_gid;
	int report_in_kb;
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
