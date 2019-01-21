#include <stdio.h>
#include <string.h>
#include <switch.h>
#include <stdbool.h>// bool = 1 == true; 0 == false;
#include "includes/download.h"
#include "includes/menuCUI.h"

void title(char *str) {
	var.half = strlen (str) / 2;
	var.half_length = var.half;
	var.final_length = HALF_SCREEN;

	for (; var.half <= var.half_length; var.half--) {
		var.final_length--;
		if (var.half <= 0) break;
	}

	printf ("\x1b[3;%dH%s%s%s", var.final_length, CONSOLE_GREEN, str, CONSOLE_RESET);
	return;
}

void menu_options(void) {
	
	char *tiles[16] = {
		"Update latest beta nXDownload",
		"Download from input.txt",
		"Download temporaly URL",
		"File Transfer Protocol (tmp disclosed)"
	};
	
	initial.m = 5;
	initial.exit = "Exit";
	
	for (initial.n=0; initial.n < 16 ; initial.n++) {
		if (tiles[initial.n] == NULL) break;
		printf("\x1b[%d;3H%s", initial.m, tiles[initial.n]);
		initial.m++;
	}
	
	initial.m = initial.m + 1;
	printf("\x1b[%d;3H%s", initial.m, initial.exit);
	
	return;
}

bool menu_main(void) {
	consoleClear();
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
		
		if (kDown & KEY_A) {
			if (select == 5) {
				nXDownloadUpdate();
				break;
			}
			
			if (select == 6) {
				FILE_TRANSFER_HTTP(NULL, "sdmc:/switch/nXDownload/", 1);
				return true;
			}
			
			if (select == 7) 
			{
				short int a = FILE_TRANSFER_HTTP_TEMPORALY();
				if (a == 1) FILE_TRANSFER_HTTP(NULL, "sdmc:/switch/nXDownload/", 2);
				return true;
			}
			
			if (select == 10) return false;
		}
		
		if (kDown & KEY_PLUS) return false;
		
		consoleUpdate(NULL);
	}
	return true;
}
