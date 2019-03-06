#include <stdio.h>
#include <stdlib.h>
#include <switch.h>
#include <unistd.h>
#include "includes/download.h"
#include "includes/menuCUI.h"
#include "includes/helper.h"

#ifdef DEBUG
#include <sys/socket.h>
#endif

int main(void) {
	consoleInit(NULL);
	curlInit();

	// redirect stdio to nxlink
	#ifdef DEBUG
	socketInitializeDefault();
	nxlinkStdio();
	#endif

	consoleClear();

	if (isFileExist("sdmc:/switch/nXDownload") == false)
		if (mkdir("sdmc:/switch/nXDownload", 0755) == -1) {
			fprintf(stderr, "%sCannot create /switch/nXDownload, exiting ...%s\n", CONSOLE_RED, CONSOLE_RESET);
			goto exit;
		}

	if (chdir("sdmc:/switch/nXDownload/") == -1) {
		fprintf(stderr, "%sCannot enter to /switch/nXDownload, exiting ...%s\n", CONSOLE_RED, CONSOLE_RESET);
		goto exit;
	}

	// false should continue
	// true should be returning

	while (true) 
		if (menu_main() == true) break;


exit:
	curlExit();
	consoleExit(NULL);

	#ifdef DEBUG
		socketExit();
		nxlinkStdio();
	#endif

	return (EXIT_SUCCESS);
}
