/* Dontwait project */

/* includes */
#include <string.h>
#include <stdio.h>
#include <switch.h>
#include <sys/socket.h>

/* custom includes */
#include "curl.h"

/* defines & voids */
#define EXIT_SUCCESS 0

void fileDownload(char *url, char *path, char *filename, char *extension, int a);
/*
inside function fileDownload() int a options:

- 1 = generates a filename of numbers from 0 up to 999998;
- 2 = prints useful text for testing curl;
- 3 = 1 & 2;
- 0 = default;

*/

int main(int argc, char **argv)
{
    gfxInitDefault();
    consoleInit(NULL);
	socketInitializeDefault(); // initialize sockets
	
    printf("Press + to continue");
	

    while(appletMainLoop())
    {
        hidScanInput();

        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        if (kDown & KEY_PLUS) break; // break in order to return to hbmenu

        gfxFlushBuffers();
        gfxSwapBuffers();
    }
	
	fileDownload("http://github.com/Dontwait00/curl-example-switch/releases/download/latest/curl-example.nro", "sdmc:/switch/", "test", ".nro", 0); // initialize download
	printf("done! Press [+] to exit");
	
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
    return EXIT_SUCCESS;
}