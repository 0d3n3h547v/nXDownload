#include <stdio.h>
#include <stdlib.h>
#include <switch.h>
#include "includes/download.h"
#include "includes/menuCUI.h"

int main(int argc, char **argv) {
	consoleInit(NULL);
	curlInit();
	
	title("nXDownload v1.0b");
	menu_options();
	
	while(appletMainLoop()) {
		hidScanInput();
		u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
		
		if (kDown & KEY_PLUS) break;
		
		consoleUpdate(NULL);
	}
	
	curlExit();
	consoleExit(NULL);
	return (EXIT_SUCCESS);
}