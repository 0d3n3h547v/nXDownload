#include <stdio.h>
#include <stdlib.h>
#include <switch.h>
#include "includes/download.h"
#include "includes/menuCUI.h"

int main(int argc, char **argv) {
	consoleInit(NULL);
	curlInit();
	int a = 0;
	
	LOOP:
	a = menu_main();
	
	printf("\n\n%s%s%s", CONSOLE_GREEN, "Finish!\n", CONSOLE_RESET);
	printf("\npress [+] to exit");
	printf("\npress [-] to continue");
	
	while(appletMainLoop()) {
		hidScanInput();
		u32 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
		
		if (kDown & KEY_PLUS) break;
		if (kDown & KEY_MINUS) {
			a = a + 1;
			break;
		}
		
		consoleUpdate(NULL);
	}
	
	if (a==1) {
		a = 0;
		goto LOOP;
	}
	
	curlExit();
	consoleExit(NULL);
	return (EXIT_SUCCESS);
}