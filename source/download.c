/* Includes */
#include <stdio.h>
#include <switch.h>
#include <stdlib.h> // used for alloc/malloc
#include <string.h> // used for mini tools
#include <unistd.h> // used for chdir()
#include <stdbool.h>// bool = 1 == true; 0 == false;
#include <curl/curl.h>
#include "includes/download.h"

/* Functions */
void curlInit(void) {
	socketInitializeDefault();
	curl = curl_easy_init();
}

void curlExit(void) {
	appletEndBlockingHomeButton();
	socketExit();
}

void nXDownloadUpdate(void) {
	char url[] = "http://projects00.000webhostapp.com/nXDownload.nro";
	chdir("sdmc:/switch/nXDownload/");
	
	FILE *dest;
	dest = fopen("nXDownloader_newer.nro", "wb");
	
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url);			// getting URL from char *url
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);		// useful for debugging
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // skipping cert. verification, if needed
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); // skipping hostname verification, if needed
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, dest);	// writes pointer into FILE *destination
        res = curl_easy_perform(curl);						// perform tasks curl_easy_setopt asked before
		fclose(dest);
		
		if (res != CURLE_OK) {
			printf("\n# Failed: %s%s%s", CONSOLE_RED, curl_easy_strerror(res), CONSOLE_RESET);
			return;
		}
	}

	curl_easy_cleanup(curl); // Cleanup chunk data, resets functions.
	remove("sdmc:/switch/nXDownload/nXDownload.nro"); 
	rename("sdmc:/switch/nXDownload/nXDownloader_newer.nro", "sdmc:/switch/nXDownload/nXDownloader.nro");
	return;
}

bool FILE_TRANSFER_HTTP_TEMPORALY(void) {
	/* get heap memory */
	consoleClear();
	void *haddr;
	Result rc;
	
	size_t size = 0;
	size_t mem_available = 0, mem_used = 0;
	
	svcGetInfo(&mem_available, 6, CUR_PROCESS_HANDLE, 0);
	svcGetInfo(&mem_used, 7, CUR_PROCESS_HANDLE, 0);
	
	if (mem_available > mem_used+0x200000) size = (mem_available - mem_used - 0x200000) & ~0x1FFFFF;
	
	if (size == 0) size = 0x2000000*16;
	// if(R_FAILED(svcSetHeapSize(&haddr, size))) printf("\x1b[1;1H%s%s%lx%s", CONSOLE_RED, "Error while allocating heap to: 0x", size, CONSOLE_RESET);
	
	if(R_FAILED(svcSetHeapSize(&haddr, 0x10000000))) return false;
	
	FILE *fp;
	fp = fopen("sdmc:/switch/nXDownload/tmpfile.txt", "w");
	
	SwkbdConfig skp; // Software Keyboard Pointer
	
	rc = swkbdCreate(&skp, 0);
	char tmpoutstr[256] = {0};
	
	if (R_SUCCEEDED(rc)) {
		
		swkbdConfigMakePresetDefault(&skp);
		swkbdConfigSetGuideText(&skp, "insert a http:// direct download link");
	
		rc = swkbdShow(&skp, tmpoutstr, sizeof(tmpoutstr));
		if (tmpoutstr == NULL) return false;
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
		if (buf[n] == '.') {
			fclose(fp);
			return true;
		}
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
					if (tmp == NULL) return false;
					fprintf(fp, ".%s", tmp);
					swkbdClose(&skp);
					break;
				}
			}
			if (kDown & KEY_B) break;
			
			consoleUpdate(NULL);
		}
	}
	
	fclose(fp);
	return true;
	
}

