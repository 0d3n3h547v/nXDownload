/* Dontwait project */

/* includes */
#include <string.h>
#include <stdio.h>
#include <switch.h>
#include <sys/socket.h>

/* custom includes */
#include "curl.h"

/* defines & voids */
void fileDownload(char *url, char path[], char filename[], char extension[], int a);

int main(void)
{
    gfxInitDefault();
    consoleInit(NULL);
	socketInitializeDefault(); // initialize sockets
	
    printf("Press + to use default url\n");
	printf("Press - to use custom url from file ''url.txt''\n");
	int b = 0;
	
    while(appletMainLoop())
    {
        hidScanInput();

        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        if (kDown & KEY_PLUS) break; // break in order to return to hbmenu
		if (kDown & KEY_MINUS)
		{
			int b = b + 1;
			break;
		}
		
        gfxFlushBuffers();
        gfxSwapBuffers();
    }
	
	if (b == 1) fileDownload(NULL, "sdmc:/switch/", "test", "nro", 1); // initialize download from txt
	if (b == 0) fileDownload("http://www.quora.com/How-can-I-convert-an-integer-to-a-char-array-in-C", "sdmc:/switch/", "test", "nro", 1); // initialize download

	printf("\n\nDone! Press [+] to exit");
	
	while(appletMainLoop())
	{
        hidScanInput();

        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        if (kDown & KEY_PLUS) break; // break in order to return to hbmenu

        gfxFlushBuffers();
        gfxSwapBuffers();
    }
	
    gfxExit();
	socketExit(); // terminate sockets
    return (EXIT_SUCCESS);
}