#ifndef HELPER_H
#define HELPER_H

#include <sys/stat.h>
#include <stdbool.h>
#include <stdio.h>
#include <switch.h>
#include <unistd.h>
#include <stdlib.h>

#include "get_next_line.h"

#define INPUT_TXT	1
#define TMPFILE_TXT	2

typedef	struct	heapMemory {
	void		*haddr;
	size_t		size;
	size_t		mem_available;
	size_t		mem_used;
}				s_heapMemory; // s prefix for struct

bool	isFileExist(const char *file);
void	userAppInit(void);
void	getHeapMemory(s_heapMemory heapMemory);
size_t	countLinesInFile(int fd);
int		getLinksInFile(const char *filename, char ***links, char ***desc);
void	freeArray(char **array);

#endif
