#include <stdio.h>
#include <stdlib.h>
#include <switch.h>
#include "includes/download.h"

int main(int argc, char **argv) {
	
	while(appletMainLoop()) {
		hidScanInput();
		u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
		
		if (kDown & KEY_PLUS) break;
	}
	
	return (EXIT_SUCCESS);
}