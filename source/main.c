#include <stdio.h>
#include <stdlib.h>
#include <switch.h>
#include "includes/download.h"
#include "includes/menuCUI.h"

int main(int argc, char **argv) {
	consoleInit(NULL);
	curlInit();
	
	// false should continue
	// true should be returning
	
	int a = 0;
	
	LOOP:
	a = menu_main();
	
	if (a==0) goto LOOP;
	
	curlExit();
	consoleExit(NULL);
	return (EXIT_SUCCESS);
}