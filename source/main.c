#include <stdio.h>
#include <stdlib.h>
#include <switch.h>
#include <sys/stat.h>
#include "includes/download.h"
#include "includes/menuCUI.h"

#ifdef DEBUG
#include <sys/socket.h>
#endif

int main(int argc, char **argv) {
	consoleInit(NULL);
	curlInit();

	// redirect stdio to nxlink
	#ifdef DEBUG
	socketInitializeDefault();
	nxlinkStdio();
	#endif

	// Create directory if not exist
	struct stat st = {0};

	if (stat("sdmc:/switch/nXDownload", &st) == -1) {
		mkdir("sdmc:/switch/nXDownload", 0755);
	}

	// false should continue
	// true should be returning

	while (true) 
		if (menu_main() == true) break;
	
	curlExit();
	consoleExit(NULL);
	#ifdef DEBUG
	socketExit();
	nxlinkStdio();
	#endif
	return (EXIT_SUCCESS);
}
