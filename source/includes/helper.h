#ifndef HELPER_H
#define HELPER_H

#include <sys/stat.h>
#include <stdbool.h>
#include <stdio.h>
#include <switch.h>

typedef	struct	heapMemory {
	void		*haddr;
	size_t		size;
	size_t		mem_available;
	size_t		mem_used;
}				s_heapMemory; // s prefix for struct

bool	isFileExist(const char *file);
void	userAppInit(void);
void	getHeapMemory(s_heapMemory heapMemory);

#endif
