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
	
	int select = 5;
	short int bkp_n = initial.n + 5;
	short int bkp_m = initial.m;
	
	/* Look here for colored text >> https://switchbrew.github.io/libnx/console_8h.html */
	printf("\x1b[%d;1H%s>%s", select, CONSOLE_CYAN, CONSOLE_RESET);
	
	while(appletMainLoop()) {
		hidScanInput();
		u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
		
		if (kDown & KEY_DUP) {
			printf("\x1b[%d;1H ", select);
			select = select - 1;
			
			if (select < 5) select = bkp_m;		// 4 there is nothing
			if (select == bkp_n) select--;		// between char tiles and exit
			
			printf("\x1b[%d;1H%s>%s", select, CONSOLE_CYAN, CONSOLE_RESET);
		}
		
		if (kDown & KEY_DDOWN) {
			printf("\x1b[%d;1H ", select);
			select = select + 1;
			
			if (select == bkp_n) select++;		// select goes to exit
			if (select > bkp_m) select = 5;		// over char exit
			
			printf("\x1b[%d;1H%s>%s", select, CONSOLE_CYAN, CONSOLE_RESET);
		}
		
		if (kDown & KEY_PLUS) break;
		
		consoleUpdate(NULL);
	}
	
	curlExit();
	consoleExit(NULL);
	return (EXIT_SUCCESS);
}