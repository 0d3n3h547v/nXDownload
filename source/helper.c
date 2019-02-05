#include "includes/helper.h"

void userAppInit(void)
{
	void *addr = NULL;
	if (svcSetHeapSize(&addr, 0x4000000) == -1)
		fatalSimple(0);
}

bool	isFileExist(const char *file)
{
	struct stat	st = {0};

	if (stat(file, &st) == -1) {
		return (false);
	}

	return (true);
}

void	getHeapMemory(s_heapMemory heapMemory)
{
	svcGetInfo(&heapMemory.mem_available, 6, CUR_PROCESS_HANDLE, 0);
	svcGetInfo(&heapMemory.mem_used, 7, CUR_PROCESS_HANDLE, 0);
	
	if (heapMemory.mem_available > heapMemory.mem_used+0x200000)
		heapMemory.size = (heapMemory.mem_available - heapMemory.mem_used - 0x200000) & ~0x1FFFFF;
	
	if (heapMemory.size == 0)
		heapMemory.size = 0x2000000*16;
	// if(R_FAILED(svcSetHeapSize(&haddr, size))) printf("\x1b[1;1H%s%s%lx%s", CONSOLE_RED, "Error while allocating heap to: 0x", size, CONSOLE_RESET);
	
	if(R_FAILED(svcSetHeapSize(&heapMemory.haddr, 0x10000000)))
		printf("\n# Error: %s%s%s", CONSOLE_RED, "Error while allocating Heap Size", CONSOLE_RESET);
}

