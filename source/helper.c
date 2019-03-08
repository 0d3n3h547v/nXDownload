#include "includes/helper.h"
#include "includes/download.h"

void	deInit(void)
{
	curlExit();
	consoleExit(NULL);

	#ifdef DEBUG
		socketExit();
		nxlinkStdio();
	#endif
}

void	freeArray(char **array)
{
	for (int i = 0; array[i]; i++) {
		free(array[i]);
		array[i] = NULL;
	}
	free(array);
	array = NULL;
}

// This function open filename and fill links, desc and return number of
// line in the file
int	getLinksInFile(const char *filename, char ***links, char ***desc)
{
	int		fd = 0;
	int		nb_lines = 0;
	char	*line = NULL;
	char	tmp_desc[512] = {0}, tmp_link[512] = {0};

	fd = open(filename, O_RDONLY);
	if (fd == -1) {
		return (-1);
	}

	// if error while reading file return NULL
	nb_lines = countLinesInFile(fd);
	if (nb_lines == -1) {
		return (-1);
	}

	// Alloc array whith number of line in file
	char	**tab = (char **)calloc(nb_lines + 1, sizeof(char *));
	char	**tab2 = (char **)calloc(nb_lines + 1, sizeof(char *));
	if (tab == NULL || tab2 == NULL) {
		printf("calloc error\n");
		return (-1);
	}

	// store description and link
	for (int i = 0; get_next_line(fd, &line) > 0; i++) {
		// ignore empty line
		if (line != NULL) {
			sscanf(line, "%s = %s\n", tmp_desc, tmp_link);

			if (tmp_link[0] != '\0' && tmp_desc[0] != '\0') {
				tab[i] = strdup(tmp_link);
				tab2[i] = strdup(tmp_desc);
			}

			memset(&tmp_desc, 0, sizeof(tmp_desc));
			memset(&tmp_link, 0, sizeof(tmp_link));
		}
		free(line);
		line = NULL;
	}

	close(fd);

	*links = tab;
	*desc = tab2;

	return (nb_lines);
}

// count number of line in file
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
		// ignore empty line
		if (line != NULL) {
			nb_lines++;
		}
		free(line);
		line = NULL;
	}

	// reset offset position
	lseek(fd, position, SEEK_SET);

	return (nb_lines);
}

void userAppInit(void)
{
	void *addr = NULL;
	if (svcSetHeapSize(&addr, 0x4000000) == (Result)-1)
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

