#include "includes/helper.h"

// This function open a file and return the content in a char *
char	**getLinksInFile(const char *filename)
{
	int		fd = 0;
	char	**array = NULL;
	char	*line = NULL;
	int		nb_lines = 0;
	char	desc[512] = {0}, link[512] = {0};

	fd = open(filename, O_RDONLY);
	if (fd == -1) {
		printf("error\n");
		return (NULL);
	}

	// if error while reading file return NULL
	nb_lines = countLinesInFile(fd);
	if (nb_lines == -1) {
		printf("error\n");
		return NULL;
	}

	// Alloc array whith number of line in file
	// x2 for description and link
	// [description]
	// [link]
	// +1 for NULL
	array = (char **)calloc((nb_lines * 2) + 1, sizeof(char *));
	if (array == NULL)
	{
		printf("error\n");
		return (NULL);
	}

	// store description and link
	for (int i = 0, j = 1; get_next_line(fd, &line); i+=2, j+=2) {
		sscanf(line, "%s = %s", desc, link);
		array[i] = strdup(desc);
		array[j] = strdup(link);
		free(line);
	}

	line = NULL;

	return (array);
}

size_t	countLinesInFile(int fd)
{
	char		*line = NULL;
	int			nb_lines = 0;
	off_t		position = 0;
	struct stat	st;

	if (fstat(fd, &st) == -1) {
		return (-1);
	}

	// if file is empty return 0
	if (st.st_size == 0) {
		return (0);
	}

	// save offset position
	position = lseek(fd, 0, SEEK_CUR);

	// count number of '\n'
	while (get_next_line(fd, &line) > 0) {
		nb_lines++;
		free(line);
		line = NULL;
	}

	// reset ptr
	lseek(fd, position, SEEK_SET);

	return (nb_lines);
}

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

