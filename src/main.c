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

void WriteUrl() {
	printf("\x1b[3;33HnXDownload v0.7b");
	
	printf("\x1b[5;30HWrite down the url");
	printf("\x1b[7;1H(is better if you use some sort of shrinker link, be sure at least that doesnt redirect you to some ads or HTTPS because is not supported from the switch.)");
	
	printf("\x1b[10;3Hhttp://a");
	printf("\x1b[11;10H^");
	
	int write = 1;
	int writeY = 10;
	
	char str[] = {'a','b','c','d','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','y','x','z','/','.','+','-','_','!','?','=','&','%','#','*'};
	
	while(appletMainLoop()) {
		hidScanInput();
		u32 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
		printf("\x1b[10;%dH%c", writeY, str[write]);
		
		if (write == 39) write = 1;
		if (write == 0) write = 38;
		
		if (kDown & KEY_DUP) write++;
		
		if (kDown & KEY_DDOWN) write--;
			
		if (kDown & KEY_A) {
			strcat(url, str[write]);
			printf("\x1b[11;%dH ", writeY);
			writeY = writeY + 1;
			write = 1;
			printf("\x1b[11;%dH^", writeY);
		}
		
		if (kDown & KEY_PLUS) break;
		
		gfxFlushBuffers();
        gfxSwapBuffers();
	}
	
	return;
}

void MainMenu() {
	
	consoleClear();
	loop = 0;
	int b = 0;
	int select = 5;
	
	printf("\x1b[3;33HnXDownload v0.7b");
	
	printf("\x1b[5;3HUpdate latest nXDownload");
	printf("\x1b[6;3HDownload from input.txt");
	printf("\x1b[7;3HWrite temporaly URL");
	printf("\x1b[9;3HExit");
	printf("\x1b[%d;1H>", select);
	
	while(appletMainLoop())
	{
        hidScanInput();

        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        if (kDown & KEY_DUP) {
			printf("\x1b[%d;1H ", select);
			select = select - 1;
			
			if (select == 4) select = 9;
			if (select == 8) select = 7;
			
			printf("\x1b[%d;1H>", select);
		}
			
		if (kDown & KEY_DDOWN) {
			printf("\x1b[%d;1H ", select);
			select = select + 1;
			
			if (select == 8) select = 9;
			if (select == 10) select = 5;
			
			printf("\x1b[%d;1H>", select);
		}
		
		if (kDown & KEY_A) {
			if (select == 9) return;
			
			if (select == 5) break;
			if (select == 6) {
				b = b + 1;
				break;
			}
			if (select == 7) {
				consoleClear();
				WriteUrl();
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