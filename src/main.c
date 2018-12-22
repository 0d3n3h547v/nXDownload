/* Dontwait project */

/* includes */
#include <string.h>
#include <stdio.h>
#include <switch.h>
#include <sys/socket.h>

/* custom includes */
#include "CustomCURL.h"

/* defines & voids */
void curlInit();
void curlExit();
void nXDownloadUpdate();
void fileDownload(char *url, char path[], char filename[], char extension[], int a);

int loop = 0;

void MainMenu() {
	
	consoleClear();
	loop = 0;
	int b = 0;
	int select = 5;
	
	printf("\x1b[3;33HnXDownload v0.6b");
	
	printf("\x1b[5;3HUpdate latest nXDownload");
	printf("\x1b[6;3HDownload from input.txt");
	printf("\x1b[8;3HExit");
	printf("\x1b[%d;1H>", select);
	
	while(appletMainLoop())
	{
        hidScanInput();

        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        if (kDown & KEY_DUP) {
			printf("\x1b[%d;1H ", select);
			select = select - 1;
			
			if (select == 4) select = 8;
			if (select == 7) select = 6;
			
			printf("\x1b[%d;1H>", select);
		}
			
		if (kDown & KEY_DDOWN) {
			printf("\x1b[%d;1H ", select);
			select = select + 1;
			
			if (select == 7) select = 8;
			if (select == 9) select = 5;
			
			printf("\x1b[%d;1H>", select);
		}
		
		if (kDown & KEY_A) {
			if (select == 8) return;
			
			if (select == 5) break;
			if (select == 6) {
				b = b + 1;
				break;
			}
		}

        gfxFlushBuffers();
        gfxSwapBuffers();
    }
	
	consoleClear();
	
	if (b == 1) fileDownload(NULL, "sdmc:/switch/", "test", "nro", 1); // initialize download from txt
	if (b == 0) nXDownloadUpdate(); // initialize download
	
	printf("\n\nFinish!\n\npress [+] to exit");
	printf("\npress [-] to continue");
	
	while(appletMainLoop()) {
		hidScanInput();
		u32 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
		
		if (kDown & KEY_PLUS) break;
		if (kDown & KEY_MINUS) {
			loop = loop + 1;
			break;
		}
		
		gfxFlushBuffers();
        gfxSwapBuffers();
	}
	
	return;
}

int main(void)
{
    gfxInitDefault();
    consoleInit(NULL);
	curlInit(); // inititialize sequel
	
	START:
    MainMenu();
	
	if (loop == 1) goto START;
	
    gfxExit();
	curlExit(); // terminate sockets
	consoleExit(NULL); // terminate app
    return (EXIT_SUCCESS);
}