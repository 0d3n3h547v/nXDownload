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
	
	printf("\x1b[5;32HWrite down the url");
	printf("\x1b[7;1H(is better if you use some sort of shrinker link, be sure at least that doesnt redirect you to some ads or HTTPS because is not supported from the switch.)");
	
	printf("\x1b[10;1Hhttp://a");
	printf("\x1b[11;8H^");
	
	printf("\x1b[20;3HPress [+] to continue; Press [-] to go back");
	
	FILE *fp;
	fp = fopen("sdmc:/switch/nXDownload/tmpfile.txt", "wb");
	
	int write = 0;
	int writeY = 8;
	
	char str[] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','y','x','z','/','.','+','-','_','!','?','=','&','%','#','*','1','2','3','4','5','6','7','8','9','0'};
	
	fprintf(fp, "http://");
	
	while(appletMainLoop()) {
		hidScanInput();
		u32 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
		printf("\x1b[10;%dH%c", writeY, str[write]);
		
		if (write == 48) write = 0;
		if (write == -1) write = 47;
		
		if (kDown & KEY_DUP) write++;
		
		if (kDown & KEY_DDOWN) write--;
			
		if (kDown & KEY_A) {
			fprintf(fp, "%c", str[write]);
			printf("\x1b[11;%dH ", writeY);
			writeY = writeY + 1;
			printf("\x1b[11;%dH^", writeY);
		}
			
		if (kDown & KEY_MINUS) {
			loop = loop + 1;
			break;
		}
		
		if (kDown & KEY_PLUS) break;
		
		gfxFlushBuffers();
        gfxSwapBuffers();
	}
	
	fclose(fp);
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
				b = b + 2;
				break;
			}
		}
			
        gfxFlushBuffers();
        gfxSwapBuffers();
    }
	
	consoleClear();
	
	if (loop == 1) return;
	
	if (b == 2) {
		WriteUrl();
		fileDownload(NULL, "sdmc:/switch/nXDownload/", "test", "txt", 2); // initialize download from tmp txt
	}
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