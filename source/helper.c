#include <stdio.h>
#include "includes/helper.h"
#include "includes/menuCUI.h"

char *getArgumentFromTmpFile(char *file) {
	
	FILE *tmpfile;
	
	if ((tmpfile = fopen(file, "rb")) == NULL) {
		
		printf("\n# Error: %s%s%s", CONSOLE_RED, "Check if exist 'tmpfile.txt'", CONSOLE_RESET);
		return NULL;
		
	} else {
		
		fgets(file, 256, tmpfile);
		fclose(tmpfile);
		
		if (file == NULL)
		{
			printf("\n# Error: %s%s%s", CONSOLE_RED, "No link founded.", CONSOLE_RESET);
			return NULL;
		}
		
	}
	
	printf("\n# %s%s%s", CONSOLE_GREEN, "Founded argument/link to use", CONSOLE_RESET);
	
	// return argument "link"
	return file; 
}

char *getArgumentFromInputFile(char *download[]) {
	
	// array download will contain the argument <download-link>
	// array description will contain the argument <desc-of-download>
	char description[512][512];
	
	// just using n to count on how many links have founded; used for debugging
	int n;
	FILE *dest;
	
	// i think this is the only way to be sure to open a file in GOOD condition, without racing
	if ((dest = fopen("input.txt","r")) != NULL) {
		for (n = 0; n != 512; n++) {
	
			if (fscanf(dest, "%s = %s", description[n], download[n]) != 2) break;
		
			printf("\x1b[1;1H Position N.%d", n);
		
			if (n == 511) printf("\n# %s%s%s", CONSOLE_RED, "Too many links that i can't handle! My MAX is 511 links!", CONSOLE_RESET);
		
			consoleUpdate(NULL);
		}
		
		// containing the number of how many links founded; used for resetting n
		int max_n = n;
		fclose(dest);
	
		if (description[n] == NULL || download[n] == NULL) { // error: no arguments founded as the file was open
			printf("\n# %s%s%s",
			CONSOLE_RED, 
			"There was an err while reading arguments:\nCheck that 'input.txt' at least is made like this:\n===============================\n<title> = <download/link/url>", 
			CONSOLE_RESET);
		
			return NULL;
		}
		
		while(appletMainLoop()) {
			
			hidScanInput();
			u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
		
			printf("\x1b[1;1H Position N.%d", n);
			printf("\x1b[5;1HStart = %s%s%s\n", CONSOLE_BLUE, description[n], CONSOLE_RESET);
			printf("\nURL = %s%s%s", CONSOLE_GREEN, download[n], CONSOLE_RESET);
			printf("\x1b[10;1HPress D-PAD [->] to look forward\n");
			printf("Press D-PAD [<-] to look backward\n");
			printf("\nPress [A] to start download\n");
			printf("Press [B] to go back\n");
			
			if (kDown & KEY_DLEFT) {
				consoleClear();
				if ((n--) < 0) n = max_n;
			}
			
			if (kDown & KEY_DRIGHT) {
				consoleClear();
				if ((n++) > max_n) n = 0;
			}
		
			if (kDown & KEY_B) return NULL;
			
			if (kDown & KEY_A) {
				break;
			}
			
			consoleUpdate(NULL);
		}
		
		printf("\n# Selected url = %s%s%s", CONSOLE_GREEN, download[n], CONSOLE_RESET);
		consoleUpdate(NULL);
		
		// passing url to char* for libcurl; Now we need tmp1 to parse the code to get the filename
		fclose(dest);
		
	} else { // error opening dest
	
		printf("\n# %s%s%s", CONSOLE_RED, "There is no input.txt!", CONSOLE_RESET);
		consoleUpdate(NULL);
		return NULL;
		
	}
	
	return download[n];
}