#include <stdio.h>
#include <stdlib.h>
#include <switch.h>
#include <sys/stat.h>
#include "includes/download.h"
#include "includes/menuCUI.h"

int main(int argc, char **argv) {
	consoleInit(NULL);
	curlInit();

	// Create directory if not exist
	struct stat st = {0};

	if (stat("sdmc:/switch/nXDownload", &st) == -1) {
		mkdir("sdmc:/switch/nXDownload", 0755);
	}

	// false should continue
	// true should be returning

	while (true) {
		if (menu_main() == true)
			break ;
	}

	curlExit();
	consoleExit(NULL);
	return (EXIT_SUCCESS);
}
