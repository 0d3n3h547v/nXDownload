#include "includes/helper.h"
#include "includes/download.h"

void	deInit(void)
{
	curlExit();
	consoleExit(NULL);
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

// This function pop a keyboard.
// This function alloc with calloc. Don't forget to free
// message if the message to print in background
// size is size to alloc for the message
char	*popKeyboard(char *message, size_t size)
{
	SwkbdConfig	skp; // Software Keyboard Pointer
	Result		rc = swkbdCreate(&skp, 0);
	char		*tmpout = NULL;

	// +1 for the '\0'
	tmpout = (char *)calloc(sizeof(char), size + 1);
	if (tmpout == NULL)
		return (NULL);

	if (R_SUCCEEDED(rc)) {
		swkbdConfigMakePresetDefault(&skp);
		swkbdConfigSetGuideText(&skp, message);
		rc = swkbdShow(&skp, tmpout, size);
		swkbdClose(&skp);
	} else {
		free(tmpout);
		tmpout = NULL;
	}

	return (tmpout);
}

static bool	checkDesc(char *line)
{
	while (*line) {
		if (!isascii(*line))
			{ return (false); }
		++line;
	}

	return (true);
}

static bool	checkUrl(char *line)
{
	if (!strncmp(line, "http://", 7)) {
		line += 7;
	} else if (!strncmp(line, "https://", 8)) {
		line += 8;
	} else {
		return (false);
	}


	return (checkDesc(line));
}

static bool	checkLine(const char *line)
{
	char	*tmp = strdup(line);
	char	*ptr = strtok(tmp, " =");

	// strlen for exclude line with only '\n'
	if (ptr == NULL || strlen(line) == 1)
		{ free(tmp); return (false); }

	for (int i = 0; ptr; i++) {
		// if i >= 2 it mean we have more than desc and url
		if (i == 2)
			{  free(tmp); return (false); }

		// Check if desc and url good formatted
		if (i == 0 && checkDesc(ptr) == false)
			{  free(tmp); return (false); }
		else if (i == 1 && checkUrl(ptr) == false)
			{  free(tmp); return (false); }

		ptr = strtok(NULL, " =");
	}

	return (true);
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
	for (int i = 0; get_next_line(fd, &line) > 0;) {
		if (checkLine(line) == true) {
			sscanf(line, "%s = %s\n", tmp_desc, tmp_link);

			if (tmp_link[0] != '\0' && tmp_desc[0] != '\0') {
				tab[i] = strdup(tmp_link);
				tab2[i] = strdup(tmp_desc);
				i++;
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

	// count number of valid lines
	while (get_next_line(fd, &line) > 0) {
		if (checkLine(line) == true)
			{ nb_lines++; }
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

