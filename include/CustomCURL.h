#ifndef _CUSTOMCURL_H_
#define _CUSTOMCURL_H_

/* includes */
#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h> // used for chdir()

#define TIMETYPE double
#define TIMEOPT CURLINFO_TOTAL_TIME
#define MINIMAL_PROGRESS_FUNCTIONALITY_INTERVAL     3
#define STOP_DOWNLOAD_AFTER_THIS_MANY_BYTES         6000

CURL *curl; 
CURLcode res;

struct myprogress {
  TIMETYPE lastruntime;
  CURL *curl;
};

static int xferinfo(void *p, curl_off_t dltotal, curl_off_t dlnow) {
  struct myprogress *myp = (struct myprogress *)p;
  CURL *curl = myp->curl;
  TIMETYPE curtime = 0;
 
  curl_easy_getinfo(curl, TIMEOPT, &curtime);
 
  if((curtime - myp->lastruntime) >= MINIMAL_PROGRESS_FUNCTIONALITY_INTERVAL) {
    myp->lastruntime = curtime;
	printf("TOTAL TIME: %f \r", curtime);
  }
  
  printf("DOWNLOAD: %" CURL_FORMAT_CURL_OFF_T " of %" CURL_FORMAT_CURL_OFF_T "\r", dlnow, dltotal);
  
  consoleUpdate(NULL);
  return 0;
}

static int older_progress(void *p, double dltotal, double dlnow) {
  return xferinfo(p, (curl_off_t)dltotal, (curl_off_t)dlnow);
}

// this "write_callback" probably is used for writing into FILE *destinaton (FILE *stream)
size_t write_callback(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
	return written;
}

void curlInit()
{
	socketInitializeDefault();
	curl = curl_easy_init(); // initializing/calling functions
}


void curlExit()
{
	appletEndBlockingHomeButton();
	socketExit();
}

void nXDownloadUpdate()
{
	char url[] = "http://projects00.000webhostapp.com/nXDownload.nro";
	char path[] = "sdmc:/switch/nXDownload/";
	
	chdir(path);
	FILE *dest;
	dest = fopen("nXDownload_newer.nro", "w");
	
	if (curl)
	{
		appletBeginBlockingHomeButton(0);
		printf("\n\n* URL = %s\n\n* Download...\n", url);
		curl_easy_setopt(curl, CURLOPT_URL, url); // getting URL from char *url
		printf("\n* DEBUG MODE\n");
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); // useful for debugging
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // following HTTP redirects
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 120L); // a max of setopt timeout for 20s
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // skipping cert. verification, if needed
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); // skipping hostname verification, if needed
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, dest); // writes pointer into FILE *destination
        res = curl_easy_perform(curl); // perform tasks curl_easy_setopt asked before
		
		if (res != CURLE_OK) {
			printf("\n# Failed: %s", curl_easy_strerror(res));
			fclose(dest);
		}
		
	    appletEndBlockingHomeButton();
		fclose(dest); // closing FILE *stream
	}
	
	remove("sdmc:/switch/nXDownload/nXDownload.nro"); 
	rename("sdmc:/switch/nXDownload/nXDownload_newer.nro", "sdmc:/switch/nXDownload/nXDownload.nro"); // Wow. this is like reinxtool source! xD
	return;
}

