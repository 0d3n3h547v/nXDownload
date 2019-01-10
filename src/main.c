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
void fileDownload(char *url, char path[], int a);

int loop = 0;

void WriteUrl() {
	
	printf("\x1b[3;25HnXDownload v0.9b %s%s%s", CONSOLE_GREEN, "(experimental)", CONSOLE_RESET);
	
	FILE *fp;
	fp = fopen("sdmc:/switch/nXDownload/tmpfile.txt", "w");
	
	/* get heap memory */
	
	void *haddr;
	Result rc;
	
	size_t size = 0;
	size_t mem_available = 0, mem_used = 0;
	
	svcGetInfo(&mem_available, 6, CUR_PROCESS_HANDLE, 0);
	svcGetInfo(&mem_used, 7, CUR_PROCESS_HANDLE, 0);
	
	if (mem_available > mem_used+0x200000) size = (mem_available - mem_used - 0x200000) & ~0x1FFFFF;
	
	if (size == 0) size = 0x2000000*16;
	// if(R_FAILED(svcSetHeapSize(&haddr, size))) printf("\x1b[1;1H%s%s%lx%s", CONSOLE_RED, "Error while allocating heap to: 0x", size, CONSOLE_RESET);
	
	if(R_FAILED(svcSetHeapSize(&haddr, 0x10000000))) return;
	
	SwkbdConfig skp; // Software Keyboard Pointer
	
	rc = swkbdCreate(&skp, 0);
	char tmpoutstr[256] = {0};
	
	if (R_SUCCEEDED(rc)) {
		
		swkbdConfigMakePresetDefault(&skp);
		swkbdConfigSetGuideText(&skp, "insert a http:// direct download link");
	
		rc = swkbdShow(&skp, tmpoutstr, sizeof(tmpoutstr));
		fprintf(fp, "%s", tmpoutstr);
		swkbdClose(&skp);
		
	}
	
	char *token; char buf[256];
	token = strtok(tmpoutstr, "/");
	
	while(token != NULL) {
		token = strtok(NULL, "/");
		if (token != NULL) strcpy(buf, token);
	}
	
	if(R_FAILED(svcSetHeapSize(&haddr, size))) printf("\x1b[1;3H%s%s%lx%s", CONSOLE_RED, "Error while allocating heap to: 0x", size, CONSOLE_RESET);
	
	int n;
	for (n = 0; buf[n] != '\0'; n++)  {
		if (buf[n] == '.') goto SKIP;
	}
	
	if (buf[n] == '\0') {
		
		printf("\x1b[6;3H%s%s%s", CONSOLE_YELLOW, "No extension founded for your filename from your link. Want to add it now?", CONSOLE_RESET);
		printf("\x1b[8;3HPress [A] to continue");
		printf("\x1b[9;3HPress [B] to skip");
		
		while(appletMainLoop()) {
			hidScanInput();
			u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
		
			if (kDown & KEY_A) {
				rc = swkbdCreate(&skp, 0);
				
				if (R_SUCCEEDED(rc)) {
					
					char tmp[16] = {0};
					swkbdConfigMakePresetDefault(&skp);
					swkbdConfigSetGuideText(&skp, "insert extension i.e. 'nsp' or 'rar' without '.'");
	
					rc = swkbdShow(&skp, tmp, sizeof(tmp));
					fprintf(fp, ".%s", tmp);
					swkbdClose(&skp);
					break;
				}
			}
			if (kDown & KEY_B) break;
				
		
			consoleUpdate(NULL);
		}
	}
	
	SKIP:
	
	fclose(fp);
	return;
	
}

void MainMenu() {
	
	consoleClear();
	loop = 0;
	int b = 0;
	int select = 5;
	
	printf("\x1b[3;25HnXDownload v0.9b %s%s%s", CONSOLE_GREEN, "(experimental)", CONSOLE_RESET);
	
	printf("\x1b[5;3HUpdate latest beta nXDownload");
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
			if (select == 9) return; // exit
			
			if (select == 5) break; // update
			if (select == 6) { // download from input.txt
				b = b + 1;
				break;
			}
			if (select == 7) { // write temporaly link
				consoleClear();
				b = b + 2;
				break;
			}
		}
			
        consoleUpdate(NULL);
    }
	
	consoleClear();
	
	if (loop == 1) return;
	
	if (b == 2) {
		WriteUrl();
		fileDownload(NULL, "sdmc:/switch/nXDownload/", 2); // initialize download from tmpfile txt
	}
	
	if (b == 1) fileDownload(NULL, "sdmc:/switch/nXDownload/", 1); // initialize download from input txt
	if (b == 0) nXDownloadUpdate(); // initialize download
	
	printf("\n\n%s%s%s", CONSOLE_GREEN, "Finish!\n", CONSOLE_RESET);
	printf("\npress [+] to exit");
	printf("\npress [-] to continue");
	
	while(appletMainLoop()) {
		hidScanInput();
		u32 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
		
		if (kDown & KEY_PLUS) break;
		if (kDown & KEY_MINUS) {
			loop = loop + 1;
			break;
		}
		
		consoleUpdate(NULL);
	}
	return;
}

int main(void)
{
    consoleInit(NULL);
	curlInit(); // inititialize sequel
	
	START:
    MainMenu();
	
	if (loop == 1) goto START;
	
	curlExit(); // terminate sockets
	consoleExit(NULL); // terminate app
    return (EXIT_SUCCESS);
}