void FILE_TRANSFER_HTTP(char *url, char path[], int a) {
	consoleClear();
	FILE *dest;
	chdir(path);
	
	// using tmp1 for passing url to another char array
	char tmp1[256];
	
	// buf here has the main function to extract the filename and use it as argument for fopen()
	char buf[256];
	struct myprogress prog;
	
	if (a == 2) { // useful to pass data (url) between functions; but you need to write the tmpfile.txt with inside the url, before executing this function
	
		// needs to be cleaned up here after testing
		dest = fopen("tmpfile.txt", "r");
		while(!feof(dest)) fgets(tmp1, sizeof(tmp1), dest);
		url = strdup(tmp1);
		printf("\n# %s%s%s", CONSOLE_YELLOW, "Founded argument/link to use", CONSOLE_RESET);
		fclose(dest);
		
		if (url == NULL) {
			printf("\n# %s%s%s", CONSOLE_RED, "Error passing argument", CONSOLE_RESET);
			return;
		}
		
		// if int a equivale with 2, and everything was okay, the code below is skipped
		goto SKIP; 
	}
	
	consoleClear();
	
	// array i will contain the argument <desc-of-download>
	char i[512][512];
	
	// array f will contain the argument <download-link>
	char f[512][512];
	
	// just using n to count on how many links have founded; used for debugging
	int n;
	
	consoleUpdate(NULL);
	
	// i think this is the only way to be sure to open a file in GOOD condition, without racing
	if ((dest = fopen("input.txt","r")) != NULL) {
		for (n = 0; n < 512; n++) {
			
			if (fscanf(dest, "%s = %s", i[n], f[n]) != 2) break;
			
			printf("\x1b[1;1H%d links counter", n);
			
			if (n == 511) printf("\n# %s%s%s", CONSOLE_RED, "Too many links that i can't handle! My MAX is 511 links!", CONSOLE_RESET);
			
			consoleUpdate(NULL);
		}
		
		// containing the number of how many links founded; used for resetting n
		int counter = n - 1;
		fclose(dest);
		
		if (i[n] == NULL || f[n] == NULL) { // error: no arguments founded as the file was open
			printf("\n# %s%s%s",
			CONSOLE_RED, 
			"There was an err while reading arguments:\nCheck that 'input.txt' at least is made like this:\n===============================\n<title> = <download/link/url>", 
			CONSOLE_RESET);
			
			return;
		}
		
		while(appletMainLoop()) {
			hidScanInput();
			u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
			
			printf("\x1b[1;1H%d links counter", n);
			printf("\x1b[5;1HStart = %s%s%s\n\nURL = %s%s%s", CONSOLE_BLUE, i[n], CONSOLE_RESET, CONSOLE_GREEN, f[n], CONSOLE_RESET);
			printf("\x1b[10;1HPress D-PAD [->] to look forward\n");
			printf("Press D-PAD [<-] to look backward\n");
			printf("\nPress [A] to start download\n");
			printf("Press [B] to go back\n");
			
			if (kDown & KEY_DLEFT) {
				consoleClear();
				n--;
				if (n < 0) n = counter;
			}
			
			if (kDown & KEY_DRIGHT) {
				consoleClear();
				n++;
				if (n > counter) n = 0;
			}
			
			if (kDown & KEY_B) {
				curlExit();
				return;
			}
			
			if (kDown & KEY_A) {
				sprintf(tmp1, "%s", f[n]);
				break; 
			}
			
			consoleUpdate(NULL);
		}
			
		printf("\n# Done!");
		consoleUpdate(NULL);
		
		// passing url to char* for libcurl; Now we need tmp1 to parse the code to get the filename
		url = strdup(tmp1);
		fclose(dest);
		
	} else { // error opening dest
		printf("\n# %s%s%s", CONSOLE_RED, "There is no input.txt!", CONSOLE_RESET);
		consoleUpdate(NULL);
		return;
	}
	
	SKIP:
	
	if (a == 0 || a == 1 || a == 2) { // this parses the url to mantain the last token, which (usually) contains the filename
		
		char *token;
		token = strtok(tmp1, "/");
	
		while(token != NULL) {						// getting filename from link
			token = strtok(NULL, "/");				// get next token
			if (token != NULL) strcpy(buf, token);	// copy token to buf to save possible filename from url
		}
		
		int n;
		for (n = 0; buf[n] != '\0'; n++)  {
			if (buf[n] == '.') goto SKIP_S;
		}
		
		void *haddr;
		if(R_FAILED(svcSetHeapSize(&haddr, 0x10000000))) return;
		
		printf("\n\n# %s%s%s", CONSOLE_YELLOW, "No extension founded for your filename from your link. Want to add it now?\n", CONSOLE_RESET);
		printf("\n  Press [A] to continue");
		printf("\n  Press [B] to skip\n");
		
		Result rc;
	
		SwkbdConfig skp;
		rc = swkbdCreate(&skp, 0);
	
		while(appletMainLoop()) {
			hidScanInput();
			u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
		
			if (kDown & KEY_A) {
				rc = swkbdCreate(&skp, 0);
				
				if (R_SUCCEEDED(rc)) {
				
					char tmp[16] = {0};
					swkbdConfigMakePresetDefault(&skp);
					swkbdConfigSetGuideText(&skp, "Remember to add `.` like `[example].nsp`");
	
					rc = swkbdShow(&skp, tmp, sizeof(tmp));
					strcat(buf, tmp);
					swkbdClose(&skp);
					break;
				}
			}
		
			if (kDown & KEY_B) break;
		
			consoleUpdate(NULL);
		}
		
	}
	
	SKIP_S:
	
	dest = fopen(buf, "r");
	
	if (dest == NULL) {
		
		printf("\n# %s%s%s%s%s", CONSOLE_GREEN, "No (", buf, ") to overwrite", CONSOLE_RESET);
		dest = fopen(buf, "wb");
	
	} else { // the file exist
		printf("\n# %s%s%s%s%s", CONSOLE_YELLOW, "File (", buf, ") exist already, overwrite?\n", CONSOLE_RESET);
		printf("\nPress [A] to continue\nPress [B] to go back"); // little warning
		
		while (appletMainLoop()) {
			hidScanInput();
			u32 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
			
			if (kDown & KEY_A) {
				break;
			}
			
			if (kDown & KEY_B) {
				fclose(dest); // closing early
				return;
			}
			
			consoleUpdate(NULL);
		}
		
		fclose(dest);
		dest = fopen(buf, "wb");
		
	}
	
	printf("\n# %s%s%s", CONSOLE_GREEN, "Starting downloading...\n", CONSOLE_RESET);
	
    if (curl)
	{
		prog.lastruntime = 0;
		prog.curl = curl;
		printf("\n\n# URL = %s%s%s", CONSOLE_GREEN, url, CONSOLE_RESET);
		printf("\n# File = %s%s%s%s\n", CONSOLE_GREEN, path, buf, CONSOLE_RESET);
		
		curl_easy_setopt(curl, CURLOPT_URL, url); // getting URL from char *url
		
		printf("\n# Debug active\n");
		
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);						// useful for debugging
		curl_easy_setopt(curl, CURLOPT_REDIR_PROTOCOLS, CURLPROTO_HTTP);	// following only HTTP redirects
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);					// skipping cert. verification, if needed
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);					// skipping hostname verification, if needed
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, dest);					// writes data into FILE *destination
		curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, older_progress);
		curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, &prog);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
		res = curl_easy_perform(curl);
		
		fclose(dest);
		
		if (res != CURLE_OK) printf("\n# Failed: %s%s%s", CONSOLE_RED, curl_easy_strerror(res), CONSOLE_RESET);
		
		free(url);
	}
	
	curl_easy_cleanup(curl);
	return; // terminate function; Congrats, you did it!
}