void fileDownload(char *url, char path[], int a)
{
	consoleClear();
	FILE *dest;
	char tmp1[256];
	char buf[256];
	struct myprogress prog;
	
	if (a == 2) { // useful to pass data (url) between functions; but you need to write the tmpfile.txt with inside the url, before executing this function
	
		dest = fopen("sdmc:/switch/nXDownload/tmpfile.txt", "r");
		while(!feof(dest)) fgets(tmp1, sizeof(tmp1), dest);
		url = strdup(tmp1);
		printf("\n# done reading plain url");
		fclose(dest);
		
		if (url == NULL) {
			perror("\nError passing argument");
			return; // terminate function
		}
		
		goto SKIP;
	}
	
	// if int a equivale with 2, the code below is skipped
	
	chdir("sdmc:/switch/nXDownload/"); // changing path to find the files needed for nXD
	
	consoleClear();
	char i[512][512];
	char f[512][512];
	int n;
	
	consoleUpdate(NULL);
	
	if ((dest = fopen("input.txt","r")) != NULL) {
		for (n = 0; n < 512; n++) {
			
			if (fscanf(dest, "%s = %s", i[n], f[n]) != 2) break;
			
			printf("\x1b[1;1H%d links counter", n);
			
			if (n == 511) printf("\n# Too many links that i can't handle! My MAX is 511 links!");
			
			consoleUpdate(NULL);
		}
		
		int counter = n - 1;
		fclose(dest);
		
		if (i[n] == NULL || f[n] == NULL) { // error: no arguments as the file is open
			printf("\n# There was an err while reading arguments:\nCheck that 'input.txt' at least is made like this:\n===============================\n<title> = <download/link/url>");
			return;
		}
		
		printf("\x1b[5;1HStart = %s%s%s\nURL = %s%s%s", CONSOLE_RED, i[n], CONSOLE_RESET, CONSOLE_GREEN, f[n], CONSOLE_RESET);
		
		while(appletMainLoop()) {
			hidScanInput();
			u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
			
			printf("\x1b[1;1H%d links counter", n);
			printf("\x1b[5;1HStart = %s%s%s\nURL = %s%s%s", CONSOLE_RED, i[n], CONSOLE_RESET, CONSOLE_GREEN, f[n], CONSOLE_RESET);
			printf("\x1b[9;1HPress D-PAD [->] to look forward\n");
			printf("Press D-PAD [<-] to look backward\n");
			printf("\nPress [B] to go back\n");
			printf("Press [A] to start download\n");
			
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
				return; // terminate function
			}
			
			if (kDown & KEY_A) {
				sprintf(tmp1, "%s", f[n]);
				break; 
			}
			
			consoleUpdate(NULL);
		}
			
		printf("\n# Done!");
		consoleUpdate(NULL);
		url = strdup(tmp1);
		fclose(dest);
		
	} else { // error opening dest
		perror("Error dest");
		consoleUpdate(NULL);
		return;
	}
	
	SKIP:
	chdir(path); // change dir
	
	if (a == 0 || a == 1) { // this parses the url to mantain the last token, which (usually) contains the filename
		
		char *token;
		token = strtok(tmp1, "/");
	
		while(token != NULL) {
			token = strtok(NULL, "/");
			if (token != NULL) strcpy(buf, token);
		}
		
	}
	
	if (buf == NULL) { 
		perror("\n# Failed");
		curlExit();
		return; // terminate function
	}
	
	dest = fopen(buf, "r");
	
	if (dest == NULL) {
		fclose(dest);
		perror("\n# Failed buf (not important)");
		dest = fopen(buf, "wb");
	} else { // the file exist
		printf("\n# File %s exist already, overwrite?\n\nPress [A] to continue\nPress [B] to exit", buf); // little warning
		
		while (appletMainLoop()) {
			hidScanInput();
			u32 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
			
			if (kDown & KEY_A) {
				break;
			}
			if (kDown & KEY_B) {
				fclose(dest); // as you decided to return, we need to close FILE *stream
				curlExit();
				return; // terminate function
			}
			
			consoleUpdate(NULL);
		}
		
		fclose(dest);
		dest = fopen(buf, "wb");
		
	}
	
	printf("\n# Starting curl...\n");
	
    if (curl)
	{
		appletBeginBlockingHomeButton(0);
		prog.lastruntime = 0;
		prog.curl = curl;
		printf("\n\n* URL = %s\n* File = %s%s\n\n* Download...\n", url, path, buf);
		curl_easy_setopt(curl, CURLOPT_URL, url); // getting URL from char *url
		printf("\n* DEBUG MODE\n");
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); // useful for debugging
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // following HTTP redirects
		curl_easy_setopt(curl, CURLOPT_REDIR_PROTOCOLS, CURLPROTO_HTTP);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // skipping cert. verification, if needed
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); // skipping hostname verification, if needed
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, dest); // writes pointer into FILE *destination
		curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, older_progress);
		curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, &prog);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
        res = curl_easy_perform(curl); // perform tasks curl_easy_setopt asked before
		
		if (res != CURLE_OK) {
				printf("\n# Failed: %s", curl_easy_strerror(res));
				fclose(dest);
		    }
		
		free(url);
		appletEndBlockingHomeButton();
		fclose(dest); // closing FILE *stream
	
	}
	
	return; // terminate function; Congrats, you did it till here!
}

#endif