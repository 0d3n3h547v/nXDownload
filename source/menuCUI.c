#include <stdio.h>
#include <string.h>
#include <switch.h>
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
	
	struct menu initial;
	
	char *tiles[16] = {
		"Update latest beta nXDownload",
		"Download from input.txt",
		"Download temporaly URL",
		"File Transfer Protocol (FTP)",
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